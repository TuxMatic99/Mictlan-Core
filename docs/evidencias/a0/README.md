# Evidencias A0

Logs originales comprimidos sin retirar diagnósticos. Las fechas de GitHub están
en UTC. No mezclar la ejecución local bloqueada con la aprobación de CI.

| Archivo | Revisión/contenido |
|---|---|
| local-base.log.gz | Base exacta 2651317, Debug 676/676 |
| local-antes.log.gz, antes-resumen.txt | Producción base con primera batería, 842 pruebas / 56 fallos; publicado en 99fde0f8 |
| ci-antes-debug/release/san.log.gz | 99fde0f8; run 43674333390; jobs 132691045334, 132691045555, 132691045480 |
| local-implementacion-debug.log.gz | Código 755f88c1, 842/842 |
| local-final-debug/release/asan.log.gz y .xml.gz | Archivos de afc8920; 846/846, 846/846 y 8 aprobados + 854 bloqueados por ptrace |
| local-clasificacion-*.json | Lista completa de casos por categoría; no cambia el fallo devuelto por CTest |
| local-compilaciones.tar.gz | Logs de configure/build locales, nombres originales; incluye avisos |
| entorno-abi-local.txt | Herramientas, SO, símbolos y diff de interfaces; recogido antes de asignar SHA de entrega C |
| ci-final-debug/release/san.log.gz | afc8920682e7f863c2162e620b12aaeaac3dd7a7; run 43675995049; jobs 132696152573, 132696152377, 132696152679 |
| referencias-github.json | PR #7–#13 y main comprobados antes del último commit de informe |
| SHA256SUMS | Integridad de los archivos de esta carpeta, salvo el propio manifiesto |

Comprobar y leer sin ejecutar los contenidos:

```sh
cd docs/evidencias/a0
sha256sum -c SHA256SUMS
gzip -cd ci-final-san.log.gz
tar -tzf local-compilaciones.tar.gz
```

La clasificación local da prioridad a informes reales (`ERROR: AddressSanitizer`,
`ERROR: LeakSanitizer`, `runtime error:` o resúmenes ASan/UBSan) frente al bloqueo.
Sólo clasifica infraestructura cuando coinciden el error fatal de LeakSanitizer
y `does not work under ptrace`. Un caso sin fallo ni skipped es aprobado; skipped
es omitido; otros fallos son funcionales hasta diagnóstico. Se verificó que los
854 fallos locales instrumentados cumplen el patrón de infraestructura. Los ocho
aprobados son verificadores auxiliares, no prueba de memoria dinámica de Core.

La CI anterior instrumentada tiene 31 casos con diagnóstico instrumentado y 25
fallos funcionales. La CI final contiene todos los nombres anteriores (676/692)
y 170 A0, con cero fallos/omisiones. Sus logs registran las opciones habilitadas,
el checkout exacto, herramientas y enlace. El workflow hace checkout de la cabeza
de la PR, no del merge virtual. No se cambiaron las opciones de detectores para
obtener aprobación.
