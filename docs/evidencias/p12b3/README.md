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
