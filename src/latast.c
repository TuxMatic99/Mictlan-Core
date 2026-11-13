/*
The MIT License (MIT)

Copyright (c) Latino - Lenguaje de Programacion

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#include "latast.h"
#include "latino.h"
#include "latlex.h"
#include "latmem.h"
#include <limits.h>
#include <string.h>

int yyparse(ast **root, yyscan_t scanner);
char *analizar_fmt(const char *s, size_t len);

ast *latA_nodo(nodo_tipo nt, ast *l, ast *r, int nlin, int ncol) {
  ast *a = (ast *)malloc(sizeof(ast));
  a->tipo = nt;
  a->izq = l;
  a->der = r;
  a->valor = NULL;
  a->nlin = nlin;
  a->ncol = ncol;
  return a;
}

ast *latA_logico(int b, int nlin, int ncol) {
  ast *a = (ast *)malloc(sizeof(ast));
  a->tipo = NODO_VALOR;
  nodo_valor *val = (nodo_valor *)malloc(sizeof(nodo_valor));
  val->tipo = VALOR_LOGICO;
  val->val.logico = b;
  a->valor = val;
  a->nlin = nlin;
  a->ncol = ncol;
  return a;
}

ast *latA_nulo(void *nulo, int nlin, int ncol) {
  ast *a = (ast *)malloc(sizeof(ast));
  a->tipo = NODO_VALOR;
  nodo_valor *val = (nodo_valor *)malloc(sizeof(nodo_valor));
  val->tipo = VALOR_NULO;
  val->val.cadena = nulo;
  a->valor = val;
  a->nlin = nlin;
  a->ncol = ncol;
  return a;
}

ast *latA_numerico(double d, int nlin, int ncol) {
  ast *a = (ast *)malloc(sizeof(ast));
  a->tipo = NODO_VALOR;
  nodo_valor *val = (nodo_valor *)malloc(sizeof(nodo_valor));
  val->tipo = VALOR_NUMERICO;
  val->val.numerico = d;
  a->valor = val;
  a->nlin = nlin;
  a->ncol = ncol;
  return a;
}

ast *latA_cadena(const char *s, int nlin, int ncol) {
  ast *a = (ast *)malloc(sizeof(ast));
  a->tipo = NODO_VALOR;
  nodo_valor *val = (nodo_valor *)malloc(sizeof(nodo_valor));
  val->tipo = VALOR_CADENA;
  val->val.cadena = analizar_fmt(s, strlen(s));
  a->valor = val;
  a->nlin = nlin;
  a->ncol = ncol;
  return a;
}

ast *latA_literal(const char *s, int nlin, int ncol) {
  ast *a = (ast *)malloc(sizeof(ast));
  a->tipo = NODO_VALOR;
  nodo_valor *val = (nodo_valor *)malloc(sizeof(nodo_valor));
  val->tipo = VALOR_CADENA;
  val->val.cadena = analizar_fmt(s, strlen(s));
  a->valor = val;
  a->nlin = nlin;
  a->ncol = ncol;
  return a;
}

ast *latA_var(const char *s, int nlin, int ncol, bool esconst) {
  ast *a = (ast *)malloc(sizeof(ast));
  a->tipo = NODO_IDENTIFICADOR;
  nodo_valor *val = (nodo_valor *)malloc(sizeof(nodo_valor));
  val->tipo = VALOR_CADENA;
  val->val.cadena = strdup(s);
  val->esconst = esconst;
  a->valor = val;
  a->nlin = nlin;
  a->ncol = ncol;
  /* debug eliminado: latA_var */
  return a;
}

ast *latA_asign(ast *val, ast *sim) {
  return latA_nodo(NODO_ASIGNACION, val, sim, sim ? sim->nlin : 0,
                   sim ? sim->ncol : 0);
}

ast *latA_asign_le(ast *exp, ast *id, ast *pos) {
  nodo_lista_elem *a = (nodo_lista_elem *)malloc(sizeof(nodo_lista_elem));
  a->tipo = NODO_LISTA_ASIGNAR_ELEMENTO;
  a->exp = exp;
  a->id = id;
  a->pos = pos;
  return (ast *)a;
}

