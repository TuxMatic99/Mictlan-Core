# Colecciones de Mictlán

Una lista organiza elementos por posición; un diccionario asocia claves de texto
con valores. Esta referencia describe contratos comprobados en P1.2-B2 y conserva
las peculiaridades de Latino. `diccionario` es un alias de módulo de `dic`.

## Copiar y compartir

`b = a` comparte el mismo contenedor: cambiarlo mediante b se ve mediante a.
`lista.invertir` y `lista.concatenar` crean nodos nuevos y comparten sus elementos.
Una lista anidada es como una caja: dos contenedores pueden señalar la misma caja.
Cambiar su contenido se ve por ambos caminos; reemplazar un nodo de uno de ellos
no reemplaza el nodo del otro. `lista.extender` también comparte elementos.

```mict
a = [[1], 2]
b = a
c = lista.concatenar(a, [])
b[1] = 5
escribir(a[1])
escribir(c[1])
a[0][0] = 9
escribir(c[0][0])
```

Salida: `5`, `2`, `9`, en líneas separadas. Este ejemplo se ejecuta automáticamente.

No existe una función pública de copia profunda universal. El clonado interno
usado por agregar/insertar copia números y listas anidadas dentro de una lista,
pero comparte otros elementos como diccionarios. Clonar un diccionario copia la
tabla y sus celdas; los contenedores usados como valores siguen compartidos.
Los ciclos no se clonan ni se comparan recursivamente sin límite: se diagnostican.
Invertir y concatenar pueden conservar referencias cíclicas sin recorrerlas.

## Índices, claves e igualdad

Las posiciones empiezan en 0. Las bibliotecas conservan el truncamiento de índices
fraccionarios finitos hacia cero; se recomienda usar enteros. NaN, infinito y
números fuera del rango de int producen error antes de la conversión. Insertar
admite la posición final n; eliminar_indice requiere una posición existente.
Estas reglas describen las funciones de lista, no redefinen los corchetes de la VM.

Las claves son cadenas exactas: "ñ" ocupa dos bytes UTF-8 y no equivale a "n".
No hay normalización Unicode ni orden de inserción garantizado. La longitud de un
diccionario cuenta entradas; el límite de 63 bytes es para insertar una clave,
no para la longitud de sus valores. [] y {} son contenedores distintos vacíos.

`indice` y `eliminar` usan una igualdad por tipo más limitada que `contiene`.
Por compatibilidad `contiene([2], "2")` es verdadero e `indice([2], "2")` es -1.
El buscador por índice tampoco reconoce booleanos, nulo ni diccionarios como
coincidencias; no debe confundirse con una API moderna de igualdad estricta.
Comparar mira primero longitudes: [9] precede a [0, 0]. Entre números compara
valores sin truncar la diferencia; NaN produce error y los infinitos se ordenan.
Una futura igualdad uniforme requerirá nombres nuevos y un plan de compatibilidad.

## Costes y errores

n y m son cantidades de elementos; B son bytes del texto producido; C es el coste
del clonado selectivo, E el coste de comparar un elemento y T el de representar
objetos cuando el comparador heredado convierte tipos distintos a texto. Los
costes cuentan recorridos lógicos y reservas; no incluyen todos los costes del
registro de memoria de la VM ni del sistema operativo. El hash tiene coste
esperado constante para claves acotadas; una cubeta con muchas colisiones puede
requerir recorrer n entradas. Enumerar un diccionario recorre toda su capacidad.

Los argumentos tienen tipos estrictos. Un error no debe corromper la pila ni
retirar entradas ajenas. Las operaciones que modifican colecciones no devuelven
un elemento; no dependa de un retorno propio. Las reglas de asignación de P1.2-A
completan con nulo las posiciones de retorno que falten.

## lista.agregar

Añade un elemento al final.

`lista.agregar(l, valor)`

**Argumentos:** l: lista; valor: cualquier objeto

**Resultado:** Sin valor devuelto; en una asignación normal se completa con nulo.

**Original:** Modifica l.

**Entradas vacías:** Añade el primer elemento.

**Índices y claves:** No usa índice.

**Duplicados e igualdad:** Admite valores repetidos; agregar conserva además su guardia histórica contra listas estructuralmente iguales.

