# Cadenas internadas y bytecode en P1.1

`latG_habilitar` conserva la duración histórica de las cadenas. Si el anfitrión puede cumplir el contrato ampliado, llama a `latG_habilitar_cadenas` después de habilitar G y antes de compilar o ejecutar. La elección es por VM. El ejecutor de consola conserva su modo de memoria anterior.

El recolector añade las cadenas al mismo registro de direcciones. Traza los valores de la pila activa, los contextos, los módulos y las entradas de listas y diccionarios. Conserva las cadenas de constantes y etiquetas junto a sus descriptores y las rutas de origen usadas por todas las funciones compiladas. Los nombres usados como claves se copian en las entradas del diccionario. La caché de nombres se invalida antes del barrido.

Un consumidor C que guarda un texto de la VM más allá de un punto seguro conserva el objeto con `latG_retener`, fija la dirección de `getCadena(objeto)` o copia los bytes en memoria propia. Para una celda de pila, fija el contenido antes de volver a usar la celda. `latG_soltar` cancela una fijación. Una fijación protege contra el recolector durante la ejecución. El cierre de la VM invalida todas las referencias a ella.

Cada función nativa autorizada es síncrona y cumple este contrato, incluidos los retornos por error. Una llamada nativa sin autorizar, un recurso externo o una referencia de origen desconocido suspenden la recolección según el contrato G. La compilación conserva el bytecode y las constantes hasta cerrar. La compactación ajusta la reserva de cada función al tamaño usado antes de publicarla. Ante un fallo de compactación se conserva el bloque original. La compilación sigue usando la capacidad temporal de Alpha.

Los layouts y símbolos existentes conservan su ABI. El único símbolo público nuevo es `latG_habilitar_cadenas`. En sistemas sin separación entre VM y código nativo, esta API sigue requiriendo consumidores que respeten el contrato de propiedad.
