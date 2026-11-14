# Decisiones previas a implementar P1.2-B1

Base exacta: `c25eb7e5008e1201ebd1292da7b2da0dd0a554a0`, PR #7 abierta en
borrador. Rama nueva: `investigacion/p12b1-cadenas`. Se conservan todos los
incrementos anteriores y la base de la PR será `investigacion/p12a-conformidad`.

La suite anterior pasa 67/67. Se fijan 51 comprobaciones nuevas antes de tocar
`latstrlib.c`: 12 pasan, 39 fallan. De los fallos, 20 corresponden a API nueva
que todavía no existe, 18 a límites inválidos sin diagnóstico correcto y uno
al contador de buffers nativos: **100 llamadas a invertir dejan 100 buffers C**
incluso después de cerrar la VM. Las pruebas no confunden API aún no implementada
con regresiones heredadas. Los logs previos se conservan en `evidencias/p12b1`.

| Área | Contrato preservado | Corrección / alternativa |
| --- | --- | --- |
| `longitud` | Bytes; diccionario -> número de entradas; otros tipos -> 0 | `longitud_bytes` exige texto; `longitud_unicode` cuenta escalares UTF-8 válidos |
| `invertir` | Inversión de bytes, incluso si rompe UTF-8 | Liberar buffer temporal. Añadir `invertir_unicode` por puntos de código, sin afirmar grafemas |
| `separar` | Conjunto de bytes delimitadores, espacios si omitido/vacío; elimina tokens vacíos | Alias explícito `separar_delimitadores`; `separar_texto` usa subcadena completa y conserva vacíos |
| `subcadena` | Inicio cero y cantidad en bytes; fracciones truncadas como históricamente | Rechazar inicio/cantidad negativos e inicio posterior al final; limitar cantidad al texto restante. `subcadena_unicode` exige enteros no negativos y usa puntos de código |
| `insertar` | Bytes; negativos desde el final; posición superior al final se limita al final | Rechazar posiciones anteriores al inicio, no finitas o fuera de int; liberar temporal |
| `reemplazar` | Coincidencias no solapadas; cuarto argumento es cantidad, no posición; 0/omitido/negativo -> todas; doble interpretación histórica de escapes | Liberar tres buffers; patrón vacío genera error en lugar de usar NULL |
| `eliminar` | Solo primera coincidencia; patrón vacío no cambia texto | Dimensionar exactamente y liberar temporal |
| Relleno | Repite `max(0, n - bytes(base) + 1)` veces el texto completo | Conservar el desfase histórico, calcular bytes reales y evitar escrituras fuera de la reserva |
| Mayúsculas/minúsculas | Conversión C por byte y locale; no es conversión Unicode | Liberar temporales; pasar unsigned char a ctype. Alternativas ASCII deterministas |
| Comparaciones | Igualdad exacta y orden de bytes de strcmp; magnitud no limitada a -1/1 | No convertir en colación lingüística ni normalizar implícitamente |
| `char`, regex | Contratos heredados | Validar tipos/rangos antes de reservar; liberar temporales de coincidencias y evitar copia de todo el sufijo por grupo |

Las nuevas operaciones Unicode validarán UTF-8 completo: rechazarán
continuaciones sueltas, sobrelargos, sustitutos, secuencias truncadas y valores
mayores de U+10FFFF. Un acento combinante y un emoji ZWJ demuestran que puntos
de código y grafemas no son lo mismo. UAX #29 se usará para diseñar la futura
API de grafemas; no se implementará una aproximación con ese nombre.

No se añade dependencia de ejecución. Segmentación de grafemas, normalización,
cambios de caja Unicode completos y colación lingüística se reservan para un
incremento con tablas Unicode versionadas y pruebas oficiales. Se documentará
la opción de una biblioteca pequeña, sin incorporarla aquí.

Los índices heredados finitos dentro del rango de int mantienen truncamiento
hacia cero. Los nuevos índices Unicode exigen enteros. La representación de
texto sigue terminada por NUL: no es una biblioteca para buffers binarios.

## Hallazgos adicionales verificados durante la implementación

El uso de la segunda interpretación de escapes reproduce un heap-buffer-overflow
con ASan para una secuencia literal \u incompleta y un desbordamiento entero
con UBSan para un escape numérico enorme. B1 valida esas entradas en la función
nativa antes de llamar al lector histórico; no modifica el lector usado por
el parser. Los logs y dos regresiones conservan la evidencia.

La revisión de concatenación demuestra además que una primera conversión puede
quedar sin liberar si la segunda falla por una colección cíclica. Una prueba
específica falla antes del arreglo y pasa después. La implementación nueva
toma prestados los textos existentes, usa conversiones diferidas para otros
valores y libera sus reservas antes de propagar el error.
