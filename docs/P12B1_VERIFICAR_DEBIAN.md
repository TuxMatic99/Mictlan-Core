# Reproducir P1.2-B1 en Debian

Comprobado en Debian 13 trixie, x86_64, GCC 14.2.0. Las compilaciones permanecen
en carpetas separadas. No es necesario instalar el ejecutable en el sistema.

## Preparación

```sh
sudo apt update
sudo apt install build-essential cmake git libreadline-dev locales python3
sudo localedef -i es_ES -f UTF-8 es_ES.UTF-8
sudo localedef -i fr_FR -f UTF-8 fr_FR.UTF-8

git clone --branch investigacion/p12b1-cadenas \
  https://github.com/TuxMatic99/Mictlan-Core.git Mictlan-Core-B1
cd Mictlan-Core-B1
git status --short --branch
git log -4 --oneline
git merge-base --is-ancestor c25eb7e5008e1201ebd1292da7b2da0dd0a554a0 HEAD
```

El último comando debe devolver 0: B1 conserva exactamente P1.2-A como
antecesora. Usa el SHA de la PR para una repetición inmóvil de la revisión.
Si ya tienes cambios en otra carpeta, utiliza esta copia nueva para probar.

Con CMake 4, añade `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` a cada configuración;
el subproyecto heredado de linenoise necesita esa opción de compatibilidad.

## Debug y Release

```sh
cmake -S . -B build-b1-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-b1-debug --parallel 2
ctest --test-dir build-b1-debug --output-on-failure
ctest --test-dir build-b1-debug -L p12b1 --output-on-failure

cmake -S . -B build-b1-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-b1-release --parallel 2
ctest --test-dir build-b1-release --output-on-failure

python3 tools/generar_doc_cadenas.py --check
./build-b1-release/mictlan tests/cadenas/herencia.lat
./build-b1-release/mictlan tests/cadenas/unicode.mict
```

En Linux son **128 pruebas** por perfil, incluidas las 67 anteriores.
Los 40 ejemplos públicos de la referencia se ejecutan como `.lat` y `.mict`.
No debe haber omisiones de los locales español y francés de P1.2-A.
Los stdout/stderr por programa quedan en `build-b1-debug/cadenas/`.

## Antes del arreglo

El commit `1ef03a5b310c95cdce0a2df7533d81d89fd2116d` conserva las primeras
pruebas sobre la implementación anterior. Los 20 casos de API nueva ausente
son expectativas futuras; no son defectos de compatibilidad de Latino.

```sh
git worktree add --detach ../Core-B1-antes 1ef03a5b310c95cdce0a2df7533d81d89fd2116d
cmake -S ../Core-B1-antes -B build-b1-antes -DCMAKE_BUILD_TYPE=Debug
cmake --build build-b1-antes --parallel 2
ctest --test-dir build-b1-antes -L p12b1 --output-on-failure
```

Se esperan 12/51 aprobadas y 39 fallidas: 20 por funciones aún inexistentes,
18 por límites heredados sin diagnóstico correcto y una por 100 buffers C
sin liberar. Un código de error de CTest es la evidencia, no una aprobación.
Las pruebas adicionales de concatenación y escapes se documentan en el informe.

## Instrumentación

```sh
cmake -S . -B build-b1-asan -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-b1-asan --parallel 2
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
LSAN_OPTIONS=exitcode=23 \
ctest --test-dir build-b1-asan --output-on-failure
```

La configuración instrumentada registra **144 pruebas**, porque añade 16
regresiones instrumentadas anteriores. Si LSan falla por ptrace, clasifica
las ejecuciones afectadas como `BLOCKED_INFRASTRUCTURE`; no las cuentes como
aprobadas. No desactives detectores ni sus códigos de error. El workflow de
Core repite toda la matriz en GitHub Actions y conserva JUnit y logs.

## Mediciones y compatibilidad binaria

```sh
python3 tests/cadenas/medir.py ./build-b1-release/mictlan_bench_cadenas > mediciones-b1.csv
nm -D --defined-only build-b1-release/liblatino.so | awk '{print $3}' | sort > simbolos-b1.txt
```

El benchmark usa un proceso nuevo por escenario, una llamada de calentamiento,
10.000 repeticiones de 20 bytes o 100 de 262.140 bytes y textos ASCII/UTF-8.
Mide tiempo monotónico, el contador `mv->memoria_usada` y pico RSS en Linux.
No exige un tiempo fijo para aprobar una prueba.

Para comparar con P1.2-A, construye su biblioteca Release en otro directorio y
compila el mismo consumidor contra sus cabeceras y `liblatino.so`. Puedes
repetirlo con la biblioteca nueva mediante `LD_LIBRARY_PATH`, sin recompilarlo:

```sh
git worktree add --detach ../Core-A c25eb7e5008e1201ebd1292da7b2da0dd0a554a0
cmake -S ../Core-A -B build-a -DCMAKE_BUILD_TYPE=Release
cmake --build build-a --target liblatino --parallel 2
cc tests/cadenas/benchmark.c -I../Core-A/include -Lbuild-a \
  -Wl,-rpath,"$PWD/build-a" -llatino -o consumidor-a
python3 tests/cadenas/medir.py ./consumidor-a > mediciones-a.csv
LD_LIBRARY_PATH="$PWD/build-b1-release" ./consumidor-a invertir 20 100 ascii
nm -D --defined-only build-a/liblatino.so | awk '{print $3}' | sort > simbolos-a.txt
diff -u simbolos-a.txt simbolos-b1.txt
```

Se conservan los **285 símbolos** observados. En la base, el benchmark omite con
código 77 los escenarios de funciones nuevas; esas omisiones no son aprobaciones
de funcionalidad. Los CSV guardados en la entrega incluyen tres rondas por
perfil. Para comparar tiempos usa sus medianas y considera el ruido del sistema.
