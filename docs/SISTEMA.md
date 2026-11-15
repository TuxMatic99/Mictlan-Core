# Biblioteca de sistema de Mictlán

`sis` conecta el programa con el reloj, el entorno y algunas funciones del sistema
operativo. La referencia A0 verifica Linux Debian 13 y Linux Ubuntu de CI. Una
función puede depender del sistema: saber que existe no garantiza el mismo
resultado en Windows o macOS. Los contratos históricos que resultan poco intuitivos
se explican expresamente; no se ha creado una API de procesos nueva.

## Reloj y calendario

Un instante Unix es un número de segundos desde el 1 de enero de 1970, 00:00:00
UTC. El mismo instante puede mostrarse con distinta hora según la zona local.
Es parecido a escribir una misma distancia en unidades distintas: el acontecimiento
no cambia, su representación sí. La precisión numérica del lenguaje es double;
no todos los enteros mayores que 2^53 son exactamente representables.

` sis.tiempo(formato, instante) ` devuelve un número si formato es nulo y una
cadena o nulo si formato es cadena. El número sin formato es el reloj actual;
**ignora el segundo argumento**, por compatibilidad. Con formato, instante nulo
usa ahora y un número usa ese instante, truncando la fracción hacia cero. Así,
1.9 se convierte en 1 y -1.9 en -1. Se comprueban finitud, rango de time_t y
posibilidad de representarlo como calendario. Fallos de reloj/conversión producen
error; una cadena de formato vacía o una salida que no cabe en 64 bytes devuelve
nulo. No significa necesariamente que el instante sea inválido. La zona es local;
los nombres de meses/días y ciertos formatos dependen de LC_TIME/libc. Véanse los
ejemplos tiempo_epoch, tiempo_trunc y tiempo_vacio.

`sis.fecha(selector)` consulta el reloj actual. Los selectores de la tabla son
cadenas; otros selectores o tipos producen error. Fallos de reloj, calendario o
formateo producen error recuperable. Los casos C fijan un reloj de año bisiesto
para verificar valores exactos; el ejemplo CLI fecha_iso comprueba tipo y longitud.

| Selector | Resultado | Regla |
|---|---|---|
| `seg`, `min`, `hora` | Número | Campos locales; segundos pueden incluir segundo intercalar si libc lo admite |
| `d_mes`, `mes` | Número | Día y mes empiezan en 1 |
| `sem` | Número | Semana ISO `%V`, de 1 a 53; su año ISO puede diferir del año civil |
| `año` | Número | Año civil local |
| `d_sem` | Número | Domingo=1, lunes=2, … sábado=7 |
| `d_año` | Número | 1 de enero=1; puede llegar a 366 |
| `horario_de_verano` | Booleano | Conversión histórica de tm_isdst: cero es falso; incluso -1 (desconocido) es verdadero. No usarlo como detector universal de zona |
| `iso` | Cadena | UTC, forma `2024-02-29T23:59:59Z`; no consulta calendario local |

## Esperas, señales y terminal

`sis.dormir(segundos)` no devuelve un valor utilizable. Acepta número finito no
negativo, trunca a milisegundos y espera. En Linux el número de milisegundos
truncado debe caber en int (máximo 2147483647 en la plataforma comprobada).
Interrupciones reanudan el tiempo restante; un error operativo se propaga. La
planificación puede prolongar la espera: no es un reloj de precisión. La prueba
de límite sustituye nanosleep para no esperar semanas. En Windows se conserva
DWORD, cuya validación de plataforma queda pendiente. Ejemplo dormir_cero.

`sis.avisar(numero)` registra el manejador histórico de una señal: trunca a int,
comprueba rango y el resultado de signal. Devuelve falso si no se ha observado
ninguna señal o el último número observado. Ese estado **es global y persiste**
entre llamadas y VM. No es una cola ni un mecanismo de sincronización por VM.
Los números dependen del sistema y hay señales que no se pueden interceptar. El
ejemplo avisar_invalido demuestra el rechazo de 0 en Linux; los casos válidos y
la notificación se comprueban con un proveedor C controlado, sin enviar señales.

