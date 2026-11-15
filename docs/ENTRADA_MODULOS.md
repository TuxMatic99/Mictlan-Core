# Entrada de datos y módulos en Mictlán

Una entrada de texto es una secuencia de bytes. Interpretar «0012» como el número
12 pierde los ceros iniciales: eso sirve para calcular, pero no para guardar un
código postal. Por eso hay tres funciones distintas. Ninguna recibe argumentos.

| Función | Resultado y uso | Vacío / EOF / error | Coste y límite |
|---|---|---|---|
| `leer()` | Infiere número si `strtod` consume todo; si no, cadena. Conserva la función de Latino. | Línea vacía → cadena vacía; EOF o fallo de lectura/reserva → nulo. Un error al publicar en la VM sí se propaga. | O(n) tiempo y memoria; sin límite artificial de 8 MiB; sujeto a representabilidad y memoria. |
| `leer_texto()` | Cadena exacta sin terminador de línea. Recomendada para nombres, identificadores y texto. | Vacío → cadena vacía; EOF → nulo; NUL, límite, lectura o reserva → error. | O(n), máximo 8 MiB de contenido. |
| `leer_numero()` | Número decimal finito, con punto y exponente opcional; acepta espacios ASCII exteriores. Recomendada para cálculos. | Vacío/texto no numérico → error; EOF → nulo; errores operativos → error. | O(n), máximo 8 MiB de contenido. |

LF termina una línea. CR inmediatamente anterior a LF también se retira; un CR
interior o un CR final sin LF se conserva. EOF con bytes entrega la última línea,
aunque no acabe en LF. No se devuelve una línea parcial cuando se detecta un
error de lectura. Las alternativas modernas distinguen nulo (fin normal) de un
error; en el ejecutable éste termina la ejecución. Un anfitrión C puede recibir
el estado de la llamada protegida y continuar en la misma VM. No se incorpora
una nueva sintaxis de excepciones.

La entrada redirigida no añade eco ni interpreta teclas de edición. `leer`
interactivo conserva Readline en Unix; las funciones nuevas leen stdin mediante
el lector común. Un terminal puede producir eco por su propia configuración.
La batería valida Linux con archivos/pipes sintéticos; no certifica todas las
configuraciones de terminal, Windows o macOS.

Las cadenas conservan UTF-8 byte por byte, sin normalización ni validación de
Unicode. «ñ» ocupa dos bytes; el límite se mide en bytes, no en letras ni grafemas.
Un NUL incrustado no es texto representable en estas cadenas: se rechaza y se
consume el resto de la línea. Una línea que excede el límite también se descarta
hasta LF cuando la fuente permite continuar. Una fuente que no termina la línea
puede seguir bloqueando mientras se descarta; no se promete timeout de E/S.

## Inferencia y conversiones

`leer()` acepta `0`, `-0`, ceros iniciales, notación científica, hexadecimal,
NaN e infinitos según `strtod`. Acepta espacios iniciales, pero un espacio final
impide consumir todo y produce una cadena. Desbordamientos producen infinitos y
subdesbordamientos pueden dar cero: no es una conversión estricta.

El ejecutable establece LC_NUMERIC=C: `1.5` es número, `1,5` es texto. En la API C,
`leer()` respeta el locale que establezca el anfitrión: en es_ES/fr_FR, `1,5` es
número y `1.5` es texto. Esto se conserva por compatibilidad. `leer_numero()` usa
siempre punto decimal, sin cambiar el locale global ni el del hilo.

`anumero(valor)` conserva el contrato histórico: devuelve nulo cuando el resultado
es cero o falla la conversión. Para programas nuevos, `anumero_estricto(valor)`
distingue cero de error. Acepta números finitos o cadenas decimales con signo,
punto y exponente opcional; admite espacios ASCII exteriores. Rechaza NaN,
infinito, hexadecimal, coma decimal, texto residual y una mantisa no nula que
se pierda a cero. Conserva cero negativo y subnormales representables. Las dos
funciones devuelven un valor nuevo y no modifican el argumento. La estricta es
O(n) para texto y O(1) para un número. `leer_numero()` reutiliza su reconocedor.

El modelo sigue siendo double binario: muchos decimales, como 0.1, se aproximan.
No se añade aritmética decimal exacta. Véase [P12B3_REFERENCIA.md](P12B3_REFERENCIA.md) y
la tabla completa de entrada en [P12B4B_DECISIONES.md](P12B4B_DECISIONES.md).

## Importar un módulo

`incluir(nombre)` recibe una cadena y ejecuta su archivo en la VM actual. No tiene
valor de retorno utilizable: úsalo como instrucción. No crea un espacio de nombres
nuevo ni cambia las reglas de ámbitos de P1.2-A. Las funciones que deban quedar
disponibles siguen esas reglas, incluida la declaración global cuando corresponde.

Sin extensión, busca primero todos los candidatos `.lat`: directorio de trabajo,
directorio del módulo llamador y LATINO_LIB. Después busca `.mict` en esos mismos
lugares. Por ello un `.lat` de LATINO_LIB tiene prioridad sobre un `.mict` local.
Con `.lat`/`.mict` explícito busca exactamente ese nombre. Una ruta absoluta no se
prefija con otros directorios. LATINO_LIB representa un solo directorio. Cada ruta
candidata debe caber en 1024 bytes incluido el terminador; las largas generan error.

Importar de nuevo ejecuta de nuevo: no hay caché. El módulo vacío, inexistente,
con sintaxis inválida o con error de ejecución falla; se restaura el estado del
llamador antes de propagar el error. Los efectos anteriores al fallo no se revierten.
No se promete una transacción. Un ciclo sigue consumiendo el presupuesto heredado
de llamadas: conviene evitarlo; no hay un nuevo detector de ciclos.

