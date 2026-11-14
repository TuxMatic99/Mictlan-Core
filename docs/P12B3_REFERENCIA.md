# Matemáticas y conversiones: referencia inicial de Mictlán

El módulo `mate` ofrece 39 nombres heredados y nueve nuevos. Se conserva
`anumero` y se añade `anumero_estricto` como función global. No se necesita importar
el módulo. Todos los argumentos de mate son números, salvo que la firma no
reciba argumentos. Un tipo incorrecto produce un error recuperable; no hay
conversión implícita de texto dentro de mate. Los resultados son valores nuevos;
ninguna función modifica los argumentos. Semilla y aleatoriedad sí cambian el
estado de su generador. Las listas de descomposición son nuevas.

Un número de coma flotante es una aproximación sobre una rejilla binaria: igual
que una regla graduada no distingue todas las distancias, un double no representa
todos los decimales. Por eso 0.1+0.2 puede diferir de 0.3. Para aproximaciones,
compare una diferencia con una tolerancia adecuada al problema; no redondee cada
operación para ocultar el error. Véase [precisión y límites](P12B3_PRECISION.md).

NaN significa «no es un número» y suele señalar un dominio inválido; infinito
puede representar un desbordamiento o un polo. Ninguno sustituye una validación
de datos. Las funciones heredadas conservan libm; las nuevas funciones seguras
convierten los casos especificados en errores. `-0` compara igual que `0`, pero
conserva información de signo útil, por ejemplo en atan2. Use los tres predicados
para clasificar valores; NaN no se detecta comparándolo por igualdad.

Cada ejemplo siguiente se ejecuta automáticamente como `.lat` y `.mict`.
La salida esperada se escribió a partir del contrato, no del intérprete.
Los alias tienen ejemplos propios. Coste: max/min O(n) y espacio auxiliar O(1);
las conversiones de texto O(bytes), sin buffer temporal; las demás operaciones
son escalares. El rechazo aleatorio tiene coste esperado constante (<2 palabras
para los intervalos admitidos), sin una cota finita determinista de iteraciones.
El coste interno de libm depende de la plataforma.

## `mate.abs(x)`

Distancia al cero, siempre sin signo negativo. Devuelve **número**.

Argumentos y dominio: Todos los números, incluidos no finitos. Casos límite: NaN → NaN; ±inf → +inf; -0 → +0.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Se mantiene el nombre histórico.

```latino
poner(mate.abs(-3))
```

Salida esperada:

```text
3
```

## `mate.acos(x)`

Ángulo cuyo coseno es x, en radianes. Devuelve **número**.

Argumentos y dominio: [-1,1]. Casos límite: Fuera de dominio e infinitos → NaN; ±0 → pi/2; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.acos(1))
```

Salida esperada:

```text
0
```

## `mate.acosh(x)`

Inversa del coseno hiperbólico. Devuelve **número**.

Argumentos y dominio: [1,+inf]. Casos límite: x<1 → NaN; +inf → +inf; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.acosh(1))
```

Salida esperada:

```text
0
```

## `mate.asen(x)`

Ángulo cuyo seno es x, en radianes. Devuelve **número**.

Argumentos y dominio: [-1,1]. Casos límite: Fuera de dominio e infinitos → NaN; conserva ±0; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.asen(0))
```

Salida esperada:

```text
0
```

## `mate.asenh(x)`

Inversa del seno hiperbólico. Devuelve **número**.

Argumentos y dominio: Todos los reales. Casos límite: Conserva ±inf, ±0 y NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.asenh(0))
```

Salida esperada:

```text
0
```

## `mate.atan(x)`

Ángulo cuya tangente es x, en radianes. Devuelve **número**.

Argumentos y dominio: Todos los reales. Casos límite: ±inf → ±pi/2; conserva ±0; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.atan(0))
```

Salida esperada:

```text
0
```

## `mate.atanh(x)`

Inversa de la tangente hiperbólica. Devuelve **número**.

Argumentos y dominio: (-1,1); polos en ±1. Casos límite: ±1 → ±inf; fuera de [-1,1] → NaN; conserva ±0; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.atanh(0))
```

