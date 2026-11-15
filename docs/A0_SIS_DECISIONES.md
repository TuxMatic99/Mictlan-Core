# A0 — contratos de sis y decisiones antes del parche

Base: `2651317e4224f5499765b5b0ae16d1404a0adfc4` (B4-B, PR #12).
Esta entrega inventaría y prueba; aún no modifica producción. Los resultados
esperados proceden de los contratos siguientes y de calendarios/bytes literales,
no de copiar salidas del intérprete.

## Inventario público de sis

| Nombre y firma | Resultado y contrato heredado | Dependencias / errores / propiedad | Decisión A0 |
|---|---|---|---|
| `dormir(segundos)` | Sin retorno utilizable. Segundos numéricos convertidos a milisegundos truncados. | sleep_ms(int) en Unix; Sleep en Windows; NaN/rango se convierten sin validar; nanosleep pierde el resto tras EINTR. | Validar finitud, signo y rango antes de convertir. Conservar truncamiento a ms. Reintentar resto tras EINTR, propagar fallo operativo. |
| `ejecutar(comando)` | Cadena, sin retorno; imprime lo que imprima el hijo. Estado no cero del hijo históricamente ignorado. | system; no recursos propios; fallo -1 ignorado. | Conservar estado del hijo ignorado; distinguir fallo operativo -1 mediante error. Prueba sustituye backend o usa productor propio. |
| `pipe(comando)` | Cadena de stdout; quita exactamente un LF final y el CR precedente. Vacío → cadena vacía. No devuelve estado del hijo. | popen/pclose, buffer latM de MAX_STR_LENGTH=1 MiB, lectura máxima 1 MiB−1. Cierre -1 y ferror ya producen error. | Rechazar salida >1 MiB−1 y NUL, sin entregar prefijo. Consumir salida restante finita antes de pclose; reintentar EINTR; reservar antes de abrir y liberar ante error de publicación. Conservar salida del hijo con estado no cero. |
| `fecha(formato)` | Selector cadena: seg, min, hora, d_mes, mes, sem, año, d_sem, d_año, horario_de_verano, iso. | time/localtime/gmtime/strftime, zona local salvo iso UTC. Punteros y strftime sin comprobación. | Comprobar reloj, conversión y formato. Conservar valores e índices: domingo=1, mes enero=1, d_año=1 desde 1 enero, semana ISO %V. |
| `tiempo(formato, instante)` | Formato nulo → segundos actuales, segundo argumento ignorado. Con cadena: instante nulo → ahora; número → segundos desde época, truncamiento a time_t. strftime en 64 bytes; resultado vacío/no cabe → nulo. | Reloj, time_t, zona local y LC_TIME. Cast sin validar y localtime_r sin comprobar. | Validar finitud y rango antes del cast, comprobar reloj sólo cuando se necesita y conversión a calendario. Mantener el límite 64 y nulo histórico; no añadir crecimiento silencioso. |
| `salir()` | Termina el proceso con estado 0. | exit; la VM deja de ejecutar. | Sin cambio. Prueba C sustituye exit; CLI sintético termina sólo su propio proceso. |
| `avisar(numero)` | Registra señal con número truncado; devuelve falso si no se observó señal, en otro caso último número global. | signal y variable global senial; no restablece automáticamente ese valor. Conversiones inválidas y SIG_ERR ignorado. | Validar conversión y SIG_ERR; mantener política global/sticky y truncamiento. Pruebas no envían señales a otros procesos. |
| `cwd()` | Cadena del directorio de trabajo, nulo si no cabe/falla getcwd. | Buffer 1024 local. | Conservar; sólo directorio temporal propio en casos nuevos. |
| `iraxy(primero, segundo)` | Sin retorno. Unix imprime ESC[primero;segundof tras truncar; orden heredado conservado. | Terminal/Windows COORD; casts numéricos antes de rango. | Validar int antes del cast; no reinterpretar ni intercambiar coordenadas. Windows y su rango COORD requieren certificación aparte. |
| `usuario()` | USER en Unix, username en Windows; cadena o nulo. | getenv; no autentica identidad. | Conservar; la batería fija/retira un valor sintético en el proceso hijo. |
| `operativo(nombre)` | Booleano: igualdad exacta con constante de compilación. | Plataforma, cadena; mayúsculas importan. | Conservar; no detección de distribución. |
| `os(nombre)` | Alias exacto de operativo. | Mismo puntero nativo. | Conservar. |
| `os_version_mayor()` | Número mayor obtenido del sistema. | buscar_os_version, uname/Windows/macOS. | Linux: analizar segmentos decimales sin avanzar fuera del texto; cero si falla proveedor. |
| `os_version_menor()` | Segmento menor. | Igual. | Conservar partes válidas y cero en partes ausentes. |
| `os_version_parche()` | Segmento parche. | Igual. | Sólo un tercer segmento separado por punto; un sufijo `6.8-12` no es parche 12. |

Ayudantes C públicos conservados: `buscar_os_version`, `sleep_ms`, `latSO_tiempo`,
`latOS_veriones`, `senial` y apertura de biblioteca. No se cambian firmas,
estructuras públicas ni símbolos. Para el ayudante void sleep_ms, un argumento
negativo debe retornar sin dormir y dejar EINVAL; no tiene canal de error VM.
Los selectores C de versión inválidos siguen devolviendo nulo.

## Semántica y límites de las correcciones

Los valores válidos anteriores conservan resultado, tipo y truncamiento. Se
convierten fallos operativos o casts indefinidos en errores recuperables. La
excepción explícita es el prefijo silencioso de `pipe`: una salida que exceda su
capacidad deja de presentarse como completa. No se elimina su límite ni se crea
una segunda API de procesos. El estado no cero del hijo sigue siendo distinto de
un fallo de lectura/cierre: puede haber salida útil pese a ese estado.

El vaciado de una tubería evita cerrar esperando un productor finito bloqueado
por falta de lectura, pero **no introduce timeout** ni detiene productores
infinitos. Una API futura podría devolver stdout y estado por separado; aquí se
documenta la propuesta sin cambiar la firma. No reintentar pclose después de -1
sobre un FILE ya cerrado. No se promete recuperación de todos los recursos del
sistema si su implementación no permite obtener el estado del hijo.

Las fechas dependen de la zona local y LC_TIME; ISO usa UTC. No se cambia la zona
ni el locale del equipo. Las pruebas fijan TZ=UTC0 sólo en su proceso y emplean
un reloj sintético (2024-02-29 23:59:59 UTC) en contratos C. El calendario puede
no representar todo time_t aunque el cast sea válido: ese fallo también debe
producir error. El instante explícito -1 es válido; no confundirlo con el resultado
-1 de una llamada fallida a time().

En Linux se conserva el rango de milisegundos que admitía sleep_ms(int). Se valida
antes de convertir el producto segundos×1000. El tratamiento portátil de Windows
conserva su representación DWORD y queda separado de la validación Linux; no se
promete el mismo máximo en todas las plataformas. La pausa no es un temporizador
exacto: planificación e interrupciones pueden prolongarla.

`buscar_os_version` carece de estado de error explícito. El parche Linux devuelve
componentes cero ante fallo y conserva errno del proveedor; no añade un símbolo
nuevo. La rama macOS contiene problemas estáticos (fallthrough, lectura nula) y
la Cygwin está incompleta: se registran para una auditoría de plataforma, sin
presentar la matriz Linux como prueba de esas ramas.

## Pruebas y fuente de los oráculos

Se comprueban valores, tipos, retorno/error, activación de fallos, reservas y
flujos abiertos/cerrados. Un centinela 777 y una ejecución válida posterior en
la misma VM verifican pila/contextos/contador/manejador. Los fallos no se invierten
para obtener aprobaciones. Antes de destruir la VM, el arnés registra recursos
pendientes; su limpieza auxiliar no se cuenta como limpieza de producción.

Las pruebas C sustituyen time, conversiones de calendario, strftime, uname,
nanosleep, signal, system, exit y FILE de popen por proveedores controlados.
Las pruebas CLI compilan su propio `productor.c`: sólo escribe bytes fijos y
`productor.fin` en el directorio temporal. No interpreta órdenes ni descarga o
carga software desconocido. La prueba de exceso exige que el productor termine
y comprueba el disco además del error. Se reutilizan envoltorio y sonda de B4-A.

Referencias primarias de contratos de libc consultadas (no especificación del
lenguaje): [nanosleep](https://www.man7.org/linux/man-pages/man2/nanosleep.2.html),
[popen/pclose](https://www.man7.org/linux/man-pages/man3/popen.3.html),
[strftime](https://www.man7.org/linux/man-pages/man3/strftime.3.html),
[localtime/gmtime](https://www.man7.org/linux/man-pages/man3/localtime.3.html).
El retorno no cero de pclose es estado del hijo; -1 indica fallo. nanosleep puede
entregar el tiempo restante al interrumpirse. Las funciones de calendario pueden
fallar, y strftime devuelve cero cuando no puede entregar el resultado solicitado.
