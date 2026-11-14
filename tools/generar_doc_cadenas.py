#!/usr/bin/env python3
"""Genera referencia y ejemplos desde contratos/oraculos revisados, sin ejecutar Core."""
import json,re,sys
from pathlib import Path
raiz=Path(__file__).resolve().parents[1]
api=json.loads((raiz/'tests/cadenas/api.json').read_text())
registrados=set(re.findall(r'\{"([a-z_]+)",\s*str_', (raiz/'src/latstrlib.c').read_text()))
assert registrados=={x['nombre'] for x in api},'Inventario incompleto o nombres duplicados'
def celda(s):return s.replace('|','\\|').replace('\n',' ')
filas=['# Inventario de cadena · P1.2-B1','','31 nombres heredados y nueve añadidos. La tabla describe la propuesta final;','los defectos previos y su evidencia están en [el informe](P12B1_INFORME.md).','','| Función y propósito | Argumentos | Resultado | Vacío | UTF-8 | Índices | Inválidos | Latino | Manual |','|---|---|---|---|---|---|---|---|---|']
for a in api:
 filas.append('| '+' | '.join(celda(x) for x in (a['nombre']+': '+a['proposito'],a['firma'],a['resultado'],a['vacio'],a['utf8'],a['indices'],a['invalidos'],'Nueva B1' if a['nuevo'] else 'Contrato válido conservado',a['manual']))+' |')
filas+=['','## Diferencias verificadas con la referencia histórica','','El [manual de Latino](https://manual.lenguajelatino.org/es/stable/librerias/Cadena.html) se contrasta con código y ejecución. Los códigos indican diferencias concretas; «—» no convierte el manual en especificación de casos no documentados.','','| Código | Diferencia |','|---|---|','| M1 | Los índices y longitudes heredados son bytes, no caracteres Unicode. |','| M2 | comparar devuelve el resultado de strcmp; solo el signo es portable. |','| M3 | El cuarto argumento limita sustituciones; no es una posición. |','| M4 | El relleno aplica n − bytes(base) + 1 repeticiones cuando es positivo. |','| M5 | La caja se convierte por byte C, no con reglas Unicode completas. |','| M6 | separar interpreta un conjunto de bytes y descarta campos vacíos. |','| M7 | char/bytes trabajan con bytes C; no son conversiones de puntos de código. |','','Las columnas de argumentos inválidos recogen validaciones B1 además de los errores de tipo heredados. La evidencia previa conserva accesos inválidos y fugas; no se promete compatibilidad con resultados de comportamiento indefinido.','']
inventario='\n'.join(filas)
intro='''# Referencia de cadenas de Mictlán

P1.2-B1 · 13 de noviembre de 2026. Las firmas son iguales en `.lat` y `.mict`.
El [inventario completo](P12B1_INVENTARIO.md) distingue los contratos heredados.

## Bytes, puntos de código y grafemas

Un byte es una unidad de almacenamiento. UTF-8 usa de uno a cuatro bytes por
punto de código Unicode. Un grafema reúne los puntos que normalmente vemos
como un solo carácter. «e» seguida de acento combinante ocupa dos puntos;
un emoji unido con ZWJ también puede contener varios.

| Texto | Bytes UTF-8 | Puntos de código | Grafemas de ejemplo según UAX #29, sin API implementada |
|---|---:|---:|---:|
| niño | 5 | 4 | 4 |
| é | 2 | 1 | 1 |
| é | 3 | 2 | 1 |
| 👩‍💻 | 11 | 3 | 1 |
| 🇲🇽 | 8 | 2 | 1 |

`longitud` conserva bytes y sus otros tipos históricos. Para código nuevo,
`longitud_bytes` exige texto y `longitud_unicode` valida UTF-8 y cuenta puntos.
No hay todavía una función `longitud_grafemas`.

La [referencia UAX #29](https://www.unicode.org/reports/tr29/) define reglas de
segmentación de grafemas; contar bytes o puntos no implementa esas reglas.
B1 no añade normalización, caja Unicode completa ni comparación lingüística.
`mayusculas_ascii` deja ñ y á intactas; el nombre hace explícito ese límite.

Los índices heredados están basados en cero y en bytes. Los nuevos índices
Unicode están basados en cero y en puntos de código y exigen enteros no
negativos. Una cantidad que sobrepasa el resto se limita al resto del texto.
Las cadenas siguen terminadas por NUL: no son buffers binarios con ceros internos.

## Ejemplos verificados

Cada bloque siguiente se genera desde un resultado esperado explícito de
`tests/cadenas/api.json`, forma parte de `documentacion.mict` y se ejecuta con
ambas extensiones. El generador no consulta resultados del intérprete.

'''
ref=[intro]
fuente=['# Ejemplos de la referencia, con oraculos independientes de Core.']
salida=[]
for a in api:
 ref += ['## cadena.'+a['nombre'],'',a['proposito'],'','**Firma:** `cadena.'+a['firma']+'`. **Devuelve:** '+a['resultado']+'.','','```mict',a['ejemplo'],'```','','Resultado:','','```text',a['esperado'].rstrip('\n'),'```','','**Vacío:** '+a['vacio']+' **Argumentos inválidos:** '+a['invalidos'],'','**Texto e índices:** '+a['utf8']+' '+a['indices'],'','**Casos límite y compatibilidad:** '+a['limites'],'','**Recomendación:** '+a['recomendacion'],'']
 fuente += ['# cadena.'+a['nombre'],a['ejemplo']]
 salida += [a['esperado']]
productos={raiz/'docs/P12B1_INVENTARIO.md':inventario,raiz/'docs/CADENAS.md':'\n'.join(ref),raiz/'tests/cadenas/documentacion.mict':'\n'.join(fuente)+'\n',raiz/'tests/cadenas/documentacion.out':''.join(salida)}
for p,texto in productos.items():
 if '--check' in sys.argv:
  if not p.exists() or p.read_text()!=texto:raise SystemExit('Desactualizado: '+str(p))
 else:p.write_text(texto)
print('Inventario: 40 nombres; referencia y ejemplos sincronizados.')