ast *latA_si(ast *cond, ast *th, ast *el) {
  nodo_si *a = (nodo_si *)malloc(sizeof(nodo_si));
  a->tipo = NODO_SI;
  a->cond = cond;
  a->entonces = th;
  a->_sino = el;
  return (ast *)a;
}

ast *latA_mientras(ast *cond, ast *stmts) {
  return latA_nodo(NODO_MIENTRAS, cond, stmts, cond->nlin, cond->ncol);
}

ast *latA_hacer(ast *cond, ast *stmts) {
  return latA_nodo(NODO_REPETIR, cond, stmts, cond->nlin, cond->ncol);
}

ast *latA_desde(ast *dec, ast *cond, ast *inc, ast *stmts) {
  ast *tmp = stmts;
  while (tmp->der != NULL) {
    tmp = tmp->der;
  }
  tmp->der = latA_nodo(NODO_BLOQUE, inc, NULL, inc->nlin, inc->ncol);
  return latA_nodo(NODO_BLOQUE, dec, latA_mientras(cond, stmts),
                   dec->nlin, dec->ncol);
}

static bool rango_literal(const ast *nodo, double *numero) {
  bool negativo = nodo->tipo == NODO_MENOS_UNARIO;
  if (negativo || nodo->tipo == NODO_MAS_UNARIO) nodo = nodo->izq;
  if (!nodo || nodo->tipo != NODO_VALOR || nodo->valor->tipo != VALOR_NUMERICO)
    return false;
  *numero = negativo ? -nodo->valor->val.numerico : nodo->valor->val.numerico;
  return isfinite(*numero);
}

ast *latA_para(ast *identificador, ast *inicio, ast *fin, ast *incremento,
               ast *sentencias) {
  double primero = 0, ultimo = 0, paso = 1;
  bool valido = (!inicio || rango_literal(inicio, &primero)) &&
                rango_literal(fin, &ultimo) &&
                (!incremento || rango_literal(incremento, &paso)) && paso != 0;
  latA_destruir(inicio);
  latA_destruir(fin);
  latA_destruir(incremento);
  if (!valido) {
    latA_destruir(identificador);
    latA_destruir(sentencias);
    return NULL;
  }
  int linea = identificador->nlin, columna = identificador->ncol;
  const char *nombre = identificador->valor->val.cadena;
  bool constante = identificador->valor->esconst;
  ast *asign = latA_asign(latA_numerico(primero, linea, columna), identificador);
  ast *cond = latA_nodo(paso > 0 ? NODO_MENOR_QUE : NODO_MAYOR_QUE,
                       latA_var(nombre, linea, columna, constante),
                       latA_numerico(ultimo, linea, columna), linea, columna);
  ast *suma = latA_nodo(NODO_SUMA, latA_var(nombre, linea, columna, constante),
                       latA_numerico(paso, linea, columna), linea, columna);
  ast *inc = latA_asign(suma, latA_var(nombre, linea, columna, constante));
  return latA_desde(asign, cond, inc, sentencias);
}

ast *latA_funcion(ast *nombre, ast *params, ast *stmts, int nlin, int ncol) {
  nodo_funcion *a = (nodo_funcion *)malloc(sizeof(ast));
  a->tipo = NODO_FUNCION_USUARIO;
  a->nombre = nombre;
  a->params = params;
  a->stmts = stmts;
  a->nlin = nlin;
  a->ncol = ncol;
  return (ast *)a;
}

ast *latA_clase(ast *nombre, ast *base, ast *stmts, int nlin, int ncol) {
  nodo_clase *a = (nodo_clase *)malloc(sizeof(ast));
  a->tipo = NODO_CLASE;
  a->nombre = nombre;
  a->base = base;
  a->stmts = stmts;
  a->nlin = nlin;
  a->ncol = ncol;
  return (ast *)a;
}

