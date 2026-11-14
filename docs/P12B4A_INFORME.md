# P1.2-B4-A: integridad y fiabilidad de archivos

**Código corregido y batería ampliada publicados en la [PR #11, abierta en
borrador](https://github.com/TuxMatic99/Mictlan-Core/pull/11)**, contra
`investigacion/p12b3-matematicas`. Revisión de código y pruebas validada:
**`257d80e64e38e82b1396755070ea8b6fb756a654`**; árbol
`c873554a71728605537718ce5624590d18839312`.
Los commits de cierre posteriores añaden informes y registros; no cambian ese
código. La CI comprueba la cabeza exacta, no una mezcla de bibliotecas o una
revisión deducida de la cadena histórica de versión.

Base autorizada: **`b0a2d860363e07274feae1d0cf2b421515386d73`**. La nueva rama es
`investigacion/p12b4a-integridad-archivos`. No se modificó main, no se fusionaron
PR y se conserva la ascendencia P0.2-E/F/G, P1.1 y P1.2-A/B1/B2/B3.

## Entregas y evidencia anterior al código

| Entrega | Commit | Contenido y comprobación |
|---|---|---|
| 1: diagnóstico y pruebas | `78032e14cd290d28b88b89c991dcccfafda5fbfb` | 80 pruebas nuevas y decisiones; ningún cambio en src/include; Debug original 294/331, con las 251 previas aprobadas |
| 2: corrección | `389e3160179429f44648dba7a67ecaa40ad746b7` | Motor compartido de copia, E/S completa, buffers/AST y recuperación; CI 331/331 Debug/Release y 347/347 instrumentadas |
| 3: ampliación/validación | `257d80e64e38e82b1396755070ea8b6fb756a654` | 140 pruebas nuevas totales, FIFO no bloqueante por rechazo previo, referencia con 52 programas; CI 391/391, 391/391 y 407/407 |
| Cierre documental | Cabeza de la PR, descendiente de la revisión anterior | Este informe, guía Debian y registros de CI/locales; consultar SHA exacto de la cabeza en GitHub |

Las decisiones semánticas se publicaron **antes** de implementar:
[P12B4A_DECISIONES.md](P12B4A_DECISIONES.md). La evidencia inicial de CI está en
[run 43650598285](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43650598285):
Debug y Release 294/331; instrumentación 310/347. Los mismos 37 casos B4-A
fallaron y las 267 pruebas instrumentadas anteriores se conservaron aprobadas.
De esos 37, cuatro exigen puntos de fallo del motor atómico que la copia antigua
no alcanzaba: son cobertura de la nueva política, no fallos de llamadas que no
existían. El arnés exige activación y por ello los marca como fallidos.

## Reparaciones funcionales

| Defecto reproducido | Causa | Corrección y prueba posterior |
|---|---|---|
| Origen ausente trunca/crea destino; flujos abiertos | Se abrían ambos antes de validar, y longjmp saltaba los cierres | Validar fuente primero; un solo dueño cierra cada flujo; destino anterior/ausente intacto en los casos de error |
| Copia sobre mismo inode destruye la fuente | Sólo comparaba nombres, no identidad del archivo | Comparar dispositivo/inode; rechazar alias duros; ambos contenidos intactos |
| Copia a symlink modifica otro archivo de la prueba | fopen seguía el destino | Motor atómico existente rechaza destino final no regular; el archivo centinela conserva exactamente sus bytes |
| Copia ignora errores de lectura/escritura/cierre | Bucle fgetc/fputc sin comprobaciones | Copia binaria por bloques de 16 KiB al mismo motor atómico; comprobar lectura, cierre fuente, write, fsync, close y rename antes de confirmar |
| EINTR y escrituras cortas | No completaba el progreso parcial | Reintentar sólo la parte pendiente; inyecciones de EINTR con y sin bytes transferidos; no se repiten ni pierden bytes |
| .mict no se ejecuta; .lat oculta errores y altera archivo activo | Filtro lat, resultado protegido ignorado, estado no restaurado | Ruta común .lat/.mict; propagación, restauración de archivo/REPL/línea/columna; casos anidados y recuperación en la misma VM |
| Cargador ejecuta pese a fallo de cierre | latA_analizar_arch ignoraba fclose y lectura incompleta | Verificar lectura completa y cierre antes de analizar; no se modifica la gramática |
| Fuga al publicar lectura o crear lista | Error de VM salta free del buffer | Guard privado y free antes de relanzar; de un buffer pendiente a cero |
| Fuga del AST ante fallo en interfaz de compilación | Se destruía sólo tras retorno normal | Destruir AST al salir protegido; de 13 reservas pendientes a cero; CI posterior sin informe de fuga en ese caso |
| FIFO sintética sin escritor queda esperando | fopen previo a validar tipo | stat previo y fstat posterior; rechazo sin esperar y sin modificar destino |

La ampliación de FIFO se probó antes de corregirla: **102/103** casos B4-A
aprobados y un timeout funcional; después **103/103**. Registros
[antes](evidencias/p12b4a/ampliacion-antes.log) y
[después](evidencias/p12b4a/ampliacion-despues.log). El bloqueo era del caso
funcional con una FIFO propia, no de infraestructura.

El fallo de compilación inicial también produjo un informe LSan de **26 bytes
en cuatro reservas residuales** tras la limpieza auxiliar del arnés. No es
la medida total de los 13 nodos/reservas observados antes de esa limpieza.
Se conserva el registro [CI inicial instrumentada](evidencias/p12b4a/ci-antes-asan.log).
La prueba inyecta el fallo al entrar al compilador: no acredita recuperación
de toda posible reserva interna del compilador/parser.

## Inventario y compatibilidad

El [inventario completo](P12B4A_INVENTARIO.md) documenta 14 nombres públicos,
sus firmas, resultados, límites, coste, propiedad y las dos interfaces C de
lectura/escritura atómica. La [referencia educativa](ARCHIVOS.md) incluye un
programa comprobado por cada nombre, también por alias.

* Se mantienen `leer`/`intentar_leer` y `eliminar`/`borrar` como alias exactos,
  todas las firmas C, los **285 símbolos públicos**, cabeceras y estructuras
  consumidas por SDK. Sin símbolos nuevos, funciones nuevas ni dependencias.
* La copia conserva el reemplazo de destinos regulares, pero lo hace con inode
  nuevo y permisos **0600**. No conserva metadatos; otros enlaces duros al destino
  viejo no cambian. Mismo inode, destino simbólico y fuente no regular se rechazan.
* Linux: un único motor temporal/rename/fsync, el existente en Core. Fuera de
  Linux la copia protegida falla sin mutar; es una limitación explícita de
  compatibilidad y no se presenta como soporte portable probado.
* `escribir`, `anexar` e `intentar_escribir` siguen siendo directas: pueden dejar
  datos parciales al fallar. `renombrar` conserva el reemplazo POSIX. No se promete
  una transacción donde el contrato histórico no la ofrece.
* `escribir`, `anexar`, `duplicar` y `ejecutar` **no tienen retorno utilizable**.
  La hipótesis inicial de ejemplos que esperaban nulo fue rechazada por pruebas;
  se corrigió la documentación, no la VM. Ocho pruebas negativas verifican la
  llamada anidada inválida y el estado de los archivos tras ella.
* `.lat` y `.mict` ejecutan los mismos **26 programas por extensión**, con salida,
  código, diagnóstico cuando corresponde y árbol final de archivos iguales.
  `ejecutar` cubre además ambos sufijos y llamadas anidadas en C.

No se cambian precedencia, ámbitos, gramática ni la conversión global de tipos.
Sólo `src/latfilelib.c` y las comprobaciones de E/S de su cargador en
`src/latast.c` cambian producción. La prueba histórica de escritura diferida
sustituye un dispositivo del sistema por inyección sobre su propio archivo
sintético, manteniendo las aserciones. `atomic_io_regression.c` se reutiliza.

## Resultados comprobados

CI de la revisión `257d80e…`:
[run 43651899432](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43651899432).
Se consultaron los logs de los tres jobs, incluidos SHA, árbol, herramientas y
resumen completo de CTest; no se infieren aprobaciones sólo del icono de la PR.

| Entorno/perfil | Aprobadas | Fallo funcional/sanitizador | Omitidas | Bloqueadas | Evidencia |
|---|---:|---:|---:|---:|---|
| Local Debian 13 Debug | 391/391 | 0 | 0 | 0 | [log](evidencias/p12b4a/local-debug.log) |
| Local Debian 13 Release | 391/391 | 0 | 0 | 0 | [log](evidencias/p12b4a/local-release.log) |
| Local ASan+UBSan+LSan | 5/407, sólo ABI/documentación | No se establece resultado de ejecución | 0 | 402 | [diagnóstico](evidencias/p12b4a/local-asan-resumen.txt), [log comprimido](evidencias/p12b4a/local-asan.log.gz) |
| CI Ubuntu 24.04 Debug | 391/391 | 0 | 0 | 0 | [job 132621998048](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43651899432/job/132621998048), [log](evidencias/p12b4a/ci-debug.log) |
| CI Ubuntu 24.04 Release | 391/391 | 0 | 0 | 0 | [job 132621998817](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43651899432/job/132621998817), [log](evidencias/p12b4a/ci-release.log) |
| CI ASan+UBSan+LSan | 407/407 | 0 | 0 | 0 | [job 132621997755](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43651899432/job/132621997755), [log](evidencias/p12b4a/ci-asan.log) |

La compilación/configuración final aprobó en todos esos perfiles. El XML local
confirma `does not work under ptrace` en **cada una de las 402 pruebas bloqueadas**;
CTest las presenta como fallidas, aquí se clasifican explícitamente por su causa.
No se desactivó ningún detector. Los cinco casos locales aprobados son los cuatro
verificadores de documentación B1/B2/B3/B4-A y la comparación ABI; no validan
la ejecución instrumentada de Core. La CI sí la ejecutó satisfactoriamente.

Se conservan las **251** pruebas previas Debug/Release y las **267** instrumentadas.
Comparación de nombres: [conservacion-suite.txt](evidencias/p12b4a/conservacion-suite.txt).
Las 140 incorporaciones son 32 casos C de contrato, 53 de inyección de fallos,
52 CLI (.lat/.mict), una regresión atómica histórica incorporada, una comprobación
de enlace y una comprobación de sincronía documental. Los oráculos son contratos
de contenido literal, tipos, estados y conservación de recursos, no salidas
copiadas de la implementación original.

## Entorno, enlace y recursos

Local: Debian 13, GCC 14.2.0, glibc 2.41, CMake 4.4.5, Python de shell 3.12.14;
CTest usa `/usr/bin/python3.13` **3.13.5**, seleccionado por FindPython3.
CI: Ubuntu 24.04, GCC 13.3.0, glibc 2.39, CMake 3.31.6, Python 3.12.3.
[Versiones, hashes de bibliotecas y ABI](evidencias/p12b4a/entorno-abi.txt).

Comandos locales efectivos, además de los de la entrega inicial:

```sh
cmake -S . -B ../build-p12b4a-release -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  '-DCMAKE_C_FLAGS=-I/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/include' \
  '-DCMAKE_EXE_LINKER_FLAGS=-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu' \
  '-DCMAKE_SHARED_LINKER_FLAGS=-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu'
cmake --build ../build-p12b4a-release --parallel 2
env -u LD_LIBRARY_PATH -u LD_PRELOAD \
  ctest --test-dir ../build-p12b4a-release --output-on-failure \
  --output-junit p12b4a-resultados.xml
```

Debug usa el mismo esquema con tipo Debug y su árbol separado. Instrumentación
usa Debug y añade `-fsanitize=address,undefined,float-cast-overflow
-fno-omit-frame-pointer` a C_FLAGS y a ambos grupos de flags de enlace locales,
con `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1`,
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`, `LSAN_OPTIONS=exitcode=23`.
Los prefijos de readline corresponden sólo a este entorno de trabajo; la
[guía Debian 13](P12B4A_DEBIAN13.md) usa paquetes normales sin esos prefijos.

Las pruebas B4-A limpian LD_LIBRARY_PATH/LD_PRELOAD/LD_AUDIT y equivalentes de
macOS únicamente en procesos hijos; seleccionan su build y verifican ruta real
mediante dladdr antes de iniciar la prueba. Se registra SHA-256 de la biblioteca.
Una biblioteca ajena sintética provoca el rechazo esperado, seguido de la
selección correcta. La invocación global de CI también limpia esas variables;
no se reescriben los comandos de las suites anteriores. El nombre de versión
heredado no se confunde con un identificador de revisión.

Una copia de **9 MiB + 17 bytes binarios** confirma ausencia del límite textual
de 8 MiB. Cincuenta copias consecutivas conservan la memoria administrada de la
VM: **81136 → 81136 bytes** después de calentar las cadenas, en este build local.
[Registro](evidencias/p12b4a/recursos-repetidos.txt). Esta cifra no mide toda la
memoria del proceso ni demuestra estabilidad de cualquier carga. Los fallos
inyectados comprueban flujos, fd, buffers crudos y temporales antes de limpiar.
La mejora de copia por bloques evita llamadas por byte; no se afirma un factor
de aceleración, pues no se realizó un benchmark comparativo de rendimiento.

## Limitaciones y pendientes

1. **Portabilidad:** escritura atómica/copia protegida sólo se validan en Linux.
   Hace falta adaptar y probar la durabilidad en otras plataformas.
2. **Concurrencia y durabilidad:** directorio estable, sistema local, sin otro
   escritor. No se promete protección ante sustituciones concurrentes de rutas,
   cortes de energía o fallos del almacenamiento que incumpla fsync. Si no se
   puede borrar un temporal tras fallar, puede permanecer `.latino-*`.
3. **Después de confirmar:** un fallo de sync/cierre del directorio no revierte
   rename; estado 2/advertencia lo distingue. La escritura directa puede ser
   parcial y renombrar puede reemplazar datos según su contrato heredado.
4. **VM y compilador:** se prueba recuperación de archivos/pila/contextos y la
   limpieza de temporales propiedad de la biblioteca. No se afirma cobertura
   exhaustiva de agotamiento de memoria dentro del parser/compilador ni se
   moderniza la política global de funciones nativas sin retorno.
5. **Texto:** límite 8 MiB por lectura/atómico textual, sin NUL; bytes UTF-8 se
   conservan pero no se validan ni normalizan. El límite INT_MAX−2 de programas
   se conserva por inspección; no se creó un fixture de varios GiB.
6. **Instrumentación local:** sigue bloqueada por ptrace. Se aporta la matriz CI
   completa sin modificar detectores; su aprobación no sustituye el registro
   del bloqueo local.

No hay pruebas obligatorias fallidas en la CI de la revisión auditada. Las
restricciones anteriores son contratos y límites explícitos, no aprobaciones
inventadas de plataformas, fallos o condiciones no ejecutadas.
