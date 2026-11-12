# P0.2-E — Recuperación de pila después de errores

Fecha de referencia: 11 de noviembre de 2026, México. Ejecuciones: 12 de noviembre de 2026 UTC.
Base Core: `0b4f8df58935e425640824406406d95c9dc3281f`, rama `investigacion/propiedad-listas-compartidas`.
Rama experimental: `investigacion/p02e-recuperacion-pila`.
SDK comprobado: `65c75c8ba1f79c9af91d8634b1edd03bf25f1747`.
El commit que contiene este informe identifica la implementación examinada; el informe de entrega añade su SHA y la PR.

La corrección recupera la pila de operandos de la llamada fallida sin vaciar valores anteriores y conserva la recuperación de contextos y `numejec` de P0.2-A/B/C. La regresión funcional pasa; la aprobación completa de memoria queda pendiente por el bloqueo de LSan. No se han corregido los defectos de propiedad de listas ni modificado el SDK.

## Causa demostrada

La pila es el espacio donde la VM coloca argumentos y resultados. `ptrpila` indica cuántos están activos y `tope` señala la próxima celda libre. La pila de operandos es distinta de `numejec`, que cuenta llamadas en ejecución, y de `ptrctx`, que cuenta contextos de variables.

Recorrido del error en esta base:

1. `latC_llamar_funcion` instala un manejador con `setjmp` y ejecuta `latMV_funcion_correr`.
2. El bytecode apila los cuatro argumentos y el descriptor de la función. `latMV_call_function` retira ese descriptor y valida la cantidad de argumentos. También puede completar argumentos ausentes o preparar argumentos variables.
3. La llamada normal incrementa `numejec`, puede crear un contexto y ajusta `ptrprevio`; después entra en la función nativa.
4. La función científica retira primero el último argumento. En `ciencia.sumar2(3, 4, 5, "texto")`, la comprobación numérica de `"texto"` provoca `latC_error` → `latD_lanzar` → `longjmp`.
5. El salto abandona el resto de la función nativa y su retorno normal: no se consumen los tres argumentos restantes. También se saltan los ajustes normales de la llamada.
6. P0.2-A/C ya recuperaban contextos y `numejec` en el manejador. Faltaba recuperar la pila y sus campos auxiliares.

Antes de cambiar Core, el nuevo ejecutable de regresión se compiló contra la biblioteca exacta de la base. Siete casos fallaron en la primera vuelta: `directa`, `anidada`, `prefijo`, `anidada_prefijo`, `prestados`, `reentrada` y `propagar`. Los casos directos dejaron profundidad 3 en lugar de 0; con dos valores anteriores, dejaron 5 en lugar de 2. Los casos con prefijo también demostraron la alteración de `ptrprevio`. Los controles `consumido` y `exito` ya pasaban, 200 vueltas cada uno.

## Cambio y contrato de memoria

Solo se añaden 17 líneas a `src/latdo.c`. El manejador guarda la profundidad inicial, `ptrprevio` y `prev_args` antes de `setjmp`. Son variables locales constantes que no cambian entre `setjmp` y `longjmp`. El campo `status` del manejador ya era `volatile` en la base.

Al producirse un error se conservan las recuperaciones existentes y se aplica:

```text
profundidad posterior = mínimo(profundidad al producirse el error,
                               profundidad al entrar al manejador)
```

Si sobra pila, se ajustan juntos `ptrpila` y `tope`. Se restauran `ptrprevio` y `prev_args`, utilizados por el ajuste de argumentos y retornos. Cada manejador anidado conserva su propio límite; también se preserva la cadena anterior de manejadores. Las llamadas correctas no pasan por esta recuperación y mantienen sus resultados.

**Esto es un descarte de operandos nuevos, no una transacción que deshaga todos los cambios de la función.** Si un consumidor C recibe argumentos colocados antes de `latC_llamar_funcion` y los retira, no se vuelven a activar celdas consumidas. Si esos argumentos previos no se consumen, el llamador conserva la responsabilidad de gestionarlos. La interfaz no recibe un número de argumentos con el que inferir otra frontera. Tampoco se restauran variables o contenidos de objetos modificados por la función.

