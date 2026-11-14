# Compatibilidad en Mictlán Alpha

**13 de noviembre de 2026.** Ámbito comprobado en Linux x86_64. Una versión experimental puede corregir defectos observables con una regresión, evidencia antes y después y registro en este contrato.

| Área | Compromiso para esta Alpha |
| --- | --- |
| Archivos | `.mict` es la extensión principal. `.lat` usa el mismo analizador e intérprete. Son extensiones en minúsculas. Los siete programas originales de Latino y `coverage` se conservan como pruebas. |
| Sintaxis y resultados | Se preserva el comportamiento de los programas válidos cubiertos. Los cambios generales de gramática o coerción necesitan una propuesta y comparación con pruebas anteriores. |
| Bibliotecas | Se mantienen `latC_*`, los nombres y estructuras públicas y `liblatino.so`. Se conserva `dic` y se añade `diccionario` como alias de la misma tabla. La carga nativa conserva las convenciones históricas de nombres. |
| Binarios | El CI compara el conjunto de exportaciones con F+E y P0.2-G, los campos examinados y un ejecutable del SDK construido con F+E. Los binarios de Latino original requieren una comprobación propia de ABI. |
| Identidad | La versión del producto se obtiene de `include/mictlan_version.h`. `--version` muestra Mictlán. `LAT_VERSION` y `LAT_DERECHOS` conservan el valor heredado para consumidores. La extensión de VS Code tiene su propia versión. |
| Errores | El ejecutor informa por stderr y devuelve 1 al fallar. `latC_llamar_funcion` devuelve `LAT_ERRRUN` con valor 2 para errores de ejecución protegidos. Se conserva el texto que verifican los contratos de ciencia. |

## Correcciones observables

`--help` admite además `-h`, `--` permite rutas que empiezan con guion y las opciones desconocidas devuelven error. Los argumentos que siguen al archivo son del programa, incluido el caso histórico `menu(argc, argv)`. Las expresiones `-e` devuelven error cuando falla su ejecución. La consola admite continuar una entrada con una barra invertida al final de cada línea intermedia.

El parser devuelve un error ante sintaxis inválida y libera nodos descartados. Las asignaciones compuestas y `menu` conservan sus valores y corrigen accesos inválidos detectados por ASan. Los límites de `para ... en rango(...)` son literales numéricos finitos en esta Alpha. Un rango vacío ejecuta cero iteraciones. Una entrada incompatible o un paso cero generan un diagnóstico.

`incluir("nombre")` consulta primero todas las ubicaciones históricas de `.lat`, luego busca `.mict` en esas ubicaciones. `incluir("nombre.mict")` o `incluir("nombre.lat")` selecciona el archivo con su extensión. Un módulo con error propaga el diagnóstico al llamante. El orden de carpetas es el directorio actual, el del archivo que incluye y `LATINO_LIB`.

La asociación MIME de Linux declara solo `.mict`. En VS Code se puede asignar `.lat` a Mictlán en un proyecto concreto. Esto permite usar otras herramientas con programas Latino en el mismo equipo. Los iconos originales y las atribuciones se conservan.

## Alcance del recolector

El recolector se habilita explícitamente en programas anfitriones que cumplen el [contrato de memoria](p02g_contrato_memoria.md). La consola y el SDK de distribución conservan el ciclo de vida F de forma predeterminada. La compilación repetida y las cadenas internadas conservan memoria hasta cerrar la VM. Los ensayos de agotamiento inyectan un fallo en una reserva y una reasignación protegidas. La inicialización, el parser y las mutaciones parciales de contenedores tienen rutas que requieren pruebas propias.

## Precisión semántica P1.2-A

Se corrigen dos resultados demostrados: `<=` aplica la misma conversión
numérica que las otras relaciones cuando alguno de los operandos es numérico,
y los literales decimales usan punto también al compilar desde un anfitrión
con coma decimal. Se conservan las precedencias, las coerciones de igualdad
y los marcos heredados. [Decisiones anteriores al parche](P12A_DECISIONES.md),
[reglas verificadas](SEMANTICA_P12A.md) y [pruebas](../tests/conformidad/README.md).
El `.l` enumera dos aliases de retorno que las tablas distribuidas no reservan:
`regresar` y `devolver` siguen siendo identificadores. La acción decimal se
actualiza en ambos archivos sin regenerar las tablas, para preservar programas
válidos con esos nombres.