**Copias y referencias:** Clona el valor con la política heredada: al clonar una lista se copian sus números y listas anidadas; otros elementos, incluidos diccionarios, se comparten. Al clonar un diccionario se crea una tabla superficial con celdas nuevas y payloads compartidos.

**Coste aproximado:** O(C + T)

**Errores:** Error de tipo si no se recibe una lista. Agregar rechaza una lista igual al destino como referencia circular; clonar rechaza listas cíclicas o profundidad superior a 256.

**Compatibilidad:** Heredada; se corrigen índices indefinidos en insertar.

**Referencia histórica:** El manual describe la mutación; no define toda la política selectiva de clonado.

```mict
l = [1]
lista.agregar(l, 2)
escribir(l)
```

Salida:

```text
[1, 2]
```

## lista.insertar

Coloca un elemento antes de una posición.

`lista.insertar(l, valor, indice)`

**Argumentos:** l: lista; valor: cualquier objeto; indice: número

**Resultado:** Sin valor devuelto; en una asignación normal se completa con nulo.

**Original:** Modifica l.

**Entradas vacías:** Añade el primer elemento.

**Índices y claves:** Índice desde 0; fracciones finitas truncadas hacia cero; -0.9 equivale a 0. No hay índices negativos. Se valida el rango de int antes de convertir. Insertar admite 0..longitud, inclusive.

**Duplicados e igualdad:** Admite valores repetidos; agregar conserva además su guardia histórica contra listas estructuralmente iguales.

**Copias y referencias:** Clona el valor con la política heredada: al clonar una lista se copian sus números y listas anidadas; otros elementos, incluidos diccionarios, se comparten. Al clonar un diccionario se crea una tabla superficial con celdas nuevas y payloads compartidos.

**Coste aproximado:** O(n + C)

**Errores:** Error de tipo si no se recibe una lista. Índice no finito o no representable, o fuera de 0..n.

**Compatibilidad:** Heredada; se corrigen índices indefinidos en insertar.

**Referencia histórica:** El manual describe la mutación; no define toda la política selectiva de clonado.

```mict
l = [1]
lista.insertar(l, 2, 0)
escribir(l)
```

Salida:

```text
[2, 1]
```

## lista.extender

Añade a l los elementos de otra.

`lista.extender(l, otra)`

**Argumentos:** l, otra: listas

**Resultado:** Sin valor devuelto; asignación normal completa con nulo.

**Original:** Modifica l.

**Entradas vacías:** Extender con [] no cambia el destino.

**Índices y claves:** No usa índices. Extender una lista consigo misma duplica una sola vez sus nodos originales.

**Duplicados e igualdad:** Conserva duplicados.

**Copias y referencias:** Comparte los elementos existentes; no clona los objetos anidados. Reemplazar un nodo en una lista no reemplaza el nodo de otra, pero mutar un contenedor anidado compartido sí es visible.

**Coste aproximado:** O(m)

**Errores:** Error de tipo si no se recibe una lista. También valida el tipo de otra.

**Compatibilidad:** Heredada.

**Referencia histórica:** La compartición de elementos y la autoextensión se verifican en Core.

```mict
l = [1]
lista.extender(l, l)
escribir(l)
```

Salida:

```text
[1, 1]
```

## lista.eliminar_indice

Retira el elemento situado en una posición.

`lista.eliminar_indice(l, indice)`

**Argumentos:** l: lista; indice: número

**Resultado:** Sin valor devuelto; asignación normal completa con nulo.

**Original:** Modifica l.

**Entradas vacías:** Siempre produce error de rango.

**Índices y claves:** Índice desde 0; fracciones finitas truncadas hacia cero; -0.9 equivale a 0. No hay índices negativos. Se valida el rango de int antes de convertir. Rango: 0..n-1.

**Duplicados e igualdad:** Retira exactamente un nodo.

**Copias y referencias:** No destruye los objetos compartidos restantes.

**Coste aproximado:** O(n)

**Errores:** Error de tipo si no se recibe una lista. Error de rango, NaN, infinito o fuera de int.

**Compatibilidad:** Heredada para índices válidos; ahora rechaza conversiones indefinidas.

