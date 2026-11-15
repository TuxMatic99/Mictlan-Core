# Inventario reproducible de Core — A0

Censo: **92 archivos**, **26 subsistemas**, **222 registros nativos** (incluidos opcionales), **545 candidatos de definición C**.

Se incluyen todos los archivos versionados de src/, include/ y tools/parser/, también binarios, generados y componentes no activados. Un hash o una prueba aprobada no acredita corrección. El índice de definiciones es léxico: puede incluir alternativas de preprocesador y no reemplaza un análisis C por plataforma. Los contratos se documentan por subsistema; sis recibe además auditoría por función.

Regenerar: `python3 tools/auditoria/inventario.py`; verificar sin escribir: `python3 tools/auditoria/inventario.py --comprobar`. Toda ruta debe pertenecer exactamente a un subsistema; una nueva ruta sin clasificar falla. El censo usa los bytes del checkout y omite marcas de tiempo para que el resultado sea reproducible.

[Archivos y dependencias directas](archivos.csv) · [API nativa y aridades](nativas.csv) · [Índice de definiciones](definiciones_indice.csv) · [Pendientes priorizados](../PENDIENTES_CORE.md).

## LEX — Tokens, palabras reservadas y literales

Archivos: `include/latlex.h`, `src/latlex.c`, `src/latlex.h`, `tools/parser/latlex.l`.

**Dependencias:** Flex, latlocale, objetos/AST.

**Contrato y herencia:** Reconocer léxico Latino/Mictlán; conversiones de literales mediante ayudante C.

**Errores relevantes:** Entradas inválidas, EOF en literal, reserva del scanner.

**Pruebas existentes o relacionadas:** tests/conformidad/; tests/matematicas/; tests/alpha/.

**Pendiente / límite de la auditoría:** Límites adversos de tokens, NUL/UTF-8 y agotamiento dentro del scanner pendientes.

## PAR — Gramática y precedencias; AST

Archivos: `include/latparse.h`, `src/latparse.c`, `src/latparse.h`, `tools/parser/latparse.y`.

**Dependencias:** Bison, lexer, latast.

**Contrato y herencia:** Semántica P1.2-A; ^ y ** mantienen prioridades históricas distintas.

**Errores relevantes:** Sintaxis incompleta, pila de parser, limpieza de producciones abortadas.

**Pruebas existentes o relacionadas:** tests/conformidad/; tests/alpha/; tests/entrada_modulos/.

**Pendiente / límite de la auditoría:** Correspondencia gramática/generados y liberación ante cada error no exhaustivamente comprobadas.

## GEN — Regeneración de parser y lexer

Archivos: `tools/parser/generar_parser.sh`.

**Dependencias:** Flex/Bison, rutas relativas, shell.

**Contrato y herencia:** Script heredado mueve fuentes y cabeceras generadas.

**Errores relevantes:** No set -e; comodines; versiones no fijadas.

**Pruebas existentes o relacionadas:** Sin prueba de reproducción exacta de generados en CTest.

**Pendiente / límite de la auditoría:** No ejecutar en el checkout de trabajo sin aislamiento; reproducibilidad pendiente.

## AST — Construcción, carga y destrucción del AST

Archivos: `include/latast.h`, `src/latast.c`.

**Dependencias:** malloc, archivos, scanner/parser.

**Contrato y herencia:** B4-A comprueba lectura/cierre; quien recibe AST lo destruye.

**Errores relevantes:** E/S, NUL, reserva, archivo vacío y error sintáctico.

**Pruebas existentes o relacionadas:** tests/archivos/; tests/entrada_modulos/.

**Pendiente / límite de la auditoría:** Fallo de reserva en todas las producciones y profundidad máxima pendientes.

## COMP — Compilación, bytecode y llamadas protegidas

Archivos: `include/latdo.h`, `src/latbytecode.h`, `src/latdo.c`, `src/laterror.h`.

