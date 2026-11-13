# Propiedad y recolección durante la ejecución

La VM de P0.2-F conserva un registro privado de descriptores, listas y tablas. El registro permite liberar cada reserva al cerrar. P0.2-G agrega un recolector no móvil que se activa desde un anfitrión C. Cada recolección es explícita y síncrona, entre llamadas protegidas. El anfitrión declara mediante `latG_retener` los objetos que seguirá utilizando desde C. Un programa puede seguir usando el cierre de F con la API histórica.

## Referencias y duración

| Origen | Referencia | Duración en el modo experimental |
| --- | --- | --- |
| `latM_asignar` | Reserva genérica | El asignador no deduce su tipo. Su propietario la libera con `latM_liberar`. |
| `latO_crear` | Descriptor registrado | Sus referencias se trazan según su tipo. El descriptor tiene identidad propia. |
| `latO_clonar` | Descriptor nuevo o copia de valor | Conserva la semántica F. Comparte o copia el contenido según su tipo. |
| `latC_apilar` | Copia en celda de pila | Las celdas activas conservan el contenido. `latC_desapilar` presta una celda hasta que se reutilice. |
| `latL_crear` | Lista genérica de `void *` | `latC_crear_lista` identifica una lista de valores. Una lista genérica restante impide la recolección. |
| Inserción y extracción de lista | Puntero a descriptor | La lista mantiene el objeto alcanzable. Tras extraerlo, el anfitrión puede fijarlo antes de recolectar. |
| `latH_asignar` | Descriptor por entrada | Copia el valor y conserva su identidad de contenido. `latH_obtener` presta el descriptor. |
| `latH_limpiar` | Tabla, entradas y cubetas | En modo G, los descriptores quedan en el registro porque `dic.valores` puede compartirlos. |
| Contextos | Descriptores y mapas | La VM traza contextos activos y de etiquetas. Los marcos finalizados liberan su tabla. |
| Funciones y constantes | Bytecode, nombres y objetos de `meta` | El compilador conserva sus constantes como raíces. El bytecode y las cadenas internadas viven hasta el cierre. |
| FFI | Celdas y contenidos de la VM | Una función autorizada conserva fuera de la llamada solo valores fijados. El resultado apilado queda vivo por la pila. |
| Clases, instancias, `T_CPTR`, carga dinámica y `latR` | Propiedad opaca | La VM conserva el régimen F cuando se usan estas rutas. |

El anfitrión usa un solo hilo por VM y serializa ejecución, mutación y recolección. Las reservas pertenecen a una VM. Se fija un descriptor, lista de datos o tabla registrada de esa VM, con una llamada a `latG_soltar` por cada fijación. El anfitrión conserva la fijación hasta su último uso. No debe pasar a `free`, destruir o reasignar una reserva fijada. Una celda prestada de la pila puede copiarse como valor escalar o usarse para obtener el contenido registrable de una lista o un diccionario. Los punteros a nodos de listas y a cubetas tienen la vigencia de su contenedor y de sus mutaciones.

`latG_autorizar` registra una función C por su dirección. El anfitrión declara que el código autorizado cumple el contrato también en sus retornos por error. Una función que conserva un objeto debe fijarlo antes de que abandone la llamada. `latG_recolectar` devuelve `LAT_G_OCUPADO` durante bytecode o FFI, incluso en una reentrada. El manejador de `longjmp` restablece la profundidad de ejecución. Una llamada nativa sin autorización inhibe las recolecciones posteriores de esa VM. La carga dinámica se inhibe antes de que puedan ejecutarse inicializadores externos. El compilador de Core registra sus constantes de manera explícita. El bytecode suministrado mediante la API C se trata como opaco.

## Algoritmo y condiciones de barrido

Se marcan los contextos activos, las celdas activas de pila, argumentos globales, módulos registrados, constantes y fijaciones del anfitrión. Se recorren listas de valores y tablas mediante una cola integrada en el registro. Las cubetas de tablas se recorren como entradas hash. El marcador consulta el registro antes de seguir un puntero. Las referencias externas o desconocidas interrumpen la operación antes del barrido. Los ciclos terminan porque cada reserva entra una vez en la cola.

Se limpia la caché de variables y se liberan tablas inalcanzables, nodos de listas y descriptores. La VM no mueve objetos ni invoca destructores de recursos externos durante el barrido. `latM_liberar` retira la reserva antes de liberarla. `latM_reasignar` rechaza mover una reserva registrada en modo G. Una recolección abortada conserva la región. El cierre sigue usando la propiedad de F, incluso para una VM que dejó de poder recolectar.

El trazado conserva ciclos y alias con cambios en tres clases de reservas. El conteo de referencias requeriría cambiar cada copia de descriptor y tratar ciclos. `nref` carece de esas transiciones. Las regiones temporales exigirían decidir qué cruza cada frontera, incluidas llamadas C anidadas. El registro existente permite un trazado acotado y raíces exactas sin leer la pila nativa. El coste de cada recolección depende del registro y de las aristas visitadas. El anfitrión decide la frecuencia y mide las pausas.

## Compatibilidad y límites

`latmemoria.h` agrega funciones y tipos. Los layouts de `lat_mv`, `lat_objeto`, `lista`, `hash_map` y los símbolos previos se conservan. `latG_medir` cuenta reservas registradas y bytes de esas reservas según el asignador. `bytes_vm` proviene del contador de `latM` e incluye metadatos y nodos. RSS incluye además la pila, bibliotecas, páginas del asignador y, según el perfil, los sanitizadores.

La recolección de listas y diccionarios no acota las compilaciones repetidas, la creación de cadenas internadas distintas ni el uso de clases y recursos opacos. La primera implementación no hace recolección automática, concurrente ni incremental. Los programas con callbacks no auditados pueden usar la VM normalmente bajo el cierre de F. La validez de los punteros retenidos depende del contrato del anfitrión. La API histórica expone estructuras C y permite escribir directamente en ellas. En modo G se modifican los contenedores con sus funciones de Core.
