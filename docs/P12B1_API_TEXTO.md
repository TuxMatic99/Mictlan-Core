# API de texto explícita y evolución Unicode

B1 añade nueve nombres sin quitar los 31 heredados. Los programas existentes
conservan su modo de contar y separar. Las funciones nuevas están disponibles
en `.lat` y `.mict`; sus nombres son extensiones propias de Mictlán y no se
promete que el intérprete Latino original las conozca.

| Necesidad | Heredado | Alternativa B1 | Propuesta posterior |
|---|---|---|---|
| Bytes de almacenamiento | longitud, con otros tipos admitidos | longitud_bytes, solo texto | — |
| Puntos de código | Sin contrato específico | longitud_unicode | — |
| Grafemas percibidos | Sin función | No implementado | longitud_grafemas, segmentación UAX #29 versionada |
| Inversión | invertir, bytes | invertir_unicode, puntos de código | invertir_grafemas |
| Subcadena | subcadena, inicio/cantidad en bytes | subcadena_unicode, inicio/cantidad en puntos | subcadena_grafemas |
| Posición | indice/encontrar, bytes | indice_unicode | Posición por grafemas |
| Conjunto delimitador | separar, conjunto de bytes, sin vacíos | separar_delimitadores, alias explícito | Separación por conjunto de puntos de código |
| Separador completo | No distinguirlo de separar | separar_texto, conserva vacíos | Opciones explícitas para omitir vacíos |
| Caja | mayusculas/minusculas, C por byte y locale | mayusculas_ascii/minusculas_ascii | Caja Unicode completa y reglas de idioma |
| Igualdad | es_igual, bytes exactos | Se conserva | Igualdad con normalización y case folding explícitos |
| Orden | comparar, signo de strcmp | Se conserva | comparar_idioma en extensión optativa |
| Reemplazo | reemplazar vuelve a interpretar escapes | Se conserva con validaciones de entradas inseguras | reemplazar_literal |

## Contratos elegidos

UTF-8 es una codificación, no una unidad de longitud. Las nuevas funciones con
sufijo `_unicode` validan la cadena completa. Rechazan sobrelargos, sustitutos,
continuaciones sueltas, entradas truncadas y valores mayores de U+10FFFF.
Son válidos los escalares no asignados y los no caracteres; no se inventa una
restricción de repertorio basada en idioma. No se acepta NUL interior porque
la representación histórica de Core lo usa como terminador.

`subcadena_unicode` e `indice_unicode` usan posiciones iniciales cero.
La cantidad de subcadena es un número de puntos, no la posición final.
Se permiten ventanas vacías al final, y se limita una cantidad excedente al
resto. Los índices nuevos exigen enteros no negativos hasta INT_MAX. Los
índices heredados siguen truncando fracciones finitas dentro de int.

`separar_texto` trata el separador como una secuencia exacta no vacía. Por
ejemplo, `::a::::` separado con `::` produce `["", "a", "", ""]`.
No interpreta regex ni reglas de campos entre comillas. `separar_delimitadores`
conserva el conjunto de bytes y los campos no vacíos del contrato histórico.

`invertir_unicode("é")` invierte el acento y la e como dos puntos distintos.
Eso **no** es inversión visual por caracteres percibidos. La referencia y los
tests incluyen ese caso y un emoji ZWJ para impedir que el nombre se interprete
como una promesa de segmentación de grafemas.

## UAX #29 y dependencias

[Unicode UAX #29](https://www.unicode.org/reports/tr29/) distingue fronteras de
grafemas extendidos y contempla combinantes, secuencias de emoji y otros
contextos. Una implementación futura deberá fijar una versión de Unicode,
elegir el perfil de conformidad y ejecutar el corpus oficial de fronteras de
esa versión. No se implementa un contador aproximado con el nombre grafema.

B1 implementa validación y recorrido UTF-8 sin tablas externas ni nuevas
bibliotecas de ejecución. El archivo privado `latstr_utf8.inc` se compila como
parte de `latstrlib.c`. No añade símbolos exportados. Python 3 es una dependencia
de desarrollo para sincronizar documentación y ejecutar mediciones, no del
intérprete distribuido.

Para una entrega posterior se puede evaluar
[utf8proc](https://github.com/JuliaStrings/utf8proc). Su
[licencia](https://github.com/JuliaStrings/utf8proc/blob/master/LICENSE.md)
separa el código con permiso MIT de los avisos aplicables a sus datos Unicode.
Debian trixie ofrece
[libutf8proc3](https://packages.debian.org/trixie/libutf8proc3): la ficha amd64
consultada indica 368 kB instalados para 2.9.0-1+b2, además del paquete de
desarrollo si se compila contra ella. Su incorporación exigiría revisar versión
de tablas y cobertura de grafemas/caja, además de conservar esos avisos.
No se instala ni enlaza en B1. La colación por idioma se diseñaría como
capacidad optativa con idioma y reglas explícitos, manteniendo Core ligero.