**Dependencias:** AST, VM, propiedad, setjmp/longjmp.

**Contrato y herencia:** Pila/contador/contexto recuperables; temporales se limpian mediante guard privado.

**Errores relevantes:** Tipo, rango, compilación, reserva y saltos de control.

**Pruebas existentes o relacionadas:** tests/conformidad/; tests/stack_recovery_regression.c; tests/p11_pila.c; tests/bytecode*.

**Pendiente / límite de la auditoría:** ROMPER/CONTINUAR anidados, agotamiento dentro del compilador y todas las instrucciones requieren casos nuevos.

## VM — Ejecutor, pila, contextos y registro de bibliotecas

Archivos: `include/latmv.h`, `src/latmv.c`.

**Dependencias:** Objetos, mapas, bytecode, bibliotecas nativas.

**Contrato y herencia:** Los objetos compartidos siguen el registro de Core; las celdas no implican propiedad independiente.

**Errores relevantes:** Aridad, pila, recursión, fallos de nativas.

**Pruebas existentes o relacionadas:** tests/conformidad/; tests/context*; tests/stack_recovery_regression.c; tests/p11_pila.c.

**Pendiente / límite de la auditoría:** Límites de pila profunda, interacción clase/error, cachés de variables y estados auxiliares pendientes.

## OBJ — Tipos, conversiones, texto y comparación

Archivos: `include/latobj.h`, `src/latnumerico.h`, `src/latobj.c`.

**Dependencias:** memoria, listas/mapas, latlocale.

**Contrato y herencia:** Compatibilidad de conversiones no estrictas; alternativas B1/B3 explícitas.

**Errores relevantes:** Tipos inválidos, profundidad/ciclos, rango y reservas.

**Pruebas existentes o relacionadas:** tests/object_text*; tests/matematicas/; tests/colecciones/.

**Pendiente / límite de la auditoría:** Igualdad de estructuras complejas, cast T_INTEGER/T_CHAR y OOM interno necesitan revisión específica.

## MEM — Asignador, propiedad y recolección explícita

Archivos: `include/latmem.h`, `include/latmemoria.h`, `src/latmem.c`, `src/latpropiedad.h`.

**Dependencias:** VM, mapas, objetos, bytecode.

**Contrato y herencia:** P0.2-E/F/G y P1.1: raíces explícitas, nativas autorizadas, no recolectar durante ejecución.

**Errores relevantes:** OOM, referencia opaca, VM ocupada, ciclos compartidos.

**Pruebas existentes o relacionadas:** tests/p02f*; tests/p02g*; tests/p11_gc_cadenas.c; tests/alpha/agotamiento.c.

**Pendiente / límite de la auditoría:** Matriz de agotamiento y raíces para todos los tipos/nativas pendiente; no cambiar política global en A0.

## GC_LEG — Recolector histórico condicionado

Archivos: `include/latgc.h`, `src/latgc.c`.

**Dependencias:** HABILITAR_GC, objetos/listas.

**Contrato y herencia:** Separado del registro y GC explícito actuales.

**Errores relevantes:** Compilaciones alternativas y doble propiedad.

**Pruebas existentes o relacionadas:** Suite normal no certifica cada configuración de HABILITAR_GC.

**Pendiente / límite de la auditoría:** Determinar soporte de cada configuración antes de distribuir.

## LIST — Listas y biblioteca lista

Archivos: `include/latlist.h`, `src/latcolecciones.h`, `src/latlist.c`, `src/latlistlib.c`.

**Dependencias:** Nodos enlazados, registro de objetos.

**Contrato y herencia:** B2: mutación/copia superficial/alias documentados; índices validados.

**Errores relevantes:** Vacías, rango, duplicados, ciclos, OOM.

**Pruebas existentes o relacionadas:** tests/colecciones/; tests/diagnostico*lista*; tests/p02f*.

