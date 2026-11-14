# Modelo numérico, precisión y límites

Los programas normales de Mictlán almacenan sus números como `double`
(`T_NUMERIC`). En Debian 13 x86_64 y en la CI Ubuntu 24.04 se comprueba
`FLT_RADIX=2`, `DBL_MANT_DIG=53` y `DBL_MAX_EXP=1024`: IEEE 754 binary64.
Las etiquetas C `T_INTEGER` y `T_CHAR` existen en la ABI histórica, pero no son
un segundo modelo de enteros arbitrarios para los programas. B3 no las rediseña.

| Propiedad de binary64 | Valor |
|---|---|
| Bits significativos | 53, incluido el bit implícito |
| Separación entre 1 y el siguiente representable | 2^-52 ≈ 2.220446049250313e-16 |
| Mayor finito | (2−2^-52)·2^1023 ≈ 1.7976931348623157e308 |
| Menor positivo normal | 2^-1022 ≈ 2.2250738585072014e-308 |
| Menor positivo subnormal | 2^-1074 ≈ 4.9406564584124654e-324 |
| Enteros consecutivos representables | Todos entre −2^53 y +2^53; después hay huecos |
| Límite conservador de la API aleatoria nueva | ±(2^53−1) = ±9007199254740991 |

`0.1`, `0.2` y `0.3` no son fracciones binarias finitas. Por ello `0.1+0.2==0.3`
no es una prueba apropiada de corrección matemática. En los puntos pequeños
seleccionados por `contratos.c`, sen²(x)+cos²(x) usa tolerancia absoluta
8·DBL_EPSILON; log(exp(x)) y tan(atan(x)) usan 16·DBL_EPSILON. Son márgenes para
esos casos bien condicionados, no una garantía global. Cerca de un polo, para
números enormes o tras muchas operaciones se necesita analizar propagación de
error y elegir tolerancias absolutas y relativas según el problema.

La reconstrucción `ldexp(fracción,exponente)` de `descomponer_binario(x)` sí se
comprueba por igualdad exacta: en base dos no pierde información para un x
finito representable. Incluye DBL_MAX, DBL_MIN y DBL_TRUE_MIN. El signo de cero
se comprueba con `signbit` desde C, porque -0==0 en comparaciones numéricas.

Las funciones heredadas utilizan la biblioteca matemática C. Un dominio
inválido puede producir NaN; un polo o desbordamiento, infinito; un resultado
muy pequeño, subnormal o cero. `pot(0,0)` sigue dando 1. La alternativa
`potencia_segura(0,0)` rechaza ese caso expresamente. `raiz_segura`, `log_seguro`
y `potencia_segura` verifican los contratos descritos, pero no ofrecen aritmética
exacta ni acotan el error de todas las funciones de libm. No se alteran los
operadores `/`, `%`, potencia ni la precedencia del parser. La división por cero
probada en esta biblioteca es la de las fórmulas `base` y `porc`, con las reglas
IEEE de double. La semántica de operadores continúa cubierta por P1.2-A.

`redondear` usa round: las mitades se alejan de cero, tanto para positivos como
negativos. No es redondeo bancario ni una solución general para dinero. `piso`,
`techo` y `trunc` tienen reglas distintas y devuelven números double. No cambia
el modo de redondeo del proceso. Las comprobaciones se realizaron con el modo
predeterminado, sin `-ffast-math` ni desactivación de subnormales.

La impresión habitual usa `%.16g`; es legible, pero no siempre permite recuperar
los 53 bits de cualquier double. La API estricta valida la sintaxis decimal y
el rango: no convierte un decimal en exacto ni promete detectar todos los
redondeos hacia un valor finito distinto (por ejemplo, un entero demasiado grande
puede redondearse a otro). Rechaza resultados infinitos y una mantisa decimal
no cero que se redondee a cero. Acepta subnormales representables, incluso cuando
strtod informa ERANGE por subdesbordamiento. No depende de errno, que otras
operaciones de locale pueden cambiar. `"0e-99999"` es cero válido.

## Aleatoriedad: contrato de reproducibilidad

El generador nuevo es SplitMix64, algoritmo de Sebastiano Vigna dedicado a CC0;
se conserva su aviso en `src/latmem.c`. Añade ocho bytes al registro privado por
VM y no requiere paquetes externos. Estado inicial 0; semilla explícita entera
entre 0 y 2^53−1. Cada palabra usa aritmética modular uint64. Sembrar una VM no
altera otra, ni rand de C. El generador se usa para simulación y ejemplos
reproducibles; no tiene garantías criptográficas.

Para amplitud w se calcula t=2^64 mod w y se descartan palabras x<t. El intervalo
restante tiene un número entero de bloques de w palabras; por ello cada residuo
x mod w aparece el mismo número de veces. La prueba inyecta 0,5,6 para w=10 y
exige descartar las dos primeras. También verifica ambos extremos del intervalo
máximo permitido. Esto prueba la reducción sin recurrir a una prueba estadística
aleatoria que pudiera fallar por azar. Uniformidad de la reducción no significa
independencia perfecta ni una prueba universal de calidad estadística del PRNG.

La secuencia pública se fija mediante los cinco valores bajos de 32 bits de las
palabras de referencia con estado 0: 0x7b1dcdaf, 0xa1b965f4, 0x8009454f,
0x724c81ec, 0x51a8749b. Una semilla y el mismo orden de llamadas reproducen los
resultados; cambiar intervalos puede cambiar cuántas palabras se rechazan.
Incluso un intervalo de un solo valor consume una palabra. Los errores de
argumentos se producen antes de avanzar el estado.

`aleatorio`/`alt` conservan rand y el módulo histórico para entradas válidas.
No ofrecen una secuencia portable, no reciben una nueva semilla automáticamente
y conservan sesgo. Cuando la amplitud supera RAND_MAX+1, su rango alcanzable no
cubre todo el intervalo. Se documenta esta limitación y se recomienda la función
nueva; no se sustituye silenciosamente el generador histórico.

## Límites y trabajos posteriores

Continúan pendientes: aritmética decimal/arbitraria; una política global de
NaN, dominios y tipos; rediseño de ayudantes C de tipos enteros; generador con
garantías criptográficas; concurrencia de una misma VM y del locale/rand global;
formato decimal reversible para todo double; verificación Windows/macOS y otros
modelos de coma flotante. `latC_strtod_c` conserva un nombre de locale en 64 bytes:
las pruebas cubren C/es_ES.UTF-8/fr_FR.UTF-8, no todos los nombres extensos posibles.
No se promete compatibilidad fuera de los entornos comprobados.

Referencias: [manual histórico](https://manual.lenguajelatino.org/es/stable/librerias/Matematica.html),
[normalización GNU libc](https://www.gnu.org/software/libc/manual/html_node/Normalization-Functions.html),
[frexp en Debian](https://manpages.debian.org/testing/manpages-dev/frexp.3.en.html),
[WG14 DR329 sobre exactitud de operaciones binarias](https://www.open-std.org/jtc1/sc22/wg14/www/docs/dr_329.htm),
[SplitMix64 original](https://prng.di.unimi.it/splitmix64.c) y
[copia del original consultada](https://github.com/svaarala/duktape/blob/master/misc/splitmix64.c).