**Referencia histórica:** El manual también rechaza índices negativos; no especifica fracciones ni números no finitos.

```mict
l = [4, 5]
lista.eliminar_indice(l, 0)
escribir(l)
```

Salida:

```text
[5]
```

## lista.invertir

Devuelve los elementos en orden inverso.

`lista.invertir(l)`

**Argumentos:** l: lista

**Resultado:** Lista nueva.

**Original:** Conserva l.

**Entradas vacías:** Devuelve [].

**Índices y claves:** No usa índices.

**Duplicados e igualdad:** Conserva duplicados.

**Copias y referencias:** Comparte los elementos existentes; no clona los objetos anidados. Reemplazar un nodo en una lista no reemplaza el nodo de otra, pero mutar un contenedor anidado compartido sí es visible.

**Coste aproximado:** O(n)

**Errores:** Error de tipo si no se recibe una lista.

**Compatibilidad:** Heredada; recorrido lineal en lugar de búsquedas sucesivas.

**Referencia histórica:** El resultado nuevo se comprueba sin imponer una copia profunda.

```mict
escribir(lista.invertir([1, 2, 3]))
```

Salida:

```text
[3, 2, 1]
```

## lista.longitud

Cuenta los elementos, incluidos nulo y los repetidos.

`lista.longitud(l)`

**Argumentos:** l: lista

**Resultado:** Número entero representado por el tipo numérico del lenguaje.

**Original:** Conserva l.

**Entradas vacías:** Devuelve 0.

**Índices y claves:** No usa índices.

**Duplicados e igualdad:** Cuenta cada nodo.

**Copias y referencias:** No copia ni comparte un contenedor nuevo.

**Coste aproximado:** O(1)

**Errores:** Error de tipo si no se recibe una lista.

**Compatibilidad:** Heredada.

**Referencia histórica:** Coincide con el recuento descrito.

```mict
escribir(lista.longitud([1, nulo, 1]))
```

Salida:

```text
3
```

## lista.indice

Busca la primera posición de un valor.

`lista.indice(l, valor)`

**Argumentos:** l: lista; valor: cualquier objeto

**Resultado:** Número: índice encontrado o -1.

**Original:** Conserva l.

**Entradas vacías:** Devuelve -1.

**Índices y claves:** Índice desde 0; no recibe índice de entrada.

**Duplicados e igualdad:** Primera coincidencia por igualdad del mismo tipo: números, cadenas y listas. Booleanos, nulo y diccionarios no coinciden con este buscador heredado.

**Copias y referencias:** No copia objetos. Compara listas anidadas con la política descrita en comparar.

**Coste aproximado:** O(n × E)

**Errores:** Error de tipo si no se recibe una lista. La comparación de listas cíclicas o demasiado profundas puede producir error.

**Compatibilidad:** Heredada; no se unifica silenciosamente con contiene.

**Referencia histórica:** El manual no detalla las diferencias de igualdad entre contiene e indice.

```mict
escribir(lista.indice([2, 2, 3], 2))
```

Salida:

```text
0
```

## lista.encontrar

Busca la primera posición de un valor. Alias exacto de indice.

`lista.encontrar(l, valor)`

**Argumentos:** l: lista; valor: cualquier objeto

**Resultado:** Número: índice encontrado o -1.

**Original:** Conserva l.

**Entradas vacías:** Devuelve -1.

**Índices y claves:** Índice desde 0; no recibe índice de entrada.

**Duplicados e igualdad:** Primera coincidencia por igualdad del mismo tipo: números, cadenas y listas. Booleanos, nulo y diccionarios no coinciden con este buscador heredado.

**Copias y referencias:** No copia objetos. Compara listas anidadas con la política descrita en comparar.

**Coste aproximado:** O(n × E)

**Errores:** Error de tipo si no se recibe una lista. La comparación de listas cíclicas o demasiado profundas puede producir error.

**Compatibilidad:** Heredada; no se unifica silenciosamente con contiene.

**Referencia histórica:** El manual no detalla las diferencias de igualdad entre contiene e indice.

```mict
escribir(lista.encontrar([2, 2, 3], 2))
```

Salida:

```text
0
```

## lista.eliminar

Retira la primera coincidencia; si no la encuentra, no hace nada.

