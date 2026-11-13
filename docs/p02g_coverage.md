# Recuperación del caso coverage

El registro de CTest se importó en `b9df6f2` sin `ejemplos/coverage.lat`. El primer incremento recuperó la versión `2800fe11e11811b0cc37d76ac72a9be217789880` del historial de Latino. La revisión actual usa el contenido ejecutable de `777b91acfcb0836d94c3e4184001974ef1be0f1d`. Conserva condicionales, selección, bucles, operadores, recursión, listas, diccionarios, cadenas, argumentos variables y retornos múltiples. Se omiten comentarios sobre un módulo JSON externo y los dos bucles de argumentos variables expresan su límite mediante `lista.longitud(args)`.

CTest ejecuta el programa por ruta absoluta y coteja las 78 líneas de salida. Comprueba 3 para la suma, 55 para Fibonacci, la concatenación de los argumentos variables y los valores de la asignación múltiple. La regresión de marcos comprueba también el valor global homónimo, un prefijo de pila anterior a la llamada, argumentos variables vacíos y un argumento `nulo`.

Fuente del programa. [Historial de Latino](https://github.com/lenguaje-latino/latino/blob/777b91acfcb0836d94c3e4184001974ef1be0f1d/ejemplos/coverage.lat).

La recursión devuelve −80 en F+E para `fibo(10)`. El marco era condicional y una llamada con el mismo nombre reutilizaba el contexto. Cada llamada interpretada por `CALL_FUNCTION` ahora conserva sus parámetros en un contexto propio. El operando `a=1` de `STORE_NAME` distingue la vinculación de parámetros. Una prueba independiente comprueba el cálculo de Fibonacci y la conservación de una variable global homónima.

En la concatenación, el bucle leía una copia desactualizada de `i` en el marco después de asignarle cero. La escritura actualiza también el marco que se está ejecutando y conserva la actualización global existente. La regresión comprueba las dos lecturas por separado.

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
