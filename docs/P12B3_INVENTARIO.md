# Inventario de matemáticas y conversiones P1.2-B3

Auditados `src/latmathlib.c`, `src/latbaselib.c`, `src/latobj.c` y `src/latlocale.c`.
Esta tabla enumera todos los nombres públicos del lenguaje afectados; no son
símbolos C nuevos. Las firmas reciben números salvo las dos conversiones globales.
Los 39 nombres heredados siguen registrados. Nueve nombres mate y una conversión
son adiciones. Para ejemplos, precisión individual y recomendaciones véase la
[referencia](P12B3_REFERENCIA.md). [Decisiones](P12B3_DECISIONES.md) conserva la
propuesta inicial y la rectificación de anumero al comprobar la suite anterior.

| Firma | Resultado y propósito | Dominio | No finitos, cero y errores | Contrato |
|---|---|---|---|---|
| `mate.abs(x)` | número: Distancia al cero, siempre sin signo negativo. | Todos los números, incluidos no finitos. | NaN → NaN; ±inf → +inf; -0 → +0. | Heredado; conservar firma |
| `mate.acos(x)` | número: Ángulo cuyo coseno es x, en radianes. | [-1,1]. | Fuera de dominio e infinitos → NaN; ±0 → pi/2; NaN → NaN. | Heredado; conservar firma |
| `mate.acosh(x)` | número: Inversa del coseno hiperbólico. | [1,+inf]. | x<1 → NaN; +inf → +inf; NaN → NaN. | Heredado; conservar firma |
| `mate.asen(x)` | número: Ángulo cuyo seno es x, en radianes. | [-1,1]. | Fuera de dominio e infinitos → NaN; conserva ±0; NaN → NaN. | Heredado; conservar firma |
| `mate.asenh(x)` | número: Inversa del seno hiperbólico. | Todos los reales. | Conserva ±inf, ±0 y NaN. | Heredado; conservar firma |
| `mate.atan(x)` | número: Ángulo cuya tangente es x, en radianes. | Todos los reales. | ±inf → ±pi/2; conserva ±0; NaN → NaN. | Heredado; conservar firma |
| `mate.atanh(x)` | número: Inversa de la tangente hiperbólica. | (-1,1); polos en ±1. | ±1 → ±inf; fuera de [-1,1] → NaN; conserva ±0; NaN → NaN. | Heredado; conservar firma |
| `mate.atan2(y,x)` | número: Ángulo de un punto (x,y); distingue cuadrantes. | Reales; casos de ejes y ceros según libm/IEEE. | Con x>0 conserva el signo de y=±0; x<0 distingue ±pi. NaN en cualquier entrada → NaN; infinitos tienen ángulos límite. | Heredado; conservar firma |
| `mate.cos(x)` | número: Coseno de un ángulo en radianes. | Reales finitos. | ±0 → 1; ±inf y NaN → NaN. | Heredado; conservar firma |
| `mate.cosh(x)` | número: Coseno hiperbólico. | Reales. | ±0 → 1; ±inf → +inf; puede desbordarse; NaN → NaN. | Heredado; conservar firma |
| `mate.sen(x)` | número: Seno de un ángulo en radianes. | Reales finitos. | Conserva ±0; ±inf y NaN → NaN. | Heredado; conservar firma |
| `mate.senh(x)` | número: Seno hiperbólico. | Reales. | Conserva ±0 y ±inf; puede desbordarse; NaN → NaN. | Heredado; conservar firma |
| `mate.tan(x)` | número: Tangente de un ángulo en radianes. | Reales salvo polos pi/2+k*pi. | Conserva ±0; ±inf y NaN → NaN; el pi representable no es el pi exacto, no usarlo para detectar polos por igualdad. | Heredado; conservar firma |
| `mate.tanh(x)` | número: Tangente hiperbólica, acotada entre -1 y 1. | Reales. | Conserva ±0; ±inf → ±1; NaN → NaN. | Heredado; conservar firma |
| `mate.exp(x)` | número: Exponencial de base e. | Reales. | ±0 → 1; +inf → +inf; -inf → +0; NaN → NaN; puede desbordarse o perderse a cero. | Heredado; conservar firma |
| `mate.log(x)` | número: Logaritmo natural (base e). | x>0; polo en 0. | ±0 → -inf; x<0 → NaN; +inf → +inf; NaN → NaN. | Heredado; conservar firma |
| `mate.log10(x)` | número: Logaritmo de base diez. | x>0; polo en 0. | ±0 → -inf; x<0 → NaN; +inf → +inf; NaN → NaN. | Heredado; conservar firma |
| `mate.max(x, ...)` | número: Selecciona el mayor de los argumentos. | Uno o más números. | Sin argumentos → error; omite NaN si hay otro valor; sólo NaN → NaN. Ordena infinitos; primer empate conserva signo de cero. | Heredado; conservar firma |
| `mate.min(x, ...)` | número: Selecciona el menor de los argumentos. | Uno o más números. | Sin argumentos → error; omite NaN si hay otro valor; sólo NaN → NaN. Ordena infinitos; primer empate conserva signo de cero. | Heredado; conservar firma |
| `mate.raiz(x)` | número: Raíz cuadrada. | x≥0. | x<0 → NaN; +inf → +inf; conserva -0; NaN → NaN. | Heredado; conservar firma |
| `mate.raizc(x)` | número: Raíz cúbica, también de números negativos. | Todos los reales. | Conserva ±0, ±inf y NaN. | Heredado; conservar firma |
| `mate.techo(x)` | número: Menor entero que no es menor que x. | Todos los números. | -2.5 → -2; conserva ±0, ±inf y NaN; devuelve número, no etiqueta entera C. | Heredado; conservar firma |
| `mate.piso(x)` | número: Mayor entero que no supera x. | Todos los números. | -2.5 → -3; conserva ±0, ±inf y NaN. | Heredado; conservar firma |
| `mate.pot(x,y)` | número: Eleva x a la potencia y. | Reales donde la potencia real esté definida. | Base negativa y exponente no entero → NaN. 0^0 → 1, 0 con exponente negativo → infinito. NaN^0 y 1^NaN → 1. Ceros/infinitos siguen libm, incluido signo para exponentes enteros impares. | Heredado; conservar firma |
| `mate.frexp(x, extra)` | número: Obtiene sólo la fracción normalizada de x. | x y extra numéricos; el valor de extra no se utiliza. | Para finitos x=f*2^e, 0.5≤abs(f)<1 si x≠0. Conserva ±0, ±inf y NaN. El exponente no se devuelve ni modifica una variable del llamador. | Heredado; conservar firma |
| `mate.ldexp(x,e)` | número: Multiplica x por 2 elevado al exponente truncado. | x numérico; trunc(e) debe ser finito y caber en int C. | Exponente inválido → error antes del cast; x NaN/±inf/±0 se conserva. Desbordamiento → infinito; subdesbordamiento → subnormal o cero. | Heredado; conservar firma |
| `mate.trunc(x)` | número: Quita la parte fraccionaria hacia cero. | Todos los números. | -2.9 → -2; conserva ±0, ±inf y NaN. | Heredado; conservar firma |
| `mate.aleatorio([a [, b]])` | número: Obtiene un entero usando el rand histórico de C. | 0, 1 o 2 números finitos cuyo truncamiento cabe en int; límite único no negativo. | 0 args → 0 o 1; uno → [0,trunc(a)]; dos → extremos ordenados e inclusivos. -0 → 0. NaN/inf/rango inválido → error. Tiene sesgo de módulo; intervalos más anchos que RAND_MAX+1 sólo alcanzan un subconjunto. | Heredado; conservar firma |
| `mate.pi()` | número: Constante matemática pi. | Sin argumentos. | Siempre positiva y finita; no admite entradas NaN ni infinitos. | Heredado; conservar firma |
| `mate.tau()` | número: Constante matemática 2*pi. | Sin argumentos. | Siempre positiva y finita; no admite entradas NaN ni infinitos. | Heredado; conservar firma |
| `mate.e()` | número: Constante matemática e. | Sin argumentos. | Siempre positiva y finita; no admite entradas NaN ni infinitos. | Heredado; conservar firma |
| `mate.redondear(x)` | número: Entero más cercano; mitades alejadas del cero. | Todos los números. | 2.5 → 3; -2.5 → -3. Conserva ±0, ±inf y NaN; -0.25 → -0. No es redondeo bancario. | Heredado; conservar firma |
| `mate.base(parte, porcentaje)` | número: Recupera la base a partir de una parte y su porcentaje. | Dos números; fórmula parte/(porcentaje/100). | División por cero conserva ±inf/NaN de double; NaN/inf y signo del cero siguen las dos operaciones. | Heredado; conservar firma |
| `mate.parte(base, porcentaje)` | número: Calcula la parte correspondiente a un porcentaje. | Dos números; fórmula (base/100)*porcentaje. | NaN/inf y signo de cero siguen división y producto; 0*inf → NaN. | Heredado; conservar firma |
| `mate.porc(parte, base)` | número: Calcula la proporción parte/base (no multiplica por cien). | Dos números; fórmula parte/base. | Base cero → ±inf o NaN; casos no finitos y ceros según división double. | Heredado; conservar firma |
| `mate.alt([a [, b]])` | número: Alias histórico de aleatorio. Obtiene un entero usando el rand histórico de C. | 0, 1 o 2 números finitos cuyo truncamiento cabe en int; límite único no negativo. | 0 args → 0 o 1; uno → [0,trunc(a)]; dos → extremos ordenados e inclusivos. -0 → 0. NaN/inf/rango inválido → error. Tiene sesgo de módulo; intervalos más anchos que RAND_MAX+1 sólo alcanzan un subconjunto. | Heredado; conservar firma |
| `mate.rnd(x)` | número: Alias histórico de redondear. Entero más cercano; mitades alejadas del cero. | Todos los números. | 2.5 → 3; -2.5 → -3. Conserva ±0, ±inf y NaN; -0.25 → -0. No es redondeo bancario. | Heredado; conservar firma |
| `mate.porciento(parte, base)` | número: Alias histórico de porc. Calcula la proporción parte/base (no multiplica por cien). | Dos números; fórmula parte/base. | Base cero → ±inf o NaN; casos no finitos y ceros según división double. | Heredado; conservar firma |
| `mate.porcentaje(parte, base)` | número: Alias histórico de porc. Calcula la proporción parte/base (no multiplica por cien). | Dos números; fórmula parte/base. | Base cero → ±inf o NaN; casos no finitos y ceros según división double. | Heredado; conservar firma |
| `mate.descomponer_binario(x)` | lista [número, número]: Separa el número en fracción y exponente binario. | Número finito. | ±0 → [±0,0]; rechaza NaN/inf; admite el mínimo subnormal y el máximo finito. La lista es nueva. | Nuevo B3 |
| `mate.es_finito(x)` | lógico: Verdadero para valores que no son infinitos ni NaN. | Número, incluidos no finitos. | No convierte cadenas: un tipo no numérico provoca error. ±0 es finito, no infinito y no NaN. | Nuevo B3 |
| `mate.es_infinito(x)` | lógico: Verdadero para +inf y -inf. | Número, incluidos no finitos. | No convierte cadenas: un tipo no numérico provoca error. ±0 es finito, no infinito y no NaN. | Nuevo B3 |
| `mate.es_nan(x)` | lógico: Verdadero para el valor que señala un resultado numérico indefinido. | Número, incluidos no finitos. | No convierte cadenas: un tipo no numérico provoca error. ±0 es finito, no infinito y no NaN. | Nuevo B3 |
| `mate.semilla_aleatoria(s)` | nulo: Reinicia el generador moderno de esta VM. | Entero exacto en [0,9007199254740991]. | Semilla inicial por VM: 0; error de tipo/rango no cambia el estado. No afecta rand ni aleatorio/alt. -0 equivale a 0. | Nuevo B3 |
| `mate.aleatorio_entero(minimo,maximo)` | número: Obtiene un entero en un intervalo inclusivo sin sesgo de módulo. | Enteros exactos en ±9007199254740991, mínimo≤máximo. | Iguales → ese entero (consume una palabra); invertidos, fracciones, NaN o inf → error. Los errores no consumen el estado. Cero de salida no conserva signo. | Nuevo B3 |
| `mate.raiz_segura(x)` | número: Raíz cuadrada con dominio comprobado. | Número finito x≥0. | Negativos → error; NaN/inf → error de finitud; conserva -0; admite subnormales. | Nuevo B3 |
| `mate.log_seguro(x)` | número: Logaritmo natural con dominio comprobado. | Número finito x>0. | ±0 y negativos → error; NaN/inf → error de finitud; subnormales positivos admitidos. | Nuevo B3 |
| `mate.potencia_segura(x,y)` | número: Potencia con dominio y rango comprobados. | Finitos; base negativa exige exponente entero; base cero exige exponente positivo. | 0^0 se rechaza en esta API; desbordamiento y pérdida de resultado no nulo a cero → error; subnormal representable aceptado. -0 sigue pow para exponente positivo. | Nuevo B3 |
| `anumero(valor)` | número o nulo: Conversión numérica histórica, deliberadamente laxa. | Números, cadenas, lógicos, nulo, listas y diccionarios. | Cero → nulo, también "0", -0, falso, nulo y contenedores vacíos. Cadenas completamente convertibles usan strtod C (hex, NaN/inf incluidos); otras usan primer byte: "abc" → 97, "12x" → 49, " 0 " → 32. Listas/diccionarios → longitud. No usar para validar datos. | Heredado; conservar firma |
| `anumero_estricto(valor)` | número: Convierte un decimal validado; cero sigue siendo un número. | Número finito o cadena decimal ASCII con signo, punto y exponente; espacios ASCII exteriores opcionales. | Admite .5 y 1.; rechaza cadena vacía, booleanos, colecciones, hexadecimal, coma, NaN/inf y basura. Conserva -0. Desbordamiento o mantisa no nula redondeada a cero → error; subnormales representables aceptados. | Nuevo B3 |

