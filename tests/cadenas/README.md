# Conformidad y mediciones de cadena

Los oráculos `.out` y `api.json` contienen resultados explícitos escritos antes
de las implementaciones o de cada arreglo adicional. No se obtienen comparando
dos versiones de Core ni se regeneran ejecutando el intérprete.

La primera batería, preservada en el commit `1ef03a5`, tenía 51 pruebas.
La batería final añade 61 a las 67 de P1.2-A: total 128 en Linux.

| Grupo | Qué comprueba |
|---|---|
| herencia.lat | Contratos y aliases originales, español, vacíos, bytes, escapes y regex |
| limites_validos.mict | Relleno multibyte dimensionado correctamente; ventanas y posiciones válidas |
| unicode.mict | Bytes frente a puntos; ñ, emoji, combinantes, ZWJ, subcadenas y posiciones |
| separacion_explicita.mict | Separador completo, campos vacíos, alias delimitador y caja ASCII |
| negativos.tsv y sus programas | Diagnósticos para tipos, rangos, UTF-8 inválido y escapes inseguros |
| documentacion.mict | Los 40 ejemplos de la referencia, con el mismo código y resultados |
| temporales.c | Buffers C liberados; todas las 1.112.064 codificaciones de escalares, incluyendo U+0000 en el lector privado, y truncamientos multibyte |
| unicode_api.c | API de biblioteca compartida, recuperación tras error, pila y prefijo conservados |
| concatenacion_temporales.c | Limpieza de conversiones si la segunda falla por una colección cíclica |
| strings_io_regression.c, ya existente | 1.000 ciclos de recorte/separación, alias internados y E/S con diez cierres de VM |
| generar_doc_cadenas.py --check | Inventario de registros públicos y sincronización de documentación/ejemplos |

Cada programa se copia a `.lat` y `.mict` y utiliza el verificador estricto de
P1.2-A: salida completa, código, diagnóstico esperado y rechazo de cualquier
informe de sanitizador, incluso junto a un error esperado. No se recorta stdout.
U+0000 se valida como escalar por el lector interno, pero no es un carácter
interior admitido por la representación terminada por NUL de Core.

Las pruebas de contabilidad incluyen el código real de `latstrlib.c` con
observadores de reservas/liberaciones y enlazan con Core estático real. No
sustituyen la validación de la biblioteca compartida ni LSan: se ejecutan además
los programas y la API C con la biblioteca real en la matriz instrumentada.

`api.json` es el catálogo revisable de firmas y ejemplos. El generador solo
formatea ese catálogo y contrasta los nombres con `libstr`; no pregunta a Core
cuál debería ser un resultado. Para actualizarlo conscientemente:

```sh
python3 tools/generar_doc_cadenas.py
python3 tools/generar_doc_cadenas.py --check
ctest --test-dir build-b1-debug -L p12b1 --output-on-failure
```

`benchmark.c` y `medir.py` son mediciones, no umbrales de aprobación. El CSV
separa memoria administrada por la VM de RSS, que incluye otras reservas y el
coste del proceso. Las cadenas internadas diferentes pueden seguir retenidas
hasta cerrar la VM según el contrato anterior. B1 no modifica el recolector.

[Informe](../../docs/P12B1_INFORME.md) ·
[Referencia](../../docs/CADENAS.md) ·
[Reproducción Debian](../../docs/P12B1_VERIFICAR_DEBIAN.md).