`lista.eliminar(l, valor)`

**Argumentos:** l: lista; valor: cualquier objeto

**Resultado:** Sin valor devuelto; asignación normal completa con nulo.

**Original:** Modifica l.

**Entradas vacías:** No hace nada.

**Índices y claves:** Busca desde el índice 0.

**Duplicados e igualdad:** Primera coincidencia por igualdad del mismo tipo: números, cadenas y listas. Booleanos, nulo y diccionarios no coinciden con este buscador heredado.

**Copias y referencias:** No destruye otras referencias al objeto retirado.

**Coste aproximado:** O(n × E)

**Errores:** Error de tipo si no se recibe una lista. La comparación anidada puede producir error.

**Compatibilidad:** Heredada.

**Referencia histórica:** Primera coincidencia y política de tipos comprobadas; no equivale a eliminar_indice.

```mict
l = [2, 2, 3]
lista.eliminar(l, 2)
escribir(l)
```

Salida:

```text
[2, 3]
```

## lista.contiene

Indica si algún elemento coincide con el comparador histórico.

`lista.contiene(l, valor)`

**Argumentos:** l: lista; valor: cualquier objeto

**Resultado:** Booleano.

**Original:** Conserva l.

**Entradas vacías:** Devuelve falso.

**Índices y claves:** No devuelve una posición.

**Duplicados e igualdad:** Basta una coincidencia. Entre tipos distintos puede comparar representaciones textuales: contiene([2], "2") es verdadero, mientras indice devuelve -1.

**Copias y referencias:** No copia objetos. Los pares de números se comparan sin truncar decimales.

**Coste aproximado:** O(n × E)

**Errores:** Error de tipo si no se recibe una lista. NaN en una comparación numérica, ciclos de listas o profundidad superior a 256 producen error.

**Compatibilidad:** Heredada, con corrección de falsos positivos numéricos.

**Referencia histórica:** El manual no especifica las conversiones textuales ni la diferencia con indice.

```mict
escribir(lista.contiene([0.1], 0.9))
```

Salida:

```text
falso
```

## lista.comparar

Compara primero longitudes; solo si son iguales compara elementos sucesivos.

`lista.comparar(a, b)`

**Argumentos:** a, b: listas

**Resultado:** Número -1, 0 o 1.

**Original:** Conserva ambas listas.

**Entradas vacías:** Dos listas vacías dan 0; la vacía precede a cualquier lista no vacía.

**Índices y claves:** El orden es primero por longitud, no lexicográfico.

**Duplicados e igualdad:** Admite repetidos. En cada par conserva los criterios heredados salvo el defecto de decimales. [nulo] no se considera igual a [nulo] por este comparador histórico.

**Copias y referencias:** No clona. Recorre listas anidadas y detecta pares de listas cíclicas.

**Coste aproximado:** O(N × h + T), h ≤ 256; O(h) de pila. N: elementos visitados.

**Errores:** Error de tipo; NaN numérico, ciclo encontrado o profundidad excedida. Inf == Inf; -Inf precede a Inf.

**Compatibilidad:** Heredada; corrige resta double convertida a int y recursión ilimitada.

**Referencia histórica:** La precedencia de longitud se toma del código, no de una suposición de orden lexicográfico.

```mict
escribir(lista.comparar([9], [0, 0]))
escribir(lista.comparar([0.1], [0.9]))
```

Salida:

```text
-1
-1
```

## lista.concatenar

Crea una lista con los elementos de a seguidos por los de b.

`lista.concatenar(a, b)`

**Argumentos:** a, b: listas

**Resultado:** Lista nueva.

**Original:** Conserva a y b.

**Entradas vacías:** Dos vacías dan []; una vacía deja los elementos de la otra en nodos nuevos.

**Índices y claves:** No usa índices.

**Duplicados e igualdad:** Conserva repetidos.

**Copias y referencias:** Comparte los elementos existentes; no clona los objetos anidados. Reemplazar un nodo en una lista no reemplaza el nodo de otra, pero mutar un contenedor anidado compartido sí es visible.

**Coste aproximado:** O(n + m)

**Errores:** Error de tipo si no se recibe una lista.

**Compatibilidad:** Heredada; recorrido lineal.

