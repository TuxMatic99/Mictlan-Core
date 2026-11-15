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

#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(__WIN32__)
#include <io.h>
#endif

#include "latino.h"
#include "latdo.h"
#include "latmem.h"
#include "laterror.h"

#define LIB_BASE_NAME ""

char *analizar_fmt(const char *s, size_t len);
char *analizar(const char *s, size_t len);

static bool eslegible(const char *archivo) {
    FILE *f = fopen(archivo, "r");
    if (f == NULL) {
        return false;
    }
    fclose(f);
    return true;
}

char *tipo(int tipo) {
    switch (tipo) {
        case T_NULL:
            return "nulo";
            break;
        case T_BOOL:
            return "logico";
            break;
        case T_NUMERIC:
            return "decimal";
            break;
        case T_STR:
            return "cadena";
            break;
        case T_LIST:
            return "lista";
            break;
        case T_DIC:
            return "diccionario";
            break;
        case T_FUN:
            return "fun";
            break;
        case T_CFUN:
            return "cfun";
            break;
        case T_CPTR:
            return "(void *)";
            break;
        default:
            return "indefinido";
            break;
    }
}

void base_poner(lat_mv *mv) {
    lat_objeto *o = latC_desapilar(mv);
    latO_imprimir(mv, o, false);
    printf("\n");
}

void str_formato(lat_mv *mv);

void base_imprimirf(lat_mv *mv) {
    // FIXME: Imprimir todos los formatos de C / C++
    str_formato(mv);
    lat_objeto *str = latC_desapilar(mv);
    latO_imprimir(mv, str, true);
}

struct incluir_programa {
    ast *nodo;
    lat_objeto *func;
    int estado;
};

static void incluir_compilar_ejecutar(lat_mv *mv, void *datos) {
    struct incluir_programa *p = datos;
    p->func = latC_analizar(mv, p->nodo);
    p->estado = latC_llamar_funcion(mv, p->func);
}

static bool incluir_fuente(lat_mv *mv, const char *prefijo, const char *nombre,
                            const char *extension) {
    char ruta[MAX_PATH_LENGTH];
    int n = snprintf(ruta, sizeof(ruta), "%s%s%s", prefijo, nombre, extension);
    if (n < 0 || (size_t)n >= sizeof(ruta))
        latC_error(mv, "Ruta de módulo demasiado larga");
    if (!eslegible(ruta)) return false;

    /* El nombre queda en la VM porque lo usa el bytecode de las funciones
       del módulo incluso después de liberar su árbol. */
    lat_objeto *temporal = latC_crear_cadena(mv, ruta);
    char *archivo = latC_checar_cadena(mv, temporal);
    latM_liberar(mv, temporal);
    int estado;
    ast *nodo = latA_analizar_arch(archivo, &estado);
    if (estado != 0 || !nodo) {
        latA_destruir(nodo);
        latC_error(mv, "No se pudo analizar el módulo '%s'", ruta);
    }
    char *anterior = mv->nombre_archivo;
    bool repl = mv->global->REPL;
    int linea = mv->nlin, columna = mv->ncol;
    mv->nombre_archivo = archivo;
    mv->global->REPL = false;
    struct incluir_programa p = {nodo, NULL, 0};
    struct lat_longjmp guard;
    estado = latE_proteger(mv, &guard, incluir_compilar_ejecutar, &p);
    if (p.func) latO_destruir(mv, p.func);
    latA_destruir(nodo);
    mv->nombre_archivo = anterior;
    mv->global->REPL = repl;
    mv->nlin = linea;
    mv->ncol = columna;
    if (estado || p.estado) latD_lanzar(mv, estado ? estado : p.estado);
    return true;
}

