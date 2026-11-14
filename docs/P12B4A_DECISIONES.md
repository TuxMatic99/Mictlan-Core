# P1.2-B4-A: decisiones anteriores a la implementación

Base exacta: `b0a2d860363e07274feae1d0cf2b421515386d73` (P1.2-B3).
Rama nueva: `investigacion/p12b4a-integridad-archivos`, destino de la PR:
`investigacion/p12b3-matematicas`. Las PR #7, #8, #9 y #10 se comprobaron
abiertas, en borrador y sin fusionar, con cabezas c25eb7e, 6b8204a,
3125a00 y b0a2d86 respectivamente. No se modifica ninguna de ellas ni main.

## Diagnóstico reproducible

La entrega inicial contiene pruebas y este documento; no cambia `src/` ni
`include/`. Debug sobre la implementación original: **294/331 aprobadas,
37 errores funcionales; 251/251 anteriores y 43/80 nuevas aprobadas**.
Registro: [antes-debug.log](evidencias/p12b4a/antes-debug.log).
No se atribuye este resultado a Release ni a sanitizadores.

| Caso | Observación anterior | Oráculo independiente |
|---|---|---|
| Copiar origen ausente | Crea/trunca destino y deja un flujo abierto | Error; destino ausente o bytes anteriores intactos |
| Fallo de apertura destino | Deja abierto el origen | Error y cero recursos abiertos |
| Mismo inode por enlace duro | Destruye el origen | Rechazar antes de escribir |
| Destino enlace simbólico | Modifica su objetivo | Rechazar; objetivo intacto |
| Origen directorio | Éxito aparente, destino truncado | Rechazar origen no regular |
| Errores leer/escribir/cerrar copia | Se ignoran, puede publicar contenido incompleto | Error antes del reemplazo; destino intacto |
| `ejecutar` `.mict` | Omite el programa | Misma ejecución que `.lat` |
| `ejecutar` `.lat` | No restaura archivo/REPL/ubicación; oculta error de ejecución | Restaurar y propagar; siguiente llamada funciona |
| Cierre del lector de fuente | Ejecuta pese a fallo de cierre | No ejecutar fuente cuya lectura/cierre no se confirmó |
| Publicar lectura / crear lista falla | Un buffer temporal queda reservado | Liberarlo antes de propagar el error |
| Compilar falla (inyección) | 13 reservas del AST pendientes | Destruir AST y restaurar estado |
| EINTR / escritura parcial | Lectura falla; escritura queda incompleta | Reintentar interrupción y completar progreso parcial |

Cuatro fallos de la batería inicial (copia 12–15: fsync archivo/directorio,
rename y reserva) **no alcanzan su punto de inyección en la copia antigua**.
Son contratos nuevos del motor compartido que todavía no usa `duplicar`,
no evidencia de que la copia antigua llamara mal a esas funciones.
El arnés exige que cada fallo se active; una ruta no ejercitada nunca aprueba.

Las pruebas C observan descriptores, flujos, temporales y reservas antes de
limpiar los recursos propios que la base deja pendientes. Las pruebas CLI
comparan el árbol de archivos completo, además del diagnóstico y salida.
Todos los datos son sintéticos y cada proceso trabaja en un directorio
temporal propio. La regresión histórica `strings_io` conserva la comprobación
de escritura diferida, ahora inyectada en su propio archivo en lugar de usar
un dispositivo del sistema. `atomic_io_regression.c` se incorpora sin duplicar
su implementación a CTest. No se usan documentos del usuario.

## Contratos elegidos

1. **Duplicar:** conservar la firma y el reemplazo de destinos regulares
   existentes, pero preparar la copia binaria en un temporal del mismo
   directorio, mediante el único motor atómico existente. Fuente regular,
   distinta del inode de destino; comprobar lectura y cierre de origen antes
   de reemplazar. Memoria acotada por bloque, sin límite artificial de 8 MiB
   para copia binaria. Ante cualquier fallo anterior al rename: destino intacto
   (o sigue ausente). No seguir enlaces simbólicos en el destino.
2. **Consecuencias explícitas:** el destino reemplazado tiene un inode nuevo y
   permisos 0600, como `escribir_atomico`; no se preservan metadatos ni se
   actualizan otros enlaces duros al antiguo destino. Copiar al mismo inode,
   a un enlace simbólico o desde un directorio pasa a ser error. No se promete
   protección frente a escritores concurrentes o cambios adversos de rutas.
   Se requiere directorio estable y sistema de archivos local.
