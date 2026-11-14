#!/usr/bin/env python3
"""Compara todos los símbolos dinámicos definidos con la base exacta de B1."""
from pathlib import Path
import subprocess,sys
p=subprocess.run([sys.argv[1],'-D','--defined-only',sys.argv[2]],check=True,text=True,capture_output=True)
simbolos=[line.split()[-1] for line in p.stdout.splitlines() if line.split()]
# GCC exporta guardias ODR al instrumentar ASan; no pertenecen a la ABI de Core.
# No se filtran simbolos del proyecto ni funciones del compilador desconocidas.
instrumentacion=[s for s in simbolos if s.startswith('__odr_asan.')]
actual=sorted(s for s in simbolos if s not in instrumentacion)
base=Path(__file__).with_name('abi-base285.txt').read_text().splitlines()
assert len(base)==285 and len(set(base))==285
assert actual==base,{'perdidos':sorted(set(base)-set(actual)),'nuevos':sorted(set(actual)-set(base))}
print(f'ABI: 285 símbolos preservados, sin símbolos nuevos de Core; {len(instrumentacion)} guardias ODR de ASan identificadas. lista.unir es un alias de lenguaje.')
