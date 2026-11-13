# Especificación inicial de Mictlán

**Edición Alpha 0.1 · 13 de noviembre de 2026.** Describe la sintaxis reconocida y el comportamiento comprobado de esta propuesta. La gramática fuente está en [latparse.y](../tools/parser/latparse.y) y el analizador léxico en [latlex.l](../tools/parser/latlex.l). Las características señaladas como experimentales requieren pruebas adicionales para formar parte del contrato de estabilidad.

## Archivos y palabras

El intérprete lee programas de texto con extensión `.mict` o `.lat`. La entrada habitual es UTF-8. Los identificadores usan letras ASCII, dígitos y guion bajo, con una letra o guion bajo inicial. Los nombres distinguen mayúsculas. La regla de constantes reconoce las secuencias de letras mayúsculas y guiones bajos. La tabla de símbolos admite hasta 63 bytes por nombre o clave. Los comentarios empiezan con `#` o `//` y terminan al final de la línea. También se admiten comentarios `/* ... */`. Los saltos de línea actúan como espacio en la gramática del archivo.

| Grupo | Palabras reservadas actuales |
| --- | --- |
| Literales | `verdadero`, `cierto`, `falso`, `nulo` |
| Selección | `si`, `osi`, `sino`, `fin`, `elegir`, `caso`, `otro`, `defecto` |
| Iteración | `mientras`, `repetir`, `hasta`, `desde`, `para`, `en`, `rango`, `romper`, `continuar` |
| Funciones y alcance | `funcion`, `fun`, `retorno`, `retornar`, `regresar`, `devolver`, `ret`, `global` |
| Etiquetas y objetos | `ir`, `clase`, `constructor`, `hereda`, `super`, `mi`, `estatico`, `nueva` |

`escribir`, `incluir` y `error` son funciones de la biblioteca base. El escáner heredado puede ignorar caracteres ajenos al léxico. Evita identificadores Unicode mientras se define su contrato.

## Gramática de referencia

Fragmento EBNF de las construcciones habituales. Las alternativas completas y precedencias están en la gramática fuente.

```ebnf
programa       = { sentencia } ;
sentencia      = destino "=" expresión | llamada | función | selección
               | bucle | retorno | "global" (asignación | función) ;
llamada        = acceso "(" [ expresión { "," expresión } ] ")" ;
función        = ("funcion" | "fun") identificador "(" [ parámetros ] ")"
                 sentencia { sentencia } "fin" ;
parámetros     = identificador { "," identificador } [ "," "..." ] | "..." ;
retorno        = palabra_de_retorno expresión { "," expresión } ;
selección      = "si" expresión bloque { "osi" expresión bloque }
                 [ "sino" bloque ] "fin" ;
bucle          = "mientras" expresión bloque "fin"
               | "repetir" bloque "hasta" expresión
               | "desde" "(" asignación ";" expresión ";" sentencia ")" bloque "fin"
               | "para" identificador "en" "rango" "(" límites ")" bloque "fin" ;
lista          = "[" [ expresión { "," expresión } ] "]" ;
diccionario    = "{" [ clave ":" expresión { "," clave ":" expresión } ] "}" ;
acceso         = identificador { "." identificador | "[" expresión "]" } ;
```

`elegir` usa `caso valor:` y un caso final opcional `otro:` o `defecto:`. `para` acepta un fin, inicio y fin, o inicio, fin y paso. El límite final es exclusivo y el paso predeterminado es 1. Los límites actuales se expresan como literales finitos. `continuar` está reservado y su ejecución queda fuera del contrato. Las clases, la herencia, los métodos estáticos y las etiquetas `ir` conservan su sintaxis heredada y son experimentales.

## Variables y funciones

Una asignación crea o actualiza una asociación entre nombre y valor. El tipo pertenece al valor. Una función interpretada ejecuta su cuerpo en un marco propio con sus parámetros. Al buscar un nombre se consulta el marco activo y luego el contexto global. Los marcos heredan valores del llamante. Una asignación ordinaria a un nombre ya global puede actualizar ese global y la copia activa, salvo la vinculación de parámetros. `global` dirige una asignación o una definición al contexto global. Evita depender de ámbitos léxicos de clausura o de cambios en copias de marcos anteriores.

