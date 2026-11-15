# Mictlán A0 — inventario de Core y primera auditoría funcional de sis

A0 entrega un censo reproducible del núcleo, una matriz de riesgos y reparaciones
comprobadas de `sis`. **No declara que Core esté terminado ni libre de defectos.**
La [PR #13](https://github.com/TuxMatic99/Mictlan-Core/pull/13) permanece abierta en
borrador contra `investigacion/p12b4b-entrada-modulos`.

## Revisiones y alcance conservado

| Punto de control | SHA | Contenido |
|---|---|---|
| Base B4-B | `2651317e4224f5499765b5b0ae16d1404a0adfc4` | Base exacta, PR #12 |
| Entrega A | `99fde0f8c29b7b5e84fa2d3f362803a091d369c9` | Inventario, contratos, 166 regresiones y evidencia anterior; producción sin cambios |
| Entrega B | `755f88c15f5a600c0d719ab2bd22e07651efd00a` | Reparaciones C y mismas regresiones aprobadas |
| Entrega C, código/batería completos | `afc8920682e7f863c2162e620b12aaeaac3dd7a7` | 170 controles A0, manual, guía y registros locales |

El commit que contiene este informe añade evidencia y precisión documental sobre
la entrega C, sin cambiar producción ni pruebas. La punta exacta y su CI también
se registran en la descripción de la PR; `git rev-parse HEAD` identifica cualquier
copia descargada. No hay reescritura de historia ni una segunda rama A0.

La primera entrega es hija directa de B4-B. Se comprobó la conservación de las
PR #7–#12 abiertas en borrador, y de main. No se modificaron SDK ni Lab. El único
archivo de producción cambiado es `src/latsyslib.c`; no cambian include/, gramática,
parser, precedencia, ámbitos ni estructuras utilizadas por SDK.

## Inventario y clasificación

[INVENTARIO_CORE.md](auditoria/INVENTARIO_CORE.md) cubre las **92 entradas
versionadas** de src/, include/ y tools/parser/, distribuidas en **26 subsistemas**.
Cada subsistema registra propósito, dependencias, contrato heredado, errores,
pruebas relacionadas y riesgos. El censo incluye generados, componentes inactivos,
binarios heredados y el gitlink de regex, sin ejecutarlos ni descargar submódulos.

Los CSV contienen hashes/tamaños/includes, **222 registros nativos** y **550
candidatos léxicos de definición C** (545 antes de añadir cinco ayudantes).
El índice léxico no equivale a un parser C ni a revisión semántica individual.
Los contratos generales se organizan por subsistema; los 15 nombres públicos de
sis tienen tabla individual en [A0_SIS_DECISIONES.md](A0_SIS_DECISIONES.md).

[PENDIENTES_CORE.md](PENDIENTES_CORE.md) es el documento maestro actualizable.
Cada fila tiene identificador, categoría, prioridad/gravedad, archivo/función,
evidencia o reproducción pendiente y criterio de acción. Se distinguen defectos
reproducidos, limitaciones conocidas, compatibilidad deliberada y propuestas.
Las coincidencias con pruebas existentes no se presentan como cobertura completa.

## Reparaciones y evidencia anterior

[A0_DIAGNOSTICO.md](A0_DIAGNOSTICO.md) y los logs antes del parche se conservaron.
Las expectativas proceden de calendarios, límites de tipos, bytes y contratos
explícitos, no de copiar la salida original. Los 56 fallos nuevos corresponden a
diez familias de defectos, no a 56 bugs independientes.

| Hallazgo | Causa anterior | Corrección y regresión |
|---|---|---|
| Conversión de tiempo inválida | double→time_t sin finitud/rango | Límites exactos antes del cast y validación del calendario; tiempo_nan/inf/limite_* |
| Calendario/fecha fallidos | Retornos de time/localtime/gmtime ignorados; NULL usado | Comprobación y error VM; fecha_reloj/local/utc, tiempo_local/reloj |
| Recursos de pipe | Abría antes de reservar; publicación podía saltar limpieza | Reserva anterior a apertura, publicación protegida, liberación antes de propagar; pipe_reserva/publicar |
| Salida incompleta | Una lectura limitada presentada como salida completa; NUL ocultaba sufijo | Lectura por bloques, rechazo explícito de exceso/NUL y drenaje de salida finita; pipe_exceso/nul |
| Interrupción de lectura | EINTR tratado como final fallido | clearerr y reintento conservando bytes; pipe_eintr |
| Duración/señal/coordenadas inválidas | Cast a entero antes de validar | Ayudante numérico existente, signo/rango/finitud; dormir/avisar/iraxy límites |
| Espera interrumpida o fallida | Ignoraba nanosleep y el tiempo restante | Reanudar resto en EINTR, propagar otros errores; dormir_eintr/error |
| Formatos fallidos | strftime==0 ignorado en semana/ISO | Error antes de publicar buffer sin resultado; fecha_formato_* |
| Versión Linux incorrecta | strtol con avance incondicional y overflow; datos sin inicializar tras uname fallido | Inicializar, analizar segmentos decimales acotados; version_* |
| Error operativo ignorado | signal==SIG_ERR y system==-1 sin comprobación | Error recuperable; avisar_fallo, aux_ejecutar_error |

Detalles de implementación: [A0_CORRECCIONES.md](A0_CORRECCIONES.md).
Se reutilizan `latN_entero`, `latE_proteger` y el mecanismo de errores de Core.
No se cambia el modelo de memoria ni se registra una nueva función pública.

## Resultados comprobados

La base limpia local aprobó **676/676** antes de añadir A0. Primera entrega:

| Entorno anterior, SHA 99fde0f8 | Aprobadas | Fallidas | Omitidas / bloqueadas |
|---|---:|---:|---:|
| Local Debian 13 Debug | 786/842 | 56 | 0 / 0 |
| CI Debug | 786/842 | 56 | 0 / 0 |
| CI Release | 786/842 | 56 | 0 / 0 |
| CI ASan+UBSan+LSan | 802/858 | 56 | 0 / 0 |

En los 56 fallos instrumentados iniciales hay **31 con diagnóstico de
instrumentación y 25 fallos funcionales del arnés**. Los registros muestran casts
fuera de rango y uso de punteros de calendario nulos. Las fugas de recursos se
reproducen mediante contadores antes de la limpieza auxiliar del arnés; esa
limpieza no se atribuye a producción. No se presentan las ejecuciones iniciales
como aprobadas.

[CI anterior, run 43674333390](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43674333390).
En el commit de reparaciones 755f88c1, [run 43675469176](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43675469176)
aprobó 842/842 Debug, 842/842 Release y 858/858 instrumentadas.

La batería final de afc8920 conserva las 676 ordinarias y las 692 instrumentadas
y añade **170 controles**: 98 contratos C, 70 ejecuciones de 35 pares .lat/.mict,
un verificador del inventario y otro de ejemplos/documentación. Los cuatro casos
adicionales de cierre prueban sleep_ms negativo, un ejemplo de señal inválida en
ambas extensiones y la sincronización del manual.

[CI final, run 43675995049](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43675995049),
checkout exacto **afc8920682e7f863c2162e620b12aaeaac3dd7a7**:

| Perfil | Aprobadas | Fallidas | Omitidas | Bloqueadas | Tiempo CTest |
|---|---:|---:|---:|---:|---:|
| Debug | 846/846 | 0 | 0 | 0 | 22.93 s |
| Release | 846/846 | 0 | 0 | 0 | 26.85 s |
| ASan + UBSan + LSan | 862/862 | 0 | 0 | 0 | 52.92 s |

Jobs: Debug 132696152573, Release 132696152377, instrumentado 132696152679.
Se comprobaron los logs completos, el SHA y los 676/692 nombres anteriores,
además de 170 nombres A0 en cada perfil. No hubo informes de sanitizadores en
estas suites ejecutadas. Los tiempos son duración de la batería, no benchmarks
comparables de rendimiento del lenguaje.

Las ejecuciones locales finales se hicieron sobre los mismos archivos de
producción/pruebas publicados en afc8920, antes de asignar ese SHA. Los XML y logs
conservados registran cada caso. Debug y Release: **846/846**, sin fallos ni omisiones.
Instrumentación local: CTest devuelve fallo en **854/862 por infraestructura de
ptrace**, y aprueban ocho verificadores auxiliares. No se atribuyen esas ocho
aprobaciones a pruebas dinámicas de memoria, ni se cuentan las 854 como aprobadas.
No se desactivó ASan, UBSan, float-cast-overflow o LSan.

Compilar/configurar los tres perfiles finales terminó correctamente. Se conservan
avisos heredados de compilación; A0 no promete una compilación sin warnings. Los
intentos iniciales de arnés y el ajuste de llamada lógica durante el parche se
registran en sus informes; no se mezclan con resultados de suites aprobadas.

## Recursos, compatibilidad y aislamiento

Cada contrato C comprueba recuperación con una llamada válida posterior en la
**misma VM**, centinela de pila, contexto, contador de ejecución, manejador de
error y nombre de archivo. Las tuberías instrumentadas cuentan apertura, cierre,
reserva y liberación. Un productor propio cubre salida vacía, LF/CRLF, UTF-8,
estado no cero, límite y exceso; además se verifica `productor.fin` y el disco.
No se ejecuta software desconocido ni se usan documentos de usuario.

Los 35 programas nuevos tienen fuentes idénticas en .lat y .mict y los mismos
resultados esperados. Los ejemplos del manual se verifican contra los fixtures
antes de ejecutarse. Se mantienen los 15 nombres de sis y sus firmas/aridades.

Los **285 símbolos C** definidos coinciden exactamente con la referencia en
Debug, Release y la biblioteca instrumentada. El verificador sólo separa los
siete guardias ODR identificados de ASan; no oculta símbolos nuevos del proyecto.
Las cabeceras no cambiaron. No se recompiló SDK en esta misión: se preservaron ABI,
cabeceras y regresiones C de Core, sin afirmar una prueba nueva del SDK.

Se reutiliza `tests/archivos/ejecutar.py` y su sonda B4-A: limpia variables heredadas,
fija la biblioteca del árbol, comprueba ruta efectiva y registra SHA256. La prueba
histórica de biblioteca ajena sintética sigue incluida. El ejecutable de contratos
C usa la biblioteca estática del mismo build. El texto ABI «Latino 1.5.0» no es un
identificador de revisión; los SHA de git/biblioteca completan la trazabilidad.

Compatibilidad deliberada: tiempo sin formato ignora instante; strftime vacío o
sin espacio devuelve nulo; pipe quita un salto final; el estado no cero del hijo
sigue sin ser error del lenguaje; avisar mantiene estado global persistente.
Cambio explícito: NUL/exceso ya no producen un prefijo silencioso. NaN, infinitos,
rangos inválidos y errores operativos cubiertos pasan a errores recuperables.

## Reproducción, límites y siguiente auditoría

Guía: [A0_DEBIAN13.md](A0_DEBIAN13.md). Referencia educativa:
[SISTEMA.md](SISTEMA.md). Los [registros originales y su manifiesto](evidencias/a0/README.md) permiten
auditar los resultados. Los comandos de configuración figuran en la guía y en
los logs de compilación, con los prefijos locales de readline separados de la
reproducción normal de Debian. Se usó --parallel 4 en la validación local final.

Entorno local: Debian 13.6, GCC 14.2.0, glibc 2.41, CMake 4.4.5, Python 3.12.14.
CI: Ubuntu 24.04, GCC 13.3.0, glibc 2.39, CMake 3.31.6, Python 3.12.3.
Los locales C, es_ES.UTF-8 y fr_FR.UTF-8 usados por las suites anteriores estaban
presentes. A0 no cambia la configuración regional global del equipo.

Limitaciones: no se certifican Windows/macOS/Cygwin ni terminales interactivas;
no se eliminan los límites de formato o los datos globales de señales; el drenaje
de pipe **no es un timeout** ni controla productores infinitos. No hay promesa de
concurrencia de todas las bibliotecas. No se infiere corrección de cada reserva
interna del compilador a partir de los fallos inyectados en sis. No se midió una
mejora de rendimiento ni se declara aceleración; pipe sigue siendo lineal en los
bytes consumidos y mantiene su reserva temporal máxima histórica.

Antes de una distribución experimental deben revisarse los P0 del documento
maestro: agotamiento interno de parser/compilador, reproducibilidad de generados y
procedencia/reconstrucción de binarios incluidos. La siguiente auditoría funcional
puede abordar clases, control de flujo anidado, límites de VM y carga nativa con
fixtures propios. Sus indicios siguen siendo propuestas o limitaciones hasta que
haya una reproducción concreta; no se convierten automáticamente en defectos.