static void base_incluir(lat_mv *mv) {
    const char *nombre = latC_checar_cadena(mv, latC_desapilar(mv));
    const char *sufijo = strrchr(nombre, '.');
    bool explicito = sufijo && (!strcmp(sufijo, ".mict") || !strcmp(sufijo, ".lat"));
    if (strlen(nombre) >= MAX_PATH_LENGTH)
        latC_error(mv, "Nombre de módulo demasiado largo");

    const char *archivo = mv->nombre_archivo ? mv->nombre_archivo : "";
    const char *sep = strrchr(archivo, PATH_SEP[0]);
    char directorio[MAX_PATH_LENGTH] = "";
    if (sep) {
        size_t longitud = (size_t)(sep - archivo) + 1;
        if (longitud >= sizeof(directorio))
            latC_error(mv, "Directorio del programa demasiado largo");
        memcpy(directorio, archivo, longitud);
        directorio[longitud] = '\0';
    }
    const char *lib = getenv("LATINO_LIB");
    char dir_lib[MAX_PATH_LENGTH] = "";
    if (lib && *lib) {
        int longitud = snprintf(dir_lib, sizeof(dir_lib), "%s%s", lib, PATH_SEP);
        if (longitud < 0 || (size_t)longitud >= sizeof(dir_lib))
            latC_error(mv, "LATINO_LIB demasiado largo");
    }
    const char *directorios[] = {"", directorio, dir_lib};
    /* Agotar las ubicaciones heredadas de .lat antes de buscar .mict. */
    const char *extensiones[] = {explicito ? "" : ".lat", ".mict"};
    for (size_t e = 0; e < (explicito ? 1u : 2u); ++e) {
        if (incluir_fuente(mv, "", nombre, extensiones[e])) return;
        bool absoluta = nombre[0] == '/' || nombre[0] == PATH_SEP[0];
#ifdef _WIN32
        absoluta |= nombre[0] != '\0' && nombre[1] == ':';
#endif
        if (absoluta) continue;
        for (size_t i = 1; i < sizeof(directorios) / sizeof(*directorios); ++i)
            if (*directorios[i] && incluir_fuente(mv, directorios[i], nombre, extensiones[e])) return;
    }
    if (explicito) latC_error(mv, "No se encontró el módulo '%s'", nombre);

    char libpath[MAX_PATH_LENGTH];
    char funcname[MAX_ID_LENGTH];
#if defined(_WIN32)
    const char *prefijo = "latino-", *extension = ".dll";
#elif defined(__MSYS__)
    const char *prefijo = "msys-latino-", *extension = ".dll";
#elif __APPLE__
    const char *prefijo = "liblatino-", *extension = ".dylib";
#else
    const char *prefijo = "liblatino-", *extension = ".so";
#endif
    int n = snprintf(libpath, sizeof(libpath), "%s%s%s", prefijo, nombre, extension);
    int m = snprintf(funcname, sizeof(funcname), "latC_abrir_liblatino_%s", nombre);
    if (n < 0 || (size_t)n >= sizeof(libpath) || m < 0 || (size_t)m >= sizeof(funcname))
        latC_error(mv, "Nombre de módulo nativo demasiado largo");
    if (latC_cargarlib(mv, libpath, funcname) != 0)
        latC_error(mv, "Error al cargar libreria dinamica '%s'", libpath);
}

enum entrada_estado { ENTRADA_OK, ENTRADA_EOF, ENTRADA_IO,
                      ENTRADA_MEMORIA, ENTRADA_LIMITE, ENTRADA_NUL };
enum entrada_modo { ENTRADA_HEREDADA, ENTRADA_TEXTO, ENTRADA_NUMERO };
#define ENTRADA_MAX_TEXTO (8u * 1024u * 1024u)

/* Nunca publica un prefijo de una línea fallida. Después de un error de
   tamaño/NUL/reserva consume hasta LF si la fuente permite continuar. */
