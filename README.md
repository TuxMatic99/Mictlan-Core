# Mictlán Core

Mictlán es un lenguaje de programación de propósito general en español, derivado de Latino. Core contiene el intérprete de consola, la máquina virtual y las bibliotecas básicas. La versión de este árbol se define en [mictlan_version.h](include/mictlan_version.h).

## Primer programa en Debian

```bash
sudo apt install build-essential cmake libreadline-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
./build/mictlan --version
./build/mictlan --help
```

Guarda `escribir("Hola desde Mictlán")` en `primero.mict` y ejecuta `./build/mictlan primero.mict`. Los archivos `.lat` compatibles se ejecutan del mismo modo. Para una prueba rápida, usa `./build/mictlan -e 'escribir(6 * 7)'`. El [SDK](https://github.com/TuxMatic99/Mictlan-SDK) ofrece un paquete de consola con ejemplos de ciencia y herramientas de VS Code.

```bash
ctest --test-dir build --output-on-failure
```

La matriz del SDK comprueba Core, SDK, compatibilidad y sanitizadores con revisiones exactas. El [contrato de propiedad](docs/p02g_contrato_memoria.md) describe el recolector experimental que puede activar un anfitrión C. La [especificación Alpha](docs/ESPECIFICACION_ALPHA.md) documenta el lenguaje observable.

## Compatibilidad y créditos

El [contrato de compatibilidad](docs/COMPATIBILIDAD.md) explica el alcance de `.lat`, los nombres históricos y la biblioteca `liblatino.so`. Mictlán conserva las contribuciones y la licencia MIT de Latino y sus colaboradores. Consulta [LICENSE](LICENSE), la [documentación original de Latino Core](docs/historia/README_LATINO_CORE.md) y el [proyecto de origen](https://github.com/lenguaje-latino/latino-Core).

La distribución de referencia de esta Alpha es Debian 13 x86_64. El SDK gráfico tiene validación y distribución propias.