**Pendiente / límite de la auditoría:** Ciclos de igualdad y límites combinados todavía requieren auditorías adicionales.

## DIC — Hash map y biblioteca dic

Archivos: `include/latdic.h`, `src/latdic.c`, `src/latdiclib.c`.

**Dependencias:** Listas de cubetas, registro, hashing.

**Contrato y herencia:** B2: borrar sólo una entrada y mantener longitud; sin orden de inserción prometido.

**Errores relevantes:** Colisiones, crecimiento, claves largas, valores compartidos.

**Pruebas existentes o relacionadas:** tests/colecciones/; tests/hash*; tests/p02g*.

**Pendiente / límite de la auditoría:** OOM de redimensionamiento y combinaciones grandes de alias pendientes.

## STR — Biblioteca cadena, formato y regex

Archivos: `src/latstr_utf8.inc`, `src/latstrlib.c`.

**Dependencias:** libc/regex, objetos, listas, UTF-8 privado.

**Contrato y herencia:** B1: bytes heredados y alternativas explícitas por puntos de código/ASCII.

**Errores relevantes:** UTF-8 inválido, rango, formatos, reservas.

**Pruebas existentes o relacionadas:** tests/cadenas/; tests/strings_io*; tests/p02g_formato.c.

**Pendiente / límite de la auditoría:** Grafemas, comparación lingüística y Unicode avanzado son propuestas, no soporte implementado.

## MATH — Biblioteca mate

Archivos: `src/latmathlib.c`.

**Dependencias:** libm, double, estado de generador.

**Contrato y herencia:** B3: dominios heredados y alternativas explícitas; redondeo/libm sin precisión decimal exacta.

**Errores relevantes:** NaN, infinito, cast entero, intervalo aleatorio, rango.

**Pruebas existentes o relacionadas:** tests/matematicas/.

**Pendiente / límite de la auditoría:** No criptografía; reproducibilidad/precisión por plataforma delimitadas; no prometer igualdad decimal exacta.

## BASE — Biblioteca base, entrada y módulos

Archivos: `src/latbaselib.c`, `src/latlocale.c`.

**Dependencias:** strtod_l, stdin, parser/compilador, cargador nativo.

**Contrato y herencia:** B4-B: leer compatible; leer_texto/leer_numero; .lat antes de .mict; sin caché.

**Errores relevantes:** E/S, EOF, NUL, límites, errores de módulo y conversión.

**Pruebas existentes o relacionadas:** tests/entrada_modulos/; tests/matematicas/; tests/conformidad/.

**Pendiente / límite de la auditoría:** Ciclos profundos, todos los OOM internos, edición interactiva y plataformas no Linux pendientes.

## FILE — Biblioteca archivo y persistencia

Archivos: `src/latfilelib.c`.

**Dependencias:** FILE, stat, rename/fsync, cargador fuente.

**Contrato y herencia:** B4-A: copia/reemplazo protegido; escritura directa sigue pudiendo quedar parcial.

**Errores relevantes:** Apertura/lectura/escritura/cierre, EINTR, mismo inode, FIFO.

**Pruebas existentes o relacionadas:** tests/archivos/; tests/atomic_io_regression.c.

**Pendiente / límite de la auditoría:** Cortes de energía, escritores concurrentes, temporales no borrables y soporte no Linux delimitados.

## SYS — Biblioteca sis y ayudantes de sistema

Archivos: `src/latsyslib.c`.

**Dependencias:** libc, time_t, señales, tuberías, pausas, uname.

**Contrato y herencia:** Contrato detallado en A0_SIS_DECISIONES.md; prioridad funcional de A0.

**Errores relevantes:** Conversión no finita, time/localtime/strftime, tubería y limpieza.

**Pruebas existentes o relacionadas:** tests/system_regression.c; tests/sistema/ (A0).

**Pendiente / límite de la auditoría:** Resultados anteriores y posteriores separados; no confundir fallos operativos con estado del programa hijo.

