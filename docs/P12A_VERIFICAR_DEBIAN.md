# Verificar P1.2-A en Debian

Comprobado en Debian 13 (trixie), Linux x86_64, GCC 14.2.0. No se requiere
instalar Mictlán sobre una versión existente. Los directorios de compilación
son independientes. En Debian, CMake 3.25 o posterior es suficiente; con
CMake 4 añade `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` al configurar por la
compatibilidad del CMake heredado de `linenoise`.

## Preparar una copia de revisión

```sh
sudo apt update
sudo apt install build-essential cmake git libreadline-dev locales
sudo localedef -i es_ES -f UTF-8 es_ES.UTF-8
sudo localedef -i fr_FR -f UTF-8 fr_FR.UTF-8

git clone --branch investigacion/p12a-conformidad \
  https://github.com/TuxMatic99/Mictlan-Core.git Mictlan-Core-p12a
cd Mictlan-Core-p12a
git status --short --branch
git log -3 --oneline
git merge-base --is-ancestor 9f3a1965f2794c8921adf9cdfa3e0c192060c657 HEAD
```

El último comando devuelve 0 si la revisión inicial es antecesora de tu copia.
Para verificar una revisión exacta usa el SHA del PR, no solo el nombre móvil
de la rama. Si ya tienes trabajo local, conserva esa carpeta y usa esta copia
nueva; no se necesita cambiar ni limpiar tu rama actual.

## Compilar y ejecutar

```sh
cmake -S . -B build-p12a-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-p12a-debug --parallel 2
ctest --test-dir build-p12a-debug --output-on-failure
ctest --test-dir build-p12a-debug -L p12a --output-on-failure

./build-p12a-debug/mictlan tests/conformidad/numeros.mict
./build-p12a-debug/mictlan tests/conformidad/logica.lat
./build-p12a-debug/mictlan tests/conformidad/recursion.mict
```

La suite completa tiene **67 pruebas** en Debug y Release. El subconjunto
P1.2-A tiene **25**. Cada programa se ejecuta con las dos extensiones. Los
locales español y francés deben aparecer como ejecutados; si faltan, CTest
informa omisión por código 77 y no has verificado esos escenarios. Generarlos
con `localedef` permite ejecutarlos. No se necesita cambiar el idioma de tu sesión.

```sh
cmake -S . -B build-p12a-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-p12a-release --parallel 2
ctest --test-dir build-p12a-release --output-on-failure
```

## Comprobar los fallos anteriores al arreglo

El commit `2a2c3042d404d6796a02fafcd5acbf89bcabc86a` añade las pruebas y las decisiones sin aplicar las
correcciones. En una copia adicional de ese commit:

```sh
git worktree add --detach ../Mictlan-Core-p12a-antes 2a2c3042d404d6796a02fafcd5acbf89bcabc86a
cmake -S ../Mictlan-Core-p12a-antes -B build-p12a-antes -DCMAKE_BUILD_TYPE=Debug
cmake --build build-p12a-antes --parallel 2
ctest --test-dir build-p12a-antes -L p12a --output-on-failure
```

Con los locales instalados, deben fallar cuatro de las 25 pruebas: comparación
mixta en `.lat` y `.mict`, y lectura decimal por API C en español y francés.
Las 42 pruebas preexistentes siguen pasando. El retorno no nulo de CTest es
la evidencia del defecto; no lo conviertas en una aprobación.

## Instrumentación adicional

Este paso usa los detectores existentes sin modificar el recolector:

```sh
cmake -S . -B build-p12a-asan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-p12a-asan --parallel 2
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build-p12a-asan --output-on-failure
```

Aquí se registran **83 pruebas**, porque la configuración instrumentada añade
16 regresiones anteriores. Un informe de sanitizador invalida la ejecución.
Si LSan indica que no funciona bajo `ptrace`, registra
`BLOCKED_INFRASTRUCTURE` y repite en una terminal sin depurador o en el CI.
No desactives la detección ni uses `exitcode=0` para obtener una aprobación.

El workflow `.github/workflows/conformidad-p12a.yml` ejecuta Debug, Release
e instrumentación, genera los locales y conserva los resultados y las salidas
como artefactos de Actions. `ctest --output-junit resultados.xml` permite
obtener también resultados estructurados en una ejecución local.
