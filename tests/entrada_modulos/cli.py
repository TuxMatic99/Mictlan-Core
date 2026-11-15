#!/usr/bin/env python3
"""Casos de lenguaje, ejecutados dentro del aislamiento de archivos/ejecutar.py."""
import json,os,pathlib,subprocess,sys
raiz=pathlib.Path.cwd()
assert str(raiz)==os.environ['MICTLAN_ARCHIVOS_TMP']
cli,caso,ext,locale=sys.argv[1:]
c=json.loads(pathlib.Path(__file__).with_name('casos.json').read_text())[caso]
env=os.environ.copy();env['HOME']=str(raiz);env['INPUTRC']=str(raiz/'inputrc');env['LATINO_LIB']=str(raiz/'lib');env['LC_ALL']=locale
(raiz/'inputrc').write_text('')
for nombre,contenido in c.get('inicial',{}).items():
 p=raiz/nombre;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(contenido)
fuente=pathlib.Path(__file__).with_name('programas')/(caso+'.'+ext)
assert fuente.read_text()==c['programa']
p=raiz/('programa.'+ext);p.write_bytes(fuente.read_bytes())
r=subprocess.run([cli,str(p)],input=c.get('entrada','').encode(),env=env,capture_output=True,timeout=15)
salida=r.stdout.decode();errores=r.stderr.decode()
print(salida,end='');sys.stderr.write(errores)
assert not any(s in salida+errores for s in ('AddressSanitizer','UndefinedBehaviorSanitizer','LeakSanitizer','runtime error:')),'Informe de sanitizador'
actual={str(q.relative_to(raiz)):q.read_text() for q in raiz.rglob('*') if q.is_file() and q not in (p,raiz/'inputrc')}
assert actual==c.get('final',c.get('inicial',{})),('Disco',actual,c)
assert r.returncode==c.get('codigo',0),(r.returncode,errores)
assert salida==c.get('salida',''),(salida,c.get('salida',''))
if 'diagnostico' in c:assert c['diagnostico'] in errores,errores
else:assert not errores,errores