## PAQ — Carga nativa explícita y fallback histórico

Archivos: `src/latpaqlib.c`.

**Dependencias:** dlopen/dlsym o LoadLibrary/GetProcAddress.

**Contrato y herencia:** paquete.cargar y latC_cargarlib; no sandbox; GC pasa a opaco.

**Errores relevantes:** Símbolo ausente, carga fallida, error de inicializador.

**Pruebas existentes o relacionadas:** Regresiones FFI previas; B4-B sólo intercepta fallback.

**Pendiente / límite de la auditoría:** Handles tras fallo de símbolo/entrada y liberación de bibliotecas requieren auditoría propia; no cargar software desconocido.

## CLASS — Clases, instancias, métodos y super

Archivos: `include/latclass.h`, `src/latclass.c`.

**Dependencias:** VM, mapas, constructores y contexto mi.

**Contrato y herencia:** Interfaz heredada parcialmente implementada; no declarar clases listas por tener estructuras.

**Errores relevantes:** Método/constructor ausente, instancia inválida, error de ejecución.

**Pruebas existentes o relacionadas:** tests/alpha/; cobertura específica por inventariar.

**Pendiente / límite de la auditoría:** TODO de mi, nargs y llamada protegida ignorada en latC_llamar_metodo; reproducción dedicada pendiente.

## REPL — Bucle interactivo e historial

Archivos: `include/latrepl.h`, `src/latrepl.c`.

**Dependencias:** Readline/linenoise, parser y VM.

**Contrato y herencia:** Interacción histórica de consola; no cambiar en A0.

**Errores relevantes:** EOF, error sintáctico, fallo repetido, señales.

**Pruebas existentes o relacionadas:** Regresiones CLI indirectas; pruebas PTY completas pendientes.

**Pendiente / límite de la auditoría:** Recursos de terminal/historial y restauración tras señales sin certificación completa.

## CLI — Entrada ejecutable, API central y versión

Archivos: `include/latino.h`, `include/mictlan_version.h`, `src/latino.c`.

**Dependencias:** Todo Core, args, locale, build.

**Contrato y herencia:** mictlan conserva .lat/.mict; 285 símbolos C de referencia.

**Errores relevantes:** Argumentos, archivo ausente, ejecución y salida.

**Pruebas existentes o relacionadas:** tests/alpha/; tests/conformidad/; tests/colecciones/abi.py.

**Pendiente / límite de la auditoría:** Versión histórica no identifica revisión; ABI de Windows y packaging pendiente por plataforma.

## RES — Registro de recursos y colecciones de recursos

Archivos: `src/latlista_recursos.c`, `src/latlista_recursos.h`, `src/latrecurso.c`, `src/latrecurso.h`.

**Dependencias:** Asignador, callbacks y handles.

**Contrato y herencia:** Propiedad y cierre de recursos P0.2; consumidores C/SDK conservados.

**Errores relevantes:** Doble cierre, orden, OOM, callback inválido.