/**
 * \brief Crea un nodo AST para instanciación de clase
 *
 * Esta función crea un nodo que representa la creación de una nueva
 * instancia de una clase usando la palabra clave 'nueva'.
 *
 * Ejemplo en Latino:
 *   persona = nueva Persona("Juan", 25)
 *
 * \param nombre AST del nombre de la clase a instanciar
 * \param args AST de los argumentos para el constructor (puede ser NULL)
 * \param nlin Número de línea en el código fuente
 * \param ncol Número de columna en el código fuente
 * \return Puntero al nodo AST creado
 */
ast *latA_nueva(ast *nombre, ast *args, int nlin, int ncol) {
  nodo_nueva *a = (nodo_nueva *)malloc(sizeof(nodo_nueva));
  a->tipo = NODO_NUEVA;
  a->nombre = nombre;
  a->args = args;
  a->nlin = nlin;
  a->ncol = ncol;
  return (ast *)a;
}

/**
 * \brief Crea un nodo AST para referencia a la instancia actual
 *
 * Esta función crea un nodo que representa la palabra clave 'mi',
 * que es equivalente a 'this' o 'self' en otros lenguajes.
 * Se usa dentro de métodos de clase para referirse a la instancia actual.
 *
 * Ejemplo en Latino:
 *   mi.nombre = "Juan"
 *   escribir(mi.edad)
 *
 * \param nlin Número de línea en el código fuente
 * \param ncol Número de columna en el código fuente
 * \return Puntero al nodo AST creado
 */
ast *latA_mi(int nlin, int ncol) {
  ast *a = latA_nodo(NODO_MI, NULL, NULL, nlin, ncol);
  return a;
}

/**
 * \brief Crea un nodo AST para llamada a la clase padre
 *
 * Esta función crea un nodo que representa la palabra clave 'super()',
 * que se usa para llamar al constructor o métodos de la clase padre
 * en una jerarquía de herencia.
 *
 * Ejemplo en Latino:
 *   clase Empleado hereda Persona
 *       constructor(nombre, cargo)
 *           super(nombre)  // Llama al constructor de Persona
 *           mi.cargo = cargo
 *       fin
 *   fin
 *
 * \param args AST de los argumentos para pasar al constructor padre
 * \param nlin Número de línea en el código fuente
 * \param ncol Número de columna en el código fuente
 * \return Puntero al nodo AST creado
 */
ast *latA_super(ast *args, int nlin, int ncol) {
  ast *a = latA_nodo(NODO_SUPER, args, NULL, nlin, ncol);
  return a;
}

void latA_destruir(ast *a) {
  if (a) {
      switch (a->tipo) {
      case NODO_SI: {
        nodo_si *nsi = (nodo_si *)a;
        latA_destruir(nsi->cond);
        latA_destruir(nsi->entonces);
        if (nsi->_sino)
          latA_destruir(nsi->_sino);
        break;
      }
      case NODO_FUNCION_USUARIO: {
        nodo_funcion *fun = (nodo_funcion *)a;
        if (fun->params)
          latA_destruir(fun->params);
        if (fun->stmts)
          latA_destruir(fun->stmts);
        latA_destruir(fun->nombre);
        break;
      }
      case NODO_LISTA_ASIGNAR_ELEMENTO: {
        nodo_lista_elem *nelem = (nodo_lista_elem *)a;
        latA_destruir(nelem->exp);
        latA_destruir(nelem->id);
        latA_destruir(nelem->pos);
        break;
      }
      case NODO_CLASE: {
        nodo_clase *clase = (nodo_clase *)a;
        latA_destruir(clase->nombre);
        latA_destruir(clase->base);
        latA_destruir(clase->stmts);
        break;
      }
      case NODO_NUEVA: {
        nodo_nueva *instancia = (nodo_nueva *)a;
        latA_destruir(instancia->nombre);
        latA_destruir(instancia->args);
        break;
      }
      case NODO_IDENTIFICADOR:
      case NODO_VALOR:
        if (a->valor) {
          if (a->valor->tipo == VALOR_CADENA) {
            free(a->valor->val.cadena);
          }
          free(a->valor);
        }
        break;
      case NODO_ASIGNACION: {
        /* Las asignaciones compuestas comparten el destino con su operación. */
        ast *operacion = a->izq;
        bool compuesto = operacion &&
            ((operacion->tipo >= NODO_SUMA && operacion->tipo <= NODO_MODULO) ||
             operacion->tipo == NODO_CONCATENAR) && operacion->izq == a->der;
        latA_destruir(operacion);
        if (!compuesto) latA_destruir(a->der);
        break;
      }
      default:
        if (a->izq)
          latA_destruir(a->izq);
        if (a->der)
          latA_destruir(a->der);
      }
      free(a);
  }
}

