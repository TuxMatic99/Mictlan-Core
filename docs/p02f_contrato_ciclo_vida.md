# P0.2-F1: contrato propuesto antes de implementar

Referencia: 11 de noviembre de 2026, Mexico; auditoria: 12 de noviembre UTC.
Base comprobada en GitHub: `e7ece25f77502075b0dcad381d9b09a867b06a07`, rama
`investigacion/p02f-ciclo-vida-listas`, hija de
`d498a757b3492911abb707a7027d20a5f52605f4`.
Implementacion aislada: `investigacion/p02f-work`. No se integra P0.2-E aqui.

## Auditoria de lo existente

Un envoltorio `lat_objeto` contiene un valor escalar o un puntero al contenido.
Copiar el envoltorio no crea una lista nueva ni define quien debe destruirla.

| Operacion | Comportamiento comprobado en la base | Problema de propiedad |
| --- | --- | --- |
| `latO_crear` | Reserva un envoltorio; `nref=0` | No existe un propietario general de los temporales |
| `latO_clonar` | Clona numeros/listas recursivamente; otras entradas de listas se comparten; diccionarios/contextos copian envoltorios | No es una copia profunda uniforme; listas ciclicas pueden recursar indefinidamente |
| `latC_apilar` | Copia el valor a una celda del array de pila | No adquiere ni cuenta la propiedad del contenido |
| `latC_desapilar` | Devuelve una celda prestada que puede ser sobrescrita al apilar | No se debe destruir ni conservar como objeto de heap |
| `latH_asignar` | Reserva un envoltorio por clave; sustitucion copia sobre el existente | El contenido anterior puede quedar sin referencias visibles |
| `latH_limpiar` | Libera envoltorios y tabla, sin contenidos compartidos | Evita algunos UAF pero deja listas sin liberar |
| `latL_modificar_elemento` | Reemplaza el puntero del nodo | El objeto anterior puede quedar perdido; no se puede destruir sin conocer otros usuarios |
| `latMV_build_list` | Clona elementos; apila el contenido; libera el envoltorio temporal | Los datos de la lista quedan sin responsable de cierre |
| `latMV_store_subscr` | Clona el nuevo elemento antes de reemplazar/agregar | Los envoltorios reemplazados pueden perderse |
| Extraccion de una lista | Elimina el nodo y devuelve su valor | El valor debe sobrevivir a la destruccion del contenedor |
| SDK `devolver_vector2` | Reserva lista y numeros; apila; libera solo el envoltorio | Es el mismo problema que BUILD_LIST, no se arregla destruyendo el resultado despues de apilar |
| `latO_destruir` | Recorre listas/diccionarios; deduplica solo dentro de una lista | Dos contenedores o ciclos no tienen una propiedad global coherente |
| Cierre de VM | Libera contextos, modulos, funciones, constantes y cadenas internadas | Las listas/temporales no figuran en un registro completo |

`HABILITAR_GC` vale 0. Los macros INCREF/DECREF no tienen llamadas en `src`;
`nref` se inicializa/copia, pero no constituye conteo de referencias funcional.
No se reutilizara ese numero como si ya describiera la propiedad.

## Contrato de la primera correccion

Se propone una region de objetos ligada a cada VM creada por `latC_crear_mv`:

1. La VM es propietaria de los envoltorios creados por `latO_crear`, las listas
   creadas por `latL_crear` y las tablas creadas por `latH_crear` en esa VM.
   Las referencias almacenadas en pila, variables y contenedores son prestadas
   respecto de esa region. Un alias conserva identidad, no duplica su contenido.
2. Los contenidos de listas/diccionarios administrados permanecen vivos hasta
   cerrar la VM. `latO_destruir` libera un envoltorio que el llamador ya no usa,
   pero difiere la destruccion de esos contenidos. No destruir explicitamente
   un envoltorio prestado a un contenedor que aun lo utiliza.
3. `latM_liberar` retira del registro una reserva identificada antes de liberarla;
   asi los temporales que Core/SDK ya liberan no se vuelven a liberar al cerrar.
   `free` directo sobre objetos administrados no forma parte de este contrato.
4. Al cerrar se liberan tablas (sus envoltorios de entradas), estructuras/nodos
   de listas y, finalmente, envoltorios restantes. No se recorre recursivamente
   el grafo de valores. Alias, elementos repetidos y ciclos no causan dobles
   liberaciones por ese recorrido. Solo se liberan reservas registradas, nunca
   punteros arbitrarios encontrados dentro de un objeto.
5. Cadenas internadas, funciones registradas y modulos conservan sus responsables
   existentes. `T_CPTR` sigue siendo prestado; no se adivinan destructores para
   recursos externos. Clases/instancias requieren una auditoria adicional de
   sus contenidos propios y no se declaran resueltas por esta region.
6. Los ensayos C que utilizan `lat_mv mv={0}` y destruccion manual mantienen el
   contrato anterior; la region general se inicia solo al crear una VM real.
   La retencion explicita de constantes sigue funcionando en esos ensayos.

El registro sera privado, con busqueda por direccion y separado de `nref`.
Se ampliara la implementacion opaca ya apuntada por `constantes_propias`, sin
cambiar su posicion ni el tamano de `lat_mv`, `lat_objeto`, `lista` o `hash_map`.
Los clientes no conocen la estructura privada. Se verificaran layout, simbolos
y el SDK publicado. No se cambia la firma de ninguna funcion publica.

Esto NO es un recolector de basura durante la ejecucion. Las listas y objetos
perdidos por sustitucion pueden seguir ocupando memoria hasta cerrar la VM.
El beneficio verificable inicial es cierre seguro y sin las fugas reproducidas;
una VM de larga duracion necesita despues un contrato de raices/retencion y
recoleccion. Debe informarse esta limitacion incluso si LSan pasa al terminar.
No se comparten contenidos entre VM distintas: su vida termina con la VM
propietaria. La transferencia Core-SDK se comprueba usando la misma VM.

## Alternativas y reversibilidad

Liberar recursivamente cada valor al reemplazarlo rompe alias. Activar `nref`
exigiria instrumentar todas las copias, macros, la pila y la FFI, ademas de tratar
ciclos. Un recolector completo necesita identificar tambien referencias C
prestadas. La region es una primera etapa acotada y compatible con esas copias.

El contrato y las regresiones se conservaran en un commit anterior al parche.
La base publicada permanece intacta y permite comparar o descartar la propuesta
sin resetear trabajo ajeno. La integracion con E se probara en otra rama.

## Regresiones y criterios

Se conservan las cuatro regresiones publicadas (0, 200, 264 y 528 bytes segun
informe del coordinador; pendientes de reproduccion independiente en esta fase).
Se anaden casos C reales de valores compartidos, sublistas, diccionarios, ciclos,
extraccion, clonacion mixta, errores y retornos FFI. Las aserciones siguen activas
en Release. La clonacion de una lista ciclica debe terminar con error controlado,
no desbordar la pila; se aborda separadamente del cierre de la region.

Se ejecutaran Debug, Release y ASan+UBSan+LSan, inventario Core, modulares y
32 casos SDK sobre SHA exactos. Los bloqueos de ptrace se registran como
`BLOCKED_INFRASTRUCTURE`; solo un control LSan real permite declarar PASS.
