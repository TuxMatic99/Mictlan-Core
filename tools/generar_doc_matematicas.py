#!/usr/bin/env python3
"""La referencia y sus ejemplos comparten datos revisados; no ejecuta Core para crear oráculos."""
import json,re,sys
from pathlib import Path
raiz=Path(__file__).resolve().parent.parent
filas=json.loads((raiz/'tests/matematicas/catalogo.json').read_text())
nombres={f['nombre'] for f in filas}
registrados=set(re.findall(r'\{"([^"]+)",', (raiz/'src/latmathlib.c').read_text()))|{'anumero','anumero_estricto'}
assert nombres==registrados and len(filas)==len(nombres)==50,(nombres^registrados)
intro='''# Matemáticas y conversiones: referencia inicial de Mictlán

El módulo `mate` ofrece 39 nombres heredados y nueve nuevos. Se conserva
`anumero` y se añade `anumero_estricto` como función global. No se necesita importar
el módulo. Todos los argumentos de mate son números, salvo que la firma no
reciba argumentos. Un tipo incorrecto produce un error recuperable; no hay
conversión implícita de texto dentro de mate. Los resultados son valores nuevos;
ninguna función modifica los argumentos. Semilla y aleatoriedad sí cambian el
estado de su generador. Las listas de descomposición son nuevas.

Un número de coma flotante es una aproximación sobre una rejilla binaria: igual
que una regla graduada no distingue todas las distancias, un double no representa
todos los decimales. Por eso 0.1+0.2 puede diferir de 0.3. Para aproximaciones,
compare una diferencia con una tolerancia adecuada al problema; no redondee cada
operación para ocultar el error. Véase [precisión y límites](P12B3_PRECISION.md).

NaN significa «no es un número» y suele señalar un dominio inválido; infinito
puede representar un desbordamiento o un polo. Ninguno sustituye una validación
de datos. Las funciones heredadas conservan libm; las nuevas funciones seguras
convierten los casos especificados en errores. `-0` compara igual que `0`, pero
conserva información de signo útil, por ejemplo en atan2. Use los tres predicados
para clasificar valores; NaN no se detecta comparándolo por igualdad.

Cada ejemplo siguiente se ejecuta automáticamente como `.lat` y `.mict`.
La salida esperada se escribió a partir del contrato, no del intérprete.
Los alias tienen ejemplos propios. Coste: max/min O(n) y espacio auxiliar O(1);
las conversiones de texto O(bytes), sin buffer temporal; las demás operaciones
son escalares. El rechazo aleatorio tiene coste esperado constante (<2 palabras
para los intervalos admitidos), sin una cota finita determinista de iteraciones.
El coste interno de libm depende de la plataforma.

'''
referencia=intro
inventario='''# Inventario de matemáticas y conversiones P1.2-B3

Auditados `src/latmathlib.c`, `src/latbaselib.c`, `src/latobj.c` y `src/latlocale.c`.
Esta tabla enumera todos los nombres públicos del lenguaje afectados; no son
símbolos C nuevos. Las firmas reciben números salvo las dos conversiones globales.
Los 39 nombres heredados siguen registrados. Nueve nombres mate y una conversión
son adiciones. Para ejemplos, precisión individual y recomendaciones véase la
[referencia](P12B3_REFERENCIA.md). [Decisiones](P12B3_DECISIONES.md) conserva la
propuesta inicial y la rectificación de anumero al comprobar la suite anterior.

| Firma | Resultado y propósito | Dominio | No finitos, cero y errores | Contrato |
|---|---|---|---|---|
'''
programa='// Ejemplos documentados: oráculos independientes en documentacion.out\n';salida=''
for f in filas:
 prefix='' if f['nombre'].startswith('anumero') else 'mate.'
 firma=prefix+f['firma'];clase='Nuevo B3' if f['nueva'] else 'Heredado; conservar firma'
 inventario+=f"| `{firma}` | {f['resultado']}: {f['idea']} | {f['dominio']} | {f['limites']} | {clase} |\n"
 referencia+=f"## `{firma}`\n\n{f['idea']} Devuelve **{f['resultado']}**.\n\nArgumentos y dominio: {f['dominio']} Casos límite: {f['limites']}\n\nPrecisión: {f['precision']} {f['alternativa']}\n\n```latino\n{f['ejemplo']}```\n\nSalida esperada:\n\n```text\n{f['salida']}```\n\n"
 programa+='// '+firma+'\n'+f['ejemplo'];salida+=f['salida']