Las celdas de pila son descriptores dentro de un arreglo, con referencias prestadas a los objetos. `latC_apilar` no transfiere propiedad ni incrementa referencias; `latC_desapilar` no destruye el contenido. Por ello la recuperación tampoco llama a `latO_destruir`, no libera celdas individuales, no duplica objetos ni reserva memoria. Destruir un descriptor de pila podría liberar una dirección interior del arreglo o invalidar una lista/diccionario todavía compartido. Los propietarios originales siguen siendo responsables de liberar sus objetos.

El cambio no resuelve la propiedad de temporales que ya carecieran de propietario ni el problema general de contenedores compartidos. El saldo `memoria_usada` estable de la regresión es una comprobación adicional, no una demostración de ausencia de fugas.

## Archivos de la propuesta

| Archivo | Motivo |
| --- | --- |
| `src/latdo.c` | Recuperación acotada y restauración de campos auxiliares, solo en errores. |
| `tests/stack_recovery_regression.c` | Una VM por caso, compilador y ejecución reales, funciones anidadas, FFI, prefijo prestado, manejadores reentrantes, propagación, argumentos consumidos y múltiples retornos. |
| `CMakeLists.txt` | Ejecutable y nueve pruebas CTest de 200 vueltas; fallan ante diagnósticos de sanitizadores. |
| Este informe | Causa, contrato, resultados, comandos y límites de la validación. |

No se modifican cabeceras, estructuras públicas, `src/latmv.c`, `src/latobj.c`, `src/latdic.c`, ramas existentes ni archivos del SDK. No se añaden excepciones `WILL_FAIL`, omisiones ni ajustes que conviertan fallos conocidos en aprobaciones.

## Resultados obtenidos personalmente

Entorno: Debian GNU/Linux 13, x86_64, GCC 14.2.0, CMake/CTest 3.31.10. Debug y Release (`-O3 -DNDEBUG`) compilados por separado. Las comprobaciones de la nueva regresión no usan `assert`, de modo que permanecen activas en Release.

| Comprobación | Resultado |
| --- | --- |
| Nueva regresión contra Core original | 7 fallos reproducidos; 2 controles aprobados. |
| Nueva regresión Debug | 9/9, 200 vueltas por caso. |
| Nueva regresión Release | 9/9, 200 vueltas por caso. |
| Resistencia Debug y Release | 9/9 en cada configuración, 1.000 vueltas por caso; 22.018 entradas protegidas por configuración. |
| CTest Core Debug completo | 10/11; `coverage` falla porque no existe `ejemplos/coverage.lat`. El registro defectuoso ya está en la base. |
| Core con ASan+UBSan y fugas activadas, 22 comandos enumerados por CTest | 2 fallos UAF conocidos; 1 fallo de configuración `coverage` (también encuentra el bloqueo LSan); 19 `BLOCKED_INFRASTRUCTURE`. |
| 12 regresiones modulares | Extensiones: aprobada con el ejecutable Debug; las otras 11: `BLOCKED_INFRASTRUCTURE` por LSan. |
| SDK exacto `65c75c8`, Debug | 32/32. |
| SDK exacto, ASan+UBSan+LSan | 32 bloqueadas al comprobar los diagnósticos completos; ninguna aprobada oficialmente. |
| Regresión independiente P0.3 del SDK, Debug | `control`, `directa`, `anidada`, `pila`: 4/4 con 1.000 vueltas. |
| Esa misma regresión con sanitizadores | 4 `BLOCKED_INFRASTRUCTURE`; terminan las vueltas funcionales antes del bloqueo. |
| Verificadores P0.3 conservados, sin editar | 65/65 casos simulados; clasificador de resultados: 21/21. No sustituyen pruebas reales de Core. |

En la prueba independiente del SDK, cada modo de errores completa 10.001 entradas protegidas incluyendo las definiciones. Los errores devuelven `LAT_ERRRUN == 2`; las llamadas válidas devuelven 0 y su resultado esperado. Tras consumir únicamente el retorno legítimo, `ptrpila`, `ptrctx` y `numejec` vuelven a 0. En la regresión de Core con prefijo, la profundidad final es **2**, con ambos objetos intactos. En la reentrada se recupera también el contexto y contador **no nulos** del manejador interior antes de retornar al exterior.

