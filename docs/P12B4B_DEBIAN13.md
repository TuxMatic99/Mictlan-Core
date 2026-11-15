# Verificación independiente en Debian 13 — P1.2-B4-B

Código y batería completos: `c60b09883437f55a8ab0bb56bce6b846c4b0d92e`.
Base exacta: `82be31bf11b6fcfc3bb19979c2b62ca337edc2fd`.
[PR #12 en borrador](https://github.com/TuxMatic99/Mictlan-Core/pull/12), contra
`investigacion/p12b4a-integridad-archivos`. Los commits posteriores de informe
no cambian ese código ni esa batería.

Se necesitan las herramientas de B4-A: build-essential, cmake, git,
libreadline-dev, python3, locales y binutils. No hace falta instalar Mictlán en
el sistema. Reutiliza esas herramientas, pero crea árboles de compilación nuevos.
Los siguientes comandos no cambian ramas de otra copia ni configuran locales del
sistema. Los datos de las pruebas son sintéticos y temporales.

## Preparar una copia y comprobar la revisión

```sh
set -eu
revision=c60b09883437f55a8ab0bb56bce6b846c4b0d92e
trabajo=$(mktemp -d -t mictlan-b4b-XXXXXX)
git clone --branch investigacion/p12b4b-entrada-modulos \
  https://github.com/TuxMatic99/Mictlan-Core.git "$trabajo/core"
cd "$trabajo/core"
git switch --detach "$revision"
test "$(git rev-parse HEAD)" = "$revision"
git merge-base --is-ancestor 82be31bf11b6fcfc3bb19979c2b62ca337edc2fd HEAD
git status --short
cc --version
cmake --version
python3 --version
ldd --version
```

Verificar los locales requeridos en un proceso de prueba:

```sh
python3 - <<'PY'
import locale
for nombre in ('C', 'es_ES.UTF-8', 'fr_FR.UTF-8'):
    locale.setlocale(locale.LC_NUMERIC, nombre)
    print(nombre, locale.localeconv()['decimal_point'])
PY
```

Si faltan es_ES/fr_FR, y está instalado el paquete `locales`, se pueden crear
únicamente dentro de este directorio temporal, sin permisos de administrador:

```sh
mkdir -p "$trabajo/locales"
localedef --no-archive -i es_ES -f UTF-8 "$trabajo/locales/es_ES.UTF-8"
localedef --no-archive -i fr_FR -f UTF-8 "$trabajo/locales/fr_FR.UTF-8"
export LOCPATH="$trabajo/locales"
```

Repetir entonces la comprobación Python. LOCPATH afecta sólo esta sesión y sus
procesos hijos. No se modifica el locale global del sistema. Si falla la creación,
registrar un problema de configuración; no contabilizar las pruebas como aprobadas.

## Debug y Release

```sh
for perfil in Debug Release; do
  build="build-$perfil"
  env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
    cmake -S . -B "$build" -DCMAKE_BUILD_TYPE="$perfil"
  cmake --build "$build" --parallel 2
  env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
    ctest --test-dir "$build" --output-on-failure --output-junit resultados.xml
  python3 tests/colecciones/abi.py nm "$build/liblatino.so"
done
```

Esperado: **676/676 por perfil**, incluidos los 391 casos anteriores y 285 nuevos;
**285 símbolos C** sin pérdidas ni incorporaciones. CMake 4.x necesita añadir
`-DCMAKE_POLICY_VERSION_MINIMUM=3.5` a la configuración; el CMake de Debian 13
no necesita ese ajuste. Los builds conservan XML y `Testing/Temporary/LastTest.log`.

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
  ctest --test-dir build-Sanitizadores --output-on-failure --output-junit resultados.xml
python3 tests/colecciones/abi.py nm build-Sanitizadores/liblatino.so
```

Esperado en infraestructura compatible: **692/692** (407 anteriores + 285 nuevos).
Un mensaje `does not work under ptrace` junto con el error fatal de LSan se registra
como bloqueo de infraestructura, no como aprobación ni como fuga demostrada de
Core. Un informe real de ASan/UBSan/LSan sí es un fallo aunque coincida la salida.
No usar detect_leaks=0 ni exitcode=0 como sustituto de una ejecución válida.

## Sólo B4-B, enlace y ejemplos

```sh
env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
  ctest --test-dir build-Debug -L p12b4b --output-on-failure
# Esperado: 285 pruebas.

env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
  ctest --test-dir build-Debug -R mictlan_p12b4a_enlace_aislado -V
# Conserva y reutiliza la comprobación de biblioteca ajena de B4-A.

python3 tests/entrada_modulos/generar.py --comprobar
# 13 pares .lat/.mict y su referencia deben coincidir.

python3 tests/archivos/ejecutar.py \
  --biblioteca="$PWD/build-Debug/liblatino.so" \
  --sonda="$PWD/build-Debug/mictlan_sonda_archivos" \
  python3 "$PWD/tests/entrada_modulos/cli.py" \
  "$PWD/build-Debug/mictlan" texto mict C
```

El último comando crea su propio directorio temporal, suministra texto conocido,
comprueba salida y disco, y elimina sus fixtures. La sonda registra ruta efectiva,
versión y SHA-256 de liblatino.so. No exportar LD_LIBRARY_PATH globalmente.
El nombre histórico «Latino 1.5.0» no identifica una revisión de Mictlán: usar
además SHA del checkout y hash/ruta de la biblioteca.

Para reproducir el **antes**, crear otra copia/worktree en el commit de pruebas
`1ffcf2f8d7dbcfd246a3d374c1ce737791238d74` y otro build Debug. Allí se esperan
471/669 aprobadas y 198 fallidas: las 391 previas aprueban; las nuevas incluyen
funciones todavía inexistentes. No comparar esos 669 con los 676 finales sin
considerar los siete casos añadidos al cierre y los ajustes de arnés documentados.
