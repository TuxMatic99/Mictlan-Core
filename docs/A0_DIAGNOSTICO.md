# A0 — entrega inicial de inventario y pruebas

Producción sin cambios sobre `2651317e4224f5499765b5b0ae16d1404a0adfc4`.
PR #12 comprobada abierta en borrador en esa revisión; PR #7–#11 y main conservadas.
No existía rama ni PR A0 al comenzar. Rama nueva:
`investigacion/a0-inventario-sistema`, con destino B4-B.

El [inventario](auditoria/INVENTARIO_CORE.md) clasifica 92 entradas versionadas de
src/include/tools/parser en 26 subsistemas. Registra 222 entradas de tablas nativas,
incluidas las que no se activan, y 545 candidatos léxicos de definición C. También
incluye hashes de binarios y el SHA del submódulo sin ejecutarlos. No es una
certificación individual de todas las funciones ni una medición de cobertura.

La [matriz maestra](PENDIENTES_CORE.md) separa diez familias de defectos demostrados
de limitaciones, contratos compatibles e hipótesis de auditoría. Los
[contratos de sis](A0_SIS_DECISIONES.md) preceden a toda corrección de producción.

La base limpia aprobó 676/676 en un árbol Debug nuevo. Con 166 casos A0 añadidos,
**786/842 aprueban y 56 fallan**; 0 omitidos, 0 bloqueados. Las 676 anteriores
siguen aprobadas. A0 añade 97 contratos C/inyección, 68 ejecuciones de 34 pares
.lat/.mict y el control del inventario. Los fallos son repeticiones de clases de
problema, no 56 bugs independientes. [Log original](evidencias/a0/local-antes.log.gz)
y [detalle de fallos](evidencias/a0/antes-resumen.txt).

Se observaron flujo abierto tras falla de reserva, buffer pendiente tras falla de
publicación, EOF prematuro aparente por límite/NUL, falta de reintento EINTR,
retornos de calendario ignorados, casts inválidos y errores operativos ignorados.
Dos casos de fecha con puntero NULL terminan el proceso de prueba. La versión
6.8-12 se interpreta erróneamente como parche12. Los programas de exceso comprueban
además la marca de terminación de su productor y el disco.

Los primeros intentos de compilar el arnés tuvieron errores de declaración y
un conflicto con main del archivo estático de CLI; se corrigieron en la prueba
sin tocar producción. Una ejecución previa sin binario no se cuenta como fallo de
Core. Los resultados publicados arriba corresponden a la compilación correcta.

Comandos reales (prefijos locales de readline específicos de este entorno):

```sh
cmake -S . -B ../build-a0-debug -DCMAKE_BUILD_TYPE=Debug \
 -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
 '-DCMAKE_C_FLAGS=-I/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/include' \
 '-DCMAKE_EXE_LINKER_FLAGS=-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu' \
 '-DCMAKE_SHARED_LINKER_FLAGS=-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu'
cmake --build ../build-a0-debug --parallel 2
env -u LD_LIBRARY_PATH -u LD_PRELOAD -u LD_AUDIT \
 ctest --test-dir ../build-a0-debug --output-on-failure --output-junit a0-antes.xml
python3 tools/auditoria/inventario.py --comprobar
```

No se modificaron SDK, Lab ni main. Los fallos de sistema se sustituyen por
proveedores sintéticos; los únicos hijos de tubería reales son el productor
incluido y compilado por esta batería. No se ejecutan órdenes administrativas,
no se cambian configuraciones del equipo ni se cargan bibliotecas desconocidas.
