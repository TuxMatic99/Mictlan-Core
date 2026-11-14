# Archivos en Mictlán: referencia y ejemplos

Un archivo guarda bytes. El texto UTF-8 representa una letra con uno o varios
bytes: «ñ» ocupa dos bytes, aunque se lea como una letra. Estas funciones no
normalizan Unicode ni cambian la codificación; conservan sus bytes. Leer texto
rechaza NUL y tamaños superiores a 8 MiB; copiar admite binarios completos.

Todas las rutas y contenidos de estas firmas son cadenas; los errores de tipo
son errores de programación. Las rutas relativas parten del directorio de trabajo.
Los ejemplos siguientes sólo deben ejecutarse en un directorio temporal propio:
los nombres son sintéticos y algunas operaciones reemplazan o eliminan archivos.
La batería crea, comprueba y borra ese directorio automáticamente.

Escritura directa (`escribir`, `anexar`, `intentar_escribir`) puede quedar parcial
si falla. Un reemplazo atómico prepara otra hoja completa antes de cambiarla
por la anterior. Esa sustitución tiene un punto de confirmación: una advertencia
posterior no devuelve la hoja antigua. `duplicar` reutiliza ese mismo mecanismo.
Si el sistema impide borrar un temporal al fallar, puede quedar un archivo
`.latino-*`; no se promete recuperación tras corte de alimentación o un escritor
concurrente. Los datos ajenos a la operación no deben tocarse.

La compatibilidad completa y sus cambios se registran en
[P12B4A_DECISIONES.md](P12B4A_DECISIONES.md). No se añaden nombres de lenguaje ni
símbolos C: se preservan 14 nombres, incluidos dos alias.

## leer

`archivo.leer(ruta)`

Lee el archivo entero como una cadena.

Resultado: Cadena; falso ante ausencia, error de E/S, NUL o exceso de tamaño.

Límites y compatibilidad: Ruta debe ser cadena. Vacío devuelve "". Máximo 8 MiB. No valida UTF-8. Puede seguir enlaces de lectura. Para distinguir ausencia, usar leer_texto_seguro.

Coste aproximado: Tiempo O(n), memoria O(n).

```mict
archivo.escribir("nota", "María ñ")
poner(archivo.leer("nota"))
poner(archivo.leer("ausente"))
```

Salida esperada:

```text
María ñ
falso
```

## intentar_leer

`archivo.intentar_leer(ruta)`

Es el alias histórico de leer: llama a la misma función.

Resultado: Cadena o falso; no distingue inexistente de error.

Límites y compatibilidad: Archivo vacío produce ""; mantiene el límite 8 MiB y rechazo de NUL. Un error de tipo sigue siendo error de programación.

Coste aproximado: O(n) tiempo y memoria.

```mict
archivo.escribir("nota", "")
poner(tipo(archivo.intentar_leer("nota")))
poner(archivo.intentar_leer("ausente"))
```

Salida esperada:

```text
cadena
falso
```

## leer_texto_seguro

`archivo.leer_texto_seguro(ruta)`

Distingue una lectura correcta, una ruta ausente y un fallo operativo.

Resultado: Cadena si se leyó todo; nulo si fopen informa ENOENT; falso para los demás fallos.

Límites y compatibilidad: Vacío es "", no nulo. ENOENT incluye un directorio padre inexistente. Máximo 8 MiB, sin NUL; no valida Unicode ni hace segura una ruta controlada por terceros.

Coste aproximado: O(n) tiempo y memoria.

```mict
archivo.escribir("nota", "ñ")
poner(archivo.leer_texto_seguro("nota"))
poner(archivo.leer_texto_seguro("ausente"))
```

Salida esperada:

```text
ñ
nulo
```

## lineas

`archivo.lineas(ruta)`

Devuelve las líneas del archivo, sin los separadores LF.

Resultado: Lista nueva de cadenas; error de ejecución si no puede leer el texto completo.

Límites y compatibilidad: Vacío devuelve []; conserva líneas vacías interiores y CR. Un LF final no crea una línea vacía adicional. Máximo 8 MiB y sin NUL; las cadenas son valores de la VM, sin recurso de archivo abierto.

Coste aproximado: O(n) tiempo y memoria.

```mict
archivo.escribir("nota", "\nñ\n\nfin\r\n")
poner(lista.longitud(archivo.lineas("nota")))
archivo.escribir("vacio", "")
poner(archivo.lineas("vacio"))
```

Salida esperada:

```text
4
[]
```

## escribir

`archivo.escribir(ruta, texto)`

Crea o reemplaza el contenido mediante escritura directa.

