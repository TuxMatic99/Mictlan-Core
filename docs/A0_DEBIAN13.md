# A0 — repetir la validación en Debian 13

Base exacta B4-B: `2651317e4224f5499765b5b0ae16d1404a0adfc4`.
Rama `investigacion/a0-inventario-sistema`, [PR #13](https://github.com/TuxMatic99/Mictlan-Core/pull/13)
en borrador contra `investigacion/p12b4b-entrada-modulos`.
No instalar Core globalmente ni cambiar una copia de trabajo existente.

Se requieren herramientas ya disponibles: git, build-essential, cmake, python3,
libreadline-dev, binutils y locales. Esta guía no ejecuta administración del sistema.
Si faltan, registrar fallo de configuración; no contar pruebas no ejecutadas como
aprobadas. La compilación local de A0 se ejecutó con Debian 13, GCC 14.2.0,
glibc 2.41, Python 3.12.14 y CMake 4.4.5; CI usa Ubuntu 24.04.

## Copia aislada y revisión

El informe final enlaza la revisión probada. En esta fase, se puede capturar la
punta actual de la PR y registrar su SHA antes de compilar; nunca suponer que
el nombre de rama permanece fijo para siempre:

```sh
set -eu
trabajo=$(mktemp -d -t mictlan-a0-XXXXXX)
git clone --branch investigacion/a0-inventario-sistema \
  https://github.com/TuxMatic99/Mictlan-Core.git "$trabajo/core"
cd "$trabajo/core"
revision=$(git rev-parse HEAD)
printf '%s\n' "$revision"
git switch --detach "$revision"
test "$(git rev-parse HEAD)" = "$revision"
git merge-base --is-ancestor 2651317e4224f5499765b5b0ae16d1404a0adfc4 HEAD
test "$(git rev-parse 99fde0f8c29b7b5e84fa2d3f362803a091d369c9^)" = \
  2651317e4224f5499765b5b0ae16d1404a0adfc4
git status --short
cc --version
cmake --version
python3 --version
ldd --version
```

## Locales de las regresiones anteriores

```sh
python3 - <<'PY'
import locale
for nombre in ('C', 'es_ES.UTF-8', 'fr_FR.UTF-8'):
    locale.setlocale(locale.LC_NUMERIC, nombre)
    print(nombre, locale.localeconv()['decimal_point'])
PY
```

Si faltan y están disponibles las fuentes de locales, crearlos sólo en el temporal:

```sh
mkdir -p "$trabajo/locales"
localedef --no-archive -i es_ES -f UTF-8 "$trabajo/locales/es_ES.UTF-8"
localedef --no-archive -i fr_FR -f UTF-8 "$trabajo/locales/fr_FR.UTF-8"
export LOCPATH="$trabajo/locales"
```

Repetir la comprobación Python. No se modifica la configuración global del equipo.
A0 fija TZ=UTC0 y LC_ALL=C en sus procesos de prueba; las regresiones B4-B conservan
los otros locales. La biblioteca no cambia permanentemente zona ni locale.

## Debug y Release

```sh
for perfil in Debug Release; do
  build="$trabajo/build-$perfil"
  env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
    cmake -S . -B "$build" -DCMAKE_BUILD_TYPE="$perfil" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
  cmake --build "$build" --parallel 2
  env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
    ctest --test-dir "$build" --output-on-failure --output-junit resultados.xml
  python3 tests/colecciones/abi.py nm "$build/liblatino.so"
done
python3 tools/auditoria/inventario.py --comprobar
python3 tests/sistema/documentacion.py
```

Resultado final esperado: **846/846 por perfil**, 676 anteriores y 170 A0;
**285 símbolos C** sin pérdidas ni añadidos. La variable de política sirve para
CMake 4.x; en la versión de Debian 13 no es necesaria. No copiar los prefijos
readline del entorno de trabajo de ChatGPT: aquí se usan cabeceras y biblioteca
instaladas normalmente en Debian.

## ASan, UBSan y LSan

```sh
build="$trabajo/build-Sanitizadores"
env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
  cmake -S . -B "$build" -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  '-DCMAKE_C_FLAGS=-fsanitize=address,undefined,float-cast-overflow -fno-omit-frame-pointer' \
  '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined,float-cast-overflow' \
  '-DCMAKE_SHARED_LINKER_FLAGS=-fsanitize=address,undefined,float-cast-overflow'
cmake --build "$build" --parallel 2
env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
  ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 \
  UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  LSAN_OPTIONS=exitcode=23 \
  ctest --test-dir "$build" --output-on-failure --output-junit resultados.xml
python3 tests/colecciones/abi.py nm "$build/liblatino.so"
```

Esperado en infraestructura compatible: **862/862**, 692 anteriores y 170 A0.
El error fatal de LSan con `does not work under ptrace` es infraestructura;
conservarlo y repetir en CI o fuera del trazador. No desactivar detección de fugas
ni cambiar exitcode a cero. Un informe real de ASan/UBSan/LSan sí es fallo del
código, aunque la salida funcional esperada también aparezca. Una suite bloqueada
no demuestra ausencia de fugas.

## Casos dirigidos, aislamiento y comparación antes/después

```sh
ctest --test-dir "$trabajo/build-Debug" -L a0 --output-on-failure
ctest --test-dir "$trabajo/build-Debug" -R 'mictlan_a0_(pipe|fecha|tiempo)' \
  --verbose
```

El envoltorio B4-A crea temporales, limpia variables de carga heredadas, fija la
biblioteca del árbol y usa su sonda. La salida detallada registra ruta efectiva y
SHA256 de liblatino.so. Se conserva además la prueba B4-A que rechaza una biblioteca
ajena sintética. El contrato C de sis se enlaza al archivo estático del mismo árbol.
No se construyó un sistema de aislamiento paralelo.

Para reproducir los defectos, crear **otra copia temporal** en el commit
`99fde0f8c29b7b5e84fa2d3f362803a091d369c9`, y compilar con los mismos pasos. Ese
commit tiene inventario/pruebas, pero producción exactamente B4-B. Esperado:
842 casos, 56 fallos en Debug y Release; 858 instrumentados, 56 fallos en CI.
Los cuatro controles de cierre posteriores no existían allí. El siguiente commit,
`755f88c15f5a600c0d719ab2bd22e07651efd00a`, aprueba 842/842 y 858/858 en CI.
**No invertir esos fallos ni convertirlos en aprobaciones esperadas**: sirven como
evidencia antes del parche. Los logs originales están en docs/evidencias/a0.

Conservar SHA, herramientas, comandos, código de salida, XML y
`Testing/Temporary/LastTest.log`. Clasificar por separado aprobado, fallo funcional,
fallo de compilación/configuración, fallo instrumentado, omitido e infraestructura.
No ejecutar binarios heredados inventariados ni inicializar submódulos para esta
batería. Todos los datos y productores nuevos pertenecen al proyecto y viven en
el temporal; ninguna prueba requiere documentos del usuario.
