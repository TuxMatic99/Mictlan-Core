# Verificación independiente de P1.2-B2 en Debian 13

Guía para ejecutar por separado la revisión funcional
`c4aa656abc8302f893fe74e913867e2852552317`. Los recuentos indicados proceden de CI
ya existente; estos comandos no se ejecutaron durante el cierre documental.

## Preparar una copia y fijar el SHA

```bash
set -euo pipefail
sudo apt update
sudo apt install -y build-essential git cmake libreadline-dev python3 locales
sudo localedef -i es_ES -f UTF-8 es_ES.UTF-8
sudo localedef -i fr_FR -f UTF-8 fr_FR.UTF-8

git clone https://github.com/TuxMatic99/Mictlan-Core.git mictlan-core-p12b2
cd mictlan-core-p12b2
git fetch origin investigacion/p12b2-colecciones
revision=c4aa656abc8302f893fe74e913867e2852552317
git switch --detach "$revision"
test "$(git rev-parse HEAD)" = "$revision"
git merge-base --is-ancestor 6b8204ab92196fccfc217c80c6f381aa8ef03535 HEAD
git rev-parse HEAD
cc --version
cmake --version
python3 --version
```

Se usa una copia nueva y HEAD separado; no se modifica main ni se crea otra rama.
Los locales permiten comprobar las regresiones anteriores de coma decimal.

## Debug y Release

```bash
set -euo pipefail
for perfil in Debug Release; do
    carpeta="build-b2-${perfil}"
    cmake -S . -B "$carpeta" -DCMAKE_BUILD_TYPE="$perfil" \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5
    cmake --build "$carpeta" --parallel 2
    ctest --test-dir "$carpeta" --output-on-failure --output-junit resultados.xml
done
```

Referencia de CI: **180/180 por perfil**, incluidas las 128 pruebas anteriores.
La opción de política facilita CMake 4 con el CMake heredado de linenoise; en
versiones anteriores puede ser innecesaria. Los XML y logs quedan en cada build.

## ASan, UBSan y LSan

```bash
set -euo pipefail
cmake -S . -B build-b2-asan -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    '-DCMAKE_C_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer' \
    '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined' \
    '-DCMAKE_SHARED_LINKER_FLAGS=-fsanitize=address,undefined'
cmake --build build-b2-asan --parallel 2
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build-b2-asan --output-on-failure --output-junit resultados.xml
```

Referencia de CI: **196/196**, las 180 normales más 16 regresiones instrumentadas
anteriores. Si aparece «LeakSanitizer does not work under ptrace», clasifica las
pruebas afectadas como bloqueadas y repítelas fuera de ese entorno. Mantén activos
los detectores; una prueba bloqueada, omitida o no ejecutada no cuenta como aprobada.

## Revisar colecciones, ejemplos y ABI

```bash
ctest --test-dir build-b2-Debug -L p12b2 --output-on-failure
python3 tools/generar_doc_colecciones.py --check
python3 tests/colecciones/abi.py "$(command -v nm)" build-b2-Release/liblatino.so
./build-b2-Debug/mictlan tests/colecciones/documentacion.mict
./build-b2-Debug/mictlan tests/colecciones/listas_herencia.lat
```

B2 contiene **52 pruebas**. Sus casos CLI se ejecutan en pares .lat/.mict con los
mismos .out; los resultados individuales se guardan en build-b2-Debug/colecciones.
La ABI normal debe conservar 285 símbolos de Core. Los ejemplos no sustituyen la
suite completa. Registra SHA, versiones, comandos, códigos de salida y pruebas
aprobadas, fallidas, omitidas o bloqueadas. Distingue errores funcionales, errores
de compilación/configuración e informes de sanitizadores.

El [informe técnico](P12B2_INFORME.md) enlaza las ejecuciones de CI preexistentes,
las correcciones y los límites de compatibilidad que sirven como referencia.