Salida esperada:

```text
0
```

## `mate.atan2(y,x)`

Ángulo de un punto (x,y); distingue cuadrantes. Devuelve **número**.

Argumentos y dominio: Reales; casos de ejes y ceros según libm/IEEE. Casos límite: Con x>0 conserva el signo de y=±0; x<0 distingue ±pi. NaN en cualquier entrada → NaN; infinitos tienen ángulos límite.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.atan2(0,1))
```

Salida esperada:

```text
0
```

## `mate.cos(x)`

Coseno de un ángulo en radianes. Devuelve **número**.

Argumentos y dominio: Reales finitos. Casos límite: ±0 → 1; ±inf y NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.cos(0))
```

Salida esperada:

```text
1
```

## `mate.cosh(x)`

Coseno hiperbólico. Devuelve **número**.

Argumentos y dominio: Reales. Casos límite: ±0 → 1; ±inf → +inf; puede desbordarse; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.cosh(0))
```

Salida esperada:

```text
1
```

## `mate.sen(x)`

Seno de un ángulo en radianes. Devuelve **número**.

Argumentos y dominio: Reales finitos. Casos límite: Conserva ±0; ±inf y NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.sen(0))
```

Salida esperada:

```text
0
```

## `mate.senh(x)`

Seno hiperbólico. Devuelve **número**.

Argumentos y dominio: Reales. Casos límite: Conserva ±0 y ±inf; puede desbordarse; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.senh(0))
```

Salida esperada:

```text
0
```

## `mate.tan(x)`

Tangente de un ángulo en radianes. Devuelve **número**.

Argumentos y dominio: Reales salvo polos pi/2+k*pi. Casos límite: Conserva ±0; ±inf y NaN → NaN; el pi representable no es el pi exacto, no usarlo para detectar polos por igualdad.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.tan(0))
```

Salida esperada:

```text
0
```

## `mate.tanh(x)`

Tangente hiperbólica, acotada entre -1 y 1. Devuelve **número**.

Argumentos y dominio: Reales. Casos límite: Conserva ±0; ±inf → ±1; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.tanh(0))
```

Salida esperada:

```text
0
```

## `mate.exp(x)`

Exponencial de base e. Devuelve **número**.

Argumentos y dominio: Reales. Casos límite: ±0 → 1; +inf → +inf; -inf → +0; NaN → NaN; puede desbordarse o perderse a cero.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.exp(0))
```

Salida esperada:

```text
1
```

## `mate.log(x)`

Logaritmo natural (base e). Devuelve **número**.

Argumentos y dominio: x>0; polo en 0. Casos límite: ±0 → -inf; x<0 → NaN; +inf → +inf; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Usar log_seguro si un dominio inválido debe ser un error.

```latino
poner(mate.log(1))
```

Salida esperada:

```text
0
```

## `mate.log10(x)`

Logaritmo de base diez. Devuelve **número**.

Argumentos y dominio: x>0; polo en 0. Casos límite: ±0 → -inf; x<0 → NaN; +inf → +inf; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.log10(1))
```

Salida esperada:

```text
0
```

## `mate.max(x, ...)`

Selecciona el mayor de los argumentos. Devuelve **número**.

Argumentos y dominio: Uno o más números. Casos límite: Sin argumentos → error; omite NaN si hay otro valor; sólo NaN → NaN. Ordena infinitos; primer empate conserva signo de cero.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Se mantiene el nombre histórico.

```latino
poner(mate.max(-10000000000000000,-20000000000000000))
```

Salida esperada:

```text
-1e+16
```

## `mate.min(x, ...)`

Selecciona el menor de los argumentos. Devuelve **número**.

Argumentos y dominio: Uno o más números. Casos límite: Sin argumentos → error; omite NaN si hay otro valor; sólo NaN → NaN. Ordena infinitos; primer empate conserva signo de cero.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Se mantiene el nombre histórico.

```latino
poner(mate.min(10000000000000000,20000000000000000))
```

Salida esperada:

```text
1e+16
```

## `mate.raiz(x)`

Raíz cuadrada. Devuelve **número**.

Argumentos y dominio: x≥0. Casos límite: x<0 → NaN; +inf → +inf; conserva -0; NaN → NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. raiz_segura para convertir dominios inválidos en errores.

```latino
poner(mate.raiz(4))
```

Salida esperada:

```text
2
```

## `mate.raizc(x)`

Raíz cúbica, también de números negativos. Devuelve **número**.

Argumentos y dominio: Todos los reales. Casos límite: Conserva ±0, ±inf y NaN.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.abs(mate.raizc(-8)+2)<0.000000000001)
```