/**
 * latA_analizar_exp: Esta funcion crea el arbol abstracto de sintaxis a partir
 *de una cadena REM: liberar el nodo cuando se deje de ocupar
 **/
ast *latA_analizar_exp(char *expr, int *status) {
  ast *nodo = NULL;
  yyscan_t scanner;
  YY_BUFFER_STATE state;
  lex_state scan_state = {.insert = 0};
  *status = 1;
  /* Flex convierte strlen a int y suma dos bytes de terminacion. */
  if (expr == NULL || strlen(expr) > INT_MAX - 2 ||
      yylex_init_extra(&scan_state, &scanner) != 0) {
    return NULL;
  }
  state = yy_scan_string(expr, scanner);
  *status = yyparse(&nodo, scanner);
  yy_delete_buffer(state, scanner);
  yylex_destroy(scanner);
  return nodo;
}

/**
 * latA_analizar_exp: Esta funcion crea el arbol abstracto de sintaxis a partir
 *de un archivo REM: liberar el nodo cuando se deje de ocupar
 **/
ast *latA_analizar_arch(char *infile, int *status) {
  *status = 1;
  if (infile == NULL) {
    fputs("mictlan: especifique un archivo.\n", stderr);
    return NULL;
  }
  const char *extension = strrchr(infile, '.');
  if (!extension || (strcmp(extension, ".mict") && strcmp(extension, ".lat"))) {
    fprintf(stderr, "mictlan: El archivo '%s' debe tener extension .mict o .lat\n", infile);
    return NULL;
  }
  FILE *file = fopen(infile, "r");
  if (file == NULL) {
    fprintf(stderr, "mictlan: No se pudo abrir el archivo: %s\n", infile);
    return NULL;
  }
  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    fprintf(stderr, "mictlan: no se pudo leer '%s' como archivo de programa.\n", infile);
    return NULL;
  }
  long longitud = ftell(file);
  if (longitud < 0 || longitud > INT_MAX - 2 ||
      fseek(file, 0, SEEK_SET) != 0) {
    fclose(file);
    fprintf(stderr, "mictlan: no se pudo determinar un tamaño válido para '%s'.\n", infile);
    return NULL;
  }
  size_t fsize = (size_t)longitud;
  char *buffer = malloc(fsize + 1);
  if (buffer == NULL) {
    fclose(file);
    fprintf(stderr, "mictlan: no se pudieron reservar %zu bytes para '%s'.\n", fsize + 1, infile);
    return NULL;
  }
  size_t newSize = fread(buffer, 1, fsize, file);
  if (ferror(file)) {
    free(buffer);
    fclose(file);
    fprintf(stderr, "mictlan: error al leer '%s'.\n", infile);
    return NULL;
  }
  fclose(file);
  if (memchr(buffer, '\0', newSize)) {
    free(buffer);
    fprintf(stderr, "mictlan: '%s' contiene un byte nulo. Usa un archivo de texto.\n", infile);
    return NULL;
  }
  buffer[newSize] = '\0';
  char *archivo_previo = filename;
  filename = infile;
  ast *nodo = latA_analizar_exp(buffer, status);
  filename = archivo_previo;
  free(buffer);
  return nodo;
}