3. **Durabilidad:** un fallo de fsync/cierre del directorio después del rename
   significa «reemplazado, durabilidad sin confirmar», no «destino intacto».
   `escribir_atomico` conserva estado 2; `duplicar`, sin resultado explícito
   heredado, informa advertencia y conserva la copia completada.
4. **Plataformas:** reutilizar el motor Linux ya probado. En otras plataformas,
   `duplicar` informará que no está implementada la copia protegida antes de
   modificar el destino. No introducir un fallback destructivo ni otro motor
   atómico sin validación. Es una limitación/cambio de compatibilidad explícito
   respecto de la copia antigua no validada allí; soporte portable queda pendiente.
5. **Escribir/anexar/intentar_escribir:** conservan escritura directa y sus
   retornos; completar escrituras parciales y EINTR, comprobar cierre. Un fallo
   puede dejar un archivo parcial/truncado. Para reemplazos transaccionales,
   recomendar `escribir_atomico`, no cambiar silenciosamente las otras funciones.
6. **Lectura:** mantener límite 8 MiB, rechazo de NUL y ausencia de validación
   UTF-8; distinguir ausente/fallo sólo donde ya existe ese contrato. `lineas`
   separa por LF, conserva CR y líneas vacías interiores, y no añade una línea
   vacía por el LF final. Liberar buffers aunque la VM lance un error.
7. **Ejecutar:** `.lat` y `.mict` con el mismo camino; restaurar nombre de archivo,
   REPL y ubicación del llamador en éxito/error; propagar error de ejecución.
   Mantener archivo vacío como error y extensión desconocida como no operación.
   La compilación usa el compilador existente; no cambiar gramática ni ámbitos.
   Corregir sólo las comprobaciones de E/S del cargador de fuente relacionadas.
8. **Auxiliares:** mantener crear exclusivo, eliminar/borrar booleanos y
   renombrar con resultado nombre/falso; documentar reemplazo POSIX de rename.
   No prometer rollback de efectos ya confirmados si falla publicar un resultado.
9. **ABI:** no modificar cabeceras, estructuras ni añadir símbolos públicos;
   comprobar los 285 existentes con el verificador de B2/B3.

## Aislamiento y reproducción de la evidencia inicial

Las 80 pruebas B4-A se ejecutan mediante `tests/archivos/ejecutar.py`:
elimina variables heredadas de inyección/enlace para su proceso hijo y fija
la ruta de la biblioteca de ese build. Una sonda `dladdr` compara la ruta real,
registra versión heredada y SHA-256 del binario. Una biblioteca ajena sintética
demuestra tanto la detección como el aislamiento. Las otras suites no cambian
su entorno; para ejecutar todo Core se recomienda limpiar las variables en
la invocación de CTest. La sonda comprueba ruta y bytes del artefacto, no
pretende inferir el commit de una versión textual histórica invariable.

Entorno local: Debian 13 x86_64, GCC 14.2.0, glibc 2.41, Python 3.12.14,
CMake 4.4.5. CMake requiere `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` desde 4.x.
Readline se proporciona mediante headers y biblioteca extraídos en el entorno;
en Debian normal basta `apt install build-essential cmake libreadline-dev python3`.

```sh
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel 2
env -u LD_LIBRARY_PATH -u LD_PRELOAD ctest --test-dir build-debug --output-on-failure
# Sólo regresiones B4-A:
env -u LD_LIBRARY_PATH -u LD_PRELOAD ctest --test-dir build-debug -L p12b4a --output-on-failure
```

Comandos locales efectivos: configuración con CMake 4.4.5, tipo Debug,
`-DCMAKE_POLICY_VERSION_MINIMUM=3.5`, C_FLAGS `-I/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/include`,
EXE/SHARED_LINKER_FLAGS `-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu`;
`cmake --build ../build-p12b4a-debug --parallel 2` y
`env -u LD_LIBRARY_PATH ctest --test-dir ../build-p12b4a-debug --output-on-failure`.
Una compilación inicial del propio arnés tuvo una colisión de nombre y se
corrigió antes del registro válido; las pruebas sin ejecutable de ese intento
no se cuentan como resultados funcionales.
