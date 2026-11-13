# Propuestas posteriores para el lenguaje

**13 de noviembre de 2026.** Documento de diseño. La gramática Alpha y su ABI son las referencias para comparar cada propuesta.

| Tema | Propuesta para experimentar | Evidencia antes de adoptar |
| --- | --- | --- |
| Tipado gradual | Anotaciones optativas y un modo de comprobación que conviva con valores dinámicos. Prototipar comprobaciones en fronteras FFI y descripciones de firmas antes de fijar sintaxis. | Compatibilidad de archivos sin anotaciones, conversiones explícitas, errores localizados y coste medido al cruzar C. |
| Programación funcional | Especificar funciones como valores y entornos de captura antes de incorporar clausuras. Explorar combinadores de listas como biblioteca. | Alias, mutación, captura después de retornar, ciclos de clausura y consumo en una VM prolongada. |
| Identificadores Unicode | Estudiar XID_Start y XID_Continue de UAX 31, normalización y avisos de confusables. Mantener las palabras reservadas actuales en el perfil español. | Archivos ASCII intactos, igualdad y diagnósticos por carácter, límites en bytes, pruebas editor y consola. |
| Esperanto e interlingua | Diseñar perfiles léxicos explícitos por archivo o proyecto. Un perfil compartiría la gramática, los tipos y el formato interno. | Selección estable de perfil, errores inequívocos, interoperabilidad de módulos y pruebas de ida y vuelta. |

Primero se compararán propuestas con el corpus Latino, las dos extensiones, los símbolos y los consumidores C. Cada experimento tendrá rama, contrato y pruebas propios. [Identificadores Unicode](https://www.unicode.org/reports/tr31/).
