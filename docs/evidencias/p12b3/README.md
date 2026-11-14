# Evidencia inicial, antes de modificar producción

Fuente: `3125a0016e1adcfa0ac68c2c37ba40108a033cc9`, con los tests B3 añadidos.
`base-debug.log`: 180/180 previas aprobadas. `antes-debug.log`: 10/63 nuevas
aprobadas, 53 fallidas (incluye funciones propuestas aún ausentes, que NO son
regresiones de compatibilidad). Esta evidencia se publica antes de editar src.

Casos reproducidos: `max(-1e16,-2e16)` produce el centinela -999999999999999;
`min(1e16,2e16)` el centinela positivo; `anumero(0)` y `anumero("0")` devuelven
nulo; `aleatorio(-1)` termina por excepción aritmética; exponentes no finitos
no se rechazan; una llamada aleatorio con tres argumentos conserva operandos
sobre el prefijo 777; conversiones C de NaN no producen error recuperable.
El caso de temporales crece de 78 448 a 1 505 496 bytes administrados al ejecutar
1000 repeticiones. No es una afirmación de fuga tras destruir la VM: es retención
innecesaria durante su vida. Los logs contienen también los resultados exactos.

Comandos (directorio raíz de Core):

```sh
cmake -S . -B ../build-p12b3-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build ../build-p12b3-debug --parallel 4
ctest --test-dir ../build-p12b3-debug --output-on-failure  # antes de añadir B3: 180
ctest --test-dir ../build-p12b3-debug -L p12b3 --output-on-failure
python3 tests/matematicas/medir.py ../build-p12b3-debug/mictlan_bench_matematicas
```

En este entorno se añadió `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` por CMake 4 y
las rutas `-I…/build-deps/root/usr/include` en C_FLAGS y
`-L…/build-deps/root/usr/lib/x86_64-linux-gnu` en EXE/SHARED_LINKER_FLAGS para
readline extraído en el workspace. Debian con libreadline-dev instalado no
necesita esas rutas. Versiones completas en `entorno-inicial.txt`.
El CSV contiene tres procesos por tamaño y comprueba el máximo antes de medir.
Son mediciones Debug iniciales, no comparaciones de rendimiento Release.

Los logs versionados sólo normalizan escapes de color y espacios al final de línea.

## Evidencia posterior

`implementacion-{debug,release}.log`: 245/245 con el código
`00c3a91542521b47476b3de612afd29eca7ff787`. `final-local-{debug,release}.log`:
251/251 tras añadir los ejemplos y pruebas de locale, sin cambios de producción.
`implementacion-asan-local-bloqueado.extracto.log` conserva el diagnóstico y
resumen del intento de 261 tests: 258 bloqueados por ptrace; tres verificadores
auxiliares aprobados. No se declara aprobación instrumentada local.

`ci-antes.extracto.log`: selección de diagnósticos, configuración y recuento del
[job inicial instrumentado](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43641329286/job/132589344971).
La fuga del test azar al abortar por API ausente correspondía a una segunda VM
del arnés sin cerrar; se corrigió el arnés, no se atribuye a Core.
`ci-implementacion-asan.extracto.log`: selección de configuración, tests B3 y
recuento del [job aprobado](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43641793526/job/132590763466).
Los extractos son explícitamente parciales; los enlaces llevan al log completo.

Los CSV `benchmark-{antes,despues}-release.csv` comparan tres procesos por tamaño;
la base se recompiló en un worktree separado en 3125a001 y se enlazó el mismo
benchmark contra ella. Se usó `-O3` y el mismo GCC 14.2. Las rutas extraídas de
readline indicadas arriba se añadieron también a la compilación manual inicial,
que sin ellas falló (no se contó como medición). Los CSV Debug son complementarios.


`ci-final-{debug,release,asan}.extracto.log` conserva configuración, pruebas B3,
ABI y recuento de la [CI completa 43642854265](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43642854265),
SHA `239401f3e21eadf51bc1b2da0c899989a1091757`: 251/251, 251/251 y 267/267,
respectivamente, sin omitidas. La guía Debian queda fijada a esa revisión.