**Referencia histórica:** No promete una copia profunda.

```mict
escribir(lista.concatenar([1, 2], [2, 3]))
```

Salida:

```text
[1, 2, 2, 3]
```

## lista.crear

Construye una lista de valores nulo.

`lista.crear(cantidad)`

**Argumentos:** cantidad: número

**Resultado:** Lista nueva.

**Original:** No modifica un contenedor previo.

**Entradas vacías:** Cantidad ≤ 0 tras truncar da [].

**Índices y claves:** Índice desde 0; fracciones finitas truncadas hacia cero; -0.9 equivale a 0. No hay índices negativos. Se valida el rango de int antes de convertir. Es una cantidad, no una posición.

**Duplicados e igualdad:** Cada nodo contiene nulo.

**Copias y referencias:** Crea nodos nuevos; nulo es un valor común inmutable.

**Coste aproximado:** O(max(cantidad, 0)); memoria proporcional.

**Errores:** Error de tipo o cantidad no finita/fuera de int; memoria insuficiente para cantidades grandes.

**Compatibilidad:** Heredada para cantidades definidas; se evitan conversiones indefinidas.

**Referencia histórica:** El manual describe enteros positivos y negativos; se documenta el truncamiento observado.

```mict
escribir(lista.crear(2.9))
escribir(lista.crear(-1.8))
```

Salida:

```text
[nulo, nulo]
[]
```

## lista.separar

Une cadenas y números en un texto. El nombre histórico no divide una lista.

`lista.separar(l, separador = nulo)`

**Argumentos:** l: lista; separador: cadena o nulo; omitido equivale a nulo.

**Resultado:** Cadena o nulo si no produce ningún byte.

**Original:** Conserva l.

**Entradas vacías:** [], [""] y listas sin números/cadenas dan nulo. ["", ""] con ":" da ":".

**Índices y claves:** El separador se antepone a cada elemento admitido cuyo índice ORIGINAL sea mayor que 0. Omite otros tipos.

**Duplicados e igualdad:** Conserva textos repetidos y orden original.

**Copias y referencias:** Crea texto nuevo, no modifica ni copia los contenedores anidados. Números con %.16g, como en Latino.

**Coste aproximado:** O(n + B) tiempo y O(B) memoria temporal; B incluye todos los separadores.

**Errores:** Error de tipo del contenedor/separador, tamaño no representable o reserva fallida.

**Compatibilidad:** Heredada; corrección de reservas sin cambiar resultados.

**Referencia histórica:** El manual describe la unión bajo separar, pero su ejemplo final menciona insertar: se usa el nombre correcto.

```mict
escribir(lista.separar([1, "niño", 2], ":"))
escribir(lista.separar([nulo, "x", falso, "y"], ":"))
```

Salida:

```text
1:niño:2
:x:y
```

## lista.unir

Une cadenas y números en un texto. Nombre recomendado para programas nuevos; alias exacto de separar.

`lista.unir(l, separador = nulo)`

**Argumentos:** l: lista; separador: cadena o nulo; omitido equivale a nulo.

**Resultado:** Cadena o nulo si no produce ningún byte.

**Original:** Conserva l.

**Entradas vacías:** [], [""] y listas sin números/cadenas dan nulo. ["", ""] con ":" da ":".

**Índices y claves:** El separador se antepone a cada elemento admitido cuyo índice ORIGINAL sea mayor que 0. Omite otros tipos.

**Duplicados e igualdad:** Conserva textos repetidos y orden original.

**Copias y referencias:** Crea texto nuevo, no modifica ni copia los contenedores anidados. Números con %.16g, como en Latino.

**Coste aproximado:** O(n + B) tiempo y O(B) memoria temporal; B incluye todos los separadores.

**Errores:** Error de tipo del contenedor/separador, tamaño no representable o reserva fallida.

**Compatibilidad:** Nueva alternativa compatible.

**Referencia histórica:** El manual describe la unión bajo separar, pero su ejemplo final menciona insertar: se usa el nombre correcto.

```mict
escribir(lista.unir([1, "niño", 2], ":"))
escribir(lista.unir([nulo, "x", falso, "y"], ":"))
```

Salida:

```text
1:niño:2
:x:y
```

## dic.longitud

