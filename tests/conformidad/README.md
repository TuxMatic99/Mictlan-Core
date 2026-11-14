# Conformidad P1.2-A

Los `.out` son oráculos escritos y revisados antes de los arreglos. No se
generan ejecutando Core ni comparando dos binarios de la misma implementación.
`verificar.cmake` copia cada fuente a un programa `.lat` y otro `.mict` y exige
el resultado completo, el código de salida y ausencia de stderr en casos válidos.
En los casos negativos exige código 1, stdout vacío y el diagnóstico concreto.
Un informe de ASan, UBSan o LSan invalida cualquier caso, incluso negativo.
Solo se normalizan CRLF; no se recortan espacios ni líneas.

| Fuente | Fundamento independiente del resultado |
| --- | --- |
| `numeros.mict` | Fracciones 21/4 y 1/4, división real y resto con signo del dividendo. Representación binaria del `double` en Debian x86_64. `tipo()` mantiene el nombre histórico `decimal` incluso para 5. |
| `precedencia.mict` | Aritmética calculada con la agrupación publicada en la especificación. Los casos heredados son pruebas de compatibilidad, no una pretensión de seguir la precedencia de Python. |
| `logica.lat` | Tablas de verdad de AND y OR inclusivo; contrato explícito de selección de operandos y evaluación ansiosa. |
| `comparaciones.mict` | Orden numérico y textual; las cuatro relaciones convierten de la misma forma cuando hay un número. La asimetría histórica de igualdad queda identificada como compatibilidad. |
| `funciones.mict` | Sumas manuales, retornos múltiples, aridad faltante, descarte repetido y aliases realmente reconocidos por el escáner distribuido. |
| `recursion.mict` | Recurrencia F(0)=0, F(1)=1 y F(n)=F(n−1)+F(n−2); 6!=720; paridad. No se calcula el esperado con una función Fibonacci de Core. |
| `ambitos.mict` | Modelo documentado de marcos heredados y asignación global. Incluye consecuencias sorprendentes que se conservan por compatibilidad. |
| `asignaciones.mict` | Aritmética, intercambio simultáneo, nulos de relleno y descarte de exceso. |
| `condicional.mict` | Solo se ejecuta la rama elegida por el ternario. |
| `local_inaccesible.mict` | Un nombre creado en una llamada no se publica al volver. |
| `argumentos_exceso.mict` | Se rechaza pasar dos argumentos a una función de un parámetro fijo. |
| `locale_decimal.c` | Valores C fijados como fracciones exactas; ejecución con locales C, español y francés; se conserva el locale del anfitrión. |

Son **25 pruebas CTest**: once programas en dos extensiones y tres locales de
la API C. El código 77 de la prueba de locale indica que el sistema no lo tiene
instalado y CTest lo informa como omitido, nunca como una conversión comprobada.
El CI instala los dos locales de coma y exige que existan.

```sh
ctest --test-dir build-p12a-debug -L p12a --output-on-failure
```

Los archivos `stdout.txt` y `stderr.txt` de cada caso se conservan dentro de
`build-p12a-debug/conformidad/`. Consulta la
[guía para Debian](../../docs/P12A_VERIFICAR_DEBIAN.md).