La nueva regresión instrumentada termina sus 200 vueltas de cada caso, incluida la destrucción de propietarios y VM, sin informes ASan/UBSan antes del bloqueo final de LSan. Eso no permite declarar aprobada la seguridad completa de memoria ni descartar fugas.

### Sanitizadores e infraestructura

Opciones estrictas utilizadas para los procesos de validación:

```sh
export ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:detect_stack_use_after_return=1:exitcode=86
export UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1:exitcode=86
export LSAN_OPTIONS=leak_check_at_exit=1:exitcode=86
```

Se comprobaron referencias reales a `__asan_*` y `__ubsan_*` en la biblioteca y el nuevo ejecutable. Controles independientes provocaron un desbordamiento de memoria detectado por ASan y un desbordamiento entero detectado por UBSan. El primer control combinado fue detenido antes por UBSan; se compiló además un control exclusivo de ASan para distinguir ambos detectores. Core y SDK mantuvieron ASan+UBSan y detección de fugas.

Tanto el control LSan limpio como el que introduce una fuga intencionada de 137 bytes finalizan con:

```text
LeakSanitizer has encountered a fatal error.
LeakSanitizer does not work under ptrace (strace, gdb, etc)
```

Se conserva `BLOCKED_INFRASTRUCTURE`, no PASS. No se utiliza `LSAN_OPTIONS=exitcode=0`, supresiones ni `detect_leaks=0` para validar.

Dos pruebas históricas de Core ya tienen `detect_leaks=0` en sus propiedades CTest. Para esta comprobación se enumeraron los comandos mediante `ctest --show-only=json-v1` y se ejecutaron con las opciones estrictas anteriores, conservando directorio y tiempo límite. No se cambiaron esas propiedades fuera del alcance de P0.2-E.

Los verificadores originales SDK #30/#31 ocultan la salida del proceso cuando el código es inesperado. Sus primeras ejecuciones solo informan código 86. Se conservaron esas trazas y se ejecutaron además directamente los programas y los verificadores estrictos P0.3: muestran el diagnóstico de tipo/rango esperado seguido del bloqueo LSan. La clasificación final es bloqueo, no una aprobación ni evidencia de una nueva fuga de Core.

## Compatibilidad binaria

Respecto de la biblioteca Debug de `0b4f8df`: 278 símbolos dinámicos definidos en ambas, ninguno añadido o retirado y ningún cambio de tipo de símbolo. Ninguna cabecera pública modificada. En x86_64, `sizeof(lat_objeto)=64`, `sizeof(lat_mv)=5688`; se mantienen los desplazamientos examinados (`ptrctx=2104`, `ptrpila=2108`, `numejec=2120`, `contexto_actual=2088`, `error=2168`, registros de funciones/módulos/constantes en 5664/5672/5680).

Se revisaron los 26 binarios ELF de la compilación SDK Debug con `ldd -r`: ninguna dependencia o símbolo sin resolver. Sus 24 importaciones distintas de funciones `lat*` están exportadas por Core; los nueve consumidores que enlazan Core cargan la biblioteca experimental correcta. La regresión VM independiente se compiló y ejecutó con esa misma biblioteca.

Esto acredita la compatibilidad comprobada en Debian x86_64; no equivale a haber probado Windows, macOS, otra arquitectura o todos los usos de extensiones externas.

## Reproducción

Desde la raíz de un checkout de esta rama, con compilador C, CMake y dependencias de desarrollo de readline disponibles:

```sh
cmake -S . -B ../build-core-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build ../build-core-debug --parallel 2
ctest --test-dir ../build-core-debug -R '^mictlan_pila_' --output-on-failure
../build-core-debug/mictlan_test_stack_recovery reentrada 1000
../build-core-debug/mictlan_test_stack_recovery prefijo 1000
ctest --test-dir ../build-core-debug --output-on-failure --output-junit core-debug.xml
```

La última orden conserva el fallo conocido de `coverage`. Repetir con otro directorio y `-DCMAKE_BUILD_TYPE=Release` comprueba optimizaciones. El límite de la nueva prueba es de 1 a 1.000 vueltas por proceso.

Para repetir la prueba nueva con todos los detectores (mantener las tres variables anteriores):

