# Inventario, propiedad y compatibilidad de archivos

Base auditada: `b0a2d860363e07274feae1d0cf2b421515386d73`.
Implementación: `src/latfilelib.c`; interfaz de lectura de fuente directamente
utilizada: `latA_analizar_arch` en `src/latast.c`. Las decisiones anteriores
al código están en [P12B4A_DECISIONES.md](P12B4A_DECISIONES.md).
La [referencia educativa](ARCHIVOS.md) contiene un ejemplo automatizado por nombre.

En la tabla, `ruta`, `texto`, `origen` y `destino` son **cadenas obligatorias**;
no se convierten números implícitamente. Aridad fija. `n` es la cantidad de bytes,
`p` el tamaño de la ruta. Los costes no incluyen latencia de disco/fsync ni el
coste variable de resolución de rutas del sistema. Ninguna función devuelve
un descriptor abierto al lenguaje.

| Nombre/firma | Resultado | Vacío, límites y errores | Mutación y coste | Compatibilidad verificada |
|---|---|---|---|---|
| `leer(ruta)` | cadena / falso | Vacío `""`; ausente o fallo → falso; 8 MiB, sin NUL | Sólo lee; O(n) tiempo/memoria | Contrato conservado; reintenta EINTR, libera temporal ante longjmp |
| `intentar_leer(ruta)` | igual a leer | Misma función C y errores | Igual a leer | Alias exacto conservado |
| `leer_texto_seguro(ruta)` | cadena / nulo / falso | ENOENT → nulo; resto falso; vacío cadena; 8 MiB, sin NUL | Sólo lee; O(n) tiempo/memoria | Estados existentes conservados; publicación protegida |
| `lineas(ruta)` | lista de cadenas; error operativo | Vacío []; LF divide, CR permanece; LF final no añade línea; 8 MiB, sin NUL | Lista nueva administrada; O(n) tiempo/memoria | Contrato conservado; limpieza ante error de creación de lista |
| `escribir(ruta,texto)` | sin retorno utilizable; error operativo | Vacío trunca; cierre comprobado; sin límite propio de 8 MiB | Escritura directa con reemplazo; O(n)/O(1) auxiliar | Puede dejar parcial al fallar; completa escritura parcial/EINTR |
| `anexar(ruta,texto)` | sin retorno utilizable; error operativo | Vacío puede crear; crea si ausente; cierre comprobado | Añade directamente; O(n)/O(1) auxiliar | No es transacción; contrato conservado |
| `intentar_escribir(ruta,texto)` | booleano | Falso ante fallo operativo, error ante tipo; vacío trunca | Escritura directa; O(n)/O(1) auxiliar | Falso no garantiza bytes anteriores; se conserva |
| `escribir_atomico(ruta,texto)` | -1, 0, 1, 2 | Vacío válido; 8 MiB; destino regular/ausente; -1 no Linux | Un motor temporal+rename+sync; O(n)/O(p), O(n) disco | Estados, permisos 0600 y restricciones anteriores conservados |
| `duplicar(origen,destino)` | sin retorno utilizable; error antes de reemplazo, advertencia después si durabilidad incierta | Vacío/binario válido; sin cap 8 MiB; rechaza mismo inode, origen no regular, destino no regular/simbólico | Copia en bloque 16 KiB y mismo motor atómico; O(n)/O(p)+16 KiB | Conserva reemplazo regular; corrige pérdida de datos; cambios explícitos debajo |
| `ejecutar(ruta)` | sin retorno utilizable; propaga error | .lat/.mict; vacío error; otro sufijo no-op; fuente sin NUL hasta INT_MAX−2 | VM actual; O(n)+compilación/ejecución, AST/bytecode administrado | .mict añadido; restaura archivo/REPL/ubicación; no oculta fallo |
| `crear(ruta)` | booleano | Exclusivo: no trunca existente; vacío recién creado; cierre fallido puede dejarlo | fopen wbx; O(1) auxiliar | Sin ampliación de contrato |
| `eliminar(ruta)` | booleano | Ausente → falso; remove permite directorio vacío POSIX | Elimina entrada; O(1) auxiliar | Sin ampliación; no recursivo |
| `borrar(ruta)` | igual a eliminar | Misma función C | Igual a eliminar | Alias exacto conservado |
| `renombrar(origen,destino)` | nombre destino / falso | rename puede reemplazar existente; puede fallar entre sistemas de archivos | Mueve entrada; O(p) resultado | Sin ampliar; sin garantía de fsync o rollback tras publicar resultado |

Ruta vacía: las operaciones conservan el error/falso operativo correspondiente;
`ejecutar("")` pertenece al no-op de sufijo no reconocido. Las rutas relativas
son relativas al directorio de trabajo. Lecturas/escrituras directas siguen
los enlaces que sigue fopen. `duplicar` puede leer un origen simbólico que
resuelve a archivo regular, pero no sigue un enlace simbólico final de destino.
No hay conversión de UTF-8, normalización ni segmentación Unicode.

