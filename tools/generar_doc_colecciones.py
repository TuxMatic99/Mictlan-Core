#!/usr/bin/env python3
"""Genera referencia, inventario y ejemplos con oráculos explícitos, sin ejecutar Core."""
import argparse,json,re,sys
from pathlib import Path
raiz=Path(__file__).resolve().parents[1]
datos=json.loads((raiz/'tests/colecciones/api.json').read_text())
for mod,archivo,tabla in [('lista','latlistlib.c','liblist'),('dic','latdiclib.c','libdic')]:
 texto=(raiz/'src'/archivo).read_text().split('static const lat_CReg '+tabla+'[]',1)[1]
 nombres=set(re.findall(r'\{"([a-z_]+)"',texto))
 assert nombres=={f['nombre'] for f in datos if f['modulo']==mod},(mod,nombres)
intro='''# Colecciones de Mictlán

Una lista organiza elementos por posición; un diccionario asocia claves de texto
con valores. Esta referencia describe contratos comprobados en P1.2-B2 y conserva
las peculiaridades de Latino. `diccionario` es un alias de módulo de `dic`.

## Copiar y compartir

`b = a` comparte el mismo contenedor: cambiarlo mediante b se ve mediante a.
`lista.invertir` y `lista.concatenar` crean nodos nuevos y comparten sus elementos.
Una lista anidada es como una caja: dos contenedores pueden señalar la misma caja.
Cambiar su contenido se ve por ambos caminos; reemplazar un nodo de uno de ellos
no reemplaza el nodo del otro. `lista.extender` también comparte elementos.

```mict
a = [[1], 2]
b = a
c = lista.concatenar(a, [])
b[1] = 5
escribir(a[1])
escribir(c[1])
a[0][0] = 9
escribir(c[0][0])
```

Salida: `5`, `2`, `9`, en líneas separadas. Este ejemplo se ejecuta automáticamente.

No existe una función pública de copia profunda universal. El clonado interno
usado por agregar/insertar copia números y listas anidadas dentro de una lista,
pero comparte otros elementos como diccionarios. Clonar un diccionario copia la
tabla y sus celdas; los contenedores usados como valores siguen compartidos.
Los ciclos no se clonan ni se comparan recursivamente sin límite: se diagnostican.
Invertir y concatenar pueden conservar referencias cíclicas sin recorrerlas.

## Índices, claves e igualdad

Las posiciones empiezan en 0. Las bibliotecas conservan el truncamiento de índices
fraccionarios finitos hacia cero; se recomienda usar enteros. NaN, infinito y
números fuera del rango de int producen error antes de la conversión. Insertar
admite la posición final n; eliminar_indice requiere una posición existente.
Estas reglas describen las funciones de lista, no redefinen los corchetes de la VM.

Las claves son cadenas exactas: "ñ" ocupa dos bytes UTF-8 y no equivale a "n".
No hay normalización Unicode ni orden de inserción garantizado. La longitud de un
diccionario cuenta entradas; el límite de 63 bytes es para insertar una clave,
no para la longitud de sus valores. [] y {} son contenedores distintos vacíos.

`indice` y `eliminar` usan una igualdad por tipo más limitada que `contiene`.
Por compatibilidad `contiene([2], "2")` es verdadero e `indice([2], "2")` es -1.
El buscador por índice tampoco reconoce booleanos, nulo ni diccionarios como
coincidencias; no debe confundirse con una API moderna de igualdad estricta.
Comparar mira primero longitudes: [9] precede a [0, 0]. Entre números compara
valores sin truncar la diferencia; NaN produce error y los infinitos se ordenan.
Una futura igualdad uniforme requerirá nombres nuevos y un plan de compatibilidad.

## Costes y errores

n y m son cantidades de elementos; B son bytes del texto producido; C es el coste
del clonado selectivo, E el coste de comparar un elemento y T el de representar
objetos cuando el comparador heredado convierte tipos distintos a texto. Los
costes cuentan recorridos lógicos y reservas; no incluyen todos los costes del
registro de memoria de la VM ni del sistema operativo. El hash tiene coste
esperado constante para claves acotadas; una cubeta con muchas colisiones puede
requerir recorrer n entradas. Enumerar un diccionario recorre toda su capacidad.

Los argumentos tienen tipos estrictos. Un error no debe corromper la pila ni
retirar entradas ajenas. Las operaciones que modifican colecciones no devuelven
un elemento; no dependa de un retorno propio. Las reglas de asignación de P1.2-A
completan con nulo las posiciones de retorno que falten.

'''
ref=[intro];inv=['# Inventario de lista y dic\n\n','Generado de `tests/colecciones/api.json`, cotejado con los registros nativos.\n\n','| Función | Resultado | Modifica | Vacío | Coste | Compatibilidad |\n|---|---|---|---|---|---|\n'];programa=[];salida=[]
for f in datos:
 nombre=f["modulo"]+'.'+f['nombre']
 inv.append('| '+ ' | '.join([nombre,f['resultado'],f['mutacion'],f['vacio'],f['complejidad'],f['compatibilidad']]).replace('\n',' ')+' |\n')
 ref.append(f'## {nombre}\n\n{f["proposito"]}\n\n`{f["firma"]}`\n\n')
 for campo,titulo in [('argumentos','Argumentos'),('resultado','Resultado'),('mutacion','Original'),('vacio','Entradas vacías'),('indices_claves','Índices y claves'),('duplicados','Duplicados e igualdad'),('copias_referencias','Copias y referencias'),('complejidad','Coste aproximado'),('errores','Errores'),('compatibilidad','Compatibilidad'),('manual','Referencia histórica')]:
  ref.append(f'**{titulo}:** {f[campo]}\n\n')
 ref.append('```mict\n'+f['ejemplo']+'```\n\nSalida:\n\n```text\n'+f['salida']+'```\n\n')
 programa.append('// '+nombre+'\n'+f['ejemplo']);salida.append(f['salida'])