```sh
cmake -S . -B ../build-core-asan -DCMAKE_BUILD_TYPE=Debug \
  '-DCMAKE_C_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer'
cmake --build ../build-core-asan --parallel 2
ctest --test-dir ../build-core-asan -R '^mictlan_pila_' --output-on-failure
```

Un fallo de LSan por infraestructura debe quedar pendiente. Para ejecutar estrictamente los diagnósticos históricos que sobrescriben `ASAN_OPTIONS`, lanzar sus ejecutables directamente con las variables anteriores, sin utilizar su propiedad histórica que desactiva fugas.

Regresiones modulares, individualmente desde Core (cada script tiene su propia compilación):

```sh
bash tests/run_hash_regression.sh
bash tests/run_ffi_regression.sh
bash tests/run_context_regression.sh
bash tests/run_bytecode_regression.sh
bash tests/run_label_regression.sh
bash tests/run_modules_regression.sh
bash tests/run_constants_regression.sh
bash tests/run_strings_io_regression.sh
bash tests/run_atomic_io_regression.sh
bash tests/run_object_text_regression.sh
bash tests/run_system_regression.sh
bash tests/run_extension_regression.sh
```

SDK, desde el directorio padre de ambos repositorios, con raylib instalado y el SDK situado exactamente en `65c75c8` en una copia independiente:

```sh
cmake -S Mictlan-SDK -B build-sdk-debug -DCMAKE_BUILD_TYPE=Debug \
  -DLATINO_INCLUDE_DIR="$PWD/Mictlan-Core/include" \
  -DLATINO_LIBRARY="$PWD/build-core-debug/liblatino.so"
cmake --build build-sdk-debug --parallel 2
ctest --test-dir build-sdk-debug --output-on-failure --output-junit sdk-debug.xml
```

Para SDK instrumentado, utilizar otros directorios, la biblioteca instrumentada y agregar `-fsanitize=address,undefined -fno-omit-frame-pointer` tanto en `CMAKE_C_FLAGS` como en `CMAKE_CXX_FLAGS`. El paquete de evidencias conserva las órdenes exactas de este entorno, rutas de dependencias, inventarios CTest, salidas, resultados revisados, JUnit y hashes.

## Riesgos y coordinación P0.2-E → P0.2/P0.3

| Situación | Gravedad / estado | Acción siguiente |
| --- | --- | --- |
| `mictlan_memoria_propiedad_anidada_asan` y `mictlan_memoria_elemento_repetido_asan` | Alta: heap-use-after-free real, reproducido en la base y en esta rama, en `latO_destruir`. | Continuar el frente separado de propiedad de contenedores; no integrar aquí su solución. |
| Fugas previamente comunicadas en listas, suma vectorial y alias FFI | Pendientes: las cantidades de informes anteriores no se vuelven a certificar. | Repetir LSan en un proceso sin trazado; revisar especialmente las rutas de error y destrucción. |
| LSan bloqueado por `ptrace` | Bloqueo de validación de memoria. | Ejecutar esta rama en Debian sin depurador/trazador antes de aprobarla para integración. |
| `coverage` apunta a un archivo ausente | Media, configuración preexistente. | Reparar en una tarea separada, manteniendo visible el fallo actual. |
| Verificadores antiguos #30/#31 ocultan stderr con código inesperado | Media, observabilidad preexistente. | Revisar e integrar por separado los verificadores P0.3 ya preparados. |
| Ruta de constructores | Riesgo por revisar: usa el estado de una llamada protegida como `prev_args`; su propagación de errores no se modifica ni se certifica aquí. | Preparar una regresión específica en otro frente; no presentar la inspección como fallo reproducido. |

La propuesta recupera el fallo de los tres argumentos y preserva P0.2-A/B/C en las comprobaciones reales indicadas. La rama `investigacion/propiedad-contenedores-p02d` apareció en remoto durante la ejecución; no se incorporó ni modificó. La base de destino permanecía en `0b4f8df` al comprobarla.

Siguiente paso recomendado: revisión de la PR en borrador, repetición estricta de memoria fuera de `ptrace` y coordinación con P0.2-D. No fusionar automáticamente ni sustituir fallos conocidos por resultados esperados invertidos.