Resultado: Sin valor de retorno utilizable; usar como sentencia; error de ejecución ante fallo operativo.

Límites y compatibilidad: Ambos argumentos son cadenas. Texto vacío trunca a cero bytes. Sigue enlaces; el fallo puede dejar contenido parcial o truncado. No tiene el límite de 8 MiB de las lecturas. Preferir escribir_atomico para reemplazar datos que deben conservarse si falla. No anidar la llamada como argumento ni usarla como valor: el contrato nativo no apila un retorno y la VM informa operandos insuficientes. Modernizar esa regla global queda fuera de esta misión.

Coste aproximado: O(n) tiempo; memoria auxiliar O(1), aparte de texto y buffers stdio.

```mict
archivo.escribir("nota", "nuevo")
poner(archivo.leer("nota"))
```

Salida esperada:

```text
nuevo
```

## anexar

`archivo.anexar(ruta, texto)`

Añade texto al final; crea el archivo si no existe.

Resultado: Sin valor de retorno utilizable; usar como sentencia; error si la escritura o cierre falla.

Límites y compatibilidad: Cadenas obligatorias. Texto vacío puede crear un archivo vacío. Escritura directa: ante fallo pueden quedar bytes añadidos. No garantiza transacciones entre escritores concurrentes; sigue enlaces. No anidar la llamada como argumento ni usarla como valor: el contrato nativo no apila un retorno y la VM informa operandos insuficientes. Modernizar esa regla global queda fuera de esta misión.

Coste aproximado: O(n) sobre los bytes añadidos; memoria auxiliar O(1).

```mict
archivo.escribir("nota", "a")
archivo.anexar("nota", "ñ")
poner(archivo.leer("nota"))
```

Salida esperada:

```text
añ
```

## intentar_escribir

`archivo.intentar_escribir(ruta, texto)`

Permite consultar un fallo operativo sin interrumpir el programa.

Resultado: Verdadero sólo tras escritura y cierre correctos; falso ante fallo operativo; tipos incorrectos lanzan error.

Límites y compatibilidad: Texto vacío crea/trunca. No es escritura atómica: falso no significa que el destino conserve sus bytes. Para esa garantía antes del reemplazo usar escribir_atomico.

Coste aproximado: O(n) tiempo y O(1) memoria auxiliar.

```mict
poner(archivo.intentar_escribir("nota", "bien"))
poner(archivo.intentar_escribir("no-existe/nota", "x"))
```

Salida esperada:

```text
verdadero
falso
```

## escribir_atomico

`archivo.escribir_atomico(ruta, texto)`

Prepara un archivo temporal y reemplaza el destino sólo cuando sus bytes se han escrito, sincronizado y cerrado.

Resultado: 1: reemplazo y sincronización confirmados; 0: fallo antes del reemplazo; 2: reemplazado, sincronización/cierre del directorio sin confirmar (advierte); -1: plataforma no implementada.

Límites y compatibilidad: Linux, directorio estable, sistema de archivos local. Cadenas obligatorias, texto hasta 8 MiB; vacío válido. Destino regular o ausente, sin seguir enlace final. Nuevo inode con permisos 0600, sin preservar metadatos. No usar como simple booleano: 2 ya reemplazó el destino. No promete resistencia a cambios concurrentes de rutas.

Coste aproximado: O(n) tiempo, O(longitud de ruta) memoria y O(n) espacio temporal en disco; fsync añade latencia.

```mict
poner(archivo.escribir_atomico("nota", "completo"))
poner(archivo.leer_texto_seguro("nota"))
```

Salida esperada:

```text
1
completo
```

## duplicar

`archivo.duplicar(origen, destino)`

Copia todos los bytes de un archivo regular; también admite contenido binario y archivos vacíos.

Resultado: Sin valor de retorno utilizable; usar como sentencia. Error antes del reemplazo mantiene el destino anterior o su ausencia. Si el directorio no confirma durabilidad tras reemplazar, advierte y conserva la copia.

Límites y compatibilidad: Cadenas obligatorias. Linux; reutiliza escribir_atomico internamente. Rechaza mismo inode, destino simbólico o no regular, origen no regular. El origen simbólico que resuelve a archivo regular se admite. Reemplaza un destino regular con inode nuevo y permisos 0600; otros enlaces duros al destino viejo conservan sus bytes. Sin límite de 8 MiB para copiar. Fuera de Linux falla sin modificar destino. No anidar la llamada como argumento ni usarla como valor: el contrato nativo no apila un retorno y la VM informa operandos insuficientes. Modernizar esa regla global queda fuera de esta misión.

