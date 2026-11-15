# P1.2-B4-B: contratos y decisiones anteriores al parche

Base exacta: `82be31bf11b6fcfc3bb19979c2b62ca337edc2fd` (PR #11).
Rama nueva: `investigacion/p12b4b-entrada-modulos`; destino de la propuesta:
`investigacion/p12b4a-integridad-archivos`. Esta entrega sólo incorpora pruebas,
este documento y su registro en CMake/CI. No modifica producción.

## Inventario observado

| Interfaz | Firma y resultado | Contrato heredado / recursos / errores |
|---|---|---|
| `leer` | `leer()` → número, cadena o nulo | Linux/macOS usan Readline; Windows usa buffer de 512 bytes y consola/fgets. Infiere número mediante `strtod` del locale activo sólo si consume todo y la entrada no está vacía. EOF o lectura sin resultado → nulo. Buffer propio liberado después de publicar: un salto por error omite ese `free`. |
| `anumero` | `anumero(valor)` → número o nulo | Conversión histórica tolerante; cero y fracaso no se distinguen. Se conserva; B3 ya ofrece la alternativa estricta. |
| `anumero_estricto` | `anumero_estricto(número o cadena)` → número finito | B3: decimal ASCII con punto, exponente opcional y espacios ASCII exteriores; rechaza texto residual, hexadecimal, NaN/infinito y pérdida de una mantisa no nula a cero. Acepta cero negativo y subnormales representables. Error de tipo/sintaxis/rango. |
| `latC_strtod_c` (C público) | `(const char *, char **)` → double | Intenta usar C cambiando temporalmente el locale **global** y copiando su nombre a 64 bytes. No es un contrato seguro entre hilos; un locale por hilo también puede prevalecer. Se mantendrá firma/ABI, corrigiendo el mecanismo. |
| `incluir` | `incluir(cadena)`; sin retorno utilizable | Ejecuta fuente cada vez, sin caché. Busca como se especifica abajo; si no hay sufijo fuente ni fuente encontrada, conserva el mecanismo nativo existente. Errores de nombre, resolución, análisis, compilación y ejecución se propagan. |
| `incluir_fuente` (privada) | candidato → encontrado/no encontrado | Primero comprueba apertura, después usa `latA_analizar_arch`. La ruta internada vive con la VM/bytecode; el AST y la función temporal pertenecen al importador. Compilación fallida salta su limpieza. Línea/columna del llamador no se restauran. |
| `latA_analizar_arch` (C público) | ruta, estado → AST o NULL | Se reutiliza la lectura comprobada de B4-A (lectura/cierre completos, NUL inválido). El importador destruye el AST. No se cambia gramática ni parser. |
| `latC_llamar_funcion` / guard privado de B4-A | llamada protegida → estado | Recupera pila/contextos/contador de ejecución. La protección de temporales se reutilizará en una cabecera **privada**, sin añadir símbolos ni cambiar estructuras públicas. |

Los restantes nombres de la biblioteca base (poner/escribir/imprimir, acadena,
alogico, tipo, imprimirf, error, limpiar, beep) se conservan sin ampliar alcance.
No se probarán comandos del sistema ni bibliotecas nativas desconocidas.

## Entrada: compatibilidad deliberada y correcciones

En el ejecutable, Core establece `LC_NUMERIC=C`; cambiar `LC_ALL` en el entorno
no cambia esa decisión. Un consumidor C puede establecer otro locale antes de
llamar a `leer`. La batería comprueba ambos casos, con C, es_ES.UTF-8 y fr_FR.UTF-8.
No se promete que `leer` convierta idénticamente una coma decimal en esos locales:
es un comportamiento histórico deliberado que se conserva.

| Texto sin terminador | `leer`, C | `leer`, es/fr desde C | `leer_texto` propuesto | `leer_numero` propuesto |
|---|---|---|---|---|
| vacío | cadena vacía | igual | cadena vacía | error |
| `hola`, `María` | cadena | igual | texto intacto | error |
| `0`, `-0`, `0012` | 0, −0, 12 | igual | texto intacto | 0, −0, 12 |
| `1.5` | 1.5 | cadena | texto intacto | 1.5 |
| `1,5` | cadena | 1.5 | texto intacto | error |
| `1e3` | 1000 | igual | texto intacto | 1000 |
| ` 12` | 12 | igual | espacio conservado | 12 |
| `12 ` | cadena | igual | espacio conservado | 12 |
| `0x10`, `nan`, `-inf` | 16, NaN, −∞ | igual | texto intacto | error |
| `1e9999`, `1e-9999` | +∞, 0 | igual | texto intacto | error |
| EOF sin bytes | nulo | igual | nulo | nulo |

Alternativas modernas: `leer_texto()` devuelve cadena sin inferencia;
`leer_numero()` reutiliza exactamente el contrato de `anumero_estricto`, sin
alterar la configuración regional global. No se añaden palabras reservadas.
Ambas devuelven nulo en EOF, pero **lanzan error** si falla la operación: permiten
distinguir valor válido, EOF y error desde la API C de ejecución protegida.
No se introduce una nueva sintaxis de excepciones.

Para entrada redirigida se propone un único lector completo: LF termina una
línea, CR inmediatamente anterior se elimina, EOF con bytes entrega la última
línea. No añade eco ni interpreta controles de edición de terminal. La base
Readline añade eco a pipes/archivos y desdobla CRLF en dos lecturas; se consideran
correcciones de transporte, no cambios de inferencia. `leer` interactivo conserva
Readline en Unix. Windows no se presenta como plataforma validada en esta misión.

Las nuevas funciones limitan cada línea a **8 MiB de bytes de contenido**. `leer`
no recibe ese límite nuevo (históricamente no lo tiene en Unix). El crecimiento
comprueba tamaños y reservas. NUL incrustado es inválido: Core almacena cadenas
terminadas en NUL y no debe presentar un prefijo como línea completa. Se descarta
la línea inválida/que excede el límite antes de continuar cuando el flujo permite
seguir leyendo. Se reintenta EINTR; otros errores de lectura descartan lo parcial.
`leer` mantiene nulo ante fallos operativos; las alternativas informan error.
Tras E/S irrecuperable no se promete recuperar bytes perdidos por el proveedor.

Los buffers pertenecen a la llamada, no a la VM; deben liberarse tanto al retornar
como al fallar la creación/publicación/conversión del objeto. EOF y errores no
permiten perder ni dejar argumentos en la pila.

## Resolución de módulos que se conservará

Sin sufijo explícito, agotar **todas** las ubicaciones `.lat` antes de `.mict`:

1. `nombre.lat` relativo al directorio de trabajo.
2. `nombre.lat` relativo al directorio del archivo que importa.
3. `LATINO_LIB/nombre.lat`, si la variable no está vacía.
4. Los mismos tres lugares para `nombre.mict`.
5. Sólo entonces, mecanismo nativo histórico para un nombre sin sufijo fuente.

Con `.lat` o `.mict` explícito se busca exactamente ese nombre en los tres lugares.
Una ruta absoluta no se prefija con otros directorios. Límite heredado:
`MAX_PATH_LENGTH=1024`, incluyendo NUL; se comprueban nombres y concatenaciones.
`LATINO_LIB` es un único directorio, no una lista. Su validación de longitud es
previa incluso si hay un módulo en el directorio actual; se conserva.

No se añaden caché, orden de importación nuevo ni aislamiento de variables.
A→B→C se ejecuta en ese orden de llamadas y retorna C→B→A. Importar de nuevo
repite los efectos. Un error no revierte los efectos previos del módulo. Un ciclo
sigue siendo recursión: la prueba reduce el presupuesto de llamadas disponible
para alcanzar el límite existente con pocos niveles. No acredita una detección
anticipada de ciclos ni seguridad de una pila C arbitrariamente profunda.

Se protegerá la compilación/ejecución para destruir AST y función temporal y
restaurar archivo activo, modo REPL y ubicación del llamador antes de propagar el
mismo estado. Las rutas de funciones retenidas deben sobrevivir al AST; la prueba
hace una llamada tras recolección en la misma VM. No se cambia la política global
de propiedad ni se pretende resolver todas las reservas internas del compilador.

## Oráculos e infraestructura

Las expectativas son tablas explícitas, tipos, bytes, signos de cero, estado de
pila/contexto y contadores de reservas; no se generan copiando salidas de Core.
Las funciones modernas ausentes son expectativas nuevas, no defectos heredados.
Los programas `.lat` y `.mict` tienen el mismo contenido y oráculo independiente.
Las fixtures, HOME, INPUTRC y LATINO_LIB pertenecen al directorio temporal de cada
prueba. El caso de fallback nativo intercepta la interfaz: **no carga una biblioteca**.

Se reutilizan `tests/archivos/ejecutar.py` y su sonda de enlace: limpian variables
de carga, eligen la biblioteca del build, comprueban su ruta y registran SHA-256.
La prueba de locale cuenta cambios globales antes de permitir hilos; la base
falla esa precondición sin ejecutar una carrera de datos deliberada.

La inyección de lectura utiliza un FILE real para Readline y un lector controlado.
Un prototipo del arnés con fopencookie sin descriptor produjo un aborto de select;
se corrigió el arnés y se repitió la batería. Ese aborto **no es evidencia de un
defecto de Core**. Las reservas observadas por el arnés se cuentan antes de su
limpieza auxiliar. La inyección de compilación falla en la entrada del compilador;
no simula todos los puntos internos posibles.

## Nota de implementación (posterior a la entrega A)

El primer commit de pruebas es `1ffcf2f8d7dbcfd246a3d374c1ce737791238d74`.
La CI inicial [43664434426](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43664434426)
registró 198 fallos en los tres perfiles (669 normales, 685 instrumentadas).
El job instrumentado detectó además 2 bytes residuales en una reserva del AST
al inyectar fallo al entrar al compilador; se conserva su log comprimido.

Se corrigieron dos detalles de instrumentación sin relajar el contrato:
la publicación ahora usa `latC_apilar_string`/`latC_apilar_double`, por lo que
se observa también esa frontera; la prueba de vida de rutas activa y autoriza
la recolección **antes** de ejecutar, como exige `latmemoria.h`, y barre cadenas.
La hipótesis de habilitarla tras una nativa no autorizada era inválida. No se
cambia el recolector para hacer pasar el arnés. Las pruebas de buffers pendientes
siguen exigiendo cero y las 391 regresiones anteriores se conservan.