inventario+='''
## Conversión C y puntos relacionados

| Interfaz | Contrato comprobado y límites | Acción B3 |
|---|---|---|
| latC_adouble(mv,obj) → double | Nulo/falso=0, verdadero=1, número sin cambio, lista/dic=longitud; texto usa strtod en locale C si consume toda la cadena, si no usa primer byte. Texto vacío da 0. O(bytes) para texto. | Conservada: modificarla alteraría coerciones globales. |
| latC_aint(mv,obj) → int | Texto numérico se trunca hacia cero. Falso/nulo=0, verdadero=1, char y longitudes conservados. T_NUMERIC llama al verificador T_INTEGER y da error: inconsistencia histórica de etiquetas, no política nueva de coerción. | Texto NaN/inf/fuera de int produce error antes del cast. Tipos/firmas conservados. |
| latC_achar(mv,obj) → char | Texto numérico se trunca hacia cero; rango CHAR_MIN..CHAR_MAX según plataforma. Tipos T_INTEGER/T_CHAR conservados; el comportamiento T_NUMERIC tiene la inconsistencia anterior. | Texto no representable produce error antes del cast; conversión entera a char y longitudes se dejan para revisar compatibilidad SDK. |
| latC_checar_numerico / latC_checar_entero | Exigen respectivamente T_NUMERIC / T_INTEGER. Los programas normales usan double/T_NUMERIC. | Sin cambios. |
| latC_strtod_c(texto,fin) | Cambia temporalmente LC_NUMERIC a C, llama a strtod y restaura el locale. | Reutilizado por la API estricta; pruebas C/es_ES/fr_FR. Su estado global requiere futura solución para concurrencia. |
| leer() | Lee texto y prueba strtod; no equivale al contrato decimal estricto de la API nueva. | Fuera de este incremento; usar anumero_estricto sobre texto para validación explícita. |
| acadena(numero) | Formatea normalmente con %.16g; puede ocultar diferencias y no garantiza reconstruir todos los doubles. | Sin cambios; las pruebas C observan precisión y signo sin depender de la impresión. |

## Contraste con el manual histórico

[Matemática de Latino](https://manual.lenguajelatino.org/es/stable/librerias/Matematica.html)
explica funciones sobre números, pero no fija una política completa de NaN,
precisión ni recuperación de errores. Sus ejemplos de frexp y porc coinciden
con fracción sola y razón (no se «corrigen» para inventar otro significado).
«Entre 0 y 1» en aleatorio no implica un real continuo: se conserva 0 o 1.
La mención general de negativos se conserva para dos extremos; el caso de un
único límite negativo tenía división por cero o resultados fuera del intervalo
pretendido. Ahora da error, con alternativa explícita para intervalos negativos.
La especificación aquí procede del código, las matemáticas y las pruebas, no
únicamente del manual. Los dominios de libm permanecen en las funciones heredadas.
'''
outputs={'docs/P12B3_REFERENCIA.md':referencia.rstrip()+'\n','docs/P12B3_INVENTARIO.md':inventario,'tests/matematicas/documentacion.mict':programa,'tests/matematicas/documentacion.out':salida}
for path,contenido in outputs.items():
 p=raiz/path
 if '--check' in sys.argv:
  assert p.read_text()==contenido,'Documento o ejemplo desincronizado: '+path
 else:p.write_text(contenido)
print('50 entradas y ejemplos sincronizados; 39 nombres mate heredados, 9 nuevos y 2 conversiones.')
