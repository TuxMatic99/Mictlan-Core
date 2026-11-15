#!/usr/bin/env python3
"""Censo reproducible; no ejecuta código inventariado ni infiere ausencia de defectos."""
from pathlib import Path
import csv,fnmatch,hashlib,io,json,re,subprocess,sys
raiz=Path(__file__).resolve().parents[2]
grupos=json.loads(Path(__file__).with_name('subsistemas.json').read_text())
for g in grupos:
 for prueba in re.findall(r'tests/[\w*/._-]+',g['pruebas']):
  assert list(raiz.glob(prueba.rstrip('/'))),(g['id'],prueba)
paths=subprocess.check_output(['git','ls-files','src','include','tools/parser'],cwd=raiz,text=True).splitlines()
assert paths and len(paths)==len(set(paths))
files=[];nativas=[];funciones=[]
links={line.split("\t")[1]:line.split()[1] for line in subprocess.check_output(["git","ls-files","--stage","src","include","tools/parser"],cwd=raiz,text=True).splitlines() if line.startswith("160000")}
# Índice léxico de definiciones: conserva saltos y elimina cadenas/comentarios.
def limpiar(s):
 return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',lambda m:''.join('\n' if c=='\n' else ' ' for c in m[0]),s,flags=re.S)
for p in paths:
 gs=[g for g in grupos if any(fnmatch.fnmatch(p,pat) for pat in g['patrones'])]
 assert len(gs)==1,(p,[g['id'] for g in gs])
 g=gs[0]
 if p in links:
  files.append([p,g['id'],0,'gitlink:'+links[p],'submodulo_no_ejecutado','']);continue
 b=(raiz/p).read_bytes();texto='' if b'\0' in b else b.decode('utf-8',errors='replace')
 files.append([p,g['id'],len(b),hashlib.sha256(b).hexdigest(),'texto' if texto else 'binario',';'.join(re.findall(r'^\s*#\s*include\s*[<"]([^">]+)',texto,re.M))])
 if p.endswith('.c') and 'static const lat_CReg' in texto:
  pref=re.search(r'#define\s+LIB_\w+\s+"([^"]*)"',texto)
  lib=pref[1] if pref else '?'
  start=texto.find('static const lat_CReg')
  if start>=0:
   for m in re.finditer(r'\{\s*"([^"]+)"\s*,\s*(\w+)\s*,\s*([^}]+)\}',texto[start:]):
    nativas.append([lib,m[1],m[2],m[3].strip(),p,texto[:start+m.start()].count('\n')+1,'no' if g['id'] in ('OPTIONAL','GC_LEG') else 'si',g['id']])
 if p.endswith(('.c','.h')) and texto:
  limpio=limpiar(texto);limpio=re.sub(r'^\s*#.*(?:\\\n.*)*',lambda m:'\n'*m[0].count('\n'),limpio,flags=re.M)
  # Deliberadamente índice de candidatos, no AST ni prueba de cobertura.
  for m in re.finditer(r'(?m)^[ \t]*(?:(?:\w+|\*)[ \t\n]+)+?(\w+)\s*\(([^;{}]*?)\)\s*\{',limpio):
   if m[1] not in ('if','for','while','switch'):
    funciones.append([p,limpio[:m.start(1)].count('\n')+1,m[1],' '.join(m[2].split()),g['id']])
def csvtext(enc,rows):
 f=io.StringIO();w=csv.writer(f,lineterminator='\n');w.writerow(enc);w.writerows(rows);return f.getvalue()
out={
 'archivos.csv':csvtext(['archivo','subsistema','bytes','sha256_o_gitlink','clase','includes_directos'],files),
 'nativas.csv':csvtext(['biblioteca','nombre','implementacion_C','aridad','archivo','linea','registro_normal','subsistema'],nativas),
 'definiciones_indice.csv':csvtext(['archivo','linea_lexica','nombre','argumentos_lexicos','subsistema'],funciones),
}
md='# Inventario reproducible de Core — A0\n\n'
md+=f'Censo: **{len(files)} archivos**, **{len(grupos)} subsistemas**, **{len(nativas)} registros nativos** (incluidos opcionales), **{len(funciones)} candidatos de definición C**.\n\n'
md+='Se incluyen todos los archivos versionados de src/, include/ y tools/parser/, también binarios, generados y componentes no activados. Un hash o una prueba aprobada no acredita corrección. El índice de definiciones es léxico: puede incluir alternativas de preprocesador y no reemplaza un análisis C por plataforma. Los contratos se documentan por subsistema; sis recibe además auditoría por función.\n\n'
md+='Regenerar: `python3 tools/auditoria/inventario.py`; verificar sin escribir: `python3 tools/auditoria/inventario.py --comprobar`. Toda ruta debe pertenecer exactamente a un subsistema; una nueva ruta sin clasificar falla. El censo usa los bytes del checkout y omite marcas de tiempo para que el resultado sea reproducible.\n\n'
md+='[Archivos y dependencias directas](archivos.csv) · [API nativa y aridades](nativas.csv) · [Índice de definiciones](definiciones_indice.csv) · [Pendientes priorizados](../PENDIENTES_CORE.md).\n\n'
for g in grupos:
 nombres=[f[0] for f in files if f[1]==g['id']]
 md+=f"## {g['id']} — {g['proposito']}\n\nArchivos: "+', '.join('`'+n+'`' for n in nombres)+'.\n\n'
 for k,et in [('dependencias','Dependencias'),('contrato','Contrato y herencia'),('errores','Errores relevantes'),('pruebas','Pruebas existentes o relacionadas'),('riesgos','Pendiente / límite de la auditoría')]:md+=f"**{et}:** {g[k]}.\n\n"
md+='Las rutas con comodines en pruebas son referencias de búsqueda, no porcentajes de cobertura. No se infiere cobertura de una mera coincidencia textual. La matriz funcional ejecutada y sus resultados se registran por separado en A0_INFORME.md. SDK y Lab no forman parte de este checkout ni de esta entrega.\n'
out['INVENTARIO_CORE.md']=md
for name,data in out.items():
 p=raiz/'docs/auditoria'/name
 if '--comprobar' in sys.argv:assert p.exists() and p.read_text()==data,p
 else:p.parent.mkdir(parents=True,exist_ok=True);p.write_text(data)
print(f'Inventario: {len(files)} archivos; {len(grupos)} subsistemas; {len(nativas)} registros nativos; {len(funciones)} candidatos C')
