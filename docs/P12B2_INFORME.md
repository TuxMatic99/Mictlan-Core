# P1.2-B2: informe técnico y cierre documental

Revisión funcional: `c4aa656abc8302f893fe74e913867e2852552317`.
Cierre documental: 14 de noviembre de 2026, zona America/Mexico_City.

Este informe describe la implementación **ya publicada** de listas y diccionarios.
En este cierre se consultaron código, documentación y registros existentes de
GitHub; no se volvieron a compilar ni ejecutar las suites. Los resultados de CI
que siguen corresponden a ejecuciones anteriores, identificadas por SHA y enlace.

## Estado comprobado

Al iniciar el cierre, la [PR #9](https://github.com/TuxMatic99/Mictlan-Core/pull/9)
estaba abierta, en borrador, sin fusionar y apuntaba exactamente al SHA indicado.
Su rama es `investigacion/p12b2-colecciones` y su base es
`investigacion/p12b1-cadenas`, en `6b8204ab92196fccfc217c80c6f381aa8ef03535`.
La PR #8 y la PR #7 seguían abiertas en borrador; P1.2-A conserva el SHA
`c25eb7e5008e1201ebd1292da7b2da0dd0a554a0`.

Ya estaban publicados [el inventario](P12B2_INVENTARIO.md),
[la referencia con ejemplos](COLECCIONES.md), [las decisiones](P12B2_DECISIONES.md)
y las evidencias de fallos anteriores. Faltaban este informe y la guía de Debian.
El cierre añade solamente documentación y registros recuperados a la misma rama;
la implementación, las pruebas y los commits publicados se conservan.
El SHA del commit documental se registra en la descripción de la PR.
`main` permanecía en `b3be302bbfdc0dfc371b6871dd203f7ff9ce3a0b`.

## Correcciones funcionales publicadas

| Área | Defecto demostrado | Comportamiento corregido y evidencia |
|---|---|---|
| Eliminación de diccionario | Desconectaba toda la cubeta y no actualizaba longitud | Retira solo la entrada buscada y descuenta una vez. b2_32, b2_87 y b2_128 colisionan en la cubeta 127 inicial; eliminar la segunda conserva las otras dos. Casos dic_integridad, dic_colisiones e integridad_dic.c |
| Clave inválida en diccionario vacío | dic.eliminar({}, 3) no validaba el tipo | La clave debe ser cadena también en vacío; caso dic_tipo_vacio |
| Consulta de claves | dic.contiene recorría todas las cubetas | Usa la consulta por hash y reconoce claves cuyo valor es nulo; conserva nombres y resultado |
| Unión de listas | La reserva omitía bytes del separador para cadenas y no comprobaba realloc | Dos recorridos calculan el tamaño completo, controlan desbordamientos y comprueban la reserva. union_larga verifica texto exacto con separador multibyte |
| Temporales de la unión | Se reservaba antes de validar; un error al publicar saltaba la liberación; NULL podía usarse | Validación previa, error de reserva controlado y liberación antes de propagar un error de publicación. temporales.c comprueba cero buffers pendientes y preservación de pila |
| Índices y cantidades | Conversión indefinida de NaN, infinitos o números fuera de int | Se trunca hacia cero, se comprueba finitud/rango y se convierte. Se conservan las fracciones históricamente válidas |
| Comparación numérica en listas | La resta double convertida a int confundía 0.1 con 0.9 | comparar([0.1], [0.9]) devuelve -1 y contiene([0.1], 0.9) devuelve falso; casos comparacion_numerica y limites |
| Comparación de listas cíclicas | Recursión sin límite | Error controlado al encontrar un ciclo o superar 256 niveles; ciclos.c y profundidad.c comprueban recuperación |
| Recorridos de listas | Invertir, concatenar y comparar repetían búsquedas por índice en una lista enlazada | Recorren nodos directamente: O(n), O(n+m) y O(n) para comparar listas planas de números; se conservan referencias y originales |

Los casos se encuentran en [tests/colecciones](../tests/colecciones).
Las 43 regresiones iniciales se añadieron antes de cambiar producción, en
`91bb2f5e31799f0472b2f3cfc428a464700c5a55`: el registro conserva 12 aprobadas y
31 fallidas. Dos fallos correspondían al alias nuevo todavía ausente. Las mismas
regresiones están incluidas y aprobadas en la matriz final de la revisión funcional.

Evidencias anteriores: [fallos funcionales](evidencias/p12b2/antes-debug.log),
[desbordamiento de unión detectado por ASan](evidencias/p12b2/antes-union-asan.log)
y [buffer pendiente al fallar la publicación](evidencias/p12b2/antes-publicacion.log).
Los .out son oráculos explícitos; los verificadores comparan salida, código de
retorno y diagnóstico, y rechazan informes de sanitizadores.

El ayudante privado de eliminación usa el hash con la capacidad actual de la tabla.
Libera nodo y metadatos, pero conserva el descriptor de valor bajo propiedad de la
VM: una lista obtenida con dic.valores puede seguir apuntando a él. La prueba C
incluye 1.300 inserciones, 434 eliminaciones y 2.000 inserciones adicionales, con
recuentos físicos y consultas tras redimensionar. También verifica valores
compartidos durante una recolección explícita con raíces retenidas.

La comparación corregida se limita a las operaciones de colecciones; no cambia el
cuerpo del comparador escalar ni la gramática. Las expresiones que ya delegaban en
latL_comparar reciben esa corrección. La limpieza de publicación comparte la
definición privada de lat_longjmp con latdo.c; no modifica la disposición pública
de la VM ni sus reglas de recuperación. Estos cambios ya pertenecían al SHA funcional.

## Compatibilidad y referencias

Lista conserva sus 14 nombres heredados y ya incorpora **lista.unir** como alias
exacto de lista.separar. Dic conserva seis nombres, incluidos valores/vals, y el
módulo diccionario sigue compartiendo la tabla de dic. La referencia documenta
los 21 nombres, sus firmas, ejemplos, errores, mutación y costes.

Se conservan los contratos heredados que pueden sorprender:

* Separar/unir une cadenas y números, omite otros tipos, usa el índice original
  para decidir separadores y devuelve nulo si no produce bytes.
* Los índices finitos representables se truncan hacia cero. Insertar admite 0..n;
  eliminar_indice requiere 0..n-1. Crear con cantidad negativa representable da [].
* Comparar mira primero la longitud. Contiene e indice/eliminar mantienen criterios
  distintos entre tipos; no se introduce una igualdad uniforme silenciosamente.
* Asignar comparte el contenedor. Invertir, concatenar y extender comparten elementos.
  El clonado heredado de listas copia números y listas anidadas, pero comparte otros
  elementos; el clonado de diccionarios es superficial.
* Las claves son exactas y la enumeración no garantiza orden de inserción.

B2 añade **52 pruebas** a las **128 anteriores**: 40 ejecuciones de programas
(20 casos ejecutados como .lat y .mict), diez pruebas C, una de documentación y una
de ABI. Cada par .lat/.mict usa el mismo fuente y el mismo oráculo; solo se normalizan
finales de línea. Ambos formatos aprobaron los casos cubiertos por CI. Esto acredita
compatibilidad de la batería, no todos los programas posibles.

Se comprueban alias por asignación, listas anidadas, diccionarios usados como valores,
objetos repetidos, copias selectivas, vistas de valores, eliminación/reinserción y
ciclos de listas y diccionarios. Las regresiones anteriores de P0.2-E/F/G, P1.1 y
P1.2-A/B1 siguen dentro de la matriz.

La prueba mictlan_p12b2_abi aprobó en los tres perfiles: conserva los **285 símbolos
públicos de Core**. No cambian las cabeceras públicas respecto de B1. Las guardias
ODR generadas por ASan se identifican aparte; unir es un alias del lenguaje y no
un símbolo C nuevo. Esta comprobación cubre la biblioteca GNU/Linux de CI, no una
certificación de ABI para todas las plataformas.

## Resultados exactos de CI consultados

La ejecución [43563834405, evento push](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43563834405)
comprueba el SHA funcional exacto: sus logs de checkout muestran
`c4aa656abc8302f893fe74e913867e2852552317`. Entorno: Ubuntu 24.04, GCC 13.3.0.

| Perfil | Resultado registrado | Trabajo de CI | Clasificación |
|---|---|---|---|
| Debug | 180 aprobadas, 0 fallidas | [132353195759](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43563834405/job/132353195759) | Aprobado |
| Release | 180 aprobadas, 0 fallidas | [132353195965](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43563834405/job/132353195965) | Aprobado |
| ASan + UBSan + LSan | 196 aprobadas, 0 fallidas | [132353195946](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43563834405/job/132353195946) | Aprobado |

Extractos conservados: [Debug](evidencias/p12b2/cierre-ci-debug.log),
[Release](evidencias/p12b2/cierre-ci-release.log) e
[instrumentación](evidencias/p12b2/cierre-ci-asan.log).
La configuración instrumentada añade 16 regresiones anteriores: 196 = 128 + 52 + 16.
En esos trabajos no se registran pruebas omitidas ni bloqueadas.

También se revisó la ejecución posterior de PR
[43563903411](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43563903411),
asociada al mismo head: Debug 180/180 (trabajo 132353410486), Release 180/180
(132353410408) e instrumentación 196/196 (132353410501). Su checkout es el merge
sintético de GitHub para probar la PR; ese mecanismo no implica que la PR se haya
fusionado. La ejecución push anterior es la evidencia directa del SHA exacto.

Los registros muestran estas opciones activas:

```text
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1
LSAN_OPTIONS=exitcode=23
```

La compilación y configuración de esos trabajos terminaron correctamente. Los
resultados son evidencia existente consultada, no ejecuciones nuevas de este cierre.

## Limitaciones y pendientes

La instrumentación local de la misión anterior quedó **bloqueada por infraestructura**:
LSan informó que no funciona bajo ptrace. El registro recuperado clasifica 193
pruebas bloqueadas y tres comprobaciones documentales/ABI aprobadas; no constituye
aprobación de ejecución nativa instrumentada. Se conservan
[el diagnóstico](evidencias/p12b2/local-asan-infraestructura.log) y
[la clasificación por prueba](evidencias/p12b2/local-asan-clasificacion.tsv).
En este cierre tampoco se ejecutó una nueva validación independiente en Debian 13;
[la guía](DEBIAN_P12B2.md) permite realizarla y registrar su resultado por separado.

No hay fallos registrados en las suites de CI citadas. Permanecen como decisiones
futuras la igualdad uniforme entre tipos, una copia profunda universal y una política
explícita de orden de inserción. Las comparaciones cíclicas producen error; no hay
igualdad coinductiva. La API C destructiva heredada exige respetar propiedad y
préstamos y no equivale a una API general de referencias compartidas.

Sin recolección optativa, celdas retiradas y contenedores creados pueden conservarse
hasta cerrar la VM. Ello protege referencias existentes; el crecimiento durante
una VM larga debe evaluarse aparte del resultado de LSan al cierre. Los costes
indicados describen recorridos lógicos, no garantías de latencia. Este cierre no
realizó ni presenta nuevas mediciones de rendimiento.
