#!/usr/bin/env python3
"""Tres procesos independientes por tamaño; salida verificable, sin umbral temporal."""
import subprocess,sys
print('argumentos,repeticiones,segundos,bytes_vm_inicio,bytes_vm_final,delta_vm,rss_kib,control')
for n,reps in ((2,10000),(16,3000),(512,100)):
    for _ in range(3):
        print(subprocess.check_output([sys.argv[1],str(n),str(reps)],text=True).strip())