**Pruebas existentes o relacionadas:** tests/*recurso*; pruebas P0.2 anteriores.

**Pendiente / límite de la auditoría:** SDK no se modifica ni reconstruye aquí; callbacks/ciclos exóticos pendientes.

## DEV — Inspección lat.mostrar y lat.imprimir_pila

Archivos: `src/latdevlib.c`.

**Dependencias:** Formateo y objetos internos.

**Contrato y herencia:** Diagnóstico que expone metadatos y direcciones del propio proceso.

**Errores relevantes:** Tipo cíclico, formato/OOM, escritura de stdout.

**Pruebas existentes o relacionadas:** Pruebas indirectas de formato; sin batería exclusiva.

**Pendiente / límite de la auditoría:** Clasificar API de depuración antes de distribución; no considerar sus textos estables.

## OPTIONAL — UI histórica no integrada en build normal

Archivos: `src/latcurseslib.c`, `src/latuilib.c`.

**Dependencias:** Windows/PDCurses y terminal.

**Contrato y herencia:** No disponible por defecto; ui contiene stubs.

**Errores relevantes:** Compilación opcional y parámetros sin validar.

**Pruebas existentes o relacionadas:** No se ejecuta en la suite normal.

**Pendiente / límite de la auditoría:** No anunciar estas APIs como soporte operativo; fuera de A0 funcional.

## LINE — Edición de línea y UTF-8 de tercero incluido

Archivos: `src/linenoise/.gitignore`, `src/linenoise/CMakeLists.txt`, `src/linenoise/README.markdown`, `src/linenoise/example.c`, `src/linenoise/linenoise.c`, `src/linenoise/linenoise.h`, `src/linenoise/utf8.c`, `src/linenoise/utf8.h`.

**Dependencias:** Terminal, libc, dos fuentes compiladas.

**Contrato y herencia:** Conservar licencia/código de tercero; uso real depende de plataforma.

**Errores relevantes:** UTF-8, ancho de pantalla, memoria, señales.

**Pruebas existentes o relacionadas:** Sin auditoría exhaustiva del tercero en A0.

**Pendiente / límite de la auditoría:** Fijar versión/procedencia y evaluar alcance real en consola.

## PLATFORM — Macros portables, recursos Windows y binarios heredados

Archivos: `include/latcompat.h`, `src/latino-regex`, `src/latino.def`, `src/latino_os_version.manifest`, `src/lib/Debug/jansson_d.dll`, `src/lib/Debug/jansson_d.lib`, `src/lib/Debug/latino.dll`, `src/lib/Debug/latino.lib`, `src/lib/Debug/libcurl.dll`, `src/lib/Debug/libcurl.lib`, `src/lib/Debug/regex.lib`, `src/lib/Debug/regex2.dll`, `src/lib/Release/jansson.dll`, `src/lib/Release/jansson.lib`, `src/lib/Release/latino.dll`, `src/lib/Release/latino.lib`, `src/lib/Release/libcurl.dll`, `src/lib/Release/libcurl.lib`, `src/lib/Release/regex.lib`, `src/lib/Release/regex2.dll`, `src/vs_resources/latino.aps`, `src/vs_resources/latino.rc`, `src/vs_resources/resource.h`.

**Dependencias:** Compilador/CRT/regex y DLLs incluidas.

**Contrato y herencia:** Inventariar hashes; no ejecutar binarios incluidos para auditarlos.

**Errores relevantes:** ABI divergente, dependencias ausentes, plataforma sin probar.

**Pruebas existentes o relacionadas:** Linux ABI; Windows/macOS requieren matriz propia.

**Pendiente / límite de la auditoría:** Binarios versionados sin reconstrucción de origen en esta misión; no certificarlos por existir.

## BUILD_DOC — Build, instalación y ayuda del sistema

Archivos: `src/CMakeLists.txt`, `src/gnu-utilities/latino.desktop`, `src/gnu-utilities/latino.nanorc`, `src/gnu-utilities/mictlan.desktop`, `src/man/latino.1`, `src/man/latino.1.gz`.

**Dependencias:** CMake, empaquetado, docs heredadas.

**Contrato y herencia:** mictlan como ejecutable; liblatino mantiene nombre ABI.

**Errores relevantes:** Rutas de instalación, generados, ayuda desfasada.

**Pruebas existentes o relacionadas:** CI de Core, portable y suites anteriores.

**Pendiente / límite de la auditoría:** Manual man/editor puede no reflejar APIs nuevas; prueba de instalación limpia pendiente.

Las rutas con comodines en pruebas son referencias de búsqueda, no porcentajes de cobertura. No se infiere cobertura de una mera coincidencia textual. La matriz funcional ejecutada y sus resultados se registran por separado en A0_INFORME.md. SDK y Lab no forman parte de este checkout ni de esta entrega.