Coste aproximado: O(n) tiempo, bloque de 16 KiB más O(ruta) memoria; O(n) espacio temporal en disco.

```mict
archivo.escribir("origen", "copia ñ")
archivo.escribir("destino", "anterior")
archivo.duplicar("origen", "destino")
poner(archivo.leer("destino"))
```

Salida esperada:

```text
copia ñ
```

## ejecutar

`archivo.ejecutar(ruta)`

Analiza, compila y ejecuta un programa Mictlán o Latino en la VM actual.

Resultado: Sin valor de retorno utilizable; usar como sentencia; errores de lectura, análisis, compilación o ejecución se propagan. Restaura el archivo activo, REPL y ubicación del llamador.

Límites y compatibilidad: Sólo .lat y .mict, sensibles a mayúsculas; otras extensiones conservan el no-op histórico. Vacío sigue siendo error. Rutas relativas al directorio de trabajo. No crea una VM aislada ni revierte efectos del programa. El cargador requiere archivo posicionable, sin NUL, tamaño hasta INT_MAX−2. No es una operación para contenido no confiable. No anidar la llamada como argumento ni usarla como valor: el contrato nativo no apila un retorno y la VM informa operandos insuficientes. Modernizar esa regla global queda fuera de esta misión.

Coste aproximado: O(n) para lectura, más coste de análisis, compilación y programa; memoria de fuente/AST/bytecode y objetos de la VM.

```mict
archivo.escribir("hijo.mict", "archivo.escribir(\"marca\", \"42\")\n")
archivo.ejecutar("hijo.mict")
poner(archivo.leer("marca"))
```

Salida esperada:

```text
42
```

## crear

`archivo.crear(ruta)`

Crea exclusivamente un archivo vacío, evitando truncar uno existente.

Resultado: Verdadero si crea y cierra correctamente; falso si ya existe o falla.

Límites y compatibilidad: Ruta cadena; no crea directorios padres. Un fallo de cierre después de crear puede dejar un archivo vacío aunque devuelva falso. No promete revertir creación. Los permisos dependen de fopen y umask, a diferencia del 0600 del reemplazo atómico.

Coste aproximado: Coste de resolver ruta y abrir/cerrar; memoria auxiliar O(1).

```mict
poner(archivo.crear("nota"))
archivo.escribir("nota", "conservar")
poner(archivo.crear("nota"))
poner(archivo.leer("nota"))
```

Salida esperada:

```text
verdadero
falso
conservar
```

## eliminar

`archivo.eliminar(ruta)`

Elimina una entrada del sistema de archivos mediante remove.

Resultado: Verdadero si remove tuvo éxito; falso en otro caso, incluida ruta ausente.

Límites y compatibilidad: Ruta cadena. Conserva la capacidad POSIX de borrar directorios vacíos y enlaces (no su objetivo); no borra directorios recursivamente. No elimina los bytes de otros enlaces duros al mismo archivo. No promete fsync del directorio.

Coste aproximado: Coste del sistema de archivos y resolución de ruta; O(1) memoria auxiliar.

```mict
archivo.escribir("nota", "sintético")
poner(archivo.eliminar("nota"))
poner(archivo.eliminar("nota"))
```

Salida esperada:

```text
verdadero
falso
```

## borrar

`archivo.borrar(ruta)`

Es el alias histórico exacto de eliminar.

Resultado: Verdadero o falso, con los mismos tipos y errores que eliminar.

Límites y compatibilidad: No es borrado recursivo ni borrado físico garantizado. Mantiene remove de POSIX, incluyendo directorios vacíos.

Coste aproximado: Coste de sistema de archivos; O(1) memoria auxiliar.

```mict
archivo.crear("nota")
poner(archivo.borrar("nota"))
poner(archivo.borrar("nota"))
```

Salida esperada:

```text
verdadero
falso
```

## renombrar

`archivo.renombrar(origen, destino)`

Cambia el nombre o ubicación mediante rename del sistema.

Resultado: Cadena con el destino si tuvo éxito; falso si rename falla.

Límites y compatibilidad: Ambos argumentos cadenas. POSIX puede reemplazar un destino existente: se conserva este contrato histórico. Puede fallar entre sistemas de archivos. No confirma durabilidad con fsync ni deshace el movimiento si posteriormente falla publicar el resultado en la VM.

Coste aproximado: Resolución de rutas/operación del sistema; O(longitud del resultado) al publicar la cadena.

```mict
archivo.escribir("origen", "contenido")
poner(archivo.renombrar("origen", "destino"))
poner(archivo.leer("destino"))
```

Salida esperada:

```text
destino
contenido
```
