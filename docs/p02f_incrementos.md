# P0.2-F: registro de incrementos

## F1 — contrato y regresiones antes de corregir

Commit `651b27aa4adeffd3f116841fc9a24c4cd74cf842`, sobre `e7ece25`.
Las diez regresiones nativas y el contrato se publicaron antes del parche.
GitHub Actions, ejecucion SDK `43164505980`, reprodujo las fugas de los cuatro
experimentos: control limpio, lista simple 200 bytes, sustitucion 264 bytes y
alias 528 bytes. SDK instrumentado: 30/32; siguen 600 y 528 bytes. Los controles
ASan/UBSan y LSan integrado/autonomo pasan. No hay bloqueo ptrace en ese runner.
Localmente se observan siete fallos estructurales, ademas del bloqueo LSan.

## F2 — region de propiedad de una VM

Se registra exclusivamente cada envoltorio, lista y tabla construido por Core
para una VM real. No se registran ni destruyen automaticamente datos C externos.
La busqueda por direccion permite retirar cada liberacion explicita y evitar
volver a liberar temporales de Core/SDK. Al cerrar, tablas, listas y envoltorios
se limpian en fases sin recorrer el grafo de valores. Se conserva la limpieza
independiente de constantes y la API manual de los ensayos `lat_mv={0}`.

Archivos de implementacion: `latmem.c` contiene el registro privado y su enlace
con las liberaciones; `latobj.c` separa envoltorio/contenido y organiza el cierre;
`latlist.c`/`latdic.c` registran sus construcciones; `latmv.c` abre/cierra la region;
`latpropiedad.h` declara solo funciones internas con visibilidad oculta en ELF.
No cambian cabeceras ni estructuras publicas. La memoria del propio registro se
incluye en `memoria_usada` y tambien se libera.

`latM_reasignar` debe actualizar la direccion registrada cuando realloc mueve
el bloque; conserva la entrada si falla. Se corrige su saldo de memoria y se
evita consultar el bloque antiguo despues de realloc. Se define explicitamente
tamano cero como liberacion y retorno NULL (antes se confundia con agotamiento
de memoria). Esta mejora del contrato publico de asignacion no cambia su firma.
Una regresion adicional comprueba crecimiento, reduccion, contenido preservado,
liberacion explicita y tamano cero. La base se ensaya con el mismo ejecutable de
prueba enlazado a su biblioteca antigua; no se presupone que esas rutas pasan.

En el primer ensayo local F2, nueve de los diez casos estructurales anteriores
pasan en Debug. Sigue fallando la clonacion ciclica, que se corrige en F3 como
incremento separado. Las comprobaciones de fugas locales siguen bloqueadas por
ptrace y no se consideran aprobadas. La entrega final adjuntara los SHA y los
resultados reales de GitHub para cada incremento.

Limite principal: los contenidos sustituidos pueden permanecer reservados hasta
cerrar la VM. No se afirma que el consumo de una VM de larga duracion este
acotado, ni que exista GC durante su ejecucion. La base y el commit F1 conservan
la alternativa anterior sin reescribir historia.