```mictlan
funcion componentes(a, b)
    retorno a, b
fin
x, y = componentes(8, 16)
escribir(x + y)
```

La asignación múltiple vincula los valores en orden, completa con `nulo` los destinos sin valor y descarta el exceso. Las funciones pueden usar `...` al final de los parámetros. `[...]` forma una lista y una llamada puede expandir los argumentos. Los consumidores C usan su firma de registro y el contrato de pila de Core.

## Valores y operadores

| Valor | Ejemplos y contrato |
| --- | --- |
| Numérico | `8`, `1.5`, `2e3`. Los literales numéricos se almacenan como `double`. |
| Lógico | `verdadero`, `cierto`, `falso`. |
| Nulo | `nulo`, ausencia de valor. |
| Cadena | Comillas simples o dobles, con escapes heredados. `..` concatena. La longitud o el índice de una cadena pueden expresarse en bytes según la función. |
| Lista | `[8, 16]`. El primer índice es 0. Admite sustitución, sublistas y uso compartido. |
| Diccionario | `{vector: [8, 16]}` o `{"vector": [8, 16]}`. Acceso mediante `d.vector` o `d["vector"]`. |
| Función | Valor interpretable o función registrada por C. Los enteros y caracteres de la interfaz C conservan su representación heredada. |

`+ - * / %` realizan las operaciones aritméticas, y `^` y `**` son formas de potencia. `== != < <= > >=` comparan y `~=` aplica una expresión regular según el motor disponible. `!`, `&&` y `||` convierten operandos a lógico. Usa paréntesis cuando importen el orden o los efectos de las llamadas. La precedencia heredada de `&&` y `||` es la misma. `condición ? a : b` elige un resultado. `+= -= *= /= %= ..=` actualizan el destino y `++ --` incrementan o decrementan variables.

`alogico` considera falsos `nulo`, 0, cadenas vacías, `"0"`, `"falso"` y `"false"` sin distinguir mayúsculas en esas dos palabras, y colecciones vacías. `anumero` conserva coerciones históricas. Una conversión cuyo valor numérico sea cero devuelve `nulo`, incluso para `"0"`. Para comprobar entradas en ciencia usa el contrato de cada función, que exige valores numéricos finitos. `acadena` produce una representación textual. El orden de impresión de diccionarios queda fuera del contrato.

## Colecciones, identidad y memoria

Si `alias = original` y ambos contienen la misma lista, `alias[0] = 99` cambia lo que se lee a través de ambos nombres. Reasignar `original` a una lista nueva conserva la lista anterior mientras `alias` la alcance. Los diccionarios también pueden compartir contenidos. La clonación explícita y la construcción de listas tienen reglas propias. Se rechaza clonar una lista cíclica. La asignación de variables carece de copia profunda implícita. El [contrato de propiedad](p02g_contrato_memoria.md) define raíces, referencias de C y límites de recolección.

## Diagnósticos, bibliotecas y ejecución

Los diagnósticos de ejecución usan ruta, línea y columna cuando están disponibles. `error("mensaje")` provoca un error recuperable en una llamada protegida. El ejecutor devuelve 0 al terminar correctamente y 1 al fallar. El estado interno `LAT_ERRRUN` vale 2. La consola interactiva continúa tras errores de sintaxis o ejecución comprobados. Para un bloque interactivo, termina cada línea intermedia con `\`. No hay una construcción pública de captura de excepciones en el lenguaje Alpha.

Core incluye funciones base y módulos como `cadena`, `lista`, `dic`, `archivo`, `mate` y `paquete`. El orden de `incluir` y los nombres históricos de extensiones nativas se detallan en [Compatibilidad](COMPATIBILIDAD.md). Las funciones científicas `ciencia.longitud2`, `ciencia.producto_punto2` y `ciencia.sumar2` están disponibles en `mictlan-ciencia`. La UI experimental utiliza `host_ui` del SDK. El uso de un solo ejecutor para cargar todos los módulos se evaluará después de la Alpha.

El corpus de compatibilidad consta de siete programas originales de Latino y el caso histórico `coverage`, además de pruebas de alias, errores y módulos en las dos extensiones. Consulta [procedencia del corpus](../tests/alpha/latino/PROCEDENCIA.md).
