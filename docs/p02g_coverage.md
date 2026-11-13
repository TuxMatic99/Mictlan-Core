# Recuperación del caso coverage

El registro de CTest se importó en `b9df6f2` sin `ejemplos/coverage.lat`. El programa procede del [historial de Latino](https://github.com/lenguaje-latino/latino/blob/2800fe11e11811b0cc37d76ac72a9be217789880/ejemplos/coverage.lat). Se restaura la versión de `2800fe11e11811b0cc37d76ac72a9be217789880`, que incluye condicionales, selección, bucles, operadores, recursión, listas, diccionarios y cadenas. La versión posterior `777b91a` agrega casos de retornos múltiples y falla con `correcto` indefinida en la base F+E. El incremento posterior agrega regresiones de asignacion y retorno múltiple y recupera ese caso por separado. La variante de 75 líneas conserva su referencia independiente.

El caso usa rutas absolutas desde CTest y comprueba las 75 líneas de salida. El archivo esperado describe la semántica que la rama ejerce actualmente. Se comprueban, entre otros, 3 para la suma, 55 para Fibonacci y las mutaciones de listas y diccionarios. La salida de los operadores lógicos sigue la semántica actual de Mictlán.

La recursión devuelve −80 en F+E para `fibo(10)`. El marco era condicional y una llamada con el mismo nombre reutilizaba el contexto. Cada llamada interpretada por `CALL_FUNCTION` ahora conserva sus parámetros en un contexto propio. El operando `a=1` de `STORE_NAME` distingue la vinculación de parámetros. Una prueba independiente comprueba el cálculo de Fibonacci y la conservación de una variable global homónima.

La primera matriz remota registró 11 bytes perdidos en dos conversiones `%s` de `imprimirf`. Se conserva esa ejecución como evidencia. El formato procesa argumentos desde sus celdas activas y libera sus buffers antes de propagar errores. La regresión cubre cadenas de más de 1024 bytes, porcentajes literales, formatos incompletos y conversión fuera de rango.

## Licencia del programa restaurado

Fuente del aviso: [LICENSE.txt de Latino](https://github.com/lenguaje-latino/latino/blob/2800fe11e11811b0cc37d76ac72a9be217789880/LICENSE.txt).

```
The MIT License (MIT)

Copyright (c) 2015 - Latino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