Cuenta las claves presentes, aunque sus valores sean nulo.

`dic.longitud(d)`

**Argumentos:** d: diccionario

**Resultado:** Número entero representado por el tipo numérico.

**Original:** Conserva d.

**Entradas vacías:** Devuelve 0.

**Índices y claves:** Claves cadena: exactas y sensibles a mayúsculas, sin normalización Unicode; se admite "". Al insertar el límite heredado es 63 bytes UTF-8. Consultar o eliminar una clave más larga simplemente no la encuentra. No se promete orden de inserción.

**Duplicados e igualdad:** Una clave repetida actualiza su celda; no añade otra entrada.

**Copias y referencias:** No copia objetos.

**Coste aproximado:** O(1)

**Errores:** Error de tipo si no se recibe un diccionario.

**Compatibilidad:** Heredada; contador corregido tras eliminar.

**Referencia histórica:** La tabla del manual dice longitud de texto, pero su ejemplo cuenta entradas: Core cuenta entradas.

```mict
escribir(dic.longitud({"a": nulo, "b": 2}))
```

Salida:

```text
2
```

## dic.llaves

Obtiene una lista de las claves presentes.

`dic.llaves(d)`

**Argumentos:** d: diccionario

**Resultado:** Lista nueva de cadenas.

**Original:** Conserva d.

**Entradas vacías:** Devuelve [].

**Índices y claves:** Claves cadena: exactas y sensibles a mayúsculas, sin normalización Unicode; se admite "". Al insertar el límite heredado es 63 bytes UTF-8. Consultar o eliminar una clave más larga simplemente no la encuentra. No se promete orden de inserción.

**Duplicados e igualdad:** Cada clave aparece una vez.

**Copias y referencias:** Nodos y objetos cadena nuevos; el internado puede compartir el texto inmutable. Es una instantánea de las claves.

**Coste aproximado:** O(capacidad + n + bytes de claves)

**Errores:** Error de tipo si no se recibe un diccionario.

**Compatibilidad:** Heredada.

**Referencia histórica:** El ejemplo del manual no garantiza orden; las pruebas usan conjuntos o una sola clave.

```mict
escribir(dic.llaves({"nombre": 1}))
```

Salida:

```text
["nombre"]
```

## dic.valores

Obtiene una lista de los valores presentes.

`dic.valores(d)`

**Argumentos:** d: diccionario

**Resultado:** Lista nueva.

**Original:** Conserva d.

**Entradas vacías:** Devuelve [].

**Índices y claves:** Claves cadena: exactas y sensibles a mayúsculas, sin normalización Unicode; se admite "". Al insertar el límite heredado es 63 bytes UTF-8. Consultar o eliminar una clave más larga simplemente no la encuentra. No se promete orden de inserción.

**Duplicados e igualdad:** Valores repetidos aparecen tantas veces como claves los contienen.

**Copias y referencias:** Nodos nuevos que apuntan a las celdas existentes del diccionario. Actualizar una clave puede cambiar la celda visible en la lista. Eliminar la clave conserva esa celda si aún está referenciada; reinsertar crea otra. No es una copia profunda.

**Coste aproximado:** O(capacidad + n)

**Errores:** Error de tipo si no se recibe un diccionario.

**Compatibilidad:** Heredada; se preserva la vida de los valores tras eliminar.

**Referencia histórica:** El manual no define estas referencias compartidas ni garantiza orden.

```mict
escribir(dic.valores({"dato": 8}))
```

Salida:

```text
[8]
```

## dic.vals

Obtiene una lista de los valores presentes. Alias exacto de valores.

`dic.vals(d)`

**Argumentos:** d: diccionario

**Resultado:** Lista nueva.

**Original:** Conserva d.

**Entradas vacías:** Devuelve [].

**Índices y claves:** Claves cadena: exactas y sensibles a mayúsculas, sin normalización Unicode; se admite "". Al insertar el límite heredado es 63 bytes UTF-8. Consultar o eliminar una clave más larga simplemente no la encuentra. No se promete orden de inserción.

**Duplicados e igualdad:** Valores repetidos aparecen tantas veces como claves los contienen.

