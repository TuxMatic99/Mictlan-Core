# P1.2-B4-B — entrada de datos e importación de módulos

Implementación y batería publicadas en la [PR #12 en borrador](https://github.com/TuxMatic99/Mictlan-Core/pull/12),
contra `investigacion/p12b4a-integridad-archivos`.
Revisión exacta de código y batería: **`c60b09883437f55a8ab0bb56bce6b846c4b0d92e`**.
Los commits posteriores de cierre sólo incorporan este informe, guía y evidencias.
Fecha de revisión: 14 de noviembre de 2026, México (15 de noviembre UTC en Actions).

## Trazabilidad y conservación

| Entrega | Commit | Contenido |
|---|---|---|
| Base autorizada | `82be31bf11b6fcfc3bb19979c2b62ca337edc2fd` | PR #11, B4-A, base exacta del primer commit |
| A: antes de producción | `1ffcf2f8d7dbcfd246a3d374c1ce737791238d74` | Contratos, 278 pruebas nuevas y evidencia inicial; producción intacta |
| B: implementación | `8592713290dbc4829a023617492ed3f098b3d668` | Correcciones C, dos funciones de entrada explícitas y 669/669 Debug |
| C: batería y referencia | `c60b09883437f55a8ab0bb56bce6b846c4b0d92e` | Siete verificaciones adicionales, documentación ejecutable, ABI y resultados locales |

Se verificaron las PR #7–#11 antes de crear la rama y de nuevo al cerrar la
implementación: todas abiertas, en borrador, sin fusionar, con sus cabezas y
relaciones conservadas. `main` permanece en
`b3be302bbfdc0dfc371b6871dd203f7ff9ce3a0b`. No había rama/PR B4-B previa.
[Registro de revisiones](evidencias/p12b4b/revisiones.txt).

## Defectos demostrados y reparaciones

| Caso reproducible | Antes | Causa | Después |
|---|---|---|---|
| `leer`, EIO tras bytes de la línea | Devuelve 12 como número pese al error de lectura | Se publicaba lo que devolvía Readline sin comprobar ferror | No publica el prefijo; leer devuelve nulo, las funciones modernas informan error; llamada posterior válida en la misma VM |
| Error al publicar texto | Un buffer crudo pendiente | longjmp omite free posterior | Guard privado de B4-A y liberación antes de relanzar; cero buffers pendientes |
| Entrada redirigida `12\r\n` | Dos lecturas: 12 y cadena vacía; además eco | Backend de edición de Readline usado para un flujo de datos | Una línea 12, después EOF; sin eco añadido por Core |
| NUL incrustado | Readline lo elimina y aparenta un texto válido | Edición de controles en lugar de lectura exacta | Rechazo explícito; no se publica un prefijo o texto alterado; se consume la línea y se comprueba la siguiente |
| `latC_strtod_c` en locale decimal | Dos mutaciones del locale global por conversión | setlocale temporal y copia de nombre de 64 bytes | Locale C propio; cero mutaciones, conversión en tres hilos con locales distintos y errno comprobado |
| Importación correcta o con error de ejecución | Ubicación del llamador alterada | Sólo restauraba nombre de archivo en el retorno normal | Restauración de archivo, REPL, línea y columna en todas las rutas cubiertas |
| Fallo al entrar al compilador de módulo | Seis reservas AST pendientes y archivo activo incorrecto | Compilación fuera del bloque de limpieza | AST y función temporal se destruyen al salir protegido; cero reservas observadas y estado restaurado |

La prueba inicial del compilador produjo además un informe LSan de **2 bytes en
una reserva residual**, después de la limpieza auxiliar del arnés. Esa cifra no
es el tamaño total de las seis reservas observadas antes de limpiar. El registro
original se conserva en [CI inicial instrumentada](evidencias/p12b4b/ci-antes-asan.log.gz).
No se generaliza esta inyección a todas las reservas internas del compilador.

La base ya manejaba correctamente EOF, varias entradas numéricas, EINTR del backend
y algunos fallos de reserva. Esos casos se conservan como no regresión. La prioridad
de extensiones y la dependencia de locale de `leer` **no se clasifican como errores**.

El primer prototipo de inyección usaba fopencookie sin descriptor y provocó un
aborto interno de select en Readline. Se corrigió el arnés antes de la evidencia
publicada; no se atribuye ese aborto a Core. Durante implementación también se
corrigió la activación de GC en el arnés, que debe preceder a la ejecución, y se
observó la nueva frontera de publicación en pila sin quitar la anterior.

## Contratos y compatibilidad

[Decisiones previas al parche](P12B4B_DECISIONES.md),
[inventario final](P12B4B_INVENTARIO.md) y
[referencia educativa ejecutable](ENTRADA_MODULOS.md).

* `leer()` conserva inferencia mediante strtod del locale activo, incluido cero,
  cero negativo, notación científica, NaN/infinito, hexadecimal y la diferencia
  entre espacios iniciales y finales. El ejecutable sigue fijando LC_NUMERIC=C.
* Se añaden `leer_texto()` y `leer_numero()`, sin palabras reservadas ni símbolos C
  nuevos. EOF devuelve nulo; errores operativos se distinguen mediante error de
  ejecución. Límite explícito de 8 MiB sólo para estas alternativas.
* `leer_numero` reutiliza el reconocedor de `anumero_estricto` de B3. No se cambia
  el significado de `anumero`, ni double, ni los ámbitos o reglas de P1.2-A.
* Sin extensión se buscan CWD, directorio importador y LATINO_LIB para **.lat**, y
  sólo después esos tres lugares para **.mict**. Sufijos explícitos no se sustituyen.
  Importaciones repetidas vuelven a ejecutar; no hay caché ni espacios de nombres
  nuevos. El fallback nativo no se modifica y su prueba usa una sonda sin carga.
* Los cambios observables deliberados son el tratamiento completo de entrada
  redirigida (sin eco/edición, CRLF único, NUL rechazado), descarte de entrada con
  error y restauración/limpieza de módulos. Windows deja el buffer fijo por el
  lector CRT común, pero esta plataforma no se certifica como probada.
* Sólo cambian cuatro archivos de producción: latbaselib.c, latlocale.c y el traslado
  del guard de latfilelib.c a laterror.h privado. No se toca parser, cabeceras
  públicas, carga nativa, registro de propiedad o política global de conversión.

Hay **13 pares idénticos .lat/.mict** y 50 ejecuciones CLI con oráculos de salida,
diagnóstico, código y disco. Las seis familias de entrada se ejecutan en los tres
locales con cada extensión; los módulos y conversiones completan la matriz.
La referencia se genera desde los mismos ejemplos y un test detecta discrepancias.

## Resultados comprobados

La [CI inicial 43664434426](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43664434426)
ejecutó el commit A: **471/669 Debug**, **471/669 Release**, **487/685 instrumentadas**.
Son 198 fallos en cada perfil, incluidos contratos nuevos cuyas funciones aún no
existían. Las 391/407 anteriores aprobaron. No se presenta ese número como 198
bugs diferentes. [Resumen anterior](evidencias/p12b4b/antes-resumen.txt) y
[log local anterior](evidencias/p12b4b/local-antes.log.gz).

CI de la batería completa, revisión c60b098:
[run 43665259418](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43665259418).
Se leyeron los logs de los tres jobs, se comprobó el SHA y se compararon todos los
nombres de pruebas con la batería local. No se infiere el resultado sólo del icono.

| Entorno/perfil | Aprobadas | Fallos funcionales/sanitizador | Omitidas | Bloqueadas por infraestructura | Evidencia |
|---|---:|---:|---:|---:|---|
| Debian 13 local Debug | 676/676 | 0 | 0 | 0 | [log](evidencias/p12b4b/local-debug.log.gz) |
| Debian 13 local Release | 676/676 | 0 | 0 | 0 | [log](evidencias/p12b4b/local-release.log.gz) |
| Local ASan+UBSan+LSan | 6/692, sólo ABI/documentación | No se determina la ejecución de Core | 0 | 686 | [diagnóstico](evidencias/p12b4b/local-asan-resumen.txt), [log](evidencias/p12b4b/local-asan.log.gz) |
| CI Ubuntu 24.04 Debug | 676/676 | 0 | 0 | 0 | [job](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43665259418/job/132663092164), [log](evidencias/p12b4b/ci-debug.log.gz) |
| CI Ubuntu 24.04 Release | 676/676 | 0 | 0 | 0 | [job](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43665259418/job/132663092337), [log](evidencias/p12b4b/ci-release.log.gz) |
| CI ASan+UBSan+LSan | 692/692 | 0 | 0 | 0 | [job](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43665259418/job/132663092334), [log](evidencias/p12b4b/ci-asan.log.gz) |

Configuración y compilación aprobadas en todos esos perfiles. Cada uno de los
686 fallos locales de CTest contiene `does not work under ptrace`; se clasifica
como infraestructura y no como aprobación. No se desactivaron detectores.
Los seis aprobados locales de esa configuración son cinco verificadores de
referencia documental y ABI: no ejecutan Core instrumentado.
Los pasos de benchmarks de Release se omiten deliberadamente en otros perfiles;
no son pruebas CTest omitidas. No se declara una mejora de rendimiento B4-B a partir
de esas mediciones heredadas ni se añade un benchmark innecesario.

Se mantienen **391 pruebas normales y 407 instrumentadas**, y se añaden **285**:
234 contratos C/inyección/locale, 50 CLI y una sincronía documental.
[Conservación](evidencias/p12b4b/conservacion.txt),
[correspondencia CI/local](evidencias/p12b4b/ci-resumen.txt).
Las pruebas de GC retienen una función de módulo después de liberar su AST y
barrer cadenas; las de error exigen ejecutar de nuevo en la misma VM y conservar
pila, contexto y contador. ASan/UBSan/LSan no informaron defectos en la CI final.

## ABI, entorno, enlace y reproducción

**285 símbolos C preservados, cero añadidos o eliminados**. `include/` permanece
idéntico a la base; no cambian las estructuras usadas por SDK P1.1. Los contratos C
anteriores se ejecutan en la matriz. No se afirma una recompilación separada del
repositorio SDK en esta misión. El verificador ABI sólo separa los guardias ODR de
ASan que no forman parte de Core.

Local: Debian 13, GCC 14.2.0, glibc 2.41, CMake 4.4.5, Python de CTest 3.13.5.
CI: Ubuntu 24.04, GCC 13.3.0, glibc 2.39, CMake 3.31.6, Python 3.12.3.
[Versiones, hashes de bibliotecas y ABI](evidencias/p12b4b/entorno-abi.txt).

B4-B reutiliza literalmente el envoltorio y sonda de B4-A. Los procesos hijos
limpian variables de carga, seleccionan la biblioteca del build y verifican su
ruta real con dladdr antes de ejecutar; se registra su SHA-256. La prueba histórica
que rechaza una biblioteca ajena sintética sigue aprobando. No se introduce otro
sistema de aislamiento ni se depende de un LD_LIBRARY_PATH global del usuario.
Los ejecutables de inyección enlazan el Core estático del mismo árbol, y además
pasan por la misma sonda previa.

Comandos locales efectivos (los prefijos de readline son específicos de este
entorno; la guía Debian usa paquetes normales):

```sh
cmake -S . -B ../build-p12b4b-debug -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  '-DCMAKE_C_FLAGS=-I/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/include' \
  '-DCMAKE_EXE_LINKER_FLAGS=-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu' \
  '-DCMAKE_SHARED_LINKER_FLAGS=-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu'
cmake --build ../build-p12b4b-debug --parallel 2
ctest --test-dir ../build-p12b4b-debug --output-on-failure --output-junit p12b4b-final.xml
```

Release usa árbol propio y CMAKE_BUILD_TYPE=Release. ASan usa Debug y añade
`-fsanitize=address,undefined,float-cast-overflow -fno-omit-frame-pointer` a C_FLAGS,
y los tres sanitizadores a ambos grupos de flags de enlace. Se ejecutó CTest con
`ASAN_OPTIONS=detect_leaks=1:halt_on_error=1`,
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`, `LSAN_OPTIONS=exitcode=23`.
Release e instrumentación se invocaron con `env -u LD_LIBRARY_PATH -u LD_PRELOAD
-u LD_AUDIT`; las pruebas B4-B limpian también esas variables internamente.

[Guía completa para Debian 13](P12B4B_DEBIAN13.md), incluyendo copia aislada,
locales temporales, los tres perfiles, ABI, enlace efectivo y ejemplo ejecutable.
La CI del commit de implementación 8592713 también aprobó 669/669, 669/669 y
685/685 antes de ampliar la batería; la validación final citada arriba incluye
los siete casos posteriores.

## Límites y trabajo futuro

* No hay caché ni detección anticipada de ciclos. Se verifica el límite de llamadas
  con pocos niveles; no se certifica una recursión C arbitrariamente profunda.
* No se revierten cambios de variables o archivos realizados por un módulo antes
  de fallar. No hay transacciones de importación.
* La prueba de fallo de compilación cubre su interfaz; la limpieza de cada posible
  reserva interna del parser/compilador sigue requiriendo una auditoría separada.
* Se valida Linux, stdin redirigido y el ayudante locale concurrente. Windows,
  macOS, edición de terminal e interrupciones interactivas no tienen certificación
  adicional. La VM completa no se declara segura para uso concurrente.
* La falta de locale se comunica mediante fin=inicio y ENOMEM en el ayudante C;
  los llamadores históricos que ignoren ambos no adquieren una nueva API de error.
* Los límites de línea evitan reservas ilimitadas en las alternativas nuevas,
  pero descartar una línea sin LF puede esperar al proveedor. No hay timeout de
  entrada, y tras un error persistente no se promete recuperar todos sus bytes.
* La instrumentación local sigue bloqueada por ptrace. La aprobación instrumentada
  corresponde a GitHub Actions y se mantiene separada de ese diagnóstico local.

Todos los datos ejecutados por los nuevos casos son sintéticos y propios de sus
directorios temporales. No se cargaron módulos nativos desconocidos, ni se añadieron
redes, procesos de administración, interoperabilidad, sintaxis ajena o cambios de
configuración global como solución de producción. La PR se conserva abierta en
borrador; no se fusiona ninguna propuesta.
