# Repetir P1.2-B3 en Debian 13

Trabaje en una copia nueva; los comandos detienen el proceso si falla un paso.
No hace falta instalar Core en el sistema ni modificar main. La revisión se fija
al resolver la PR y se registra antes de compilar; para una revisión independiente
inmutable use el SHA completo publicado en el informe de la PR #10.

## Preparación

```bash
set -euo pipefail
sudo apt update
sudo apt install -y build-essential git cmake libreadline-dev python3 locales
sudo localedef -i es_ES -f UTF-8 es_ES.UTF-8
sudo localedef -i fr_FR -f UTF-8 fr_FR.UTF-8

git clone https://github.com/TuxMatic99/Mictlan-Core.git mictlan-core-p12b3
cd mictlan-core-p12b3
git fetch origin investigacion/p12b3-matematicas
revision="$(git rev-parse FETCH_HEAD)"
git switch --detach "$revision"
test "$(git rev-parse HEAD)" = "$revision"
git merge-base --is-ancestor 3125a0016e1adcfa0ac68c2c37ba40108a033cc9 HEAD
{
    git rev-parse HEAD
    uname -a
    cc --version
    cmake --version
    python3 --version
    ldd --version
} > entorno-p12b3.txt
python3 - <<'PY'
import locale
for nombre in ('C', 'es_ES.UTF-8', 'fr_FR.UTF-8'):
    locale.setlocale(locale.LC_NUMERIC, nombre)
    print(nombre, locale.localeconv()['decimal_point'])
PY
```

## Debug y Release completos

```bash
set -euo pipefail
for perfil in Debug Release; do
    carpeta="build-b3-${perfil}"
    cmake -S . -B "$carpeta" -DCMAKE_BUILD_TYPE="$perfil" \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 2>&1 | tee "config-${perfil}.log"
    cmake --build "$carpeta" --parallel 2 2>&1 | tee "build-${perfil}.log"
    ctest --test-dir "$carpeta" --output-on-failure --output-junit resultados.xml \
        2>&1 | tee "test-${perfil}.log"
done
```

Esperado con los ejemplos finales: **251/251 por perfil**, sin omitidas.
Incluye las 180 anteriores y 71 nuevas. La opción de política permite usar
CMake 4 con el CMake histórico de linenoise; CMake anterior puede no necesitarla.

## ASan, UBSan, conversiones float y LSan

```bash
set -euo pipefail
cmake -S . -B build-b3-asan -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    '-DCMAKE_C_FLAGS=-fsanitize=address,undefined,float-cast-overflow -fno-omit-frame-pointer' \
    '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined,float-cast-overflow' \
    '-DCMAKE_SHARED_LINKER_FLAGS=-fsanitize=address,undefined,float-cast-overflow' \
    2>&1 | tee config-asan.log
cmake --build build-b3-asan --parallel 2 2>&1 | tee build-asan.log
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build-b3-asan --output-on-failure --output-junit resultados.xml \
    2>&1 | tee test-asan.log
```

Esperado: **267/267**, las 251 normales y 16 regresiones instrumentadas previas.
`float-cast-overflow` añade detección explícita de conversiones como infinito a
int, que no todos los compiladores incluyen dentro de undefined. No desactive
LSan ni cambie su salida a cero. Si informa «does not work under ptrace», registre
bloqueo de infraestructura y repita fuera de un trazador/contenedor restrictivo.
Una prueba omitida por locale ausente tampoco cuenta como aprobada.

## Casos B3, ejemplos y ABI

```bash
set -euo pipefail
ctest --test-dir build-b3-Debug -L p12b3 --output-on-failure
python3 tools/generar_doc_matematicas.py --check
./build-b3-Debug/mictlan tests/matematicas/herencia.lat
./build-b3-Debug/mictlan tests/matematicas/documentacion.mict
python3 tests/colecciones/abi.py "$(command -v nm)" build-b3-Release/liblatino.so
git diff --exit-code 3125a0016e1adcfa0ac68c2c37ba40108a033cc9 -- include/
```

El verificador de B2 conserva su nombre y compara los 285 símbolos completos;
se reutiliza sin alterar la referencia. Los casos CLI generan pares .lat/.mict
con el mismo `.out` y guardan stdout/stderr bajo build-b3-Debug/matematicas.
El generador documental no obtiene oráculos del intérprete: verifica que los
archivos de referencia y los ejemplos coincidan con el catálogo revisado.

## Medir antes y después

```bash
set -euo pipefail
python3 tests/matematicas/medir.py ./build-b3-Release/mictlan_bench_matematicas \
    > benchmark-despues.csv
git worktree add --detach ../mictlan-core-b3-base 3125a0016e1adcfa0ac68c2c37ba40108a033cc9
cmake -S ../mictlan-core-b3-base -B build-b3-base-Release \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build-b3-base-Release --target liblatino --parallel 2
cc -O3 -I ../mictlan-core-b3-base/include tests/matematicas/benchmark.c \
    -L build-b3-base-Release -llatino \
    -Wl,-rpath,"$PWD/build-b3-base-Release" -o benchmark-base
python3 tests/matematicas/medir.py ./benchmark-base > benchmark-antes.csv
```

Cada CSV contiene nueve filas (tres procesos por tamaño), controles numéricos,
tiempo, memoria administrada y RSS. Compare medianas en el mismo equipo/perfil;
no use un umbral de tiempo como prueba funcional. No es necesario eliminar el
worktree de referencia. Conserve SHA, comandos, versiones, logs/XML y clasifique
por separado aprobado, fallo funcional, compilación/configuración, sanitizador,
omitido y bloqueado. Un comando mostrado aquí no implica que se haya ejecutado
en su Debian: su reproducción proporciona esa comprobación independiente.

Para ver la evidencia anterior, el commit
`1c9329b33f660490ceb83064e4d70097d0745c22` contiene las primeras pruebas sin
correcciones; sus fallos son esperados y se explican en el informe. Puede revisarlo
en otro worktree separado, sin modificar la rama B3 actual.
