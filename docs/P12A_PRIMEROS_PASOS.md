# Primeros pasos: expresiones y funciones

Una expresión calcula un valor. Una asignación le pone un nombre a ese valor.
Prueba este archivo guardándolo como `ejemplo.mict`:

```mictlan
precio = 5.25
cantidad = 4
escribir(precio * cantidad)  # 21
```

Ejecuta `./build-p12a-debug/mictlan ejemplo.mict`. El mismo contenido funciona
si cambias la extensión a `.lat`. El punto decimal se escribe igual en ambos.

## Agrupar una cuenta

La multiplicación y la potencia se hacen antes de sumar. Los paréntesis
permiten mostrar la intención sin exigir que el lector memorice una tabla:

```mictlan
escribir(2 + 3 * 4)       # 14
escribir((2 + 3) * 4)     # 20
escribir(2 ^ (3 ^ 2))     # 512
escribir((2 ^ 3) ^ 2)     # 64
```

En esta versión, `2 ^ 3 ^ 2` se agrupa como el último ejemplo. También existe
`**`, con prioridad diferente a `^`. Al mezclarlos, usa paréntesis. La
[tabla completa](SEMANTICA_P12A.md#precedencia-y-asociación) explica el detalle.

## Preguntar si algo es verdadero

`==` pregunta si dos valores son iguales; `=` asigna. `&&` exige que ambas
condiciones sean verdaderas. `||` acepta que cualquiera sea verdadera,
incluso las dos a la vez:

```mictlan
escribir(verdadero || verdadero)  # verdadero
edad = 18
escribir((edad >= 18) && (edad <= 65))
```

Usa paréntesis al combinar `&&` y `||`: en Mictlán tienen la misma prioridad.
Además, ambos lados se ejecutan siempre. Para condicionar una operación que
solo debe ocurrir en un caso, escribe un bloque:

```mictlan
divisor = 0
si divisor != 0
    escribir(10 / divisor)
sino
    escribir("El divisor debe ser distinto de cero")
fin
```

Compara números con números y textos con textos. Las conversiones automáticas
heredadas entre tipos pueden sorprender, especialmente con `==`.

## Encapsular una cuenta

Una función es una receta: recibe parámetros y puede devolver un resultado.
Aquí los nombres de parámetros se eligen distintos de los globales:

```mictlan
funcion total(precio_unitario, unidades)
    retorno precio_unitario * unidades
fin

importe = total(5.25, 4)
escribir(importe)  # 21
```

`retorno` termina la llamada. Si solo quieres los efectos de una función,
puedes llamarla como una sentencia y su resultado se descarta. Para cálculos,
escribe un retorno explícito. No dependas de que una función sin retorno
produzca un número: una asignación simple recibirá nulo.

## Una receta que se llama a sí misma

La recursión resuelve una tarea usando versiones más pequeñas de la misma
tarea. Necesita un caso base que detenga las llamadas:

```mictlan
funcion fibonacci(n)
    si n < 2
        retorno n
    fin
    retorno fibonacci(n - 1) + fibonacci(n - 2)
fin
escribir(fibonacci(12))  # 144
```

El caso base devuelve 0 para 0 y 1 para 1. Cada término siguiente suma los dos
anteriores: 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144. Esta versión es
útil para aprender, pero repite muchos cálculos; para números grandes conviene
un bucle o guardar resultados anteriores.

## Cuidar los nombres

Los parámetros pertenecen a la llamada, pero Mictlán conserva reglas de
ámbito de Latino: una asignación a un nombre que ya existe globalmente puede
cambiar ese global, incluso al reasignar un parámetro con el mismo nombre.
Un nombre local del llamante también puede ser visible en una llamada hija.
Por eso es más fácil razonar con funciones que devuelven resultados y usan
nombres de parámetros distintos de los globales. La
[especificación](SEMANTICA_P12A.md#asignaciones-y-ámbitos) muestra exactamente
qué se comprobó.

Los ejemplos son parte de una propuesta experimental. Consulta
[cómo verificarla en Debian](P12A_VERIFICAR_DEBIAN.md) para ejecutar las
pruebas automáticamente.
