# Revisión independiente de P1.2-B4-A en Debian 13

Código y batería fijados en `257d80e64e38e82b1396755070ea8b6fb756a654`.
Base exacta: `b0a2d860363e07274feae1d0cf2b421515386d73`.
La [PR #11](https://github.com/TuxMatic99/Mictlan-Core/pull/11) permanece en
borrador contra `investigacion/p12b3-matematicas`. Los commits posteriores de
informe no cambian ese código. No es necesario instalar Core ni cambiar otra rama.

## Preparar herramientas y copia aislada

```sh
sudo apt update
sudo apt install build-essential cmake git libreadline-dev python3 locales binutils
sudo localedef -i es_ES -f UTF-8 es_ES.UTF-8
sudo localedef -i fr_FR -f UTF-8 fr_FR.UTF-8

revision=257d80e64e38e82b1396755070ea8b6fb756a654
trabajo=$(mktemp -d -t mictlan-b4a-XXXXXX)
git clone --branch investigacion/p12b4a-integridad-archivos \
  https://github.com/TuxMatic99/Mictlan-Core.git "$trabajo/core"
cd "$trabajo/core"
git switch --detach "$revision"
test "$(git rev-parse HEAD)" = "$revision"
git merge-base --is-ancestor b0a2d860363e07274feae1d0cf2b421515386d73 HEAD
git status --short
cc --version
cmake --version
python3 --version
ldd --version
```

No reutilizar builds anteriores. No exportar una ruta global de bibliotecas.
Los datos de cada prueba B4-A se crean en otro directorio temporal desechable,
se comprueba su contenido y se eliminan automáticamente. Los registros y builds
permanecen en `$trabajo` para revisarlos; no se usan documentos personales.

## Debug y Release

```sh
set -eu
for perfil in Debug Release; do
  build="build-$perfil"
  env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
    cmake -S . -B "$build" -DCMAKE_BUILD_TYPE="$perfil"
  cmake --build "$build" --parallel 2
  env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
    ctest --test-dir "$build" --output-on-failure \
    --output-junit resultados.xml
  python3 tests/colecciones/abi.py nm "$build/liblatino.so"
done
```

Resultado esperado en esta revisión: **391/391** en cada perfil y **285 símbolos
C públicos**, sin nuevos símbolos de Core. CMake 4.x necesita además
`-DCMAKE_POLICY_VERSION_MINIMUM=3.5`; el CMake de Debian 13 no exige ese ajuste.
Si faltan los locales de coma decimal, clasificar esas pruebas como configuración
incompleta y corregir la instalación; no presentarlas como aprobadas.

## ASan, UBSan y LSan

```sh
env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
  cmake -S . -B build-Sanitizadores -DCMAKE_BUILD_TYPE=Debug \
  '-DCMAKE_C_FLAGS=-fsanitize=address,undefined,float-cast-overflow -fno-omit-frame-pointer' \
  '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined,float-cast-overflow' \
  '-DCMAKE_SHARED_LINKER_FLAGS=-fsanitize=address,undefined,float-cast-overflow'
cmake --build build-Sanitizadores --parallel 2
env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
  ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
  UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  LSAN_OPTIONS=exitcode=23 \
  ctest --test-dir build-Sanitizadores --output-on-failure \
  --output-junit resultados.xml
python3 tests/colecciones/abi.py nm build-Sanitizadores/liblatino.so
```

Resultado esperado en un entorno compatible: **407/407**. Los 16 casos adicionales
son regresiones instrumentadas previas que se conservan. El verificador ABI
separa únicamente los guardias `__odr_asan.*` añadidos por GCC; no omite símbolos
del proyecto. `detect_leaks=0` o `LSAN_OPTIONS=exitcode=0` no son validaciones válidas.

Si aparece `LeakSanitizer has encountered a fatal error` junto con el diagnóstico
`does not work under ptrace`, registrar el bloqueo y repetir en un entorno sin
esa restricción. No es aprobación ni prueba de fuga de Core. Un informe real de
ASan/UBSan/LSan sí es un error de sanitizador, aunque el resultado del programa
coincida con el esperado. La matriz de GitHub Actions conserva los registros
sin desactivar detectores.

## Sólo archivos, biblioteca efectiva y ejemplos

```sh
env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
  ctest --test-dir build-Debug -L p12b4a --output-on-failure
# Debe ejecutar 140 pruebas.

env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
  ctest --test-dir build-Debug -R mictlan_p12b4a_enlace_aislado -V
# Detecta una liblatino.so ajena sintética, y luego comprueba la del build.

python3 tests/archivos/ejecutar.py \
  --biblioteca="$PWD/build-Debug/liblatino.so" \
  --sonda="$PWD/build-Debug/mictlan_sonda_archivos" \
  --cli="$PWD/build-Debug/mictlan" --extension=mict --caso=doc_duplicar
# Repetir con --extension=lat para el mismo contrato.

python3 tests/archivos/generar.py --comprobar
```

El envoltorio elimina variables de enlace/inyección heredadas **sólo para sus
procesos hijos**, fija la biblioteca del build y comprueba con `dladdr` su ruta
real. Imprime el SHA-256 de ese archivo. La versión textual `Latino 1.5.0` es
histórica y no distingue revisiones; correlacionar ruta/hash con `git rev-parse
HEAD` y el registro de compilación. El ejecutable del arnés C se niega a probar
archivos si no se inició dentro del directorio temporal del envoltorio.

No ejecutar los ejemplos de archivos desde una carpeta con datos reales. El
comando anterior los coloca en un directorio temporal, comprueba también el
estado final de todos sus archivos y lo elimina al terminar.

## Repetir la evidencia anterior a las correcciones

Usar otra copia aislada, fijada en el primer commit de B4-A:
`78032e14cd290d28b88b89c991dcccfafda5fbfb`. Compilar Debug y ejecutar CTest como
arriba. Se esperan **294 aprobadas y 37 fallidas de 331**, con las **251 previas
aprobadas**. Allí las pruebas CLI aún se generan directamente desde `casos.json`.
Es un fallo esperado de diagnóstico, no una versión que deba usarse para datos.
Los casos sólo modifican fixtures sintéticos que la batería crea.

La copia de un origen ausente, por ejemplo, compara el destino contra `ANTERIOR`;
la base lo deja vacío y la corrección lo conserva. El registro histórico incluye
los recursos abiertos, buffers pendientes y errores exactos. No es necesario
crear manualmente archivos para provocar pérdida de datos.

## Guardar y clasificar resultados

Conservar SHA, tipo de build, versiones, flags, salida de CMake/build, el XML y
`build-*/Testing/Temporary/LastTest.log`. Clasificar por separado:

| Resultado | Criterio |
|---|---|
| Aprobado | Proceso correcto, salida/estado de disco correctos, sin informe de sanitizador |
| Error funcional | Aserción, contenido, recuperación o timeout de la operación incumplidos |
| Compilación/configuración | No se obtuvo ejecutable o falta una condición del entorno |
| Error de sanitizador | Informe real de ASan, UBSan o LSan |
| Bloqueado por infraestructura | El detector no puede funcionar, por ejemplo ptrace |
| Omitido | No se ejecutó; nunca sumar como aprobado |

El inventario exacto de pruebas previas se compara en
`docs/evidencias/p12b4a/conservacion-suite.txt`. El [informe](P12B4A_INFORME.md)
separa resultados locales y CI, revisiones y limitaciones.