Se conserva el fallback a extensiones nativas cuando no se encuentra fuente y el
nombre no lleva un sufijo fuente explícito. Esta misión no modifica ese mecanismo;
la batería lo sustituye por una sonda que no carga ninguna biblioteca desconocida.
Para pedir sólo fuente y obtener un error de ausencia directo, escribe el sufijo.

El coste de resolver es un número constante de candidatos; después hay que leer,
analizar, compilar y ejecutar el archivo. La memoria incluye AST, bytecode, objetos
y nombres de fuente que puedan necesitar funciones retenidas. Los temporales se
liberan incluso en los errores cubiertos. No se impone un límite nuevo al tamaño
del programa ni se promete recuperación de todas las reservas internas del parser.

## Ejemplos comprobados

Cada bloque es un programa `.mict` y también existe idéntico como `.lat` en
`tests/entrada_modulos/programas`. La batería copia sólo estos programas y sus
módulos sintéticos a un directorio temporal, suministra la entrada indicada y
comprueba salida, diagnóstico, código de terminación y archivos finales.
Los textos JSON de entrada muestran explícitamente `\n` (LF) y `\r` (CR).
Las comillas exteriores de esos textos no forman parte de lo leído.

### leer(): tipos inferidos y EOF

```mict
a=leer()
poner(tipo(a))
poner(a)
a=leer()
poner(tipo(a))
poner(a)
a=leer()
poner(tipo(a))
poner(a)
a=leer()
poner(tipo(a))
poner(a)
a=leer()
poner(tipo(a))
poner(a)
a=leer()
poner(tipo(a))
poner(a)
a=leer()
poner(tipo(a))
poner(a)
poner(leer())
```

Entrada (JSON): `"0\n-0\n0012\n1.5\n1e3\nhola\n\n"`

Salida esperada:

```text
decimal
0
decimal
-0
decimal
12
decimal
1.5
decimal
1000
cadena
hola
cadena

nulo
```

### leer(): CRLF representa una sola línea

```mict
a=leer()
poner(tipo(a))
poner(a)
poner(leer())
```

Entrada (JSON): `"12\r\n"`

Salida esperada:

```text
decimal
12
nulo
```

### leer_texto(): conservar ceros, UTF-8 y líneas vacías

```mict
poner(leer_texto())
poner(leer_texto())
poner(leer_texto())
poner(leer_texto())
```

Entrada (JSON): `"0012\r\nMaría ñ\n\n"`

Salida esperada:

```text
0012
María ñ

nulo
```

### leer_numero(): punto decimal y cero negativo

```mict
poner(leer_numero())
poner(leer_numero())
poner(leer_numero())
```

Entrada (JSON): `" 1.5 \r\n-0"`

Salida esperada:

```text
1.5
-0
nulo
```

### leer_numero(): rechazo de coma decimal

```mict
leer_numero()
```

Entrada (JSON): `"1,5\n"`

Salida esperada:

```text
```

Finaliza con código 1 y diagnóstico que contiene `Conversion numerica estricta`.

### Distinguir EOF de una línea vacía

```mict
poner(leer())
poner(leer_texto())
poner(leer_numero())
```

Entrada (JSON): `""`

Salida esperada:

```text
nulo
nulo
nulo
```

### incluir(): prioridad heredada y sufijo explícito

```mict
incluir("uno")
incluir("uno.mict")
```

Archivo sintético `lib/uno.lat`:

```mict
poner("lat")
```

Archivo sintético `uno.mict`:

```mict
poner("mict")
```

Entrada (JSON): `""`

Salida esperada:

```text
lat
mict
```

### incluir(): repetir también repite los efectos

```mict
incluir("uno.lat")
incluir("uno.lat")
```

Archivo sintético `uno.lat`:

```mict
poner("dos")
```

Entrada (JSON): `""`

Salida esperada:

```text
dos
dos
```

### incluir(): resolver desde el módulo llamador

```mict
incluir("a/uno.mict")
```

Archivo sintético `a/uno.mict`:

```mict
incluir("dos")
poner("A")
```

Archivo sintético `a/dos.lat`:

```mict
incluir("tres.mict")
poner("B")
```

Archivo sintético `a/tres.mict`:

```mict
poner("C")
```

Entrada (JSON): `""`

Salida esperada:

```text
C
B
A
```

### incluir(): diagnóstico del archivo que falla

```mict
incluir("a/uno.mict")
poner("no llegar")
```

Archivo sintético `a/uno.mict`:

```mict
incluir("dos.lat")
```

Archivo sintético `a/dos.lat`:

```mict
error("desde dos")
```

Entrada (JSON): `""`

Salida esperada:

```text
```

Finaliza con código 1 y diagnóstico que contiene `a/dos.lat:1:`.

### incluir(): error de análisis

```mict
incluir("uno.mict")
```

Archivo sintético `uno.mict`:

```mict
fin
```

Entrada (JSON): `""`

Salida esperada:

```text
```

Finaliza con código 1 y diagnóstico que contiene `Error de sintaxis`.

### incluir(): fuente ausente

```mict
incluir("ausente.mict")
```

Entrada (JSON): `""`

Salida esperada:

```text
```

Finaliza con código 1 y diagnóstico que contiene `No se encontró el módulo`.

### anumero() y anumero_estricto(): distinguir cero de fracaso

```mict
poner(anumero("0"))
poner(anumero_estricto("0"))
poner(anumero_estricto(" 1.5 "))
poner(anumero_estricto("-0"))
```

Entrada (JSON): `""`

Salida esperada:

```text
nulo
0
1.5
-0
```
