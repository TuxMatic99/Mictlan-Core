#!/usr/bin/env python3
"""Aislamiento de B4-A: datos sintéticos, biblioteca del build y diagnóstico visible."""
import argparse,hashlib,os,pathlib,subprocess,tempfile,sys
p=argparse.ArgumentParser()
p.add_argument('--biblioteca',required=True);p.add_argument('--sonda',required=True)
p.add_argument('--ajena');p.add_argument('--caso');p.add_argument('--cli');p.add_argument('--extension',choices=['lat','mict'])
p.add_argument('comando',nargs='*');a=p.parse_args()
def limpio():
    e=os.environ.copy()
    for n in ('LD_LIBRARY_PATH','LD_PRELOAD','LD_AUDIT','DYLD_LIBRARY_PATH','DYLD_INSERT_LIBRARIES'):e.pop(n,None)
    e['LD_LIBRARY_PATH']=str(pathlib.Path(a.biblioteca).resolve().parent)
    return e
def correr(cmd,env,cwd):
    r=subprocess.run(cmd,cwd=cwd,env=env,capture_output=True,text=True,timeout=30)
    if any(s in r.stdout+r.stderr for s in ('AddressSanitizer','UndefinedBehaviorSanitizer','LeakSanitizer','runtime error:')):
        sys.stdout.write(r.stdout);sys.stderr.write(r.stderr);raise RuntimeError('Informe de sanitizador')
    return r
with tempfile.TemporaryDirectory(prefix='mictlan-b4a-') as tmp:
    env=limpio();env["MICTLAN_ARCHIVOS_TMP"]=tmp;env["TMPDIR"]=tmp
    if a.ajena:
        contaminado=env.copy();contaminado['LD_LIBRARY_PATH']=str(pathlib.Path(a.ajena).resolve().parent)
        r=correr([a.sonda,a.biblioteca],contaminado,tmp)
        assert r.returncode==86 and 'Biblioteca Core inesperada' in r.stderr,(r.returncode,r.stdout,r.stderr)
        print('Enlace ajeno sintético detectado: '+r.stdout.strip())
    r=correr([a.sonda,a.biblioteca],env,tmp)
    assert r.returncode==0,(r.returncode,r.stdout,r.stderr)
    print(r.stdout,end='')
    print('Biblioteca SHA256='+hashlib.sha256(pathlib.Path(a.biblioteca).read_bytes()).hexdigest())
    if a.cli:
        import json
        spec=json.loads((pathlib.Path(__file__).with_name('casos.json')).read_text())[a.caso]
        d=pathlib.Path(tmp)
        for nombre,contenido in spec.get('inicial',{}).items():
            q=d/nombre;q.parent.mkdir(parents=True,exist_ok=True);q.write_bytes(contenido.encode())
        programa=d/('programa.'+a.extension);fuente=pathlib.Path(__file__).with_name('ejemplos')/(a.caso+'.'+a.extension)
        assert fuente.read_text()==spec['programa'], 'Ejemplo desincronizado'
        programa.write_bytes(fuente.read_bytes())
        r=correr([a.cli,str(programa)],env,tmp)
        # Comprobar disco incluso si la ejecución o el diagnóstico son incorrectos.
        actual={str(q.relative_to(d)):q.read_bytes() for q in d.rglob('*') if q.is_file() and q!=programa}
        esperado={n:c.encode() for n,c in spec['final'].items()}
        assert actual==esperado,('Contenido de disco incorrecto',actual,esperado,r.returncode,r.stderr)
        assert r.returncode==spec.get('codigo',0),(r.returncode,r.stdout,r.stderr)
        assert r.stdout==spec.get('salida',''),(r.stdout,spec.get('salida',''))
        if spec.get('diagnostico'):assert spec['diagnostico'] in r.stderr,r.stderr
        else:assert not r.stderr,r.stderr
    elif a.comando:
        r=correr(a.comando,env,tmp);sys.stdout.write(r.stdout);sys.stderr.write(r.stderr)
        if r.returncode:sys.exit(r.returncode)
