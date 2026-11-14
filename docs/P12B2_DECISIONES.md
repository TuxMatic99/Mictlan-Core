# P1.2-B2: contratos antes de las correcciones

Base exacta: `6b8204ab92196fccfc217c80c6f381aa8ef03535` (P1.2-B1).
Las PR #7 y #8 estaban abiertas y en borrador al iniciar. Esta rama depende de B1.

Los oráculos de `tests/colecciones` se escribieron antes de modificar producción:
se cuentan claves explícitas y entradas físicas, se usan colisiones FNV-1a reales,
se comparan decimales matemáticamente distintos y se compone el texto esperado
independientemente del intérprete. La ejecución inicial tiene 12 aprobadas y
31 fallidas de 43; se conserva en `evidencias/p12b2/antes-debug.log`. La base
previa conserva sus 128 pruebas aprobadas. ASan demuestra un desbordamiento al
unir tres cadenas con un separador largo: `antes-union-asan.log`.

## Alternativas y decisiones

* Eliminar: desconectar una cubeta pierde claves ajenas y deja mal la longitud.
  Se desvinculará solamente el nodo encontrado y se descontará una entrada.
  Sus valores siguen bajo la propiedad de la VM: `dic.valores` puede mantener
  referencias a sus celdas. Destruir el valor sería incompatible y peligroso.
* Igualdad: `indice`/`eliminar` usan igualdad por tipo, mientras que `contiene`
  emplea el comparador histórico, con conversiones textuales entre tipos.
  Unificarlos cambiaría programas válidos; se conserva la diferencia. Dentro
  de las comparaciones de listas, convertir la resta de dos números a `int`
  confunde 0.1 y 0.9 y puede desbordar. Se compararán números sin esa conversión,
  sin modificar el comparador escalar global. NaN no tendrá un orden inventado:
  se producirá un error controlado. La ordenación seguirá mirando primero la
  longitud, antes de los elementos; no se introduce orden lexicográfico nuevo.
* Ciclos: no existe igualdad coinductiva especificada. La recursión infinita al
  comparar listas cíclicas se sustituirá por un error controlado. Invertir y
  concatenar conservarán sus referencias superficiales, incluso con ciclos.
* Índices: las fracciones finitas se truncan hacia cero, como en Latino.
  Se rechazan números no finitos o fuera del rango de `int` antes de convertir.
  No se introducen índices negativos. `crear` con cantidad finita negativa
  seguirá devolviendo una lista vacía.
* Unión: `lista.separar` une cadenas y números; omite otros tipos, decide el
  separador por la posición original y devuelve `nulo` cuando el texto queda
  vacío. Cambiar estas peculiaridades sería incompatible. `lista.unir` será
  un alias explícito con el mismo contrato. La reserva incluirá todos los bytes
  de los separadores y se comprobará antes de escribir.
* Copias: asignar comparte contenedor; invertir, extender y concatenar comparten
  elementos. El clonado heredado de listas copia números y listas anidadas,
  pero comparte otros elementos. Clonar diccionarios es superficial. No se
  promete una copia profunda universal ni orden de inserción de diccionarios.

No se cambiarán la gramática, las estructuras públicas ni las políticas de
propiedad de P0.2-E/F/G. Los ayudantes nuevos serán privados a Core.
