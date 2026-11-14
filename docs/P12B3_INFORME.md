# P1.2-B3: matemáticas y conversiones numéricas

Implementación publicada en la [PR #10 en borrador](https://github.com/TuxMatic99/Mictlan-Core/pull/10),
contra `investigacion/p12b2-colecciones`. Base exacta:
`3125a0016e1adcfa0ac68c2c37ba40108a033cc9`. Se conservaron las dependencias
#7 `c25eb7e5008e1201ebd1292da7b2da0dd0a554a0`,
#8 `6b8204ab92196fccfc217c80c6f381aa8ef03535` y #9. No se modifica main,
no se fusionan PR ni se reescriben ramas anteriores. El trabajo local B2 que
no estaba publicado se dejó en su worktree; no se incorporó a esta misión.

## Secuencia verificable

1. Se comprobó la base y se creó `investigacion/p12b3-matematicas` desde ella.
2. **Primero se publicó** `1c9329b33f660490ceb83064e4d70097d0745c22`: contratos,
   oráculos y evidencia inicial, sin modificar producción.
3. **Después se publicó** `00c3a91542521b47476b3de612afd29eca7ff787`: correcciones
   y diez nombres nuevos del lenguaje (nueve en mate y una conversión global).
4. Se añadieron ejemplos documentados automatizados, comprobaciones de locale y
   el informe de cierre. El código de producción sigue siendo el del punto 3. La revisión completa de
   referencia es `239401f3e21eadf51bc1b2da0c899989a1091757`; los commits posteriores
   sólo registran resultados y fijan esa revisión en la guía de Debian.

La [decisión inicial y su revisión](P12B3_DECISIONES.md) distinguen errores de
implementación de contratos heredados. `anumero(0)` y `anumero("0")` **siguen
produciendo nulo**: la suite alpha ya lo especificaba. Se rechazó la propuesta
inicial de cambiarlo y se añadió `anumero_estricto`. Los tres tests nuevos que
proponían aquel cambio ahora se aplican a la alternativa estricta; su primera
versión y los fallos iniciales permanecen en Git. Ninguna de las 180 pruebas
anteriores cambia. No se oculta este ajuste como si fuera una corrección de
la función heredada.

## Defectos reproducidos y correcciones

| Caso y oráculo | Antes | Causa | Después y no regresión |
|---|---|---|---|
| max(-1e16,-2e16)=-1e16; min(1e16,2e16)=1e16 | ±999999999999999 | Centinelas que no representan todo double | Selección desde los argumentos; prueba CLI en ambas extensiones y C con DBL_MAX/inf/NaN/ceros |
| Sólo NaN en max/min | Centinela artificial | Acumulador sin un valor válido | NaN; NaN mezclado con números se sigue omitiendo; primer empate preserva signo del cero |
| aleatorio(-1) | División entera por cero | max+1=0, usado como divisor | Error de rango; negativos siguen permitidos como dos extremos |
| aleatorio/alt con inf, NaN o fuera de int | Cast indefinido; UBSan lo detecta | Conversión antes de validar | Finitud y truncamiento/rango comprobados; error recuperable |
| Extremos aleatorios cercanos a INT_MIN/INT_MAX | Riesgo de desbordamiento al intercambiar, sumar 1 o restar | Aritmética int y swap por sumas/restas | Diferencias int64 y swap simple; se conserva rand y módulo; prueba de extremos y secuencia con fuente inyectada |
| frexp(8,inf) debe ignorar el segundo valor numérico | Cast indefinido aunque el argumento se descarta | Conversión innecesaria a int | Verifica tipo y descarta valor; sigue devolviendo sólo 0.5 |
| ldexp(1,inf) y exponente fuera de int | Cast indefinido | Exponente convertido antes de validación | Error; exponentes fraccionarios finitos válidos siguen truncándose hacia cero |
| Llamada aleatorio con tres argumentos, prefijo 777 | Argumentos residuales en pila | Error de aridad antes de consumir marco | Consume el marco antes de validar; pruebas de error→éxito y preservación del prefijo |
| latC_aint/latC_achar con cadena numérica no representable | Conversión float→entero indefinida | Ausencia de límites | Error antes del cast; valores válidos conservan truncamiento |
| Mil repeticiones de operaciones escalares/extremos | Memoria VM 78 448→1 505 496 bytes | Descriptores numéricos y clones retenidos hasta cerrar la VM | Resultado escalar en pila; max/min sin lista temporal; memoria estable tras calentamiento |

La retención temporal anterior no se presenta como fuga después de destruir la
VM. Las estructuras regionales previas siguen controlando propiedad y ciclo de
vida. En el primer ensayo instrumentado, el test nuevo `azar_c` también dejaba
su segunda VM viva al fallar por API ausente: ese informe LSan corresponde al
**arnés de prueba**, no demuestra una fuga del generador de Core. La segunda VM
se crea ahora después de las aserciones iniciales y se cierra antes de comprobar
su resultado. Se conservan los logs iniciales y esta explicación.

## API y compatibilidad

`mate.descomponer_binario` devuelve los dos componentes de frexp;
`mate.aleatorio_entero` y `mate.semilla_aleatoria` ofrecen una secuencia por VM,
con reducción por rechazo sin sesgo de módulo; tres predicados clasifican finitud,
infinito y NaN; `raiz_segura`, `log_seguro`, `potencia_segura` comprueban sus
contratos; `anumero_estricto` distingue número válido (incluido cero) y error.

Se conservan los **39 nombres mate y todos sus alias**, además de anumero y sus
conversiones laxas. No cambia la gramática, la precedencia, los ámbitos ni
latC_adouble. `porc` sigue siendo una razón; redondear conserva mitades alejadas
del cero; las funciones libm históricas conservan NaN e infinitos. Los cambios
incompatibles acotados (límite aleatorio único negativo, all-NaN en extremos,
conversiones C antes indefinidas) están descritos antes de la implementación.

Los 27 programas de consola B3 —26 contratos iniciales más los ejemplos de las
50 entradas documentadas— se ejecutan dos veces: **27 .lat y 27 .mict**, con los
mismos oráculos. Se añaden 12 casos C de contratos, uno de rechazo aleatorio,
tres de locale y uno de sincronía documental: **71 pruebas B3** en total.
Las pruebas C cubren subnormales, no finitos, signo del cero, reconstrucción,
recuperación, independencia entre VM y memoria; no dependen del texto formateado.

La comprobación ABI compara todos los símbolos dinámicos definidos con la lista
histórica completa: **285 preservados, ninguno nuevo ni perdido**. Las funciones
nuevas de lenguaje son estáticas; los dos ayudantes de estado tienen visibilidad
oculta. Se mantienen todas las cabeceras públicas `include/` sin cambios. El
estado del PRNG añade ocho bytes a una representación privada ya opaca; no cambia
el tamaño público de lat_mv. Se conservan las comprobaciones ABI normales e
instrumentadas de B2; las guardias ODR de ASan se identifican separadamente.

## Resultados y clasificación

Se ejecutaron pruebas nuevas durante B3; no son sólo una consulta de resultados
anteriores. Entorno local Debian 13 x86_64, GCC 14.2.0, glibc 2.41, CMake 4.4.5,
Python 3.12.14. CI Ubuntu 24.04, GCC 13.3.0; versiones/configuración visibles en
los logs enlazados. Sanitizadores activos: address, undefined y adicionalmente
float-cast-overflow; LSan integrado con detect_leaks=1 y salida 23 en caso de error.

| Etapa | Perfil | Aprobadas | Fallidas | Omitidas/bloqueadas | Interpretación |
|---|---|---:|---:|---:|---|
| Base sin tests nuevos, local | Debug | 180 | 0 | 0 | Referencia intacta |
| Primer commit, CI | Debug | 190 | 53 | 0 | Incluye API todavía ausente y propuesta de cero después revisada |
| Primer commit, CI | Release | 190 | 53 | 0 | Evidencia previa, no entrega aprobada |
| Primer commit, CI | ASan/UBSan/LSan | 205 | 54 | 0 | Defectos funcionales y diagnósticos instrumentados antes de corregir |
| Implementación 00c3a915, local y CI | Debug | 245 | 0 | 0 | 180 anteriores +65 B3 de esa etapa |
| Implementación 00c3a915, local y CI | Release | 245 | 0 | 0 | Resultado comprobado |
| Implementación 00c3a915, CI | ASan/UBSan/LSan | 261 | 0 | 0 | 196 anteriores +65 B3 de esa etapa |
| Implementación 00c3a915, local | ASan/UBSan/LSan | 3 auxiliares | 0 clasificadas como funcionales | 258 bloqueadas | CTest las marca fallidas; todos los bloques muestran LSan/ptrace. Las tres aprobadas son dos generadores Python y nm, no ejecución instrumentada de Core |
| Con ejemplos y locales, local | Debug | 251 | 0 | 0 | 180+71; locales instalados, sin omitidas |
| Con ejemplos y locales, local | Release | 251 | 0 | 0 | 180+71 |
| Revisión completa 239401f3, CI | Debug | 251 | 0 | 0 | 180+71, sin omitidas |
| Revisión completa 239401f3, CI | Release | 251 | 0 | 0 | 180+71, sin omitidas |
| Revisión completa 239401f3, CI | ASan/UBSan/LSan | 267 | 0 | 0 | 196+71, sin omitidas ni detectores desactivados |

CI inicial: [43641329286](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43641329286).
CI de implementación: [43641793526](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43641793526),
con [instrumentación](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43641793526/job/132590763466).
CI completa: [43642854265](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43642854265),
con [Debug](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43642854265/job/132594027016),
[Release](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43642854265/job/132594026904) e
[instrumentación](https://github.com/TuxMatic99/Mictlan-Core/actions/runs/43642854265/job/132594027123).
Se consultaron los logs de cada job y su recuento exacto, no sólo el indicador verde.
Los logs/extractos locales y de CI se conservan en [evidencias](evidencias/p12b3/README.md).
El límite ptrace no se elude desactivando detectores; la aprobación instrumentada
proviene de GitHub, donde la suite completa sí se ejecuta.

Hubo un error de configuración al compilar manualmente el benchmark de base sin
la ruta de cabeceras readline del workspace; se corrigió añadiendo la ruta y se
repitió. No fue un fallo de Core ni se contó el intento como medición válida.
Las advertencias heredadas de compilación continúan en los logs; no se afirman
compilaciones libres de avisos ni se modifica código ajeno para suprimirlos.

## Rendimiento reproducible

Se midió max con tres procesos independientes por tamaño, después de una llamada
de calentamiento. Baseline Release recompilada desde **3125a001**, resultado
Release con código **00c3a915**; mismo GCC 14.2 y workspace. El control numérico
se comprueba en cada iteración. Archivos CSV contienen cada tiempo, memoria VM,
RSS máximo y suma de control; no se compara sólo una ejecución.

| Argumentos / repeticiones | Mediana antes (s) | Mediana después (s) | Crecimiento VM antes (bytes) | Después |
|---|---:|---:|---:|---:|
| 2 / 10 000 | 0.006086270 | 0.000868014 | 4 582 120 | 0 |
| 16 / 3 000 | 0.011225076 | 0.001010267 | 7 868 264 | 0 |
| 512 / 100 | 0.009201197 | 0.000854423 | 7 907 368 | 0 |

Ambos algoritmos siguen siendo O(n). La mejora elimina clones, nodos y resultados
escalares temporales, con espacio auxiliar O(1). Los tiempos son cortos y sufren
ruido del entorno compartido; muestran mejora en estas cargas, no una promesa de
aceleración universal. RSS incluye intérprete, libc y memoria ya residente; no se
confunde con memoria administrada. La VM nueva tiene coste inicial adicional de
registrar diez funciones, además de los ocho bytes de estado; su memoria estable
no significa que toda llamada que devuelve una lista sea gratuita.

## Documentación y revisión independiente

- [Inventario completo](P12B3_INVENTARIO.md): 50 entradas del lenguaje y ayudantes C.
- [Referencia educativa](P12B3_REFERENCIA.md): firma, dominio, ejemplo, resultado y límites por nombre.
- [Precisión y límites](P12B3_PRECISION.md): binary64, tolerancias, PRNG y sesgo.
- [Validación en Debian 13](DEBIAN_P12B3.md): revisión fija, tres perfiles, ABI y mediciones.

Pendientes deliberados: tipos enteros C y conversión global, aritmética decimal o
arbitraria, aleatoriedad criptográfica, concurrencia de locale/rand y una misma
VM, serialización decimal reversible, pruebas Windows/macOS. No se promete que
las funciones libm heredadas detecten todos los errores mediante excepciones.
