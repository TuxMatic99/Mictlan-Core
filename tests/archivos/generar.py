#!/usr/bin/env python3
"""Sincroniza referencia y programas; --comprobar es una prueba de documentación."""
from pathlib import Path
import json,sys
raiz=Path(__file__).resolve().parent
casos=json.loads((raiz/'casos.json').read_text())
intro='''# Archivos en Mictlán: referencia y ejemplos

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

'''
texto=intro
salidas={}
for nombre,c in casos.items():
 for ext in ('lat','mict'):salidas[raiz/'ejemplos'/f'{nombre}.{ext}']=c['programa']
 if 'documentacion' not in c:continue
 d=c['documentacion']
 texto+=f"## {d['nombre']}\n\n`{d['firma']}`\n\n{d['explica']}\n\nResultado: {d['resultado']}\n\nLímites y compatibilidad: {d['limites']}\n\nCoste aproximado: {d['coste']}\n\n```mict\n{c['programa']}```\n\nSalida esperada:\n\n```text\n{c.get('salida','')}```\n\n"
salidas[raiz.parents[1]/'docs'/'ARCHIVOS.md']=texto.rstrip()+'\n'
for destino,contenido in salidas.items():
 if '--comprobar' in sys.argv:
  assert destino.exists() and destino.read_text()==contenido,f'Documentación desincronizada: {destino}'
 else:
  destino.parent.mkdir(parents=True,exist_ok=True);destino.write_text(contenido)
print(f'{len(casos)} pares .lat/.mict y referencia sincronizados')
