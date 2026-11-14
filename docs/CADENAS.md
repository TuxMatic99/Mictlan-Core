# Referencia de cadenas de Mictlán

P1.2-B1 · 13 de noviembre de 2026. Las firmas son iguales en `.lat` y `.mict`.
El [inventario completo](P12B1_INVENTARIO.md) distingue los contratos heredados.

## Bytes, puntos de código y grafemas

Un byte es una unidad de almacenamiento. UTF-8 usa de uno a cuatro bytes por
punto de código Unicode. Un grafema reúne los puntos que normalmente vemos
como un solo carácter. «e» seguida de acento combinante ocupa dos puntos;
un emoji unido con ZWJ también puede contener varios.

| Texto | Bytes UTF-8 | Puntos de código | Grafemas de ejemplo según UAX #29, sin API implementada |
|---|---:|---:|---:|
| niño | 5 | 4 | 4 |
| é | 2 | 1 | 1 |
| é | 3 | 2 | 1 |
| 👩‍💻 | 11 | 3 | 1 |
| 🇲🇽 | 8 | 2 | 1 |

`longitud` conserva bytes y sus otros tipos históricos. Para código nuevo,
`longitud_bytes` exige texto y `longitud_unicode` valida UTF-8 y cuenta puntos.
No hay todavía una función `longitud_grafemas`.

