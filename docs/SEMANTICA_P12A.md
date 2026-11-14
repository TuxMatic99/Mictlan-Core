# Operadores, números y funciones de Mictlán

Edición experimental P1.2-A, 13 de noviembre de 2026. Describe la consola y la
API C derivadas de `9f3a1965f2794c8921adf9cdfa3e0c192060c657`, con las dos
correcciones registradas en [Decisiones](P12A_DECISIONES.md). Los mismos
programas y resultados se comprueban con las extensiones `.mict` y `.lat`.

## De la expresión al resultado

El escáner reconoce palabras y números. El parser agrupa los tokens en un
árbol de sintaxis, por ejemplo `2 + (3 * 4)`. `src/latdo.c` recorre ese árbol
y emite instrucciones: cargar 2, cargar 3, cargar 4, multiplicar y sumar.
La máquina virtual de `src/latmv.c` ejecuta ese bytecode con una pila de valores.
Así se distinguen tres problemas diferentes: reconocer un decimal, agrupar
operadores y ejecutar la operación sobre valores del tipo correcto.

Las fuentes son `tools/parser/latlex.l` y `latparse.y`; la compilación ordinaria
utiliza los archivos C ya distribuidos en `src/`. En P1.2-A se sincroniza la
acción decimal en el `.l` y el C existente y se mantienen sus tablas de tokens.
Hay una diferencia anterior: el `.l` también enumera `regresar` y `devolver`
como retorno, pero las tablas distribuidas los reconocen como identificadores.
Las pruebas preservan programas que usan esos nombres. Regenerar todo el
escáner requiere resolver primero esa incompatibilidad; véase el informe.

## Números

Los literales `5`, `5.25`, `.25`, `525e-2` y `5.25e1` producen valores del
mismo tipo numérico C, `double`. `tipo(5)` y `tipo(5.25)` escriben `decimal`,
el nombre histórico. No hay un tipo entero arbitrariamente grande para los
literales del lenguaje. Los enteros de la interfaz C son otra representación
heredada y no amplían este contrato.

El punto es el separador decimal del código, también cuando la aplicación
anfitriona usa coma. `5.25` vale 21/4. `/` es división real: `5 / 2` vale 2.5.
`%` usa el resto de punto flotante `fmod`: `-5 % 2` vale −1. `^` y `**` usan
`pow`. Las operaciones aritméticas binarias usan la conversión numérica
heredada descrita más abajo. El prefijo `-` convierte y cambia el signo;
el prefijo `+` conserva el valor sin conversión adicional.
Los valores de `double` tienen precisión finita; en Debian x86_64,
`0.1 + 0.2 == 0.3` da falso por redondeo binario. No es aritmética decimal exacta.
Esta entrega no define una nueva política para infinitos, NaN, desbordamientos
o división por cero.

## Precedencia y asociación

De mayor a menor prioridad. La asociación indica cómo se agrupan operadores
del mismo nivel. Los paréntesis siempre permiten elegir otra agrupación.

| Prioridad | Operadores o construcción | Asociación / regla | Ejemplo verificado |
| --- | --- | --- | --- |
| Acceso | `f(...)`, `obj.campo`, `obj[indice]`, `(expresión)` | Construcción de llamada o acceso; paréntesis agrupan | `sumar(identidad(10), sumar(20, identidad(12)))` → 42 |
| 7 | `**` | Izquierda | `2 ** 3 ** 2` → 64 |
| 6 | `^` | Izquierda | `2 ^ 3 ^ 2` → 64; `2 ^ 3 ** 2` → 512 |
| 5 | `*`, `/`, `%`; prefijos `+`, `-`, `!` | Binarios a izquierda. Los prefijos tienen precedencia declarada de `*` | `-2 ^ 2` → −4; `(-2) ^ 2` → 4 |
| 4 | `+`, `-` binarios | Izquierda | `10 - 3 - 2` → 5 |
| 3 | `<`, `<=`, `>`, `>=`, `==`, `!=`, `~=` | Izquierda, mismo nivel | `2 == 1 < 1` equivale a `(2 == 1) < 1` y da verdadero |
| 2 | `&&`, `\|\|` | Izquierda, mismo nivel | `verdadero \|\| falso && falso` → falso |
| 1 | `..` | Derecha | `"a" .. 2 + 3` → `"a5"` |
| 0 | `condición ? a : b` | Derecha | `1 ? 2 : 0 ? 3 : 4` → 2 |

`~=` conserva la precedencia declarada; el motor de expresiones regulares no
se valida de nuevo en esta entrega. Las formas `=`, `+=`, `-=`, `*=`, `/=`,
`%=`, `..=` son sentencias de asignación. No se presentan como expresiones
encadenables. `contador++` y `contador--` se prueban como sentencias sobre
variables numéricas; no se promete un valor de resultado para usarlas en otra
expresión. El parser admite otras combinaciones heredadas fuera de este contrato.

La potencia se calcula antes que multiplicar y sumar: `2 + 3 * 2 ^ 3` vale 26.
Las dos formas de potencia no tienen la misma prioridad. Para escribir una
potencia de potencia usa `2 ^ (3 ^ 2)`, cuyo resultado es 512.

## Comparaciones y lógica

