# Informe de ingeniería P1.2-A

**13 de noviembre de 2026, hora de Ciudad de México.**
Repositorio: [Mictlan-Core](https://github.com/TuxMatic99/Mictlan-Core).
Propuesta: [PR #7, en borrador](https://github.com/TuxMatic99/Mictlan-Core/pull/7).

## Resultado

Se incorporan **25 pruebas de conformidad**, una especificación semántica con
tabla de precedencia, una guía para estudiantes y reproducción en Debian.
Se corrigen dos defectos demostrados: comparación `<=` con tipos mezclados
y lectura decimal por API C cuando el anfitrión usa coma decimal.
Los otros comportamientos heredados se describen y prueban sin rediseñarlos.

## Estado inicial y procedencia

La referencia solicitada `9f3a1965f2794c8921adf9cdfa3e0c192060c657` existe y
corresponde a «feat(memoria): recolectar cadenas internadas bajo contrato
explícito». Al comenzar, `investigacion/p11-memoria-pila` e
`investigacion/p12-semantica` apuntaban a ella. P1.2 no contenía cambios propios.
Se creó `investigacion/p12a-conformidad` desde esa revisión. La base del PR es
P1.1, por lo que sus propuestas anteriores quedan separadas y conservadas.
`main` partía de `b3be302bbfdc0dfc371b6871dd203f7ff9ce3a0b` y no se modifica.

| Incremento publicado | Commit | Contenido |
| --- | --- | --- |
| Contrato y pruebas antes de corregir | `2a2c3042d404d6796a02fafcd5acbf89bcabc86a` | Oráculos y alternativas de compatibilidad; cuatro fallos reales |
| Corrección | `187d77834466b4300c63115943f95ee4b86fbd5e` | Conversión de `<=` y acción léxica decimal |
| Documentación y CI | `e1f511ed5a91b6d097579971192c12b67a89a033` | Especificación, guía Debian, material didáctico y matriz automatizada |

Los árboles publicados de los dos primeros commits se comprobaron idénticos
a los usados para demostrar los fallos y validar el arreglo. No se modifican
cabeceras públicas, gramática Bison, recolección, SDK ni Lab en esta misión.

## Investigación del lenguaje

El escáner distribuido reconoce enteros escritos y fracciones como tokens
numéricos y construye nodos de AST que almacenan `double`. La regla admite
exponente `e` o `E`, con signo opcional. El parser Bison decide la agrupación.
`src/latdo.c` emite cargas y operaciones para una máquina de pila;
`src/latmv.c` implementa la ejecución. Las comparaciones, las llamadas y las
asignaciones tienen instrucciones propias.

Las operaciones aritméticas binarias evalúan primero el lado izquierdo y
luego el derecho. Las formas `^` y `**` emiten la misma operación de potencia,
pero tienen prioridades distintas en la gramática. `&&` y `||` emiten la
evaluación de ambos lados antes de seleccionar el operando resultante; no
son saltos de cortocircuito. El ternario sí se compila con selección de rama.

Las funciones usan marcos de contexto clonados del llamante. Los parámetros
se vinculan en el marco activo. Las lecturas buscan ese marco y después el
global; las asignaciones ordinarias pueden actualizar también un global
homónimo. Se conserva el ajuste de pila P1.1 para descartar resultados de
llamadas usadas como sentencias. La recursión directa y mutua comprobada no
requiere modificar los marcos ni el mecanismo de retorno.

| Caso | Base | P1.2-A | Interpretación |
| --- | --- | --- | --- |
| `5.25` en consola | 5.25 | 5.25 | Ya correcto; usa punto |
| `5.25` desde API C, locale español | 5 | 5.25 | Defecto de conversión corregido |
| `5 / 2` | 2.5 | 2.5 | División real |
| `tipo(5)` y `tipo(5.25)` | `decimal` | `decimal` | Ambos son numéricos `double` |
| `verdadero \|\| verdadero` | verdadero | verdadero | OR inclusivo, no XOR |
| `2 + 3 * 2 ^ 3` | 26 | 26 | Potencia antes de multiplicación y suma |
| `2 ^ 3 ^ 2` | 64 | 64 | Asociación izquierda conservada |
| `2 ^ 3 ** 2` / `2 ** 3 ^ 2` | 512 / 64 | 512 / 64 | Precedencia diferente conservada |
| `fibonacci(12)` | 144 | 144 | Recursión correcta en el caso solicitado |
| Llamadas anidadas de suma e identidad | 42 | 42 | Resultado compuesto correcto |
| Retornos múltiples y descarte reiterado | Correctos | Correctos | Se conserva el arreglo P1.1 |
| `2 <= "3"` | falso | verdadero | Ahora coincide con las otras relaciones |
| Parámetro homónimo, solo lectura | No cambia el global | Igual | Vinculación local preservada |
| Parámetro homónimo, reasignado | Puede cambiar el global | Igual | Regla heredada documentada |

La [especificación semántica](SEMANTICA_P12A.md) contiene la tabla completa,
el orden de evaluación, las conversiones y ejemplos de ámbitos.

## Defectos y justificación

En `menor_igual`, la rama mixta usaba `getNumerico` en ambos operandos.
Ese acceso solo representa un número si el tipo corresponde: para una cadena
o colección lee los bits de otra representación. Se sustituye por
`latC_adouble`, como ya hacen las otras relaciones. Las parejas de números
mantienen el resultado. Se prueban cadenas numéricas en ambos órdenes,
booleanos, nulo y listas contra números.

El escáner reconocía el token `5.25` entero, pero convertía su texto con el
`strtod` dependiente del idioma del proceso. En un locale de coma se aceptaba
solo el prefijo 5. La consola ocultaba el problema al fijar `LC_NUMERIC=C`.
La acción léxica ahora llama a `latC_strtod_c`, ya disponible en Core.
La prueba C comprueba cinco valores, tres locales y que el locale previo del
anfitrión se conserve después de compilar y ejecutar.

Las alternativas se registraron **antes** del arreglo en
[P12A_DECISIONES.md](P12A_DECISIONES.md). Cambiar coerciones globales,
asociación de potencias, cortocircuito o ámbitos léxicos sería un diseño
semántico diferente. Esta propuesta no lo introduce implícitamente.

## Conformidad y evidencia reproducible

Los esperados se escribieron como resultados matemáticos o como reglas de
compatibilidad explícitas, no copiando la salida del ejecutable. Por ejemplo,
F(12)=144 procede de la recurrencia definida y 6!=720 de su producto.
Los casos de semántica heredada están identificados como tales en el
[catálogo de pruebas](../tests/conformidad/README.md).

Los once programas se ejecutan como `.lat` y `.mict` en directorios separados.
Se exige stdout completo, código de salida y ausencia de stderr para programas
válidos. Dos programas negativos exigen error y diagnóstico concreto.
Los informes de sanitizadores siempre invalidan el caso. Los resultados por
programa se conservan en el directorio de compilación.

| Comprobación | Resultado |
| --- | --- |
| Base, suite existente, Debug | 42/42 |
| Nuevas pruebas antes de corregir, Debug | 21/25; fallan las dos extensiones de comparación y los dos locales de coma |
| Propuesta, suite completa, Debian Debug | 67/67; sin locales omitidos |
| Propuesta, suite completa, Debian Release | 67/67; sin locales omitidos |
| Verificador, controles sintéticos | 9/9: rechaza valor/código/stderr incorrectos y ASan/UBSan/LSan, incluido diagnóstico negativo coincidente |
| Instrumentación local, 83 casos | `BLOCKED_INFRASTRUCTURE`: LSan falla bajo ptrace; no es aprobación |
| GitHub Actions, Debug / Release | 67/67 en cada perfil |
| GitHub Actions, ASan + UBSan + LSan | 83/83, detección de fugas activa; sin omisiones |

La compilación local usa Debian 13, GCC 14.2.0 y CMake 4.4.4 con
`CMAKE_POLICY_VERSION_MINIMUM=3.5`. Se proporcionaron las cabeceras de
`libreadline-dev` 8.2-6 en un prefijo de trabajo, enlazando la biblioteca
readline 8.2 del sistema. Flex 2.6.4 se usó para investigar la diferencia entre
fuente y escáner distribuido. La compilación ordinaria utiliza los C existentes.

Se conserva [evidencia de base](evidencias/p12a/ctest-base-42.log),
[fallos previos](evidencias/p12a/ctest-antes-25.log),
[Debug](evidencias/p12a/ctest-debug-67.log),
[Release](evidencias/p12a/ctest-release-67.log),
[controles del verificador](evidencias/p12a/controles-verificador.txt) y
[control C vacío de LSan](evidencias/p12a/lsan-control-vacio.stderr).
Las rutas de trabajo se normalizan a `<trabajo>` y se retiran colores ANSI;
los resultados y mensajes se conservan.

La [matriz del PR](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43551747982)
prueba la revisión `e1f511ed5a91b6d097579971192c12b67a89a033` en Ubuntu 24.04.
Los tres trabajos finalizaron correctamente con GCC 13.3.0, sin omisiones.
Los extractos conservan el bloque CTest de cada log: [Debug](evidencias/p12a/ci-debug-67.log),
[Release](evidencias/p12a/ci-release-67.log) e [instrumentado](evidencias/p12a/ci-asan-83.log).
Los artefactos de Actions conservan también salidas individuales y JUnit.
Los detectores permanecen activos con `detect_leaks=1` y un código de error
LSan distinto de cero. El éxito remoto resuelve la comprobación instrumentada
que la infraestructura local impide, sin reclasificar el bloqueo local como aprobado.

## Compatibilidad y límites conocidos

La regeneración completa del escáner mostró una diferencia previa: el `.l`
enumera `regresar` y `devolver` como aliases de retorno, pero las tablas C
actuales permiten usarlos como identificadores. Reservarlos ahora rompería
programas válidos. Se actualiza únicamente la acción decimal en ambos
archivos y se añade una prueba con esos identificadores. La especificación
Alpha se corrige para enumerar los aliases realmente admitidos: `retorno`,
`retornar` y `ret`. La resolución definitiva del conjunto de palabras necesita
una propuesta de compatibilidad independiente.

`latC_strtod_c` restaura el locale, pero lo cambia temporalmente de forma
global. No se afirma seguridad de compilación concurrente entre hilos.
Se conservan la igualdad mixta dependiente del tipo izquierdo, las conversiones
históricas, la evaluación ansiosa de los lógicos y las instantáneas de marcos.
La precedencia de regex se documenta desde la gramática; su motor no se audita
nuevamente. Tampoco se define una política nueva de no finitos ni se agota el
límite de recursión. La compatibilidad está comprobada por el corpus, sin
pretender demostrar todos los programas posibles de Latino.

## Entregables y siguiente revisión

| Entregable | Ubicación |
| --- | --- |
| Informe del comportamiento | Este archivo |
| Tabla de operadores y especificación | [SEMANTICA_P12A.md](SEMANTICA_P12A.md) |
| Programas y automatización | [tests/conformidad](../tests/conformidad/README.md) |
| Alternativas y arreglos | [P12A_DECISIONES.md](P12A_DECISIONES.md), commit `187d778` |
| Guía para estudiantes | [P12A_PRIMEROS_PASOS.md](P12A_PRIMEROS_PASOS.md) |
| Resultados y repetición en Debian | [P12A_VERIFICAR_DEBIAN.md](P12A_VERIFICAR_DEBIAN.md) y evidencias enlazadas |
| Propuesta para revisar | [PR #7 en borrador](https://github.com/TuxMatic99/Mictlan-Core/pull/7) |

El PR queda para revisión sin fusionar. La próxima decisión semántica puede
tratar explícitamente ámbitos y coerciones; debe partir de estas pruebas y
preservar un modo de compatibilidad para programas existentes.

## Referencias históricas consultadas

[Manual de Latino 1.4.0](https://manual.lenguajelatino.org/es/stable/),
[operadores](https://manual.lenguajelatino.org/es/stable/sintaxis/Operadores.html),
[aritmética](https://manual.lenguajelatino.org/es/stable/sintaxis/Aritmetica.html),
[tipos](https://manual.lenguajelatino.org/es/stable/sintaxis/Tipos-de-Datos.html) y
[lógicos](https://manual.lenguajelatino.org/es/stable/sintaxis/Logicos.html).
Se contrastaron con el parser, el bytecode y resultados ejecutados. La
formulación histórica sobre OR no se adopta como una definición de XOR ni se
asume identidad de precedencias con otros lenguajes.
