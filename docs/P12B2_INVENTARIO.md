# Inventario de lista y dic

Generado de `tests/colecciones/api.json`, cotejado con los registros nativos.

| Función | Resultado | Modifica | Vacío | Coste | Compatibilidad |
|---|---|---|---|---|---|
| lista.agregar | Sin valor devuelto; en una asignación normal se completa con nulo. | Modifica l. | Añade el primer elemento. | O(C + T) | Heredada; se corrigen índices indefinidos en insertar. |
| lista.insertar | Sin valor devuelto; en una asignación normal se completa con nulo. | Modifica l. | Añade el primer elemento. | O(n + C) | Heredada; se corrigen índices indefinidos en insertar. |
| lista.extender | Sin valor devuelto; asignación normal completa con nulo. | Modifica l. | Extender con [] no cambia el destino. | O(m) | Heredada. |
| lista.eliminar_indice | Sin valor devuelto; asignación normal completa con nulo. | Modifica l. | Siempre produce error de rango. | O(n) | Heredada para índices válidos; ahora rechaza conversiones indefinidas. |
| lista.invertir | Lista nueva. | Conserva l. | Devuelve []. | O(n) | Heredada; recorrido lineal en lugar de búsquedas sucesivas. |
| lista.longitud | Número entero representado por el tipo numérico del lenguaje. | Conserva l. | Devuelve 0. | O(1) | Heredada. |
| lista.indice | Número: índice encontrado o -1. | Conserva l. | Devuelve -1. | O(n × E) | Heredada; no se unifica silenciosamente con contiene. |
| lista.encontrar | Número: índice encontrado o -1. | Conserva l. | Devuelve -1. | O(n × E) | Heredada; no se unifica silenciosamente con contiene. |
| lista.eliminar | Sin valor devuelto; asignación normal completa con nulo. | Modifica l. | No hace nada. | O(n × E) | Heredada. |
| lista.contiene | Booleano. | Conserva l. | Devuelve falso. | O(n × E) | Heredada, con corrección de falsos positivos numéricos. |
| lista.comparar | Número -1, 0 o 1. | Conserva ambas listas. | Dos listas vacías dan 0; la vacía precede a cualquier lista no vacía. | O(N × h + T), h ≤ 256; O(h) de pila. N: elementos visitados. | Heredada; corrige resta double convertida a int y recursión ilimitada. |
| lista.concatenar | Lista nueva. | Conserva a y b. | Dos vacías dan []; una vacía deja los elementos de la otra en nodos nuevos. | O(n + m) | Heredada; recorrido lineal. |
| lista.crear | Lista nueva. | No modifica un contenedor previo. | Cantidad ≤ 0 tras truncar da []. | O(max(cantidad, 0)); memoria proporcional. | Heredada para cantidades definidas; se evitan conversiones indefinidas. |
| lista.separar | Cadena o nulo si no produce ningún byte. | Conserva l. | [], [""] y listas sin números/cadenas dan nulo. ["", ""] con ":" da ":". | O(n + B) tiempo y O(B) memoria temporal; B incluye todos los separadores. | Heredada; corrección de reservas sin cambiar resultados. |
| lista.unir | Cadena o nulo si no produce ningún byte. | Conserva l. | [], [""] y listas sin números/cadenas dan nulo. ["", ""] con ":" da ":". | O(n + B) tiempo y O(B) memoria temporal; B incluye todos los separadores. | Nueva alternativa compatible. |
| dic.longitud | Número entero representado por el tipo numérico. | Conserva d. | Devuelve 0. | O(1) | Heredada; contador corregido tras eliminar. |
| dic.llaves | Lista nueva de cadenas. | Conserva d. | Devuelve []. | O(capacidad + n + bytes de claves) | Heredada. |
| dic.valores | Lista nueva. | Conserva d. | Devuelve []. | O(capacidad + n) | Heredada; se preserva la vida de los valores tras eliminar. |
| dic.vals | Lista nueva. | Conserva d. | Devuelve []. | O(capacidad + n) | Heredada; se preserva la vida de los valores tras eliminar. |
| dic.contiene | Booleano. | Conserva d. | Devuelve falso. | O(1) esperado por hash; O(n) con colisiones extremas, más lectura de la clave. | Heredada; sustituye un recorrido completo por consulta por hash. |
| dic.eliminar | Sin valor devuelto; asignación normal completa con nulo. | Modifica d y reduce su longitud una vez si encuentra la clave. | No hace nada; valida siempre el tipo de clave. | O(1) esperado por hash; O(n) con colisiones extremas. | Corrección funcional; preserva entradas ajenas y propiedad previa. |

Los campos completos por función (firma, tipos, índices, claves, duplicados,
referencias, errores y contraste histórico) están en [COLECCIONES.md](COLECCIONES.md).
Hay 14 nombres heredados en lista y uno nuevo, unir; dic conserva sus seis nombres.
El módulo diccionario conserva la misma tabla de dic. Ningún nombre previo se retira.

## Herencia y alternativas

| Aspecto | Heredado verificado | Alternativa o corrección B2 |
|---|---|---|
| Unión | separar une texto; omite otros tipos y puede devolver nulo | unir es un alias, sin alterar el contrato |
| Índices | Desde 0, fracciones truncadas; sin negativos | Se rechazan conversiones no finitas o fuera de int |
| Eliminar clave | Desconectaba toda la cubeta y dejaba mal la longitud | Retira un nodo, conserva colisiones y valores compartidos |
| Contiene clave | Recorrido completo de cubetas | Consulta por hash, incluso con valor nulo |
| Decimales | Comparar/contiene podían igualar 0.1 y 0.9 | Comparación numérica correcta dentro de colecciones |
| Igualdad mixta | contiene e indice emplean criterios distintos | Conservada; una API uniforme queda como propuesta futura |
| Copias | Política selectiva y referencias compartidas | Se documenta y prueba, sin imponer copia profunda nueva |
| Enumeración dic | Orden de cubetas y colisiones | No se promete orden de inserción |
| Ciclos | Comparación recursiva sin protección | Error controlado; invertir/concatenar conservan referencias |

## Referencias históricas

* [Lista de Latino](https://manual.lenguajelatino.org/es/stable/librerias/Lista.html)
* [Diccionario de Latino](https://manual.lenguajelatino.org/es/stable/librerias/Diccionario.html)

Consultadas como referencia histórica el 14 de noviembre de 2026. La definición
verificada procede del código y las regresiones de Core, no de asumir que los
ejemplos del manual sean exhaustivos o infalibles. La tabla histórica de longitud
de diccionario menciona texto pese a que cuenta entradas; el ejemplo de separar
menciona insertar. Ambos se aclaran sin cambiar contratos válidos.
