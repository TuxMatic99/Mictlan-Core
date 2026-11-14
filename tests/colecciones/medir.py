#!/usr/bin/env python3
"""Cada medida usa una VM/proceso nuevo. Tiempo no es un umbral de prueba."""
import subprocess,sys
print('funcion,elementos,iteraciones,segundos,vm_inicio,vm_fin,delta_vm,rss_inicio_kib,rss_pico_kib,control')
for funcion in ('invertir','concatenar','comparar','separar','dic_contiene'):
 for n,reps in ((20,1000),(5000,5 if funcion!='dic_contiene' else 10000)):
  p=subprocess.run([sys.argv[1],funcion,str(n),str(reps)],capture_output=True,text=True,timeout=50)
  if p.returncode or p.stderr:
   sys.stderr.write(p.stderr);raise SystemExit(p.returncode or 1)
  print(p.stdout,end='')
