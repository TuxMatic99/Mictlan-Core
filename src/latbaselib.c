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
    mv->nombre_archivo = archivo;
    lat_objeto *programa = latC_analizar(mv, nodo);
    estado = latC_llamar_funcion(mv, programa);
    latO_destruir(mv, programa);
    latA_destruir(nodo);
    mv->nombre_archivo = anterior;
    if (estado != 0) latD_lanzar(mv, estado);
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

static void base_leer(lat_mv *mv) {
    // Lectura robusta de línea con manejo correcto por plataforma
#if (defined __WIN32__) || (defined _WIN32)
    char *buf = calloc(1, MAX_INPUT_SIZE);
    if (!buf) {
        latC_apilar(mv, latO_nulo);
        return;
    }

    bool ok = false;
    // Si stdin es consola interactiva, usar ReadConsoleA; si viene por pipe, usar fgets.
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    BOOL is_console = (hStdin != INVALID_HANDLE_VALUE) && GetConsoleMode(hStdin, &mode);
    if (is_console) {
        DWORD readChars = 0;
        if (ReadConsoleA(hStdin, buf, MAX_INPUT_SIZE - 1, &readChars, NULL)) {
            buf[readChars] = '\0';
            ok = true;
        }
    } else {
        ok = latC_leer_linea(buf) != NULL;
    }

    if (!ok) {
        free(buf);
        latC_apilar(mv, latO_nulo);
        return;
    }

    // Normalizar fin de línea para Windows (eliminar CRLF/ LF)
    size_t len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[--len] = '\0';
    }

    char *endptr = NULL;
    double ret = strtod(buf, &endptr);
    lat_objeto *tmp = NULL;
    // Si toda la cadena es numérica y no está vacía, regresar número; de lo contrario, cadena.
    if (endptr && *endptr == '\0' && buf[0] != '\0') {
        tmp = latC_crear_numerico(mv, ret);
    } else {
        tmp = latC_crear_cadena(mv, buf);
    }
    latC_apilar(mv, tmp);
    free(buf);
#else
    char *line = latC_leer_linea(NULL);
    if (!line) {
        latC_apilar(mv, latO_nulo);
        return;
    }
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    char *endptr = NULL;
    double ret = strtod(line, &endptr);
    lat_objeto *tmp = NULL;
    if (endptr && *endptr == '\0' && line[0] != '\0') {
        tmp = latC_crear_numerico(mv, ret);
    } else {
        tmp = latC_crear_cadena(mv, line);
    }
    latC_apilar(mv, tmp);
    free(line);
#endif
}

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
    // FIXME: Error cuando la cadena contiene separadores de miles (,)
    lat_objeto *o = latC_desapilar(mv);
    double var = latC_adouble(mv, o);
    if (!var) {
        latC_apilar(mv, latO_nulo);
        return;
    }
    lat_objeto *tmp = latC_crear_numerico(mv, var);
    latC_apilar(mv, tmp);
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
    {"alogico", base_alogico, 1},
    {"incluir", base_incluir, 1},
    {"leer", base_leer, 0},
    {"limpiar", base_limpiar, 0},
    {"tipo", base_tipo, 1},
    {"imprimirf", base_imprimirf, FUNCION_VAR_ARGS},
    {"error", base_error, FUNCION_VAR_ARGS},
    {"beep", base_beep, 0},
    {NULL, NULL, 0}};

void latC_abrir_liblatino_baselib(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_BASE_NAME, libbase);
}