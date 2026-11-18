# A1.4 — diagnóstico previo a producción

Base A0 `7371aa2f7a5983e10d7e0db23bcabcfce3fc2908`, PR #13. Se creó la
rama `investigacion/a1-oom-ast`; esta entrega no modifica código de producción.
La prueba compila los archivos reales de AST y compilador, con observación de
sus reservas mediante `--wrap` en el binario propio. `A1_INYECTADO` identifica
función, fase, ordinal y tamaño; cada hijo se ejecuta en temporal propio.

En Debian 13.6, GCC 14.2.0, CMake 4.4.5, sin modificar producción: **5 de 37
casos aprobados; 32 fallan**, cero omitidos. El control sin inyección de
`latA_nodo` devuelve 0; al fallar su primera reserva, se registra
`A1_INYECTADO api=malloc ordinal=1 bytes=40 fase=1`, el hijo termina por
SIGSEGV (retorno -11) y CTest marca la prueba como fallida. El fallo no es un
resultado esperado permanente. También se reproduce en reservas posteriores de
nodos de valores, cadenas e identificadores. La inyección en compilación de
función/bucle muestra buffers temporales pendientes y `enBucle` sin restaurar.

El caso sin inyección de compilación de `elegir` detecta dos reservas temporales
no liberadas (el control ordinario falla), una observación distinta de OOM. Los
logs completos y XML están en `evidencias/a1/antes.*.gz`; no se confunden con
aprobaciones de la suite anterior. Antes de este cambio A0 aprobó CI 846/846 y
862/862 instrumentadas en https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43676446725.

## Inventario y propiedad

| Constructor / familia | Reservas internas y propiedad actual | Contrato requerido ante OOM |
|---|---|---|
| `latA_nodo`, `latA_mi`, `latA_super`, `latA_asign`, `latA_mientras`, `latA_hacer` | Un nodo; un resultado válido toma sus hijos | NULL, errno=ENOMEM; los hijos permanecen del llamador |
| `latA_logico`, `latA_nulo`, `latA_numerico` | Nodo + nodo_valor | NULL y liberar las reservas parciales |
| `latA_cadena`, `latA_literal`, `latA_var` | Nodo + valor + texto (`analizar_fmt`/`strdup`) | NULL y liberar reservas propias, sin punteros parcialmente válidos |
| `latA_asign_le`, `latA_si`, `latA_funcion`, `latA_clase`, `latA_nueva` | Un nodo de estructura especializada; hijos se transfieren al éxito | Fallo sin liberar hijos del llamador |
| `latA_desde`, `latA_para` | Transformaciones, múltiples reservas y mutación de bloque; `para` consume literales aun al rechazar rango | Operación transaccional en OOM; la invalidez de rango conserva comportamiento histórico |
| Asignación compuesta | Destino se comparte entre asignación y operación | Destructor reconoce este alias; no liberar dos veces |
| Compilación `elegir` | Crea nodos temporales que apuntan a nodos originales prestados | Liberar sólo temporales, incluidos los anidados, en éxito y fallo |

La gramática tiene `%destructor` para `<node>`, pero el parser generado con
Bison 3.8.2 descarta la pila y el lookahead sin destruir los símbolos del lado
derecho de la regla activa que invoca `YYABORT`/`YYNOMEM`. El código actual tiene
sólo algunas comprobaciones `YYABORT` y lexer crea nodos directamente; devolver
NULL sin una vía completa de propagación no basta. Bison retorna 0 en éxito, 1
por sintaxis/abort y 2 por agotamiento. Su rama generada de error de memoria llama
`yyerror` con argumentos en orden distinto a la firma declarada, otro punto para
comprobar. Documentación oficial: https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html

El lexer generado también tiene rutas de `YY_FATAL_ERROR` que terminan el proceso
cuando fallan sus propios buffers. No se afirma repararlas por cubrir únicamente
reservas de constructores AST. La compilación tiene otras reservas y estructuras
administradas fuera de los puntos instrumentados. El objetivo es registrar las
rutas cubiertas y los límites reales; no declarar resuelto todo el compilador.

Las pruebas recorren cada reserva observada en casos sintéticos y exigen tras el
fallo una compilación y ejecución válida que asigne 42 en la misma VM, además de
pila/contexto/contador/manejador y recursos. Los errores sintácticos se ensayan
separadamente. Los fixtures de `.lat`/`.mict` previos permanecen en la suite A0.

Comandos reales:

```sh
cmake -S . -B ../build-a1-debug -DCMAKE_BUILD_TYPE=Debug \\
 -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \\
 '-DCMAKE_C_FLAGS=-I/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/include' \\
 '-DCMAKE_EXE_LINKER_FLAGS=-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu' \\
 '-DCMAKE_SHARED_LINKER_FLAGS=-L/workspace/scratch/1de5cf9cbaa6/build-deps/root/usr/lib/x86_64-linux-gnu'
cmake --build ../build-a1-debug --parallel 4
ctest --test-dir ../build-a1-debug -L a1 --output-on-failure --parallel 4 --output-junit a1-antes.xml
```

En Debian corriente basta instalar previamente las dependencias de Core; no hay
que copiar estas rutas específicas del entorno de trabajo. Se conservan los
resultados aunque la prueba retorne fallo. La instrumentación ASan/UBSan/LSan
se validará sin desactivarla; ptrace local será clasificado como infraestructura.