Salida esperada:

```text
verdadero
```

## `mate.techo(x)`

Menor entero que no es menor que x. Devuelve **número**.

Argumentos y dominio: Todos los números. Casos límite: -2.5 → -2; conserva ±0, ±inf y NaN; devuelve número, no etiqueta entera C.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Se mantiene el nombre histórico.

```latino
poner(mate.techo(-2.5))
```

Salida esperada:

```text
-2
```

## `mate.piso(x)`

Mayor entero que no supera x. Devuelve **número**.

Argumentos y dominio: Todos los números. Casos límite: -2.5 → -3; conserva ±0, ±inf y NaN.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Se mantiene el nombre histórico.

```latino
poner(mate.piso(-2.5))
```

Salida esperada:

```text
-3
```

## `mate.pot(x,y)`

Eleva x a la potencia y. Devuelve **número**.

Argumentos y dominio: Reales donde la potencia real esté definida. Casos límite: Base negativa y exponente no entero → NaN. 0^0 → 1, 0 con exponente negativo → infinito. NaN^0 y 1^NaN → 1. Ceros/infinitos siguen libm, incluido signo para exponentes enteros impares.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. potencia_segura comprueba finitud, dominio y pérdida a cero.

```latino
poner(mate.pot(2,3))
```

Salida esperada:

```text
8
```

## `mate.frexp(x, extra)`

Obtiene sólo la fracción normalizada de x. Devuelve **número**.

Argumentos y dominio: x y extra numéricos; el valor de extra no se utiliza. Casos límite: Para finitos x=f*2^e, 0.5≤abs(f)<1 si x≠0. Conserva ±0, ±inf y NaN. El exponente no se devuelve ni modifica una variable del llamador.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. descomponer_binario(x) devuelve ambos componentes.

```latino
poner(mate.frexp(8,99))
```

Salida esperada:

```text
0.5
```

## `mate.ldexp(x,e)`

Multiplica x por 2 elevado al exponente truncado. Devuelve **número**.

Argumentos y dominio: x numérico; trunc(e) debe ser finito y caber en int C. Casos límite: Exponente inválido → error antes del cast; x NaN/±inf/±0 se conserva. Desbordamiento → infinito; subdesbordamiento → subnormal o cero.

Precisión: Exacto si el resultado es representable; redondeo al entrar en subnormales. Se mantiene el nombre histórico.

```latino
poner(mate.ldexp(0.5,4.9))
```

Salida esperada:

```text
8
```

## `mate.trunc(x)`

Quita la parte fraccionaria hacia cero. Devuelve **número**.

Argumentos y dominio: Todos los números. Casos límite: -2.9 → -2; conserva ±0, ±inf y NaN.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Se mantiene el nombre histórico.

```latino
poner(mate.trunc(-2.9))
```

Salida esperada:

```text
-2
```

## `mate.aleatorio([a [, b]])`

Obtiene un entero usando el rand histórico de C. Devuelve **número**.