Con números, las relaciones tienen su significado aritmético habitual. Si
alguno de los dos operandos de `<`, `<=`, `>` o `>=` es numérico, ambos se
convierten mediante `latC_adouble`: nulo vale 0, falso 0, verdadero 1 y una
cadena numérica completa se convierte a número. Una lista o diccionario se
convierte a su longitud. Si una cadena no es completamente numérica, se
conserva la conversión histórica al valor de su primer byte. Evita depender
de esta última regla en código nuevo. Entre dos cadenas, las relaciones usan
el orden de sus bytes; no es ordenación lingüística por idioma.

`<=` se corrige para seguir esa misma regla: `2 <= "3"` es verdadero y
`"3" <= 2` es falso. Se conserva la igualdad heredada dependiente del operando
izquierdo: `2 == "02"` da verdadero y `"02" == 2` da falso. No es una relación
de equivalencia uniforme entre tipos. Compara valores del mismo tipo o
conviértelos explícitamente antes de compararlos.

| a | b | `a && b` | `a \|\| b` |
| --- | --- | --- | --- |
| falso | falso | falso | falso |
| falso | verdadero | falso | verdadero |
| verdadero | falso | falso | verdadero |
| verdadero | verdadero | verdadero | verdadero |

OR es inclusivo. Para otros tipos, `a && b` selecciona `b` si `a` es verdadero,
y `a` en otro caso; `a || b` selecciona `a` si es verdadero, y `b` en otro caso.
Por eso `7 || 9` da 7 y `7 && 9` da 9. Se conserva el clonado histórico del
valor elegido; esta sección no promete identidad de contenedores.

Ambos operandos se evalúan de izquierda a derecha: **no hay cortocircuito**.
`falso && comprobar()` ejecuta `comprobar()`. El ternario sí ejecuta solo la
rama elegida. Para proteger una llamada usa `si condición ... fin`.
`!` siempre produce un booleano. Son falsos nulo, cero, cadenas vacías,
`"0"`, `"falso"` y `"false"` (las dos palabras sin distinguir mayúsculas), listas
y diccionarios vacíos. Son verdaderos los valores opuestos admitidos.

El [manual histórico](https://manual.lenguajelatino.org/es/stable/sintaxis/Logicos.html)
no sustituye estos resultados: su descripción abreviada de OR puede leerse
como exclusión de la fila con dos verdaderos, lo que no sucede en Core.

## Funciones, retornos y recursión

```mictlan
funcion fibonacci(n)
    si n < 2
        retorno n
    fin
    retorno fibonacci(n - 1) + fibonacci(n - 2)
fin
escribir(fibonacci(12))  # 144
```

Cada llamada tiene parámetros y un marco propio. Los argumentos se evalúan
de izquierda a derecha antes de vincular parámetros. Se admiten llamadas
anidadas, recursión directa y recursión mutua cuando las funciones ya están
definidas al ejecutarse la llamada. El ejemplo da F(12)=144 con F(0)=0 y F(1)=1.
La recursión usa recursos finitos y el límite heredado `MAX_CALL_FUNCTION=1024`;
no se garantiza optimización de llamadas finales ni profundidad ilimitada.
La suite prueba profundidades pequeñas, no agota ese límite.

`retorno expresión` sale inmediatamente y entrega su valor; `retornar` y
`ret` también se reconocen. Varias expresiones separadas por comas producen
varios valores para una asignación múltiple. Una llamada como sentencia
descarta sus resultados, conservando sus efectos. No acumula valores en los
bucles comprobados. Una función que llega a `fin` no declara retornos;
`x = sin_valor()` completa `x` con nulo según la regla de asignación.
Se recomienda usar estas funciones como sentencias y expresar los retornos
que se necesitan en cálculos.

Una función de parámetros fijos completa argumentos ausentes con nulo y
rechaza el exceso. `...` conserva su soporte heredado, cubierto por las pruebas
anteriores de Alpha y `coverage`; P1.2-A no redefine expansión ni retornos
múltiples dentro de expresiones arbitrarias.

## Asignaciones y ámbitos

El tipo pertenece al valor; un nombre puede recibir primero un número y luego
una cadena. Una asignación múltiple evalúa los valores antes de vincular los
destinos: `a, b = b, a` intercambia los valores. Si faltan valores se completa
con nulo; si sobran, se descartan. Las asignaciones compuestas realizan la
operación sobre el valor anterior y lo almacenan.

El ámbito heredado se parece a entregar a cada llamada una copia del cuaderno
de nombres del llamante. El marco contiene esos nombres y sus parámetros;
al leer se busca primero allí y después en el global. Por ello una función
llamada puede ver un nombre local del llamante; no son clausuras léxicas.
Un nombre nuevo creado dentro de una función no queda visible al regresar.
Los bloques `si` y los bucles no crean otro marco.

Una asignación ordinaria a un nombre que ya existe globalmente cambia tanto
el global como el marco activo. Vincular inicialmente un parámetro **no**
modifica el global homónimo. Sin embargo, **reasignar** ese parámetro aplica
la regla ordinaria y puede modificar el global. Los nombres de funciones
globales no pueden reasignarse como variables. `global nombre = valor` dirige
el almacenamiento al contexto global. Las copias de marcos anteriores pueden
seguir leyendo el valor que tenían al entrar, aunque una llamada hija haya
modificado el global. La suite fija ese comportamiento explícitamente.

Para enseñar y escribir programas nuevos, utiliza nombres distintos para
parámetros y globales, devuelve resultados y asígnalos en el llamante. Un
cambio a ámbitos léxicos necesitaría un diseño de compatibilidad propio.

La [suite de conformidad](../tests/conformidad/README.md) y la
[guía Debian](P12A_VERIFICAR_DEBIAN.md) permiten comprobar estas reglas.