programa.append('a = [[1], 2]\nb = a\nc = lista.concatenar(a, [])\nb[1] = 5\nescribir(a[1])\nescribir(c[1])\na[0][0] = 9\nescribir(c[0][0])\n');salida.append('5\n2\n9\n')
inv.append('''
Los campos completos por función (firma, tipos, índices, claves, duplicados,
referencias, errores y contraste histórico) están en [COLECCIONES.md](COLECCIONES.md).
Hay 14 nombres heredados en lista y uno nuevo, unir; dic conserva sus seis nombres.
El módulo diccionario conserva la misma tabla de dic. Ningún nombre previo se retira.

## Herencia y alternativas

| Aspecto | Heredado verificado | Alternativa o corrección B2 |
|---|---|---|
| Unión | separar une texto; omite otros tipos y puede devolver nulo | unir es un alias, sin alterar el contrato |
| Índices | Desde 0, fracciones truncadas; sin negativos | Se rechazan conversiones no finitas o fuera de int |
| Eliminar clave | Desconectaba toda la cubeta y dejaba mal la longitud | Retira un nodo, conserva colisiones y valores compartidos |
| Contiene clave | Recorrido completo de cubetas | Consulta por hash, incluso con valor nulo |
| Decimales | Comparar/contiene podían igualar 0.1 y 0.9 | Comparación numérica correcta dentro de colecciones |
| Igualdad mixta | contiene e indice emplean criterios distintos | Conservada; una API uniforme queda como propuesta futura |
| Copias | Política selectiva y referencias compartidas | Se documenta y prueba, sin imponer copia profunda nueva |
| Enumeración dic | Orden de cubetas y colisiones | No se promete orden de inserción |
| Ciclos | Comparación recursiva sin protección | Error controlado; invertir/concatenar conservan referencias |

## Referencias históricas

* [Lista de Latino](https://manual.lenguajelatino.org/es/stable/librerias/Lista.html)
* [Diccionario de Latino](https://manual.lenguajelatino.org/es/stable/librerias/Diccionario.html)

Consultadas como referencia histórica el 14 de noviembre de 2026. La definición
verificada procede del código y las regresiones de Core, no de asumir que los
ejemplos del manual sean exhaustivos o infalibles. La tabla histórica de longitud
de diccionario menciona texto pese a que cuenta entradas; el ejemplo de separar
menciona insertar. Ambos se aclaran sin cambiar contratos válidos.
''')
ref.append('''## Ejecución y limitaciones

Los ejemplos anteriores forman `tests/colecciones/documentacion.mict`; la misma
fuente se ejecuta con extensiones .lat y .mict y salidas explícitas. La generación
se verifica con `python3 tools/generar_doc_colecciones.py --check`. La suite incluye
casos de colisiones, crecimiento, referencias, clonado selectivo, ciclos, índices
inválidos, reservas fallidas y separadores largos.

No se añade copia profunda general, igualdad uniforme ni orden de inserción.
La memoria administrada sin recolección optativa puede retener celdas retiradas
hasta cerrar la VM: preserva referencias compartidas, no es una fuga al cierre.
No se añade dependencia externa ni se modifica el parser. Véanse
[P12B2_INFORME.md](P12B2_INFORME.md) y [DEBIAN_P12B2.md](DEBIAN_P12B2.md).
''')
salidas={'docs/COLECCIONES.md':''.join(ref),'docs/P12B2_INVENTARIO.md':''.join(inv),'tests/colecciones/documentacion.mict':'\n'.join(programa),'tests/colecciones/documentacion.out':''.join(salida)}
check=argparse.ArgumentParser();check.add_argument('--check',action='store_true');args=check.parse_args()
fallos=[]
for nombre,contenido in salidas.items():
 p=raiz/nombre
 if args.check:
  if not p.exists() or p.read_text()!=contenido:fallos.append(nombre)
 else:p.write_text(contenido)
if fallos:print('Documentación desincronizada: '+', '.join(fallos),file=sys.stderr);sys.exit(1)
