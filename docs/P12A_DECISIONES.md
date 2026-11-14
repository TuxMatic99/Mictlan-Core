# P1.2-A: decisiones semánticas antes de corregir

Base comprobada: `9f3a1965f2794c8921adf9cdfa3e0c192060c657`.
Rama nueva: `investigacion/p12a-conformidad`. Las ramas P1.1 y P1.2
apuntaban a esa misma revisión al iniciar. `main` apuntaba a
`b3be302bbfdc0dfc371b6871dd203f7ff9ce3a0b` y no se modifica.

## Evidencia anterior al parche

Debian 13, GCC 14.2.0, Debug: la suite existente pasa **42/42**.
La suite nueva, con resultados fijados antes del arreglo, pasa **21/25**.
Fallan `comparaciones` en ambas extensiones y la API de decimales con
`LC_NUMERIC=es_ES.UTF-8` y `fr_FR.UTF-8`. Ninguna prueba se invierte
para aprobar un defecto.

- `5.25`, `verdadero || verdadero` y `fibonacci(12)` ya dan `5.25`,
  `verdadero` y `144` en la consola. El texto del manual sobre OR no
  justifica convertirlo en XOR.
- `2 <= "3"` da falso mientras `2 < "3"` da verdadero. En `menor_igual`,
  cuando al menos un operando es numérico, se lee el miembro `double`
  de la unión de ambos valores, incluso si uno guarda una cadena,
  booleano o colección. Se interpreta su representación, no su número.
- El escáner convierte el token decimal con `strtod` del entorno C.
  En una aplicación anfitriona con coma decimal, `5.25` acaba siendo 5,
  aunque el token completo fue reconocido. La consola fija el entorno
  numérico C y por eso oculta ese caso.

## Alternativas y decisión

| Área | Alternativas consideradas | Decisión P1.2-A |
| --- | --- | --- |
| `<=` mixto | Mantener bits accidentales; rechazar tipos distintos; aplicar la conversión numérica que usan `<`, `>` y `>=` | Usar `latC_adouble` para ambos operandos cuando alguno sea numérico. Los programas con dos números conservan su resultado. |
| Literales decimales | Depender del idioma del anfitrión; cambiar todo su entorno; convertir el token mediante la rutina C existente de Core | Usar `latC_strtod_c`, que restaura `LC_NUMERIC` al volver. El punto sigue siendo el separador del lenguaje. |
| Potencias | Asociar hacia la derecha; igualar `^` y `**`; mantener la gramática | Conservar asociación izquierda y mayor precedencia de `**`. Usar paréntesis y especificar las diferencias. |
| Lógicos | Cortocircuito; devolver siempre booleanos; separar precedencia de AND y OR; conservar | Conservar evaluación de ambos operandos, selección de un valor y precedencia compartida. |
| Ámbitos | Adoptar ámbitos léxicos; aislar toda asignación local; conservar marcos clonados | Conservar las reglas heredadas y advertir sobre parámetros reasignados que coinciden con globales y copias de marcos anteriores. |
| Igualdad mixta | Simetrizar o rechazar coerciones; conservar | Conservar dependencia del tipo izquierdo: `2 == "02"` es verdadero, `"02" == 2` es falso. Recomendar comparar tipos iguales. |

Las dos correcciones afectan entradas previamente incoherentes. No se promete
conservar resultados basados en la representación de un puntero como `double`
ni literales truncados por un idioma del proceso. Son cambios documentados;
las demás opciones requieren una propuesta futura de compatibilidad.

La rutina heredada `latC_strtod_c` modifica temporalmente el locale global del
proceso. Esta entrega verifica conversión y restauración en ejecución secuencial;
no introduce una garantía de compilación simultánea en hilos con locales distintos.