static char *entrada_linea(size_t limite, enum entrada_estado *estado) {
    size_t capacidad = 256, usados = 0;
    char *buf = malloc(capacidad);
    *estado = buf ? ENTRADA_OK : ENTRADA_MEMORIA;
    bool hay_bytes = false, lf = false;
    for (;;) {
        errno = 0;
        int c = fgetc(stdin);
        if (c == EOF) {
            if (ferror(stdin)) {
                int causa = errno;
                clearerr(stdin);
                if (causa == EINTR) continue;
                *estado = ENTRADA_IO;
            } else if (!hay_bytes && *estado == ENTRADA_OK) {
                *estado = ENTRADA_EOF;
            }
            break;
        }
        hay_bytes = true;
        if (c == '\n') { lf = true; break; }
        if (*estado != ENTRADA_OK) continue;
        if (c == 0) { *estado = ENTRADA_NUL; continue; }
        /* Un byte extra permite CRLF con exactamente limite bytes útiles. */
        if (usados == limite + 1) { *estado = ENTRADA_LIMITE; continue; }
        if (usados + 1 >= capacidad) {
            size_t nueva = capacidad > (limite + 2) / 2
                               ? limite + 2 : capacidad * 2;
            char *tmp = realloc(buf, nueva);
            if (!tmp) { *estado = ENTRADA_MEMORIA; continue; }
            buf = tmp;
            capacidad = nueva;
        }
        buf[usados++] = (char)c;
    }
    if (*estado == ENTRADA_OK) {
        if (lf && usados && buf[usados - 1] == '\r') --usados;
        if (usados > limite) *estado = ENTRADA_LIMITE;
    }
    if (*estado != ENTRADA_OK) { free(buf); return NULL; }
    buf[usados] = '\0';
    return buf;
}

static double numero_desde_texto(lat_mv *mv, const char *inicio);
struct entrada_publicacion { char *texto; enum entrada_modo modo; };
static void entrada_publicar(lat_mv *mv, void *datos) {
    struct entrada_publicacion *p = datos;
    if (p->modo == ENTRADA_NUMERO) {
        latC_apilar_double(mv, numero_desde_texto(mv, p->texto));
    } else if (p->modo == ENTRADA_TEXTO) {
        latC_apilar_string(mv, p->texto);
    } else {
        char *fin;
        /* La inferencia histórica conserva el locale del consumidor C. */
        double valor = strtod(p->texto, &fin);
        if (*p->texto && *fin == '\0') latC_apilar_double(mv, valor);
        else latC_apilar_string(mv, p->texto);
    }
}

static void entrada_leer(lat_mv *mv, enum entrada_modo modo) {
    enum entrada_estado estado = ENTRADA_OK;
    char *linea;
#if !defined(_WIN32) && !defined(__WIN32__)
    if (modo == ENTRADA_HEREDADA && isatty(fileno(stdin))) {
        /* Mantener edición/eco de terminal; no usar Readline para pipes. */
        errno = 0;
        linea = latC_leer_linea(NULL);
        if (ferror(stdin)) {
            clearerr(stdin);
            free(linea);
            linea = NULL;
            estado = ENTRADA_IO;
        } else if (!linea) {
            estado = errno == ENOMEM ? ENTRADA_MEMORIA : ENTRADA_EOF;
        }
    } else
#endif
    {
        size_t limite = modo == ENTRADA_HEREDADA
                            ? SIZE_MAX - sizeof(lat_cadena) - 2
                            : ENTRADA_MAX_TEXTO;
        linea = entrada_linea(limite, &estado);
    }
    if (!linea) {
        if (estado == ENTRADA_EOF || modo == ENTRADA_HEREDADA) {
            latC_apilar(mv, latO_nulo);
            return;
        }
        const char *causa = estado == ENTRADA_MEMORIA ? "memoria insuficiente" :
                            estado == ENTRADA_LIMITE ? "linea demasiado larga (maximo 8 MiB)" :
                            estado == ENTRADA_NUL ? "byte NUL no permitido" : "error de lectura";
        latC_error(mv, "Entrada: %s", causa);
        return;
    }
    struct entrada_publicacion p = {linea, modo};
    struct lat_longjmp guard;
    int fallo = latE_proteger(mv, &guard, entrada_publicar, &p);
    free(linea);
    if (fallo) latD_lanzar(mv, fallo);
}

static void base_leer(lat_mv *mv) { entrada_leer(mv, ENTRADA_HEREDADA); }
static void base_leer_texto(lat_mv *mv) { entrada_leer(mv, ENTRADA_TEXTO); }
static void base_leer_numero(lat_mv *mv) { entrada_leer(mv, ENTRADA_NUMERO); }

static void base_limpiar(lat_mv *mv) {
    (void)mv;
    system(latC_clear);
}

static void base_tipo(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_string(mv, tipo(a->tipo));
}

static void base_alogico(lat_mv *mv) {
    lat_objeto *o = latC_desapilar(mv);
    lat_objeto *tmp = latC_abool(mv, o) == true ? latO_verdadero : latO_falso;
    latC_apilar(mv, tmp);
}

