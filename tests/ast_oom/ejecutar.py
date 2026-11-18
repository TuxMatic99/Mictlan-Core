#!/usr/bin/env python3
"""Barrido de reservas reales: los crashes se registran como fallos, nunca como éxito."""
import json,os,re,subprocess,sys
from pathlib import Path
binario,modo,caso=sys.argv[1:]
if modo!='constructor':
 spec=json.loads(Path(__file__).with_name('casos.json').read_text())[caso]
 p=Path.cwd()/'sintetico.mict';p.write_text(spec['programa']);caso=str(p)
def correr(n):
 r=subprocess.run([binario,modo,caso,str(n)],capture_output=True,text=True,timeout=20)
 print(f'ordinal={n} retorno={r.returncode}',flush=True);print(r.stdout,end='');print(r.stderr,end='',file=sys.stderr)
 ok=r.returncode==0 and not re.search(r'AddressSanitizer|UndefinedBehaviorSanitizer|LeakSanitizer|runtime error:',r.stdout+r.stderr)
 if n:ok=ok and f'ordinal={n} ' in r.stderr and 'inyectado=1' in r.stdout
 return r,ok
r,ok=correr(0)
m=re.search(r'reservas=(\d+)',r.stdout)
if not m or not ok:sys.exit(1)
if modo=='sintaxis':sys.exit(0)
for n in range(1,int(m[1])+1):
 _,valido=correr(n);ok=ok and valido
sys.exit(0 if ok else 1)
