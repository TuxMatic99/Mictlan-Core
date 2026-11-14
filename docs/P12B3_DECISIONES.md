# P1.2-B3: decisiones anteriores a la implementación

Base exacta: `3125a0016e1adcfa0ac68c2c37ba40108a033cc9`. PR #7
`c25eb7e5008e1201ebd1292da7b2da0dd0a554a0`, #8
`6b8204ab92196fccfc217c80c6f381aa8ef03535` y #9 conservadas, abiertas en borrador.
Rama: `investigacion/p12b3-matematicas`, creada desde esa base. No se modifica main.

El [manual histórico](https://manual.lenguajelatino.org/es/stable/librerias/Matematica.html)
es una referencia: `frexp` realmente devuelve sólo la fracción y `porc` una razón,
no un porcentaje multiplicado por 100. Se conservan ambos contratos.

| Área | Contrato conservado | Corrección o alternativa acordada antes de editar src |
|---|---|---|
| max/min | uno o más números; NaN se omite cuando hay números; primer empate, incluido signo del cero | retirar centinelas ±999999999999999; infinitos ordenados; sólo NaN produce NaN; recorrido sin listas temporales |
| aleatorio/alt | 0 argumentos: entero 0 o 1; 1: [0,trunc(n)]; 2: extremos truncados y ordenados, inclusivos; generador C rand | validar finitud e intervalo de int antes del cast; calcular diferencias en 64 bits; rechazar un límite único negativo; mantener secuencia y sesgo histórico para entradas válidas |
| frexp(x, extra) | dos argumentos numéricos, devuelve sólo fracción | comprobar tipo de extra sin convertirlo a int: su valor se descarta; no cambiar firma |
| ldexp(x,e) | e finito se trunca hacia cero; libm conserva desbordamiento/subnormales | validar exponente truncado en [INT_MIN,INT_MAX] antes del cast |
| anumero | conversiones laxas de cadenas inválidas al primer byte; false/nulo/colecciones vacías a nulo | corregir únicamente cero numérico o cadena numérica completa no vacía: devolver número, preservando -0; no modificar latC_adouble ni conversiones implícitas |
| ayudantes C | contratos existentes de latC_aint/latC_achar y truncamiento de cadenas numéricas válidas | controlar NaN, infinitos y rango de int/char antes de convertir cadenas; no rediseñar etiquetas T_INTEGER/T_NUMERIC ni conversiones globales |
| libm heredada | dominios, NaN, infinitos, cero negativo, round con mitades alejadas de cero | eliminar reservas de descriptores para resultados escalares; no modificar fórmulas ni orden de operaciones |

Efectos de compatibilidad explícitos: `anumero(0)`/`anumero("0")` dejan de ser nulo;
un único límite negativo de aleatorio pasa a error (antes -1 dividía por cero y
otros negativos daban positivos fuera del intervalo pretendido); all-NaN de max/min
pasa de centinela artificial a NaN. Las conversiones fuera del rango de C no tenían
un resultado portable definido y pasan a error recuperable. Dos límites negativos
siguen admitidos. No se adopta silenciosamente una nueva política general de tipos.

## API nueva y separada

- `mate.descomponer_binario(x)` requiere finito, devuelve `[fracción, exponente]`;
  para ±0, conserva el signo y exponente 0. Reconstrucción exacta con ldexp en base 2.
- `mate.es_finito(x)`, `mate.es_infinito(x)`, `mate.es_nan(x)` exigen número y devuelven lógico.
- `mate.aleatorio_entero(min,max)`: enteros exactos en ±(2^53-1), ordenados, intervalo
  inclusivo. SplitMix64 y reducción por rechazo evitan sesgo de módulo. Estado por
  VM de 64 bits, semilla inicial 0; `mate.semilla_aleatoria(s)` admite enteros
  [0,2^53-1], devuelve nulo y sólo afecta este generador. Sin dependencias nuevas,
  estado en la representación privada ya opaca, sin cambiar cabeceras públicas.
  Algoritmo de Sebastiano Vigna, CC0, con atribución en fuente. Destinado a simulación
  reproducible, sin garantías criptográficas. El rand histórico sigue separado.
- `anumero_estricto(v)`: número finito o cadena decimal ASCII, espacios ASCII
  exteriores opcionales, signo, punto y exponente decimal. Requiere al menos un
  dígito; rechaza hexadecimal, coma, basura, NaN e infinitos. Devuelve número
  (también ±0) o error distinguible. Subnormales representables aceptados;
  mantisa no cero que se redondea a cero y desbordamiento producen error de rango.
- `mate.raiz_segura(x)`, `mate.log_seguro(x)`, `mate.potencia_segura(x,y)`:
  entradas finitas; raíz x≥0, log x>0; potencia base negativa sólo exponente entero,
  base cero sólo exponente positivo (0^0 se rechaza aquí). Resultado finito;
  potencia no nula que se pierde a cero produce error. Subnormales representables
  aceptados. Las funciones históricas continúan con sus resultados libm.

## Oráculos y disciplina

Se publican primero estas decisiones y regresiones; sus errores en la base son
intencionales y se registran sin contabilizarlos como aprobados. Valores esperados
se deducen de orden matemático, IEEE binary64, redondeos especificados, identidades
y vectores independientes de SplitMix64; no se copian salidas del intérprete.
Las identidades aproximadas usan tolerancias explícitas 8–16 DBL_EPSILON para las
entradas pequeñas seleccionadas, no una promesa de error universal de libm.
Pruebas de consola se ejecutan con ambas extensiones; C observa signo del cero,
subnormales, NaN, pila y memoria administrada sin depender del formateo decimal.

Pendientes deliberados: rediseñar el conjunto de etiquetas numéricas C, decimal
exacto, números arbitrarios, biblioteca aleatoria criptográfica, concurrencia de
locale/rand heredados y verificación Windows/macOS. No se cambia parser ni VM global.
