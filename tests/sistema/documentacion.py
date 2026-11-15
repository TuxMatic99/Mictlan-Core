#!/usr/bin/env python3
"""Verifica ejemplos y oráculos publicados; no obtiene expectativas del intérprete."""
from pathlib import Path
import json,sys
raiz=Path(__file__).resolve().parents[2]
casos=json.loads((raiz/'tests/sistema/casos.json').read_text())
partes=['<!-- A0_EJEMPLOS_INICIO -->\n']
for nombre,c in casos.items():
    for ext in ('lat','mict'):
        assert (raiz/'tests/sistema/programas'/f'{nombre}.{ext}').read_text()==c['programa'],nombre
    partes.append(f'### {nombre}\n\n```mict\n{c["programa"]}```\n\n')
    partes.append('Salida exacta (escapes JSON): `'+json.dumps(c.get('salida',''),ensure_ascii=False)+'`. ')
    partes.append('Código de salida: `'+str(c.get('codigo',0))+'`.\n\n')
    if c.get('diagnostico'):partes.append('El diagnóstico debe contener: `'+c['diagnostico']+'`.\n\n')
    if c.get('final'):partes.append('Disco esperado: `'+json.dumps(c['final'],ensure_ascii=False)+'`.\n\n')
partes.append('<!-- A0_EJEMPLOS_FIN -->')
esperado=''.join(partes);p=raiz/'docs/SISTEMA.md';actual=p.read_text()
a=actual.index('<!-- A0_EJEMPLOS_INICIO -->');b=actual.index('<!-- A0_EJEMPLOS_FIN -->')+len('<!-- A0_EJEMPLOS_FIN -->')
if '--generar' in sys.argv:p.write_text(actual[:a]+esperado+actual[b:])
else:assert actual[a:b]==esperado,'Ejemplos de SISTEMA.md desincronizados'
print(f'{len(casos)} ejemplos, sus oráculos y ambos formatos sincronizados; ejecución comprobada por CTest CLI.')