static void base_anumero(lat_mv *mv) {
    lat_objeto *o = latC_desapilar(mv);
    /* Contrato historico confirmado por tests/alpha/semantica: cero -> nulo.
       La alternativa explicita anumero_estricto distingue cero de error. */
    double x = latC_adouble(mv, o);
    if (!x) latC_apilar(mv, latO_nulo);
    else latC_apilar_double(mv, x);
}

static bool numero_espacio(unsigned char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}
static bool numero_digito(unsigned char c) { return c >= '0' && c <= '9'; }
static double numero_desde_texto(lat_mv *mv, const char *inicio) {
    const char *p = inicio;
    double x;
    bool mantisa_no_cero = false;
    while (numero_espacio((unsigned char)*p)) ++p;
    if (*p == '+' || *p == '-') ++p;
    bool digitos = false;
    while (numero_digito((unsigned char)*p)) {
        digitos = true; mantisa_no_cero |= *p != '0'; ++p;
    }
    if (*p == '.') {
        ++p;
        while (numero_digito((unsigned char)*p)) {
            digitos = true; mantisa_no_cero |= *p != '0'; ++p;
        }
    }
    if (!digitos) latC_error(mv, "Conversion numerica estricta: sintaxis invalida");
    if (*p == 'e' || *p == 'E') {
        ++p; if (*p == '+' || *p == '-') ++p;
        if (!numero_digito((unsigned char)*p))
            latC_error(mv, "Conversion numerica estricta: sintaxis invalida");
        while (numero_digito((unsigned char)*p)) ++p;
    }
    const char *fin_numero = p;
    while (numero_espacio((unsigned char)*p)) ++p;
    if (*p) latC_error(mv, "Conversion numerica estricta: sintaxis invalida");
    char *fin;
    x = latC_strtod_c(inicio, &fin);
    if (fin != fin_numero)
        latC_error(mv, "Conversion numerica estricta: sintaxis invalida");
    if (!isfinite(x) || (x == 0 && mantisa_no_cero))
        latC_error(mv, "Conversion numerica estricta: fuera de rango");
    return x;
}

static void base_anumero_estricto(lat_mv *mv) {
    lat_objeto *o = latC_desapilar(mv);
    double x;
    if (o->tipo == T_STR) x = numero_desde_texto(mv, latC_checar_cadena(mv, o));
    else if (o->tipo == T_NUMERIC) {
        x = getNumerico(o);
        if (!isfinite(x)) latC_error(mv, "Conversion numerica estricta: fuera de rango");
    } else {
        latC_error(mv, "Conversion numerica estricta: tipo invalido");
        return;
    }
    latC_apilar_double(mv, x);
}

static void base_acadena(lat_mv *mv) {
    lat_objeto *o = latC_desapilar(mv);
    char *buf = latC_astring(mv, o);
    lat_objeto *tmp = latC_crear_cadena(mv, buf);
    latC_apilar(mv, tmp);
    free(buf);
}

static void base_error(lat_mv *mv) {
    str_formato(mv);
    lat_objeto *error = latC_desapilar(mv);
    latC_error(mv, "%s", latC_checar_cadena(mv, error));
}

static void base_beep(lat_mv *mv) {
    (void)mv;
    fprintf(stderr, "\x7");
    fflush(stderr);
}

static const lat_CReg libbase[] = {
    {"poner", base_poner, 1},
    {"escribir", base_poner, 1},
    {"imprimir", base_poner, 1},
    {"acadena", base_acadena, 1},
    {"anumero", base_anumero, 1},
    {"anumero_estricto", base_anumero_estricto, 1},
    {"alogico", base_alogico, 1},
    {"incluir", base_incluir, 1},
    {"leer", base_leer, 0},
    {"leer_texto", base_leer_texto, 0},
    {"leer_numero", base_leer_numero, 0},
    {"limpiar", base_limpiar, 0},
    {"tipo", base_tipo, 1},
    {"imprimirf", base_imprimirf, FUNCION_VAR_ARGS},
    {"error", base_error, FUNCION_VAR_ARGS},
    {"beep", base_beep, 0},
    {NULL, NULL, 0}};

void latC_abrir_liblatino_baselib(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_BASE_NAME, libbase);
}