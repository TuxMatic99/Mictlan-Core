# A0 — entrega de correcciones

Diagnóstico y 166 regresiones publicados primero en
`99fde0f8c29b7b5e84fa2d3f362803a091d369c9`, PR #13 contra B4-B. Esta entrega sólo
modifica producción en `src/latsyslib.c`; reutiliza latN_entero y latE_proteger.
No cambia cabeceras, gramática ni estructuras públicas.

| Defecto | Reparación | Evidencia después |
|---|---|---|
| A0-D01 | Truncar y validar double finito contra límites exactos de time_t antes del cast; comprobar calendario | tiempo_* |
| A0-D02 | Comprobar reloj y conversiones; tm local en Unix; instante explícito no consulta reloj | fecha_reloj/local/utc, tiempo_reloj/local/reloj_explicito |
| A0-D03 | Reservar antes de abrir pipe; liberar buffer tras publicación protegida antes de relanzar error | pipe_reserva/publicar/abrir/cerrar; contadores de recursos |
| A0-D04 | Lectura completa por bloques; detectar NUL/exceso; drenar salida finita y rechazar prefijo | pipe_nul/limite/exceso C y CLI; marca productor.fin |
| A0-D05 | Conservar bytes leídos y reintentar EINTR tras clearerr | pipe_eintr |
| A0-D06 | Validar segundos, señales y coordenadas antes de casts | dormir/avisar/iraxy límites y no finitos |
| A0-D07 | nanosleep reintenta con resto, error operativo llega a VM; ayudante C conserva firma void | dormir_eintr/error |
| A0-D08 | Comprobar strftime de semana e ISO antes de publicar | fecha_formato_sem/iso |
| A0-D09 | Versión Linux inicializada, segmentos decimales separados por punto, sin overflow ni avance tras NUL | version_* |
| A0-D10 | Comprobar SIG_ERR y system==-1; conservar estado no cero del hijo | avisar_fallo, aux_ejecutar_error/hijo y CLI |

Las mismas 842 pruebas Debug pasan: **842/842**, 0 fallidas, 0 omitidas,
0 bloqueadas. Incluyen las 676 de B4-B. Los 285 símbolos C definidos coinciden
con la referencia, sin nuevos. Registro completo:
[evidencias/a0/local-implementacion-debug.log.gz](evidencias/a0/local-implementacion-debug.log.gz).
Cada contrato C comprueba una llamada válida posterior en la misma VM y un
centinela de pila, contextos, contador, manejador y nombre de archivo.

Antes del parche, GitHub Actions confirmó 56 fallos en Debug y Release sobre
99fde0f8, y diagnósticos de UBSan en casts y uso de punteros de calendario nulos.
Los logs originales de los tres perfiles se conservan como ci-antes-*.log.gz.
Run: https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43674333390
La instrumentación inicial sirve de evidencia de defectos, nunca de aprobación.

La primera compilación del parche usó un ayudante lógico inexistente; se sustituyó
por los booleanos públicos existentes y se recompiló antes de ejecutar los
resultados de esta entrega. Los ensayos de compilación fallidos no son pruebas
aprobadas. La matriz final Release/sanitizadores y documentación educativa siguen
en la tercera entrega.

Se conservan los límites y comportamientos descritos en A0_SIS_DECISIONES.md.
El rechazo de salida NUL/excesiva de pipe es una corrección explícita de pérdida
silenciosa. No hay timeout de procesos, API nueva ni certificación de plataformas
no ejecutadas. El inventario regenerado registra ahora 550 candidatos C, frente
a 545 antes del parche; son un índice léxico, no un certificado semántico.