## Conversión C y puntos relacionados

| Interfaz | Contrato comprobado y límites | Acción B3 |
|---|---|---|
| latC_adouble(mv,obj) → double | Nulo/falso=0, verdadero=1, número sin cambio, lista/dic=longitud; texto usa strtod en locale C si consume toda la cadena, si no usa primer byte. Texto vacío da 0. O(bytes) para texto. | Conservada: modificarla alteraría coerciones globales. |
| latC_aint(mv,obj) → int | Texto numérico se trunca hacia cero. Falso/nulo=0, verdadero=1, char y longitudes conservados. T_NUMERIC llama al verificador T_INTEGER y da error: inconsistencia histórica de etiquetas, no política nueva de coerción. | Texto NaN/inf/fuera de int produce error antes del cast. Tipos/firmas conservados. |
| latC_achar(mv,obj) → char | Texto numérico se trunca hacia cero; rango CHAR_MIN..CHAR_MAX según plataforma. Tipos T_INTEGER/T_CHAR conservados; el comportamiento T_NUMERIC tiene la inconsistencia anterior. | Texto no representable produce error antes del cast; conversión entera a char y longitudes se dejan para revisar compatibilidad SDK. |
| latC_checar_numerico / latC_checar_entero | Exigen respectivamente T_NUMERIC / T_INTEGER. Los programas normales usan double/T_NUMERIC. | Sin cambios. |
| latC_strtod_c(texto,fin) | Cambia temporalmente LC_NUMERIC a C, llama a strtod y restaura el locale. | Reutilizado por la API estricta; pruebas C/es_ES/fr_FR. Su estado global requiere futura solución para concurrencia. |
| leer() | Lee texto y prueba strtod; no equivale al contrato decimal estricto de la API nueva. | Fuera de este incremento; usar anumero_estricto sobre texto para validación explícita. |
| acadena(numero) | Formatea normalmente con %.16g; puede ocultar diferencias y no garantiza reconstruir todos los doubles. | Sin cambios; las pruebas C observan precisión y signo sin depender de la impresión. |

## Contraste con el manual histórico

[Matemática de Latino](https://manual.lenguajelatino.org/es/stable/librerias/Matematica.html)
explica funciones sobre números, pero no fija una política completa de NaN,
precisión ni recuperación de errores. Sus ejemplos de frexp y porc coinciden
con fracción sola y razón (no se «corrigen» para inventar otro significado).
«Entre 0 y 1» en aleatorio no implica un real continuo: se conserva 0 o 1.
La mención general de negativos se conserva para dos extremos; el caso de un
único límite negativo tenía división por cero o resultados fuera del intervalo
pretendido. Ahora da error, con alternativa explícita para intervalos negativos.
La especificación aquí procede del código, las matemáticas y las pruebas, no
únicamente del manual. Los dominios de libm permanecen en las funciones heredadas.
