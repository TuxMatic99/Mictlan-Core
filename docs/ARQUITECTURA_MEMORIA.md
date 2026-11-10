# Mictlan Core: contrato de propiedad de memoria

Estado: propuesta de arquitectura, no implementada.
Rama: investigacion/propiedad-memoria.

## 1. Problema comprobado

El modelo actual permite que varios contenedores
compartan punteros y que sus destructores intenten
liberar los mismos objetos.

Casos reproducidos con AddressSanitizer:
- Clonacion numerica: mitigada experimentalmente.
- Listas anidadas: use-after-free pendiente.
- Elementos repetidos: use-after-free pendiente.

## 2. Reglas que debe cumplir el nuevo modelo

1. Compartir un recurso no debe destruir su identidad.
2. Ninguna referencia valida debe apuntar a memoria liberada.
3. Cada reserva debe tener una politica de liberacion.
4. Las referencias prestadas no transfieren propiedad.
5. Las referencias propietarias deben poder liberarse.
6. Las estructuras ciclicas necesitan tratamiento especial.
7. Las operaciones FFI deben respetar el mismo contrato.

## 3. Componentes pendientes de integrar

- lat_objeto: identidad y envoltorios.
- lista: contenedores y elementos.
- hash_map: contextos y diccionarios.
- pila de la VM: copias por valor.
- funciones y cadenas: recursos compartidos.
- SDK/FFI: intercambio de objetos con codigo C.

## 4. Decisiones abiertas

- Identidad del recurso y lugar de su contador.
- Semantica de clonacion frente a alias.
- Adquisicion y transferencia de referencias.
- Ciclo de vida de temporales en la pila.
- Recuperacion de estructuras ciclicas.
- Compatibilidad con extensiones y ABI.

No activar HABILITAR_GC hasta resolver
estas decisiones y validar sus invariantes.

## 5. Criterios de aceptacion

- Diagnostico numerico: ASan y LSan sin errores.
- Diagnostico anidado: ASan y LSan sin errores.
- Elementos compartidos: ASan y LSan sin errores.
- Regresiones existentes de Core: aprobadas.
- Suite funcional del SDK: 32/32.
- Referencias y mutaciones: semantica preservada.

## 6. Contratos observados en Core

### Listas genericas

latL_agregar() reserva un nodo y almacena un puntero.
No incrementa contadores de referencias ni copia el
objeto apuntado.

Los nodos pueden contener lat_objeto u otros tipos de datos.

### Pila de la maquina virtual

latC_apilar() copia valores al espacio de la pila.
Para T_LIST y T_DIC conserva el recurso interno.

latC_desapilar() retrocede la pila sin liberar
explicitamente esos recursos.

### Variables y contextos

latH_asignar() crea o actualiza envoltorios.
Al copiar listas y diccionarios puede compartir
sus recursos internos.

latH_limpiar() libera envoltorios propios, pero
conserva determinados recursos compartidos.

### Destruccion

latO_destruir() puede destruir recursivamente
los elementos de una lista.

Este comportamiento entra en conflicto con
referencias compartidas sin propiedad registrada.

## 7. Invariantes del nuevo administrador

1. Cada recurso compartido tendra identidad estable.
2. Cada referencia propietaria debera contabilizarse.
3. Cada adquisicion tendra una liberacion correspondiente.
4. Una referencia prestada no destruira el recurso.
5. La pila no podra liberar recursos todavia utilizados.
6. Los contextos respetaran alias y reasignaciones.
7. Los ciclos necesitaran un mecanismo complementario.
8. No se modificara la ABI publica sin evaluar el SDK.

## 8. Estrategia de integracion

Primero: definir una API interna de retencion y liberacion.

Segundo: integrarla con contenedores de objetos.

Tercero: integrar las copias de la pila y los contextos.

Cuarto: validar las pruebas ASan y LSan existentes.

Quinto: estudiar ciclos y recuperacion de memoria.

No sustituir HABILITAR_GC ni activar un contador global
antes de completar el contrato de propiedad.
