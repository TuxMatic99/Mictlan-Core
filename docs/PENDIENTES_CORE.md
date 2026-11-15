# Pendientes maestros de Core

Base inicial del mapa A0: `2651317e4224f5499765b5b0ae16d1404a0adfc4`.
Este registro se actualiza por fases; los identificadores no se reutilizan.
Una prueba aprobada sólo acredita su caso y revisión, no ausencia de defectos.

Categorías: **D** defecto demostrado por reproducción; **L** limitación documentada;
**C** comportamiento compatible deliberado; **P** propuesta de mejora/auditoría,
incluidos indicios estáticos sin reproducción. Prioridades P0 antes de distribución,
P1 siguiente auditoría, P2 planificación posterior. Gravedad es impacto funcional,
no una afirmación de explotabilidad. Estado inicial: D reproducido sin corregir;
las demás filas no se reclasifican como bugs por una simple sospecha.

## Defectos reproducidos en sis (A0)

Todos los archivos de esta tabla son `src/latsyslib.c`. Reproducción:
`ctest --test-dir BUILD -R 'mictlan_a0_<patrón>' --output-on-failure`, con la batería
publicada antes de producción. Ver [contratos iniciales](A0_SIS_DECISIONES.md).

| ID | Prioridad / gravedad | Función | Reproducción / observado antes | Acción recomendada / estado |
|---|---|---|---|---|
| A0-D01 | P0 / alta | latSO_tiempo | tiempo_nan, inf, limite_alto, limite_bajo: convierte double inválido a time_t | Validar finitud/rango/truncamiento antes del cast; reproducido |
| A0-D02 | P0 / alta | latSO_fecha, latSO_tiempo | fecha_local/utc abortan; tiempo_local publica calendario pese al fallo | Comprobar time/localtime/gmtime y conservar diagnóstico; reproducido |
| A0-D03 | P0 / alta | latSO_pipe | pipe_reserva deja flujo abierto; pipe_publicar deja buffer pendiente | Reservar antes de abrir, proteger publicación y liberar antes de propagar; reproducido |
| A0-D04 | P0 / alta | latSO_pipe | pipe_exceso entrega prefijo; pipe_nul trunca sin advertencia | Rechazar pérdida silenciosa, consumir salida finita restante, comprobar disco; reproducido |
| A0-D05 | P1 / media | latSO_pipe | pipe_eintr falla en interrupción recuperable | Reintentar lectura interrumpida conservando bytes y único cierre; reproducido |
| A0-D06 | P0 / alta | latSO_dormir, latSO_avisar, latSO_iraxy | dormir/avisar/iraxy_nan/inf/grande: cast antes de validación | Reutilizar latN_entero donde corresponda; reproducido |
| A0-D07 | P1 / media | sleep_ms, latSO_dormir | dormir_eintr omite resto; dormir_error ignora fallo | Reintentar el resto y propagar fallo operativo; reproducido |
| A0-D08 | P1 / media | latSO_fecha | fecha_formato_sem/iso ignoran retorno cero | No publicar buffer sin resultado; reproducido |
| A0-D09 | P1 / media | buscar_os_version | version_sufijo interpreta 6.8-12 como parche12; negativa/desborde producen valores incorrectos | Inicialización y análisis decimal acotado de segmentos; reproducido |
| A0-D10 | P1 / media | latSO_avisar, latSO_ejecutar | avisar_fallo y aux_ejecutar_error ignoran error operativo | Comprobar SIG_ERR/system==-1 conservando estado no cero del hijo; reproducido |

## Cola priorizada para fases posteriores