Argumentos y dominio: 0, 1 o 2 números finitos cuyo truncamiento cabe en int; límite único no negativo. Casos límite: 0 args → 0 o 1; uno → [0,trunc(a)]; dos → extremos ordenados e inclusivos. -0 → 0. NaN/inf/rango inválido → error. Tiene sesgo de módulo; intervalos más anchos que RAND_MAX+1 sólo alcanzan un subconjunto.

Precisión: Resultado entero exacto; secuencia dependiente de libc y estado global. No es uniforme en general. aleatorio_entero y semilla_aleatoria para simulación reproducible.

```latino
poner(mate.aleatorio(0))
```

Salida esperada:

```text
0
```

## `mate.pi()`

Constante matemática pi. Devuelve **número**.

Argumentos y dominio: Sin argumentos. Casos límite: Siempre positiva y finita; no admite entradas NaN ni infinitos.

Precisión: Literal double de la constante, redondeado a binary64. Se mantiene el nombre histórico.

```latino
poner(mate.pi()>3.14 && mate.pi()<3.15)
```

Salida esperada:

```text
verdadero
```

## `mate.tau()`

Constante matemática 2*pi. Devuelve **número**.

Argumentos y dominio: Sin argumentos. Casos límite: Siempre positiva y finita; no admite entradas NaN ni infinitos.

Precisión: Literal double de la constante, redondeado a binary64. Se mantiene el nombre histórico.

```latino
poner(mate.tau()>6.28 && mate.tau()<6.29)
```

Salida esperada:

```text
verdadero
```

## `mate.e()`

Constante matemática e. Devuelve **número**.

Argumentos y dominio: Sin argumentos. Casos límite: Siempre positiva y finita; no admite entradas NaN ni infinitos.

Precisión: Literal double de la constante, redondeado a binary64. Se mantiene el nombre histórico.

```latino
poner(mate.e()>2.71 && mate.e()<2.72)
```

Salida esperada:

```text
verdadero
```

## `mate.redondear(x)`

Entero más cercano; mitades alejadas del cero. Devuelve **número**.

Argumentos y dominio: Todos los números. Casos límite: 2.5 → 3; -2.5 → -3. Conserva ±0, ±inf y NaN; -0.25 → -0. No es redondeo bancario.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Se mantiene el nombre histórico.

```latino
poner(mate.redondear(-2.5))
```

Salida esperada:

```text
-3
```

## `mate.base(parte, porcentaje)`

Recupera la base a partir de una parte y su porcentaje. Devuelve **número**.

Argumentos y dominio: Dos números; fórmula parte/(porcentaje/100). Casos límite: División por cero conserva ±inf/NaN de double; NaN/inf y signo del cero siguen las dos operaciones.

Precisión: Dos operaciones double redondeadas; se conserva su orden histórico. Se mantiene el nombre histórico.

```latino
poner(mate.base(25,50))
```

Salida esperada:

```text
50
```

## `mate.parte(base, porcentaje)`

Calcula la parte correspondiente a un porcentaje. Devuelve **número**.

Argumentos y dominio: Dos números; fórmula (base/100)*porcentaje. Casos límite: NaN/inf y signo de cero siguen división y producto; 0*inf → NaN.

Precisión: Dos operaciones double redondeadas; se conserva su orden histórico. Se mantiene el nombre histórico.

```latino
poner(mate.parte(50,25))
```

Salida esperada:

```text
12.5
```

## `mate.porc(parte, base)`

Calcula la proporción parte/base (no multiplica por cien). Devuelve **número**.

Argumentos y dominio: Dos números; fórmula parte/base. Casos límite: Base cero → ±inf o NaN; casos no finitos y ceros según división double.

Precisión: Una división double redondeada. Multiplicar explícitamente la razón por 100 para expresar un porcentaje.

```latino
poner(mate.porc(25,50))
```

Salida esperada:

```text
0.5
```

## `mate.alt([a [, b]])`

Alias histórico de aleatorio. Obtiene un entero usando el rand histórico de C. Devuelve **número**.

