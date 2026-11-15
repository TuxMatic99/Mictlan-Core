# Inventario de entrada, conversión e importación — P1.2-B4-B

Especificación inicial y alternativas: [P12B4B_DECISIONES.md](P12B4B_DECISIONES.md).
Referencia ejecutable: [ENTRADA_MODULOS.md](ENTRADA_MODULOS.md).
Este inventario describe el resultado del parche, no sólo las intenciones.

| Nombre / firma | Tipos, resultado, errores | Locale y plataforma | Propiedad y efectos |
|---|---|---|---|
| `leer()` | Sin argumentos; número/cadena/nulo. Sólo infiere si strtod consume todo. Vacío es cadena; EOF/fallo operativo es nulo. Error al publicar en la VM se propaga. | Locale activo del anfitrión; CLI fija C. Unix interactivo conserva Readline; entrada redirigida usa lector completo. Windows usa el lector CRT común; no validado aquí. | Buffer local protegido, liberado antes de relanzar; consume una línea, elimina LF y su CR precedente. No añade eco fuera del terminal. |
| `leer_texto()` (nueva) | Sin argumentos; cadena o nulo en EOF. Error de E/S, NUL, reserva o línea >8 MiB. No valida UTF-8. | Conserva bytes, sin inferencia ni locale numérico. | Mismo lector y propiedad; descarta línea inválida si puede continuar. O(n) tiempo y memoria. |
| `leer_numero()` (nueva) | Sin argumentos; double finito o nulo en EOF. Mismos errores operativos y errores de conversión estricta. | Punto decimal fijo; no cambia locale global ni por hilo. | Usa el reconocedor de B3; libera la línea incluso si falla la conversión. O(n). |
| `anumero(valor)` | Firma heredada; cero y conversión fallida → nulo. | No cambia su política histórica. | Sin cambios; alternativa estricta disponible desde B3. No modifica el argumento. |
| `anumero_estricto(número o cadena)` | Número finito; signo/punto/exponente y espacios ASCII exteriores. Rechaza hexadecimal, coma, NaN, infinitos, sintaxis residual y pérdida de mantisa no nula a cero. Cero negativo y subnormal representable válidos. | Independiente del locale. | El reconocedor privado se extrae sin duplicar sus reglas. O(n) texto, O(1) número. |
| `incluir(cadena)` | Sin retorno utilizable. Tipo inválido, ruta larga, ausencia explícita, módulo vacío, error sintáctico/compilación/ejecución → error. | PATH_SEP y LATINO_LIB históricos. `.lat` en todas las ubicaciones antes de `.mict`. | Ejecuta cada vez; comparte la VM y conserva sus ámbitos. Efectos anteriores al error no se revierten. Libera AST/función temporal y restaura archivo/REPL/línea/columna. |
| `latC_strtod_c(texto, fin)` (C existente) | double y puntero final opcional. Conserva errno de strtod_l; falta de locale → 0, fin=inicio y ENOMEM. No es conversor estricto por sí mismo. | Locale C propio por llamada; POSIX newlocale/strtod_l/freelocale, equivalente CRT Windows. Verificado con locales por hilo en Linux. | No mutación global, sin estado compartido nuevo; se libera el objeto locale. No implica que toda la VM o el parser sean concurrentes. |
| `latA_analizar_arch` (C existente) | Ruta + estado → AST; lectura/cierre/NUL verificados desde B4-A. | Cargador común de .lat y .mict. | Se reutiliza sin modificar; el importador destruye el AST. Se conserva el nombre fuente que necesita el bytecode. |
| `latC_llamar_funcion` / `latE_proteger` | Estado de ejecución / protección privada de temporales. | Mismo setjmp/longjmp existente. | Se reutiliza la recuperación de pila, contexto y contador. Guard y datos viven en el llamador, fuera del marco de setjmp. Sin nueva estructura pública. |

## Cambios de producción y necesidad

* `src/latbaselib.c`: lector completo compartido, dos funciones explícitas,
  reconocedor estricto compartido y protección/restauración de importaciones.
* `src/latlocale.c`: sustituye cambio global temporal por un locale de conversión
  propio. La prueba anterior observa dos cambios globales; la posterior ninguno.
* `src/laterror.h`: recibe el guard ya probado de B4-A como static inline privado.
* `src/latfilelib.c`: utiliza ese mismo guard; no cambia sus contratos de archivos.

No cambian `include/`, gramática, precedencia, ámbitos, representación de números,
registro de propiedad, carga nativa ni las 391/407 pruebas anteriores.
Los dos nuevos nombres sólo se registran en la biblioteca base del lenguaje:
no añaden símbolos C públicos. Se mantienen exactamente los 285 de referencia.

## Recuperación y límites verificables

La batería conserva un valor centinela en la pila y exige su integridad, contexto
cero, contador de llamadas restaurado, manejador de error retirado y una llamada
válida posterior en la **misma VM**. Cubre entrada inválida, E/S, EINTR, fallos de
reserva/crecimiento/publicación y fallos de módulo. Las rutas de funciones retenidas
se usan tras destruir el AST y barrer cadenas, con autorización explícita del GC.

La función de importación no mantiene un estado global nuevo. Su ruta internada
no se libera como buffer independiente: pertenece a Core. No se añade caché.
Los ciclos se prueban con un presupuesto de llamadas reducido para evitar una
profundidad C excesiva; la detección anticipada de ciclos sigue siendo trabajo futuro.

La inyección del compilador cubre su frontera de entrada, no cada reserva interna.
La falta de memoria al crear un locale tiene el contrato C indicado; los llamadores
históricos que no consulten fin/errno no ganan automáticamente una API de error.
La concurrencia probada corresponde al ayudante numérico, no a ejecutar una VM
simultáneamente desde varios hilos. No se promete atomicidad respecto a otros
lectores concurrentes de stdin, rollback de efectos de módulos, ni recuperación
completa del proveedor después de un error de E/S persistente.
