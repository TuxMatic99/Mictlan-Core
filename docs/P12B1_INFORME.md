# Informe P1.2-B1 · Biblioteca de cadenas

**13 de noviembre de 2026, hora de Ciudad de México.**
[PR #8 en borrador](https://github.com/TuxMatic99/Mictlan-Core/pull/8), rama
`investigacion/p12b1-cadenas`, dependiente de P1.2-A. No se fusiona la propuesta.

## Resultado de ingeniería

Se conservan los **31 nombres heredados** de `cadena` y se añaden **nueve**
operaciones explícitas, sin nuevas dependencias de ejecución. Se corrigen
buffers C perdidos, reservas insuficientes, conversiones numéricas fuera de
rango y rutas de error. La referencia contiene ejemplos ejecutados para los
**40 nombres**. La suite completa pasa **128/128** en Debug y Release y
**144/144** con ASan, UBSan y LSan en GitHub Actions, sin omisiones.

La API distingue bytes de puntos de código. Grafemas, caja Unicode completa,
normalización y colación por idioma quedan diseñados como trabajo posterior;
no se publican funciones aproximadas con esos nombres.

## Revisión inicial y procedencia

Se verificó el commit `c25eb7e5008e1201ebd1292da7b2da0dd0a554a0` y que la
PR #7 seguía abierta, en borrador y sin fusionar. La rama B1 se creó exactamente
desde él y la PR #8 apunta a `investigacion/p12a-conformidad`. Se conserva así
la implementación P0.2-E/F/G, P1.1 y P1.2-A que ya integra esa revisión.
`main` permanecía en `b3be302bbfdc0dfc371b6871dd203f7ff9ce3a0b`.

| Incremento publicado | SHA | Contenido |
|---|---|---|
| Pruebas antes de implementar | `1ef03a5b310c95cdce0a2df7533d81d89fd2116d` | Contratos heredados, expectativas nuevas y prueba de fuga |
| Arreglos y API explícita | `b052ea8a5916422c505f1f2907df276b742df665` | Temporales, límites, UTF-8 y CI |
| Concatenación y referencia ejecutable | `60017460cb672270c73544f1780f7a2f4c3ca8c1` | Limpieza tras error y 40 ejemplos documentados |

La [matriz de código verificada](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43557166204)
corresponde exactamente al último SHA de esa tabla. El incremento posterior de
informe y evidencias no altera implementación ni tests.

## Inventario y contratos

El [inventario completo](P12B1_INVENTARIO.md) enumera propósito, argumentos,
resultado, vacío, UTF-8, índices, argumentos inválidos, compatibilidad y
diferencias con el manual para cada nombre. Se deriva de los registros reales
de `libstr` y de pruebas; el generador falla si falta un nombre público.
La [referencia para estudiantes](CADENAS.md) desarrolla cada firma con ejemplo,
resultado, casos límite y recomendación.

| Regla histórica | Decisión B1 |
|---|---|
| longitud cuenta bytes y admite diccionario; otros valores dan 0 | Conservar; longitud_bytes es explícita y estricta |
| invertir invierte bytes y puede romper UTF-8 | Conservar resultado y arreglar propiedad; añadir invertir_unicode |
| separar interpreta un conjunto de bytes y descarta vacíos | Conservar; alias separar_delimitadores; separar_texto usa subcadena completa y conserva vacíos |
| subcadena usa inicio/cantidad en bytes y trunca fracciones | Conservar para entradas definidas; validar límites; añadir subcadena_unicode con enteros |
| reemplazar recibe cantidad, no posición, en su cuarto argumento | Conservar; documentar y validar patrón y escapes |
| relleno repite max(0, n - bytes(base) + 1) veces | Conservar desfase y dimensionar según bytes reales del relleno |
| mayusculas/minusculas usan ctype por byte y locale | Conservar; hacer válido el argumento unsigned char; añadir variantes ASCII deterministas |
| comparar devuelve strcmp y es_igual compara bytes | Conservar el signo y la igualdad exacta; no simular colación lingüística |

Las alternativas se registraron en [Decisiones](P12B1_DECISIONES.md) antes de
modificar implementaciones. La [propuesta de API](P12B1_API_TEXTO.md) separa
capacidades implementadas de las futuras.

## Defectos demostrados y arreglos

**Buffers de las funciones nativas.** La prueba previa de invertir deja
**100 reservas C vivas tras 100 llamadas y el cierre de la VM**. La VM interna
una copia del texto; no adopta el buffer malloc del llamante. Se publican los
resultados mediante la ruta que libera el envoltorio temporal y después se
libera el buffer C. Se aplica a invertir, insertar, subcadena, eliminar,
reemplazar, rellenos y cambios de caja. Reemplazar libera también las dos
cadenas producidas por su segunda interpretación de escapes.

**Concatenación en error.** Una prueba adicional conserva la primera conversión
y provoca un error en la segunda con una lista cíclica. Antes del arreglo
queda una conversión sin liberar. La nueva implementación toma prestados los
textos existentes y usa conversiones con error diferido para otros valores;
libera ambas antes de propagar el error. También comprueba la ruta de un
primer valor numérico. Los resultados válidos y la operación `..` se conservan.

**Reservas y límites.** El relleno anterior reservaba según n y la longitud
base, sin considerar los bytes de cada repetición. El caso con relleno `ññ`
reproduce heap-buffer-overflow con ASan, aunque la prueba funcional normal
pueda imprimir el resultado esperado. La reserva nueva usa el tamaño exacto,
comprueba desbordamiento de size_t y mantiene el número histórico de repeticiones.
Subcadena limita la copia al texto restante. Insertar valida posiciones
anteriores al inicio. Se rechazan números no finitos o fuera de int antes de
convertirlos, preservando truncamiento finito cuando era válido.

**Escapes del reemplazo.** ASan demuestra lectura fuera de reserva con una
secuencia literal `\u` incompleta y UBSan demuestra desbordamiento entero con
un escape numérico enorme. La biblioteca valida esas entradas antes de reservar
y antes de llamar al lector histórico. Se conserva la segunda interpretación
de escapes admitida, sin cambiar el lector del parser. Un patrón vacío produce
un diagnóstico en lugar de pasar NULL a la construcción de cadena.

**Otros temporales.** Regex lógico usa texto prestado o un buffer numérico en
la pila C, y valida el patrón antes de adquirir recursos. La búsqueda de grupos
copia solo el intervalo coincidente y lo libera, en lugar de copiar el sufijo
completo y perder otro buffer. `inicia_con` deja de restar NULL cuando no hay
coincidencia. `char(lista)` recorre la lista linealmente, valida antes de reservar
y conserva la omisión histórica de ceros. No se cambia el motor de regex.

Los logs [invertir y límites previos](evidencias/p12b1/antes.log),
[concatenación](evidencias/p12b1/concat-antes.log),
[relleno ASan](evidencias/p12b1/limites_validos-antes.log),
[escape ASan](evidencias/p12b1/escape_unicode-antes.log) y
[escape UBSan](evidencias/p12b1/escape_enorme-antes.log) conservan la evidencia.
El intento instrumentado de subcadena negativa terminó bloqueado por LSan;
ese log no se presenta como detección de un acceso inválido ni como aprobación.

## API nueva y Unicode

| Función añadida | Contrato implementado |
|---|---|
| longitud_bytes | Solo texto; longitud en bytes |
| longitud_unicode | Validación UTF-8 completa y número de escalares |
| invertir_unicode | Inversión por puntos de código, conservando cada secuencia UTF-8 |
| subcadena_unicode | Inicio y cantidad de puntos de código; índices enteros no negativos |
| indice_unicode | Coincidencia exacta; resultado en puntos de código |
| separar_texto | Separador completo no vacío; conserva campos vacíos |
| separar_delimitadores | Alias del conjunto de bytes heredado |
| mayusculas_ascii | Solo a-z -> A-Z; independiente del locale |
| minusculas_ascii | Solo A-Z -> a-z; independiente del locale |

Se validan español, ñ, tildes, caracteres de dos a cuatro bytes, combinantes,
emoji con ZWJ, vacío, fronteras U+10FFFF y entradas inválidas. El lector privado
se contrasta con un codificador independiente para las **1.112.064** posiciones
escalares, incluyendo U+0000 en el lector; la representación de texto público
sigue sin NUL interior. Se comprueban secuencias truncadas multibyte y las
clases inválidas por API compartida, con recuperación de pila y prefijo 777.

`longitud_unicode("é")` es 2 y la de `👩‍💻` es 3. No son un contador de
grafemas. Se consultó [UAX #29](https://www.unicode.org/reports/tr29/) para la
distinción y el diseño futuro, sin afirmar conformidad con su segmentación.
La propuesta analiza utf8proc como opción posterior; B1 no lo enlaza.

## Validación y clasificación

| Comprobación | Resultado y clasificación |
|---|---|
| Suite anterior sobre P1.2-A | 67/67 aprobadas |
| Primera batería B1 sobre la implementación anterior | 12/51 aprobadas; 18 errores de límites, un fallo de propiedad y 20 expectativas de API aún no implementada |
| Debug Debian 13 / GCC 14.2.0 | 128/128 aprobadas |
| Release Debian 13 / GCC 14.2.0 | 128/128 aprobadas |
| Instrumentación local | Compila; 143 ejecuciones nativas bloqueadas por LSan/ptrace y una comprobación documental Python aprobada; no se aprueba ejecución instrumentada local |
| CI Ubuntu 24.04 / GCC 13.3.0, Debug | 128/128 aprobadas |
| CI Ubuntu 24.04 / GCC 13.3.0, Release | 128/128 aprobadas |
| CI ASan + UBSan + LSan | 144/144 aprobadas, sin omisiones |
| Compilación/configuración final | Sin errores |
| Sanitizadores sobre propuesta en CI | Sin informes detectados en los casos ejecutados |

La suite B1 añade **61 pruebas** y conserva las 67 anteriores; el perfil
instrumentado conserva otras 16 regresiones. Se reactiva en CTest la regresión
histórica de cadenas/E/S, con sus 1.000 ciclos y diez cierres, sin reescribirla.
Los 40 ejemplos de documentación se ejecutan en ambas extensiones. Un test
contrasta el catálogo, los registros reales y los archivos generados.

Se exige salida completa, código y diagnóstico en negativos; cualquier
informe de sanitizador invalida el caso. Los bloqueos no se invierten ni se
cuentan como aprobados. Las configuraciones mantienen detect_leaks=1 y códigos
de error distintos de cero.

Evidencias: [Debug local](evidencias/p12b1/debug-128.log),
[Release local](evidencias/p12b1/release-128.log),
[bloqueo local](evidencias/p12b1/instrumentado-local-resumen.txt),
[Debug CI](evidencias/p12b1/ci-debug-128.log),
[Release CI](evidencias/p12b1/ci-release-128.log) y
[instrumentado CI](evidencias/p12b1/ci-asan-144.log).
Las rutas de trabajo y colores se normalizan en los logs publicados.

## Tiempo y memoria

Se usó el mismo consumidor C con la biblioteca P1.2-A Release y la nueva,
procesos separados por escenario, una llamada de calentamiento y tres rondas.
Hay textos ASCII y UTF-8 de 20 y 262.140 bytes, 10.000 o 100 repeticiones.
Se mide tiempo monotónico, `mv->memoria_usada` y pico RSS de Linux. No se usan
umbrales de tiempo para aprobar la suite.

| Función | Bytes ASCII | Vueltas | Mediana A, ms | Mediana B1, ms | Incremento VM A, bytes | Incremento VM B1, bytes |
|---|---:|---:|---:|---:|---:|---:|
| concatenar | 20 | 10000 | 3.153 | 2.965 | 0 | 0 |
| concatenar | 262140 | 100 | 71.49 | 63.938 | 0 | 0 |
| invertir | 20 | 10000 | 2.831 | 2.119 | 1571048 | 0 |
| invertir | 262140 | 100 | 85.903 | 70.38 | 14400 | 0 |
| insertar | 20 | 10000 | 4.067 | 3.36 | 1571048 | 0 |
| insertar | 262140 | 100 | 117.37 | 64.312 | 14400 | 0 |
| subcadena | 20 | 10000 | 3.099 | 2.41 | 1571048 | 0 |
| subcadena | 262140 | 100 | 31.301 | 31.049 | 14400 | 0 |
| reemplazar | 20 | 10000 | 5.684 | 4.415 | 1571048 | 0 |
| reemplazar | 262140 | 100 | 69.884 | 62.338 | 14400 | 0 |

Para invertir 100 veces 262.140 bytes ASCII, el pico RSS mediano registrado
baja de 27.904 KiB a 8.576 KiB. Las diferencias de tiempo de subcadena grande
son pequeñas: no se afirma una aceleración significativa de ese escenario.
El [resumen completo](evidencias/p12b1/benchmark-resumen.json) y los seis CSV
conservan las tres rondas, ambos alfabetos y escenarios nuevos.

El incremento VM cero corresponde a repetir **los mismos resultados ya
internados tras calentar**. No significa memoria cero ni elimina la retención
de cadenas distintas hasta el cierre. El contador VM no incluye malloc C;
por eso se acompaña del observador de buffers, RSS y LSan. RSS es un pico del
proceso, no una medición exacta de memoria viva por operación. Los tiempos
son mediciones de este entorno compartido, no una garantía para otro equipo.

## Compatibilidad binaria y coste

Los [285 símbolos anteriores](evidencias/p12b1/simbolos-base.txt) coinciden
exactamente con los [285 finales](evidencias/p12b1/simbolos-b1.txt).
No cambian las cabeceras públicas, la gramática, el parser ni el léxico.
El [consumidor compilado contra A](evidencias/p12b1/consumidor-anterior.csv)
ejecuta la misma operación con liblatino.so B1 sin recompilar.
Esto comprueba ese consumidor y la superficie de símbolos; no es una prueba
universal de todo binario externo.

En Release la biblioteca pasa de 267.152 a 275.872 bytes: **8.720 bytes** más
con la misma configuración local. Sus dependencias dinámicas de ejecución
siguen siendo las existentes. La implementación UTF-8 es privada, sin tablas
Unicode grandes ni nuevos símbolos exportados. Python 3 se usa en el desarrollo
para documentación y mediciones.

Entorno local: Debian 13 trixie, Linux 6.18.57 x86_64, GCC 14.2.0 y CMake 4.4.4
con la opción de compatibilidad 3.5 para linenoise. Se usan cabeceras
libreadline-dev 8.2-6 y la biblioteca readline 8.2 del sistema. Los comandos
independientes con paquetes Debian están en la guía; los directorios privados
de dependencias de esta ejecución no son necesarios en la máquina del revisor.

## Limitaciones y continuidad

- No hay segmentación de grafemas, normalización, caja Unicode completa ni colación por idioma. La caja ASCII no convierte ñ ni tildes.
- Las operaciones heredadas por byte pueden producir UTF-8 inválido. Las nuevas operaciones `_unicode` lo rechazan; no lo reparan silenciosamente.
- Se conservan el signo dependiente de plataforma de bytes, el desfase del relleno, la segunda interpretación de escapes en reemplazar y los límites heredados de regex.
- La validación añadida protege los escapes recibidos por reemplazar. No rediseña los escapes de literales del parser ni acredita todo ese lector heredado.
- No se certifica recuperación general tras agotamiento de memoria en todas las construcciones parciales. La entrega prueba rutas normales, errores de argumentos/conversión y los casos instrumentados; no introduce una política nueva de memoria o recolección.
- La prueba binaria y las mediciones corresponden a Linux x86_64. Otras plataformas necesitan su propia ejecución.

## Entregables y reproducción

[Inventario](P12B1_INVENTARIO.md), [tabla de evolución de API](P12B1_API_TEXTO.md),
[referencia en español con ejemplos](CADENAS.md),
[tests automatizados y oráculos](../tests/cadenas/README.md),
[decisiones](P12B1_DECISIONES.md), [evidencias](evidencias/p12b1/benchmark-resumen.json),
[guía Debian](P12B1_VERIFICAR_DEBIAN.md) y
[PR #8 en borrador](https://github.com/TuxMatic99/Mictlan-Core/pull/8).

La propuesta queda disponible para revisión independiente y no se fusiona.
La siguiente entrega puede implementar grafemas o caja Unicode mediante una
versión fijada de tablas y pruebas oficiales, conservando las operaciones
explícitas B1 y los contratos Latino que este inventario identifica.

## Referencias contrastadas

[Manual histórico de cadena](https://manual.lenguajelatino.org/es/stable/librerias/Cadena.html),
[UAX #29](https://www.unicode.org/reports/tr29/),
[licencia de utf8proc](https://github.com/JuliaStrings/utf8proc/blob/master/LICENSE.md) y
[ficha Debian libutf8proc3](https://packages.debian.org/trixie/libutf8proc3).
Las reglas efectivas y los resultados se verifican contra el código y la suite;
no se atribuye al manual un contrato que la implementación no cumple.
