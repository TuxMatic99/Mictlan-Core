#!/usr/bin/env python3
"""Mediciones separadas por proceso; no son umbrales de aprobacion."""
import subprocess,sys
print('funcion,texto,bytes,iteraciones,segundos,vm_inicio,vm_fin,delta_vm,rss_inicio_kib,rss_pico_kib,control')
for funcion in ('concatenar','invertir','insertar','subcadena','reemplazar','longitud_unicode','invertir_unicode'):
    for longitud,iteraciones in ((20,10000),(262140,100)):
        for texto in ('ascii','utf8'):
            p=subprocess.run([sys.argv[1],funcion,str(longitud),str(iteraciones),texto],text=True,capture_output=True)
            if p.returncode==77:continue # API moderna no presente en la base; no cuenta como prueba aprobada.
            if p.returncode or p.stderr:
                sys.stderr.write(p.stderr);raise SystemExit(p.returncode or 1)
            print(p.stdout,end='')
