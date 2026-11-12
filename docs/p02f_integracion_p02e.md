# P0.2-F + P0.2-E: integracion experimental separada

Fecha: 12 de noviembre de 2026. Esta rama no sustituye ni fusiona las propuestas.

Base F: `0699dabeca9498a1165472c9569634e86bc8d32b`, rama
`investigacion/p02f-work`. Origen E: `ea284f95e46d846771633ecdbcce7f4e61c0b31a`,
PR Core #1, cuyo padre es `0b4f8df58935e425640824406406d95c9dc3281f`.

Se aplica exactamente el parche de E sobre una copia de F en la rama
`integracion/p02f-p02e-work`. `git apply --check` pasa sin conflictos.
`src/latdo.c`, la regresion de pila y el informe original de E son identicos
byte a byte a E. CMake contiene ambas familias de pruebas. El informe antiguo
describe los resultados de su momento; no sustituye la validacion combinada.

Comandos de preparacion utilizados en una copia independiente:

```sh
git diff 0b4f8df58935e425640824406406d95c9dc3281f \
    ea284f95e46d846771633ecdbcce7f4e61c0b31a > p02e-original.patch
git apply --check p02e-original.patch
git apply p02e-original.patch
```

La recuperacion de pila descarta descriptores prestados sin destruir sus valores.
La region de F conserva los contenidos compartidos hasta cerrar la VM. Son
responsabilidades compatibles: E corrige la profundidad activa; F libera cada
reserva registrada una sola vez. Ninguna cambia cabeceras publicas.

Comprobacion local inicial: compilan Debug, Release y ASan+UBSan. Debug y Release
aprueban 21 de 22 casos Core; el restante es `coverage`, que apunta a un archivo
ausente y sigue siendo FAIL_CONFIGURATION. Las nueve secuencias de recuperacion
pasan tambien 1000 vueltas por caso en ambos perfiles, conservando el saldo
contabilizado de memoria despues de la primera vuelta. Esto no prueba que todos
los programas tengan consumo acotado: F conserva temporales hasta cerrar.

Inventarios: Debug 22, Release 22, sanitizadores 38. LSan local tiene restricciones
de ptrace y no se certifica aqui. La matriz SDK P0.2-F fija este commit y conserva
trazas, controles reales de sanitizadores, JUnit y JSON en GitHub Actions. Los
resultados finales de esa ejecucion se enlazaran desde las PR; no se presupone
su exito. No se eliminan fallos conocidos ni se utiliza exitcode=0 de LSan.