Argumentos y dominio: 0, 1 o 2 números finitos cuyo truncamiento cabe en int; límite único no negativo. Casos límite: 0 args → 0 o 1; uno → [0,trunc(a)]; dos → extremos ordenados e inclusivos. -0 → 0. NaN/inf/rango inválido → error. Tiene sesgo de módulo; intervalos más anchos que RAND_MAX+1 sólo alcanzan un subconjunto.

Precisión: Resultado entero exacto; secuencia dependiente de libc y estado global. No es uniforme en general. aleatorio_entero y semilla_aleatoria para simulación reproducible.

```latino
poner(mate.alt(0))
```

Salida esperada:

```text
0
```

## `mate.rnd(x)`

Alias histórico de redondear. Entero más cercano; mitades alejadas del cero. Devuelve **número**.

Argumentos y dominio: Todos los números. Casos límite: 2.5 → 3; -2.5 → -3. Conserva ±0, ±inf y NaN; -0.25 → -0. No es redondeo bancario.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Se mantiene el nombre histórico.

```latino
poner(mate.rnd(-2.5))
```

Salida esperada:

```text
-3
```

## `mate.porciento(parte, base)`

Alias histórico de porc. Calcula la proporción parte/base (no multiplica por cien). Devuelve **número**.

Argumentos y dominio: Dos números; fórmula parte/base. Casos límite: Base cero → ±inf o NaN; casos no finitos y ceros según división double.

Precisión: Una división double redondeada. Multiplicar explícitamente la razón por 100 para expresar un porcentaje.

```latino
poner(mate.porciento(25,50))
```

Salida esperada:

```text
0.5
```

## `mate.porcentaje(parte, base)`

Alias histórico de porc. Calcula la proporción parte/base (no multiplica por cien). Devuelve **número**.

Argumentos y dominio: Dos números; fórmula parte/base. Casos límite: Base cero → ±inf o NaN; casos no finitos y ceros según división double.

Precisión: Una división double redondeada. Multiplicar explícitamente la razón por 100 para expresar un porcentaje.

```latino
poner(mate.porcentaje(25,50))
```

Salida esperada:

```text
0.5
```

## `mate.descomponer_binario(x)`

Separa el número en fracción y exponente binario. Devuelve **lista [número, número]**.

Argumentos y dominio: Número finito. Casos límite: ±0 → [±0,0]; rechaza NaN/inf; admite el mínimo subnormal y el máximo finito. La lista es nueva.

Precisión: Resultado exacto sobre el valor double recibido; no recupera precisión perdida al leerlo. Alternativa completa a frexp.

```latino
poner(mate.descomponer_binario(8))
```

Salida esperada:

```text
[0.5, 4]
```

## `mate.es_finito(x)`

Verdadero para valores que no son infinitos ni NaN. Devuelve **lógico**.

Argumentos y dominio: Número, incluidos no finitos. Casos límite: No convierte cadenas: un tipo no numérico provoca error. ±0 es finito, no infinito y no NaN.

Precisión: Clasificación exacta. Se mantiene el nombre histórico.

```latino
poner(mate.es_finito(0))
```

Salida esperada:

```text
verdadero
```

## `mate.es_infinito(x)`

Verdadero para +inf y -inf. Devuelve **lógico**.

Argumentos y dominio: Número, incluidos no finitos. Casos límite: No convierte cadenas: un tipo no numérico provoca error. ±0 es finito, no infinito y no NaN.

Precisión: Clasificación exacta. Se mantiene el nombre histórico.

```latino
poner(mate.es_infinito(mate.log(0)))
```

Salida esperada:

```text
verdadero
```

## `mate.es_nan(x)`

Verdadero para el valor que señala un resultado numérico indefinido. Devuelve **lógico**.

Argumentos y dominio: Número, incluidos no finitos. Casos límite: No convierte cadenas: un tipo no numérico provoca error. ±0 es finito, no infinito y no NaN.

Precisión: Clasificación exacta. Se mantiene el nombre histórico.

```latino
poner(mate.es_nan(mate.raiz(-1)))
```

Salida esperada:

