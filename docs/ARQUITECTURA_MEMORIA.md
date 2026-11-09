# Mictlan Core: contrato de propiedad de memoria

Estado: propuesta de arquitectura, no implementada.
Rama: investigacion/propiedad-memoria.

## 1. Problema comprobado

El modelo actual permite que varios contenedores
compartan punteros y que sus destructores intenten
liberar los mismos objetos.

Casos reproducidos con AddressSanitizer:
- Clonacion numerica: mitigada experimentalmente.
- Listas anidadas: use-after-free pendiente.
- Elementos repetidos: use-after-free pendiente.

## 2. Reglas que debe cumplir el nuevo modelo

1. Compartir un recurso no debe destruir su identidad.
2. Ninguna referencia valida debe apuntar a memoria liberada.
3. Cada reserva debe tener una politica de liberacion.
4. Las referencias prestadas no transfieren propiedad.
5. Las referencias propietarias deben poder liberarse.
6. Las estructuras ciclicas necesitan tratamiento especial.
7. Las operaciones FFI deben respetar el mismo contrato.

## 3. Componentes pendientes de integrar

- lat_objeto: identidad y envoltorios.
- lista: contenedores y elementos.
- hash_map: contextos y diccionarios.
- pila de la VM: copias por valor.
- funciones y cadenas: recursos compartidos.
- SDK/FFI: intercambio de objetos con codigo C.

## 4. Decisiones abiertas

- Identidad del recurso y lugar de su contador.
- Semantica de clonacion frente a alias.
- Adquisicion y transferencia de referencias.
- Ciclo de vida de temporales en la pila.
- Recuperacion de estructuras ciclicas.
- Compatibilidad con extensiones y ABI.

No activar HABILITAR_GC hasta resolver
estas decisiones y validar sus invariantes.

## 5. Criterios de aceptacion

- Diagnostico numerico: ASan y LSan sin errores.
- Diagnostico anidado: ASan y LSan sin errores.
- Elementos compartidos: ASan y LSan sin errores.
- Regresiones existentes de Core: aprobadas.
- Suite funcional del SDK: 32/32.
- Referencias y mutaciones: semantica preservada.