**Copias y referencias:** Nodos nuevos que apuntan a las celdas existentes del diccionario. Actualizar una clave puede cambiar la celda visible en la lista. Eliminar la clave conserva esa celda si aún está referenciada; reinsertar crea otra. No es una copia profunda.

**Coste aproximado:** O(capacidad + n)

**Errores:** Error de tipo si no se recibe un diccionario.

**Compatibilidad:** Heredada; se preserva la vida de los valores tras eliminar.

**Referencia histórica:** El manual no define estas referencias compartidas ni garantiza orden.

```mict
escribir(dic.vals({"dato": 8}))
```

Salida:

```text
[8]
```

## dic.contiene

Comprueba si existe una clave, incluso cuando su valor es nulo.

`dic.contiene(d, clave)`

**Argumentos:** d: diccionario; clave: cadena

**Resultado:** Booleano.

**Original:** Conserva d.

**Entradas vacías:** Devuelve falso.

**Índices y claves:** Claves cadena: exactas y sensibles a mayúsculas, sin normalización Unicode; se admite "". Al insertar el límite heredado es 63 bytes UTF-8. Consultar o eliminar una clave más larga simplemente no la encuentra. No se promete orden de inserción.

**Duplicados e igualdad:** No cuenta repeticiones.

**Copias y referencias:** No copia objetos.

**Coste aproximado:** O(1) esperado por hash; O(n) con colisiones extremas, más lectura de la clave.

**Errores:** Error de tipo si no se recibe un diccionario. Error de tipo si clave no es cadena, incluso con d vacío.

**Compatibilidad:** Heredada; sustituye un recorrido completo por consulta por hash.

**Referencia histórica:** Se comprueba la clave, no se busca el valor.

```mict
escribir(dic.contiene({"dato": nulo}, "dato"))
```

Salida:

```text
verdadero
```

## dic.eliminar

Retira solo la entrada indicada. Si no existe, no hace nada.

`dic.eliminar(d, clave)`

**Argumentos:** d: diccionario; clave: cadena

**Resultado:** Sin valor devuelto; asignación normal completa con nulo.

**Original:** Modifica d y reduce su longitud una vez si encuentra la clave.

**Entradas vacías:** No hace nada; valida siempre el tipo de clave.

**Índices y claves:** Claves cadena: exactas y sensibles a mayúsculas, sin normalización Unicode; se admite "". Al insertar el límite heredado es 63 bytes UTF-8. Consultar o eliminar una clave más larga simplemente no la encuentra. No se promete orden de inserción.

**Duplicados e igualdad:** Repetir una eliminación no reduce otra vez la longitud. Otras claves de la misma cubeta sobreviven.

**Copias y referencias:** Desvincula y libera nodo y metadatos de la entrada. La celda de valor queda administrada por la VM para proteger otras referencias. Sin recolección optativa, puede vivir hasta cerrar la VM.

**Coste aproximado:** O(1) esperado por hash; O(n) con colisiones extremas.

**Errores:** Error de tipo si no se recibe un diccionario. Error de tipo si clave no es cadena.

**Compatibilidad:** Corrección funcional; preserva entradas ajenas y propiedad previa.

**Referencia histórica:** El manual no prescribe perder colisiones ni mantener una longitud incorrecta.

```mict
d = {"a": 1, "b": 2}
dic.eliminar(d, "a")
escribir(dic.longitud(d))
escribir(d.b)
```

Salida:

```text
1
2
```

## Ejecución y limitaciones

Los ejemplos anteriores forman `tests/colecciones/documentacion.mict`; la misma
fuente se ejecuta con extensiones .lat y .mict y salidas explícitas. La generación
se verifica con `python3 tools/generar_doc_colecciones.py --check`. La suite incluye
casos de colisiones, crecimiento, referencias, clonado selectivo, ciclos, índices
inválidos, reservas fallidas y separadores largos.

No se añade copia profunda general, igualdad uniforme ni orden de inserción.
La memoria administrada sin recolección optativa puede retener celdas retiradas
hasta cerrar la VM: preserva referencias compartidas, no es una fuga al cierre.
No se añade dependencia externa ni se modifica el parser. Véanse
[P12B2_INFORME.md](P12B2_INFORME.md) y [DEBIAN_P12B2.md](DEBIAN_P12B2.md).