```text
verdadero
```

## `mate.semilla_aleatoria(s)`

Reinicia el generador moderno de esta VM. Devuelve **nulo**.

Argumentos y dominio: Entero exacto en [0,9007199254740991]. Casos límite: Semilla inicial por VM: 0; error de tipo/rango no cambia el estado. No afecta rand ni aleatorio/alt. -0 equivale a 0.

Precisión: Aritmética uint64 modular exacta. Se mantiene el nombre histórico.

```latino
poner(mate.semilla_aleatoria(0))
```

Salida esperada:

```text
nulo
```

## `mate.aleatorio_entero(minimo,maximo)`

Obtiene un entero en un intervalo inclusivo sin sesgo de módulo. Devuelve **número**.

Argumentos y dominio: Enteros exactos en ±9007199254740991, mínimo≤máximo. Casos límite: Iguales → ese entero (consume una palabra); invertidos, fracciones, NaN o inf → error. Los errores no consumen el estado. Cero de salida no conserva signo.

Precisión: SplitMix64 con rechazo; reproducible por semilla y orden de llamadas. Sin garantías criptográficas. Se mantiene el nombre histórico.

```latino
poner(mate.aleatorio_entero(-3,-3))
```

Salida esperada:

```text
-3
```

## `mate.raiz_segura(x)`

Raíz cuadrada con dominio comprobado. Devuelve **número**.

Argumentos y dominio: Número finito x≥0. Casos límite: Negativos → error; NaN/inf → error de finitud; conserva -0; admite subnormales.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.raiz_segura(4))
```

Salida esperada:

```text
2
```

## `mate.log_seguro(x)`

Logaritmo natural con dominio comprobado. Devuelve **número**.

Argumentos y dominio: Número finito x>0. Casos límite: ±0 y negativos → error; NaN/inf → error de finitud; subnormales positivos admitidos.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.log_seguro(1))
```

Salida esperada:

```text
0
```

## `mate.potencia_segura(x,y)`

Potencia con dominio y rango comprobados. Devuelve **número**.

Argumentos y dominio: Finitos; base negativa exige exponente entero; base cero exige exponente positivo. Casos límite: 0^0 se rechaza en esta API; desbordamiento y pérdida de resultado no nulo a cero → error; subnormal representable aceptado. -0 sigue pow para exponente positivo.

Precisión: Resultado aproximado de libm; no se promete redondeo correcto universal. Se mantiene el nombre histórico.

```latino
poner(mate.potencia_segura(-2,3))
```

Salida esperada:

```text
-8
```

## `anumero(valor)`

Conversión numérica histórica, deliberadamente laxa. Devuelve **número o nulo**.

Argumentos y dominio: Números, cadenas, lógicos, nulo, listas y diccionarios. Casos límite: Cero → nulo, también "0", -0, falso, nulo y contenedores vacíos. Cadenas completamente convertibles usan strtod C (hex, NaN/inf incluidos); otras usan primer byte: "abc" → 97, "12x" → 49, " 0 " → 32. Listas/diccionarios → longitud. No usar para validar datos.

Precisión: Conversión double heredada; no distingue cero de fallo. anumero_estricto distingue cero y error.

```latino
poner(anumero("0"))
```

Salida esperada:

```text
nulo
```

## `anumero_estricto(valor)`

Convierte un decimal validado; cero sigue siendo un número. Devuelve **número**.

Argumentos y dominio: Número finito o cadena decimal ASCII con signo, punto y exponente; espacios ASCII exteriores opcionales. Casos límite: Admite .5 y 1.; rechaza cadena vacía, booleanos, colecciones, hexadecimal, coma, NaN/inf y basura. Conserva -0. Desbordamiento o mantisa no nula redondeada a cero → error; subnormales representables aceptados.

Precisión: Redondeo de strtod al double representable; no es decimal exacto. Se mantiene el nombre histórico.

```latino
poner(anumero_estricto(" 0 "))
```

Salida esperada:

```text
0
```