## Interfaces C y propiedad

| Interfaz | Contrato | Propietario / ABI |
|---|---|---|
| `char *latF_leer_texto_seguro(const char *, int *)` | Estado -1 error, 0 ENOENT, 1 completo; admite estado NULL; ruta NULL/vacía devuelve NULL | Resultado malloc, el llamador libera con free; firma intacta |
| `int latF_escribir_atomico(const char *, const char *)` | Mismos -1/0/1/2; ruta/texto NULL → 0 en Linux | No transfiere cadenas; firma intacta |
| `ast *latA_analizar_arch(char *, int *)` | Cargador .lat/.mict; fuente completa/posicionable/sin NUL, estado 0 correcto | AST del llamador; comprobar fread completo y fclose antes de analizar; gramática intacta |

Los buffers de lectura malloc/realloc se liberan antes de propagar un error de
publicación. La lista parcial y los objetos creados son del registro de la VM;
no se destruyen referencias compartidas para resolver una fuga de buffer.
El AST de `ejecutar` se destruye tanto tras éxito como tras error de la interfaz
de compilación; la función compilada se libera por la política existente.
Un guard privado de longjmp protege sólo estos temporales: su estado y datos
viven en el marco del llamador, fuera del marco de setjmp. La recuperación de
pila/contextos sigue usando `latC_llamar_funcion`, conservando el prefijo de pila.
No se modifica la estructura pública de la VM ni se añade otro recolector.

## Cambios observables y contratos que no se modernizan

* Copiar un origen ausente ya no crea/trunca el destino; aperturas fallidas no
  dejan el otro flujo abierto. Errores de lectura, escritura y cierre impiden
  confirmar el reemplazo. Interrupciones EINTR se reintentan sin repetir bytes.
* Una copia correcta reemplaza el destino regular mediante rename: **inode
  nuevo, permisos 0600, metadatos no preservados**. Los otros enlaces duros al
  destino viejo no cambian. Se rechazan fuente y destino con el mismo inode.
  La fuente no regular, incluida una FIFO sintética sin escritor, se rechaza
  antes de una apertura potencialmente bloqueante; fstat verifica de nuevo el
  descriptor. La garantía presupone rutas estables, sin cambios concurrentes.
* El motor atómico continúa siendo Linux. Fuera de Linux, la nueva copia
  protegida falla antes de mutar; no se afirma compatibilidad probada allí.
  Una adaptación portable es pendiente explícito, no un fallback destructivo.
* Si el rename ya ocurrió y falla sync/cierre del directorio, `duplicar` advierte
  pero no afirma que el destino anterior esté intacto. `escribir_atomico`
  conserva el estado 2. El temporal puede persistir si el sistema impide unlink;
  no se garantiza limpieza frente a pérdida de permisos o caída del proceso.
* `ejecutar` deja de omitir .mict y de ocultar errores de ejecución de .lat;
  restaura el archivo activo/REPL/ubicación, incluidos casos anidados.
  El reconocimiento por sufijo coincide con el cargador: el nombre literal
  `.lat`, antes omitido por no tener prefijo, también se reconoce; `.mict` sigue
  el mismo criterio. Sus efectos
  ya realizados sobre archivos o variables no se revierten.
* **Sin retorno no significa nulo:** las cuatro nativas sin valor no apilan
  resultado. Anidarlas en `poner(...)` falla por operandos insuficientes. Las
  primeras versiones de ejemplos asumieron nulo; la batería rechazó esa hipótesis
  (registro `ejemplos-hipotesis-retorno.log`). Se corrigió la documentación, no la
  semántica global. Ocho pruebas negativas conservan este contrato en .lat/.mict.
* Escribir/anexar directamente, renombrar con reemplazo POSIX, crear y eliminar
  conservan sus efectos heredados. No se añaden funciones ni alias.

## Qué cubren y qué no cubren las inyecciones

El arnés incluye el código real de la biblioteca y del cargador, sustituyendo
operaciones de E/S/reserva por fallos deterministas. Comprueba que cada inyección
se activa, cuenta recursos antes de limpiarlos y verifica el contenido posterior.
La prueba de compilación falla **en la interfaz de entrada al compilador**:
demuestra limpieza del AST que pertenece a archivos, no un barrido de todas las
reservas internas del compilador/parser. Agotamiento global de memoria, fallos
internos arbitrarios de esos subsistemas, recuperación tras corte de energía y
concurrencia adversa de rutas requieren trabajo independiente. No se cambian
ámbitos, precedencia, gramática ni la política global de retornos para ocultarlos.
