#!/usr/bin/env python3
"""Usa el envoltorio B4-A. Sólo copia/ejecuta fuente y productor de esta batería."""
from pathlib import Path
import hashlib,json,os,shutil,subprocess,sys
cli,productor,caso,ext=sys.argv[1:]
raiz=Path.cwd();assert str(raiz)==os.environ['MICTLAN_ARCHIVOS_TMP']
c=json.loads(Path(__file__).with_name('casos.json').read_text())[caso]
p=raiz/('programa.'+ext);f=Path(__file__).with_name('programas')/(caso+'.'+ext);assert f.read_text()==c['programa'];p.write_bytes(f.read_bytes())
helper=raiz/'productor';shutil.copyfile(productor,helper);helper.chmod(0o700);antes=hashlib.sha256(helper.read_bytes()).hexdigest()
env=os.environ.copy();env.update(TZ='UTC0',LC_ALL='C',USER='usuario_sintetico',HOME=str(raiz),INPUTRC=str(raiz/'inputrc'))
if c.get('sin_usuario'):env.pop('USER',None)
(raiz/'inputrc').write_text('')
r=subprocess.run([cli,str(p)],cwd=raiz,env=env,capture_output=True,text=True,timeout=20)
sys.stdout.write(r.stdout);sys.stderr.write(r.stderr)
assert not any(t in r.stdout+r.stderr for t in ('AddressSanitizer','UndefinedBehaviorSanitizer','LeakSanitizer','runtime error:')),'Sanitizador'
assert hashlib.sha256(helper.read_bytes()).hexdigest()==antes
actual={str(q.relative_to(raiz)):q.read_bytes() for q in raiz.rglob('*') if q.is_file() and q not in (p,helper,raiz/'inputrc')}
assert actual=={k:v.encode() for k,v in c.get('final',{}).items()},('Disco',actual,r.returncode,r.stderr)
assert r.returncode==c.get('codigo',0),(r.returncode,r.stderr)
assert r.stdout==c.get('salida',''),(r.stdout,c.get('salida',''))
if 'diagnostico' in c:assert c['diagnostico'] in r.stderr,r.stderr
else:assert not r.stderr,r.stderr