La [referencia UAX #29](https://www.unicode.org/reports/tr29/) define reglas de
segmentación de grafemas; contar bytes o puntos no implementa esas reglas.
B1 no añade normalización, caja Unicode completa ni comparación lingüística.
`mayusculas_ascii` deja ñ y á intactas; el nombre hace explícito ese límite.

Los índices heredados están basados en cero y en bytes. Los nuevos índices
Unicode están basados en cero y en puntos de código y exigen enteros no
negativos. Una cantidad que sobrepasa el resto se limita al resto del texto.
Las cadenas siguen terminadas por NUL: no son buffers binarios con ceros internos.

## Ejemplos verificados

Cada bloque siguiente se genera desde un resultado esperado explícito de
`tests/cadenas/api.json`, forma parte de `documentacion.mict` y se ejecuta con
ambas extensiones. El generador no consulta resultados del intérprete.


## cadena.char

Construye texto a partir de valores de byte.

**Firma:** `cadena.char(numero | lista_de_numeros)`. **Devuelve:** cadena; otros tipos no devuelven valor.

```mict
escribir(cadena.char([65, 0, 66]))
```

Resultado:

```text
AB
```

**Vacío:** Lista vacía o byte cero -> texto vacío. **Argumentos inválidos:** Números no finitos/fuera de int y elementos no numéricos -> error. Otros tipos: sin retorno.

**Texto e índices:** Opera por bytes; no valida UTF-8. No aplica.

**Casos límite y compatibilidad:** Conserva truncamiento numérico y conversión C a char. Los ceros de una lista se omiten; no hay NUL interior.

**Recomendación:** Para texto Unicode, escribir literales UTF-8; char no recibe puntos de código.

## cadena.bytes

Muestra los valores de los bytes del texto.

**Firma:** `cadena.bytes(texto | nulo)`. **Devuelve:** lista; nulo -> nulo.

```mict
escribir(cadena.bytes("AB"))
```

Resultado:

```text
[65, 66]
```

**Vacío:** Texto vacío -> lista vacía. **Argumentos inválidos:** Admite cadena o nulo; otros tipos producen error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Orden de bytes desde cero.

**Casos límite y compatibilidad:** Los bytes mayores de 127 conservan el signo de char de la plataforma; en Debian x86_64 pueden ser negativos.

**Recomendación:** Para medir texto, elegir longitud_bytes o longitud_unicode.

## cadena.esta_vacia

Comprueba si el texto no contiene bytes.

**Firma:** `cadena.esta_vacia(texto)`. **Devuelve:** lógico.

```mict
escribir(cadena.esta_vacia(""))
```

Resultado:

```text
verdadero
```

**Vacío:** Verdadero. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. No aplica.

**Casos límite y compatibilidad:** Un espacio no está vacío; recortar primero si ese es el criterio.

**Recomendación:** Usar con el contrato descrito.

## cadena.longitud

Cuenta bytes de texto o entradas de un diccionario.

**Firma:** `cadena.longitud(valor)`. **Devuelve:** numérico.

```mict
escribir(cadena.longitud("niño"))
```

Resultado:

```text
5
```

**Vacío:** Texto vacío -> 0. **Argumentos inválidos:** Otros tipos, incluido nulo, devuelven 0.

**Texto e índices:** Opera por bytes; no valida UTF-8. No devuelve posición.

**Casos límite y compatibilidad:** Conserva polimorfismo histórico. No cuenta letras ni grafemas.

**Recomendación:** Para código nuevo, longitud_bytes o longitud_unicode.

## cadena.minusculas

Convierte cada byte con la función C tolower.

**Firma:** `cadena.minusculas(texto)`. **Devuelve:** cadena.

```mict
escribir(cadena.minusculas("HOLA"))
```

Resultado:

```text
hola
```

**Vacío:** Texto vacío. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** C por byte y locale; no es caja Unicode. No aplica.

**Casos límite y compatibilidad:** En C.UTF-8 no transforma Ñ ni Á. Se elimina el uso inválido de char negativo en ctype.

**Recomendación:** minusculas_ascii para ASCII determinista; caja Unicode completa queda pendiente.

## cadena.mayusculas

Convierte cada byte con la función C toupper.

**Firma:** `cadena.mayusculas(texto)`. **Devuelve:** cadena.

```mict
escribir(cadena.mayusculas("hola"))
```

Resultado:

```text
HOLA
```

**Vacío:** Texto vacío. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** C por byte y locale; no es caja Unicode. No aplica.

**Casos límite y compatibilidad:** En C.UTF-8 no transforma ñ ni á. La entrada no se modifica.

**Recomendación:** mayusculas_ascii para ASCII determinista; caja Unicode completa queda pendiente.

## cadena.recortar

Quita espacios C al principio y al final.

**Firma:** `cadena.recortar(texto)`. **Devuelve:** cadena.

```mict
escribir("[" .. cadena.recortar("  hola\t") .. "]")
```

Resultado:

```text
[hola]
```

**Vacío:** Texto vacío; todo espacio C -> vacío. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** isspace por byte y locale. No aplica.

**Casos límite y compatibilidad:** Quita espacio, tabulación y saltos C; no afirma reconocer todos los espacios Unicode, como NBSP. No cambia el texto original.

**Recomendación:** Usar con el contrato descrito.

## cadena.es_numerico

Comprueba si la conversión numérica C consume todo el texto.

**Firma:** `cadena.es_numerico(texto)`. **Devuelve:** lógico.

```mict
escribir(cadena.es_numerico("5.25"))
```

Resultado:

```text
verdadero
```

**Vacío:** Verdadero, por contrato heredado. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Conversión numérica con locale C. No aplica.

**Casos límite y compatibilidad:** También admite notación científica y no finitos reconocidos por strtod. Vacío da verdadero aunque no haya dígitos.

**Recomendación:** Para entrada numérica de usuario, comprobar además no vacío y finitud.

## cadena.es_numero

Comprueba si la conversión numérica C consume todo el texto.

**Firma:** `cadena.es_numero(texto)`. **Devuelve:** lógico.

```mict
escribir(cadena.es_numero("5.25"))
```

Resultado:

```text
verdadero
```

**Vacío:** Verdadero, por contrato heredado. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Conversión numérica con locale C. No aplica.

**Casos límite y compatibilidad:** También admite notación científica y no finitos reconocidos por strtod. Vacío da verdadero aunque no haya dígitos.

**Recomendación:** Para entrada numérica de usuario, comprobar además no vacío y finitud.

## cadena.es_alfa

Comprueba si todos los bytes son alfanuméricos C.

**Firma:** `cadena.es_alfa(texto)`. **Devuelve:** lógico.

```mict
escribir(cadena.es_alfa("abc123"))
```

Resultado:

```text
verdadero
```

**Vacío:** Verdadero. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** isalnum por byte y locale; no clasificación Unicode. No aplica.

**Casos límite y compatibilidad:** El nombre incluye números. En C.UTF-8, una letra multibyte puede devolver falso. No usar como validador de nombres españoles.

**Recomendación:** Usar con el contrato descrito.

## cadena.invertir

Invierte el orden de los bytes.

**Firma:** `cadena.invertir(texto)`. **Devuelve:** cadena.

```mict
escribir(cadena.invertir("abc"))
```

Resultado:

```text
cba
```

**Vacío:** Texto vacío. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. De último byte a primero.

**Casos límite y compatibilidad:** Puede producir UTF-8 inválido. Se corrige la fuga de su buffer sin cambiar los bytes resultantes.

**Recomendación:** invertir_unicode para puntos de código; tampoco equivale a invertir grafemas.

## cadena.ejecutar

Compila y ejecuta texto como código de Mictlán.

**Firma:** `cadena.ejecutar(codigo)`. **Devuelve:** sin retorno propio.

```mict
cadena.ejecutar("escribir(42)")
```

Resultado:

```text
42
```

**Vacío:** Texto vacío -> error de ejecución. **Argumentos inválidos:** Exige cadena; tipo distinto produce error. Sintaxis inválida -> error.

**Texto e índices:** UTF-8 del compilador; no transforma texto. Diagnósticos de la fuente ejecutada.

**Casos límite y compatibilidad:** Puede producir salida y efectos del programa. Se conserva la propagación heredada de errores de la llamada interna; no se rediseña en B1.

**Recomendación:** Para lógica fija, declarar y llamar funciones.

## cadena.concatenar

Une las representaciones textuales de dos valores.

**Firma:** `cadena.concatenar(a, b)`. **Devuelve:** cadena.

```mict
escribir(cadena.concatenar("a", 4))
```

Resultado:

```text
a4
```

**Vacío:** Dos textos vacíos -> vacío. **Argumentos inválidos:** Acepta valores convertibles por acadena; conversiones incompatibles -> error.

**Texto e índices:** Copia bytes; no normaliza. No aplica.

**Casos límite y compatibilidad:** También acepta números y colecciones convertibles; conserva el contrato de la operación .. . Se libera la primera conversión si falla la segunda y se evitan copias intermedias de cadenas prestadas.

**Recomendación:** Usar con el contrato descrito.

## cadena.comparar

Compara dos textos por su orden de bytes.

**Firma:** `cadena.comparar(a, b)`. **Devuelve:** numérico: negativo, cero o positivo.

```mict
escribir(cadena.comparar("a", "z") < 0)
```

Resultado:

```text
verdadero
```

**Vacío:** Vacío contra vacío -> 0. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. No devuelve posición.

**Casos límite y compatibilidad:** No garantiza exactamente -1/1; en Debian a frente a z puede dar -25. No es orden de diccionario español.

**Recomendación:** Comprobar el signo; para igualdad usar es_igual.

## cadena.contiene

Indica si aparece una secuencia exacta de bytes.

**Firma:** `cadena.contiene(texto, fragmento)`. **Devuelve:** lógico.

```mict
escribir(cadena.contiene("mañana", "ña"))
```

Resultado:

```text
verdadero
```

**Vacío:** Fragmento vacío -> verdadero, incluso en texto vacío. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. No devuelve posición.

**Casos límite y compatibilidad:** Distingue mayúsculas y representaciones Unicode equivalentes; no normaliza.

**Recomendación:** Usar con el contrato descrito.

## cadena.termina_con

Comprueba un final exacto.

**Firma:** `cadena.termina_con(texto, sufijo)`. **Devuelve:** lógico.

```mict
escribir(cadena.termina_con("mañana", "na"))
```

Resultado:

```text
verdadero
```

**Vacío:** Sufijo vacío -> verdadero. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Compara el final en bytes.

**Casos límite y compatibilidad:** Un sufijo mayor que el texto devuelve falso. Distingue mayúsculas.

**Recomendación:** Usar con el contrato descrito.

## cadena.es_igual

Comprueba igualdad exacta entre dos textos.

**Firma:** `cadena.es_igual(a, b)`. **Devuelve:** lógico.

```mict
escribir(cadena.es_igual("é", "é"))
```

Resultado:

```text
falso
```

**Vacío:** Dos vacíos -> verdadero. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. No aplica.

**Casos límite y compatibilidad:** No aplica normalización, case folding ni colación. Las dos formas visuales del ejemplo tienen bytes diferentes.

**Recomendación:** Usar con el contrato descrito.

## cadena.indice

Busca la primera aparición exacta.

**Firma:** `cadena.indice(texto, fragmento)`. **Devuelve:** numérico.

```mict
escribir(cadena.indice("niño", "o"))
```

Resultado:

```text
4
```

**Vacío:** Fragmento vacío -> 0. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Cero inicial; posición en bytes; -1 si no aparece.

**Casos límite y compatibilidad:** Los aliases son idénticos; los índices no miden puntos de código.

**Recomendación:** indice_unicode para posiciones en puntos de código.

## cadena.encontrar

Busca la primera aparición exacta.

**Firma:** `cadena.encontrar(texto, fragmento)`. **Devuelve:** numérico.

```mict
escribir(cadena.encontrar("niño", "o"))
```

Resultado:

```text
4
```

**Vacío:** Fragmento vacío -> 0. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Cero inicial; posición en bytes; -1 si no aparece.

**Casos límite y compatibilidad:** Los aliases son idénticos; los índices no miden puntos de código.

**Recomendación:** indice_unicode para posiciones en puntos de código.

## cadena.ultimo_indice

Busca la última aparición, incluidas las solapadas.

**Firma:** `cadena.ultimo_indice(texto, fragmento)`. **Devuelve:** numérico.

```mict
escribir(cadena.ultimo_indice("ababa", "aba"))
```

Resultado:

```text
2
```

**Vacío:** Fragmento vacío -> longitud en bytes del texto. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Cero inicial; bytes; -1 si no aparece.

**Casos límite y compatibilidad:** La búsqueda preserva solapamientos. No admite posición de inicio adicional.

**Recomendación:** Usar con el contrato descrito.

## cadena.eliminar

Quita la primera aparición exacta.

**Firma:** `cadena.eliminar(texto, fragmento)`. **Devuelve:** cadena.

```mict
escribir(cadena.eliminar("aaaa", "aa"))
```

Resultado:

```text
aa
```

**Vacío:** Patrón vacío o ausente -> texto sin cambios. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Selecciona primera coincidencia en bytes.

**Casos límite y compatibilidad:** No quita todas las coincidencias. La cadena original se conserva. Se corrigen reserva excesiva y fuga.

**Recomendación:** Usar con el contrato descrito.

## cadena.separar

Divide por cualquiera de los bytes delimitadores.

**Firma:** `cadena.separar(texto, delimitadores = nulo)`. **Devuelve:** lista de cadenas.

```mict
escribir(cadena.separar("a::b:c", "::"))
```

Resultado:

```text
["a", "b", "c"]
```

**Vacío:** Texto vacío -> []; delimitador vacío/omitido -> espacio ASCII. **Argumentos inválidos:** Exige cadena; tipo distinto produce error. Segundo argumento admite nulo.

**Texto e índices:** Opera por bytes; no valida UTF-8. Descarta campos vacíos y colapsa delimitadores consecutivos.

**Casos límite y compatibilidad:** El delimitador no es una subcadena completa. Un delimitador multibyte puede cortar dentro de UTF-8. Solo el espacio ASCII es predeterminado.

**Recomendación:** separar_texto para una secuencia completa y campos vacíos.

## cadena.inicia_con

Comprueba un principio exacto.

**Firma:** `cadena.inicia_con(texto, prefijo)`. **Devuelve:** lógico.

```mict
escribir(cadena.inicia_con("mañana", "ma"))
```

Resultado:

```text
verdadero
```

**Vacío:** Prefijo vacío -> verdadero. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Compara desde byte cero.

**Casos límite y compatibilidad:** Prefijo ausente o mayor -> falso. Se elimina la resta inválida de NULL sin cambiar el resultado válido.

**Recomendación:** Usar con el contrato descrito.

## cadena.regexl

Busca una coincidencia con expresión regular POSIX ERE.

**Firma:** `cadena.regexl(texto | numero, patron)`. **Devuelve:** lógico.

```mict
escribir(cadena.regexl("abc", "^a"))
```

Resultado:

```text
verdadero
```

**Vacío:** Depende del patrón; patrón vacío coincide. **Argumentos inválidos:** Texto o numérico; patrón debe ser cadena válida para regcomp.

**Texto e índices:** Motor POSIX y locale; no contrato Unicode completo. No devuelve posiciones.

**Casos límite y compatibilidad:** Convierte números con %.16g. Error de compilación del patrón -> error. No modifica el motor ni su interpretación en B1.

**Recomendación:** Usar con el contrato descrito.

## cadena.regex

Devuelve coincidencias y grupos de una expresión POSIX ERE.

**Firma:** `cadena.regex(texto, patron)`. **Devuelve:** lista de listas de cadenas.

```mict
escribir(cadena.regex("aba", "a"))
```

Resultado:

```text
[["a"], ["a"]]
```

**Vacío:** Vacío suele dar []; grupos vacíos se omiten. **Argumentos inválidos:** Ambos textos; patrón inválido -> error.

**Texto e índices:** Motor POSIX y locale. Recorre coincidencias desde el inicio.

**Casos límite y compatibilidad:** Límites heredados: 50 coincidencias y 50 grupos. Los patrones de anchura cero conservan el límite y tratamiento anterior. Se libera cada temporal y se elimina la copia del sufijo completo.

**Recomendación:** Usar con el contrato descrito.

## cadena.insertar

Inserta texto en una posición de byte.

**Firma:** `cadena.insertar(texto, nuevo, posicion)`. **Devuelve:** cadena.

```mict
escribir(cadena.insertar("abc", "X", -1))
```

Resultado:

```text
abXc
```

**Vacío:** Insertar en vacío con posición cero produce nuevo. **Argumentos inválidos:** Cadenas y número finito dentro de int; posición anterior al inicio -> error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Cero inicial; negativos desde final; exceso positivo -> final.

**Casos límite y compatibilidad:** Las fracciones se truncan hacia cero. No distingue límites UTF-8. Se libera el temporal.

**Recomendación:** Para texto Unicode, usar posiciones calculadas y evitar cortar secuencias; inserción Unicode queda propuesta.

## cadena.rellenar_izquierda

Añade repeticiones del relleno al lado indicado.

**Firma:** `cadena.rellenar_izquierda(texto, relleno, n)`. **Devuelve:** cadena.

```mict
escribir(cadena.rellenar_izquierda("ab", "x", 4))
```

Resultado:

```text
xxxab
```

**Vacío:** Si texto está vacío, repite n+1 veces. Relleno vacío no añade bytes. **Argumentos inválidos:** n finito dentro de int y no negativo; tipos incorrectos -> error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Repeticiones = max(0, trunc(n) - bytes(texto) + 1).

**Casos límite y compatibilidad:** Conserva el desfase de uno y repite el relleno completo; un relleno multibyte necesita más memoria que uno ASCII. Reserva exacta con comprobación de desbordamiento.

**Recomendación:** No interpretar n como longitud visual final; consultar la fórmula.

## cadena.rellenar_derecha

Añade repeticiones del relleno al lado indicado.

**Firma:** `cadena.rellenar_derecha(texto, relleno, n)`. **Devuelve:** cadena.

```mict
escribir(cadena.rellenar_derecha("ab", "x", 4))
```

Resultado:

```text
abxxx
```

**Vacío:** Si texto está vacío, repite n+1 veces. Relleno vacío no añade bytes. **Argumentos inválidos:** n finito dentro de int y no negativo; tipos incorrectos -> error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Repeticiones = max(0, trunc(n) - bytes(texto) + 1).

**Casos límite y compatibilidad:** Conserva el desfase de uno y repite el relleno completo; un relleno multibyte necesita más memoria que uno ASCII. Reserva exacta con comprobación de desbordamiento.

**Recomendación:** No interpretar n como longitud visual final; consultar la fórmula.

## cadena.reemplazar

Sustituye coincidencias exactas no solapadas.

**Firma:** `cadena.reemplazar(texto, buscado, nuevo, veces = nulo)`. **Devuelve:** cadena.

```mict
escribir(cadena.reemplazar("aaaa", "a", "x", 2))
```

Resultado:

```text
xxaa
```

**Vacío:** Texto vacío -> vacío si buscado no vacío; buscado vacío -> error. **Argumentos inválidos:** Cadenas y cantidad finita dentro de int; patrón vacío o escapes inseguros -> error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Cuarto argumento es cantidad: positivo limita; 0, negativo u omitido -> todas.

**Casos límite y compatibilidad:** Conserva la segunda interpretación heredada de escapes en buscado y nuevo. Se rechazan \u y escapes numéricos fuera de int antes de reservar. No se modifica el parser. Se liberan los tres temporales.

**Recomendación:** Usar textos literales sin barras si se busca reemplazo simple; reemplazo literal explícito queda propuesto.

## cadena.subcadena

Copia una cantidad de bytes desde el inicio indicado.

**Firma:** `cadena.subcadena(texto, inicio, cantidad)`. **Devuelve:** cadena.

```mict
escribir(cadena.subcadena("abcd", 1, 2))
```

Resultado:

```text
bc
```

**Vacío:** Inicio cero y cualquier cantidad no negativa -> vacío. **Argumentos inválidos:** Números finitos dentro de int; inicio/cantidad negativos o inicio posterior al final -> error.

**Texto e índices:** Opera por bytes; no valida UTF-8. Cero inicial; inicio hasta bytes(texto); cantidad se limita al resto.

**Casos límite y compatibilidad:** Trunca fracciones hacia cero; no convierte índices negativos desde el final. Puede cortar UTF-8.

**Recomendación:** subcadena_unicode para límites de puntos de código.

## cadena.formato

Construye texto con marcadores de formato.

**Firma:** `cadena.formato(plantilla, ...valores)`. **Devuelve:** cadena; sin argumentos -> nulo.

```mict
escribir(cadena.formato("%s:%i", "hola", 42))
```

Resultado:

```text
hola:42
```

**Vacío:** Plantilla vacía -> texto vacío. **Argumentos inválidos:** Plantilla texto; opción no admitida o argumentos insuficientes -> error.

**Texto e índices:** Literal por bytes; conversiones numéricas heredadas. Consume marcadores en orden.

**Casos límite y compatibilidad:** Admite %c %i %f %d %o %x %e %s y %%. Resultado menor que MAX_STR_LENGTH. El formato de números no equivale a colación ni localización completa.

**Recomendación:** Usar con el contrato descrito.

## cadena.longitud_bytes

Cuenta los bytes sin aceptar otros tipos de valor.

**Firma:** `cadena.longitud_bytes(texto)`. **Devuelve:** numérico.

```mict
escribir(cadena.longitud_bytes("niño"))
```

Resultado:

```text
5
```

**Vacío:** 0. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Opera por bytes; no valida UTF-8. No devuelve posición.

**Casos límite y compatibilidad:** Es explícita y estricta. No valida UTF-8 ni cuenta grafemas.

**Recomendación:** Usar para almacenamiento; longitud_unicode para escalares.

## cadena.longitud_unicode

Cuenta puntos de código de UTF-8 válido.

**Firma:** `cadena.longitud_unicode(texto)`. **Devuelve:** numérico.

```mict
escribir(cadena.longitud_unicode("é"))
```

Resultado:

```text
2
```

**Vacío:** 0. **Argumentos inválidos:** Exige cadena; tipo distinto produce error. UTF-8 inválido -> error.

**Texto e índices:** Valida UTF-8 completo; cuenta escalares, no grafemas. No devuelve posición.

**Casos límite y compatibilidad:** La e y el acento combinante son dos puntos de código y un grafema. No aplica normalización.

**Recomendación:** Para letras percibidas se necesitará longitud_grafemas, aún propuesta.

## cadena.invertir_unicode

Invierte unidades de punto de código conservando cada secuencia UTF-8.

**Firma:** `cadena.invertir_unicode(texto)`. **Devuelve:** cadena.

```mict
escribir(cadena.invertir_unicode("año😀"))
```

Resultado:

```text
😀oña
```

**Vacío:** Texto vacío. **Argumentos inválidos:** Exige cadena; tipo distinto produce error. UTF-8 inválido -> error.

**Texto e índices:** Valida UTF-8 completo; cuenta escalares, no grafemas. Inversión por escalares, no por bytes.

**Casos límite y compatibilidad:** No conserva grafemas: al invertir e seguida de acento, el acento queda primero. Un emoji ZWJ puede dejar de representar la misma figura.

**Recomendación:** No usar para invertir visualmente texto con grafemas complejos.

## cadena.subcadena_unicode

Copia una cantidad de puntos de código completos.

**Firma:** `cadena.subcadena_unicode(texto, inicio, cantidad)`. **Devuelve:** cadena.

```mict
escribir(cadena.subcadena_unicode("mañana", 1, 3))
```

Resultado:

```text
aña
```

**Vacío:** Inicio cero -> vacío. **Argumentos inválidos:** Enteros no negativos hasta INT_MAX; UTF-8 inválido o inicio posterior al final -> error.

**Texto e índices:** Valida UTF-8 completo; cuenta escalares, no grafemas. Cero inicial; inicio hasta longitud_unicode; cantidad se limita al resto.

**Casos límite y compatibilidad:** No corta una secuencia UTF-8, pero sí puede separar un acento de su base. No admite índices negativos.

**Recomendación:** Usar con el contrato descrito.

## cadena.indice_unicode

Busca bytes exactos y expresa la posición en puntos de código.

**Firma:** `cadena.indice_unicode(texto, fragmento)`. **Devuelve:** numérico.

```mict
escribir(cadena.indice_unicode("niño", "o"))
```

Resultado:

```text
3
```

**Vacío:** Fragmento vacío -> 0. **Argumentos inválidos:** Exige cadena; tipo distinto produce error. Valida ambos textos como UTF-8.

**Texto e índices:** Valida UTF-8 completo; cuenta escalares, no grafemas. Cero inicial; -1 si no aparece.

**Casos límite y compatibilidad:** No normaliza formas equivalentes ni busca por colación. Un fragmento válido empieza en un límite UTF-8.

**Recomendación:** Usar con el contrato descrito.

## cadena.separar_texto

Divide por una subcadena completa, conservando campos vacíos.

**Firma:** `cadena.separar_texto(texto, separador)`. **Devuelve:** lista de cadenas.

```mict
escribir(cadena.separar_texto("::a::::", "::"))
```

Resultado:

```text
["", "a", "", ""]
```

**Vacío:** Texto vacío -> [""]; separador vacío -> error. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Secuencia de bytes exacta; textos y separador UTF-8 completos preservan sus límites. Coincidencias de separador no solapadas.

**Casos límite y compatibilidad:** No usa expresión regular ni conjunto de delimitadores. No valida UTF-8 y no es analizador CSV con comillas.

**Recomendación:** Usar con el contrato descrito.

## cadena.separar_delimitadores

Da un nombre explícito a la separación heredada.

**Firma:** `cadena.separar_delimitadores(texto, delimitadores = nulo)`. **Devuelve:** lista de cadenas.

```mict
escribir(cadena.separar_delimitadores("a,,b;c,", ",;"))
```

Resultado:

```text
["a", "b", "c"]
```

**Vacío:** Misma regla que separar. **Argumentos inválidos:** Exige cadena; tipo distinto produce error. Segundo argumento admite nulo.

**Texto e índices:** Opera por bytes; no valida UTF-8. Cero o más campos no vacíos.

**Casos límite y compatibilidad:** Alias exacto de separar: los delimitadores son bytes. Recomendar delimitadores ASCII.

**Recomendación:** separar_texto para delimitadores multibyte completos.

## cadena.mayusculas_ascii

Convierte únicamente a-z a A-Z, de forma independiente del locale.

**Firma:** `cadena.mayusculas_ascii(texto)`. **Devuelve:** cadena.

```mict
escribir(cadena.mayusculas_ascii("mañana"))
```

Resultado:

```text
MAñANA
```

**Vacío:** Texto vacío. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Solo ASCII; conserva los demás bytes. No aplica.

**Casos límite y compatibilidad:** No convierte ñ ni letras con tilde; no se presenta como caja Unicode.

**Recomendación:** Caja Unicode completa queda propuesta con dependencia versionada.

## cadena.minusculas_ascii

Convierte únicamente A-Z a a-z, de forma independiente del locale.

**Firma:** `cadena.minusculas_ascii(texto)`. **Devuelve:** cadena.

```mict
escribir(cadena.minusculas_ascii("MAÑANA"))
```

Resultado:

```text
maÑana
```

**Vacío:** Texto vacío. **Argumentos inválidos:** Exige cadena; tipo distinto produce error.

**Texto e índices:** Solo ASCII; conserva los demás bytes. No aplica.

**Casos límite y compatibilidad:** No convierte Ñ ni letras con tilde; no se presenta como caja Unicode.

**Recomendación:** Caja Unicode completa queda propuesta con dependencia versionada.