| ID / categoría | Prioridad / gravedad potencial | Archivo / función o subsistema | Evidencia o reproducción pendiente | Acción y criterio para cerrar |
|---|---|---|---|---|
| CORE-P01 / P | P0 / alta | src/latdo.c: ast_analizar, latC_analizar; src/latast.c | B4-A/B inyectan en la frontera, no en cada reserva interna | Matriz OOM dentro de compilación/parser, fuga cero y misma VM recuperable |
| CORE-P02 / P | P1 / alta | src/latclass.c: latC_llamar_metodo/latC_llamar_super; src/latmv.c | TODO de mi/nargs, estado de llamada protegida no consumido; no reproducido en A0 | Casos de constructor, herencia y error→éxito antes de prometer clases completas |
| CORE-P03 / P | P1 / alta | src/latdo.c: saltos ROMPER/CONTINUAR; tools/parser/latparse.y | TODO de recursión; auditoría de anidamiento pendiente | Oráculos independientes para bucles anidados, selección y retorno; no cambiar precedencia |
| CORE-P04 / P | P1 / alta | src/latmv.c: pila/recursión/cachés; src/latbaselib.c: incluir | Ciclos de importación sólo acotados cerca del límite | Límites efectivos con pila C controlada; error recuperable sin desbordamiento |
| CORE-P05 / P | P1 / alta | src/latpaqlib.c: latC_cargarlib/lat_sym | No se observa cierre del handle tras símbolo/entrada fallidos; revisión estática | Stubs de carga/descarga y fixture propia; definir vida útil sin cargar bibliotecas desconocidas |
| CORE-L01 / L | P0 / media | tools/parser/generar_parser.sh; src/latlex/latparse | Script sin salida temprana, rutas relativas y versiones no fijadas | Regeneración en copia aislada, comparar generados, fijar versiones/procedencia |
| CORE-L02 / L | P0 / media | src/lib/Debug, Release; src/latino-regex | DLL/Lib versionadas y gitlink 0583e5e…; no ejecutados por A0 | Inventario de procedencia/licencias y reconstrucción antes de distribuir esos binarios |
| CORE-P06 / P | P1 / alta | src/latmem.c, src/latobj.c, src/latdic.c | GC/ciclos ya tienen suites; no cubren todas las raíces/OOM | Auditorías por clase de objeto y redimensionamiento, sin rehacer propiedad global |
| CORE-L03 / L | P1 / media | src/latsyslib.c: buscar_os_version macOS/Cygwin | macOS fallthrough y posible fgets NULL; Cygwin TODO; no prueba de plataforma | Matriz nativa o stubs específicos; no declarar reparación por pasar Linux |
| CORE-L04 / L | P1 / media | src/latrepl.c; src/linenoise/*; src/latbaselib.c | Edición de terminal y señales no certificadas por pipes | Pruebas PTY propias, EOF/error/historial/UTF-8, restauración de terminal |
| CORE-P07 / P | P1 / media | src/latobj.c: latC_aint/achar/adouble | Conversiones heredadas asimétricas; B3 cubre varios límites | Tabla por tipo y rango; separar incoherencia compatible de cast indefinido |
| CORE-L05 / L | P2 / media | src/latstrlib.c; src/latstr_utf8.inc | No hay segmentación de grafemas ni comparación lingüística | UAX #29 y dependencias ligeras en propuesta explícita; no cambiar longitud en bytes |
| CORE-L06 / L | P2 / media | src/latuilib.c, src/latcurseslib.c, src/latgc.c | UI/stubs y bibliotecas no registradas en build normal | Documentar disponibilidad real; excluir promesas de distribución; auditoría propia |
| CORE-P08 / P | P1 / media | src/CMakeLists.txt, include/*, src/latino.def | ABI Linux285 verificada; otras plataformas y paquetes no certificados | Pruebas de instalación limpia y consumidores C por plataforma |
| CORE-P09 / P | P2 / baja | src/man/latino.1; src/gnu-utilities/*; src/latdevlib.c | Ayuda/editor y diagnósticos heredados pueden quedar desfasados | Sincronizar documentación y definir estabilidad de APIs de depuración |
| CORE-P10 / P | P1 / media | Todas las bibliotecas | Índice de 222 registros no equivale a 222 contratos revisados individualmente | Auditar cada biblioteca por casos y mantener sus tablas y ejemplos ejecutables |

## Contratos deliberados y propuestas de API

| ID / categoría | Área | Contrato conservado / propuesta | Estado |
|---|---|---|---|
| CORE-C01 / C | P1.2-A | Precedencias históricas, ámbitos y funciones; no modernizar silenciosamente | Conservar suite, propuestas separadas |
| CORE-C02 / C | B1/B2 | Bytes y delimitadores heredados; listas comparten objetos y dic no promete orden | Documentación existente y regresiones |
| CORE-C03 / C | B3/B4-B | anumero cero→nulo, leer dependiente de locale anfitrión; alternativas explícitas | Compatible; no tratar como defecto automáticamente |
| CORE-C04 / C | incluir | .lat en todas las rutas antes de .mict, sin caché ni rollback | Conservar; rediseño futuro separado |
| SIS-C01 / C | tiempo | Formato nulo ignora instante; strftime vacío/no cabe→nulo (64 bytes) | Conservar |
| SIS-C02 / C | pipe/ejecutar | No exponen estado del hijo, aunque sea no cero | Conservar; propuesta resultado estructurado en otra fase |
| SIS-C03 / C | avisar | Señal observada global y persistente entre llamadas/VM | Conservar; propuesta estado por VM exige diseño separado |
| SIS-P01 / P | pipe | Límite y drenaje no son timeout ni control de productor infinito | API de proceso acotado futura; no ampliar A0 |
| SIS-P02 / P | fecha/tiempo | Funciones con zona explícita y formato ampliable | Alternativas explícitas futuras; no cambiar 64 bytes en la heredada |

## Criterio para una distribución experimental

No basta que A0 pase. Antes de declarar una distribución hay que revisar P0,
definir plataformas soportadas, preservar resultados reproducibles, publicar límites
y un canal de reportes. Cada cierre debe enlazar caso anterior, causa, commit,
resultado posterior y alcance de instrumentación. Un indicio pasa a D sólo después
de reproducirse. Una nueva fase actualiza estado/evidencia sin borrar la fila ni
las limitaciones históricas. A0 no declara terminado todo el lenguaje.