`sis.iraxy(primero, segundo)` no devuelve valor. Trunca ambos números finitos a
int y conserva el orden heredado. En Unix imprime ESC[primero;segundof; ejemplo
iraxy_fraccion muestra los bytes escapados. Esto suele mover el cursor en un
terminal compatible; una salida redirigida sólo contiene esos bytes. No garantiza
que unas coordenadas existan en pantalla. En Windows COORD y su orden/rango
históricos necesitan revisión separada; la matriz Linux no los certifica.

`sis.salir()` termina todo el proceso con estado 0. El código posterior no se
ejecuta. No es un retorno de función ni un error recuperable. El ejemplo salir
se ejecuta exclusivamente en su proceso de prueba.

## Directorio, usuario y plataforma

| Firma | Resultado y ejemplo | Límites y errores |
|---|---|---|
| `sis.cwd()` | Cadena del directorio actual; ejemplo cwd | nulo si getcwd falla o no cabe en 1024 bytes; no cambia directorio |
| `sis.usuario()` | USER en Unix, username en Windows; ejemplos usuario/usuario_ausente | Cadena, incluso vacía, o nulo si no existe la variable. Es un dato del entorno, no una comprobación de identidad |
| `sis.operativo(nombre)` | Booleano por igualdad exacta con constante de compilación; ejemplo os | Exige cadena; distingue mayúsculas. No identifica la distribución Linux |
| `sis.os(nombre)` | Alias de operativo; ejemplo os | Mismo contrato y función nativa |
| `sis.os_version_mayor()` | Número del primer segmento; ejemplo versiones | Linux: versión del kernel, no versión de Debian |
| `sis.os_version_menor()` | Número del segundo segmento; ejemplo versiones | Segmento ausente/no válido se queda en cero |
| `sis.os_version_parche()` | Número del tercer segmento; ejemplo versiones | Sólo segmentos decimales separados por punto: 6.8-12 → 6,8,0; 6.8.12-sufijo → 6,8,12 |

Las funciones de versión devuelven ceros si uname falla; no permiten distinguir
ese fallo de una versión cero. El análisis detiene el desbordamiento y conserva
sólo los segmentos anteriores válidos. La conversión final a double puede perder
precisión para componentes enormes; no sucede en las versiones sintéticas usuales.
No se prometen los mismos proveedores ni contratos de versión en otras plataformas.

## Salida de un programa controlado

`sis.ejecutar(comando)` recibe una cadena, ejecuta el comando mediante system y no
devuelve valor. Sus efectos son los del comando. Un fallo operativo (-1) produce
error; un estado no cero del hijo sigue ignorándose por compatibilidad.

`sis.pipe(comando)` recibe una cadena y devuelve stdout como texto. Una salida
vacía devuelve cadena vacía. Quita exactamente un LF final y, si lo precede,
un CR; conserva otros saltos, espacios y bytes UTF-8. No realiza conversión
Unicode. El estado no cero del hijo no impide devolver su salida. Los errores de
apertura/lectura/cierre o publicación se propagan después de limpiar temporales.
Lecturas interrumpidas por EINTR se reintentan conservando los bytes ya leídos.

La capacidad histórica es 1048575 bytes **antes** de retirar LF/CR. Ahora NUL o
una salida mayor producen error, en vez de entregar silenciosamente un prefijo.
La salida restante finita se consume antes de cerrar. Esto no añade timeout ni
hace terminar un productor infinito. La memoria temporal es un buffer de 1 MiB,
un bloque de lectura de 8192 bytes y la cadena administrada resultante; el tiempo
es lineal en los bytes consumidos, también cuando se rechaza una salida grande.

Ambas funciones usan el intérprete de órdenes de la plataforma. Los ejemplos de
esta referencia usan únicamente `./productor`, compilado a partir de
`tests/sistema/productor.c` y copiado en un temporal por la batería. Ese programa
sólo admite casos fijos, escribe bytes conocidos y una marca de terminación. No
se necesita ejecutar comandos de administración ni descargar programas. Para
repetirlos, use CTest según A0_DEBIAN13.md: copiar sólo el fragmento requiere
preparar ese productor. Ejemplos pipe_normal, pipe_hijo, pipe_exceso y ejecutar_hijo.

## Ejemplos comprobados

Todos los bloques siguientes existen como .lat y .mict con oráculos explícitos.
`tests/sistema/documentacion.py` comprueba que no diverjan del manual; los tests
CLI ejecutan ambos y comprueban stdout, error y disco. Se usa TZ=UTC0, LC_ALL=C,
usuario sintético y un directorio temporal por caso. Fuera de ese entorno los
valores de fecha/usuario/directorio pueden variar. Los escapes JSON hacen visibles
saltos de línea y secuencias de terminal; no son sintaxis de Mictlán.

<!-- A0_EJEMPLOS_INICIO -->
### tiempo_epoch

```mict
poner(sis.tiempo("%Y-%m-%d %H:%M:%S",0))
```

Salida exacta (escapes JSON): `"1970-01-01 00:00:00\n"`. Código de salida: `0`.

### tiempo_negativo

```mict
poner(sis.tiempo("%Y-%m-%d %H:%M:%S",-1))
```

Salida exacta (escapes JSON): `"1969-12-31 23:59:59\n"`. Código de salida: `0`.

### tiempo_trunc

```mict
poner(sis.tiempo("%S",1.9))
poner(sis.tiempo("%S",-1.9))
```

Salida exacta (escapes JSON): `"01\n59\n"`. Código de salida: `0`.

### tiempo_vacio

```mict
poner(sis.tiempo("",0))
```

Salida exacta (escapes JSON): `"nulo\n"`. Código de salida: `0`.

### tiempo_largo

```mict
poner(sis.tiempo("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",0))
```

Salida exacta (escapes JSON): `"nulo\n"`. Código de salida: `0`.

### tiempo_sin_formato

```mict
poner(tipo(sis.tiempo(nulo,"ignorado")))
```

Salida exacta (escapes JSON): `"decimal\n"`. Código de salida: `0`.

### tiempo_nan

```mict
sis.tiempo("%Y",mate.raiz(-1))
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `Tiempo fuera de rango`.

### tiempo_inf

```mict
sis.tiempo("%Y",mate.exp(10000))
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `Tiempo fuera de rango`.

### tiempo_rango

```mict
sis.tiempo("%Y",9223372036854775808)
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `Tiempo fuera de rango`.

### fecha_iso

```mict
poner(tipo(sis.fecha("iso")))
poner(cadena.longitud(sis.fecha("iso")))
```

Salida exacta (escapes JSON): `"cadena\n20\n"`. Código de salida: `0`.

### fecha_invalida

```mict
sis.fecha("invalida")
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `formato de tiempo`.

### fecha_tipo

```mict
sis.fecha(12)
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `cadena`.

### dormir_cero

```mict
sis.dormir(0)
poner("ok")
```

Salida exacta (escapes JSON): `"ok\n"`. Código de salida: `0`.

### dormir_negativo

```mict
sis.dormir(-1)
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `Duracion fuera de rango`.

### dormir_infinito

```mict
sis.dormir(mate.exp(10000))
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `Duracion fuera de rango`.

### iraxy_fraccion

```mict
sis.iraxy(2.9,3.9)
```

Salida exacta (escapes JSON): `"\u001b[2;3f"`. Código de salida: `0`.

### iraxy_nan

```mict
sis.iraxy(mate.raiz(-1),3)
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `Coordenada fuera de rango`.

### usuario

```mict
poner(sis.usuario())
```

Salida exacta (escapes JSON): `"usuario_sintetico\n"`. Código de salida: `0`.

### usuario_ausente

```mict
poner(sis.usuario())
```

Salida exacta (escapes JSON): `"nulo\n"`. Código de salida: `0`.

### os

```mict
poner(sis.os("LINUX"))
poner(sis.operativo("otra"))
```

Salida exacta (escapes JSON): `"verdadero\nfalso\n"`. Código de salida: `0`.

### cwd

```mict
poner(tipo(sis.cwd()))
```

Salida exacta (escapes JSON): `"cadena\n"`. Código de salida: `0`.

### versiones

```mict
poner(tipo(sis.os_version_mayor()))
poner(tipo(sis.os_version_menor()))
poner(tipo(sis.os_version_parche()))
```

Salida exacta (escapes JSON): `"decimal\ndecimal\ndecimal\n"`. Código de salida: `0`.

### pipe_normal

```mict
poner(sis.pipe("./productor normal"))
```

Salida exacta (escapes JSON): `"hola\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_crlf

```mict
poner(sis.pipe("./productor crlf"))
```

Salida exacta (escapes JSON): `"hola\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_doble

```mict
poner(sis.pipe("./productor doble"))
```

Salida exacta (escapes JSON): `"hola\n\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_vacio

```mict
poner(sis.pipe("./productor vacio"))
```

Salida exacta (escapes JSON): `"\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_utf8

```mict
poner(sis.pipe("./productor utf8"))
```

Salida exacta (escapes JSON): `"María ñ 😀\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_sinlf

```mict
poner(sis.pipe("./productor sinlf"))
```

Salida exacta (escapes JSON): `"hola\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_hijo

```mict
poner(sis.pipe("./productor hijo"))
```

Salida exacta (escapes JSON): `"hola\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_nul

```mict
sis.pipe("./productor nul")
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `NUL`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_limite

```mict
poner(cadena.longitud(sis.pipe("./productor limite")))
```

Salida exacta (escapes JSON): `"1048575\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### pipe_exceso

```mict
sis.pipe("./productor exceso")
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `Salida de tuberia demasiado larga`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### ejecutar_hijo

```mict
sis.ejecutar("./productor hijo")
poner("fin")
```

Salida exacta (escapes JSON): `"hola\nfin\n"`. Código de salida: `0`.

Disco esperado: `{"productor.fin": "terminado\n"}`.

### salir

```mict
sis.salir()
poner("no llegar")
```

Salida exacta (escapes JSON): `""`. Código de salida: `0`.

### avisar_invalido

```mict
sis.avisar(0)
```

Salida exacta (escapes JSON): `""`. Código de salida: `1`.

El diagnóstico debe contener: `No se pudo registrar la senial`.

<!-- A0_EJEMPLOS_FIN -->
