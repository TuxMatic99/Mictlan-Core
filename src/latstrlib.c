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

#include "latino.h"
#include "latgc.h"
#include "latmem.h"
#include "latpropiedad.h"
#include <limits.h>
#include <stdint.h>

#define LIB_CADENA_NAME "cadena"

static int oct(int o) {
    int d = 0, i = 0;
    do {
        d += (o % 10) * pow(8, i);
        ++i;
        o /= 10;
    } while (o != 0);
    return d;
}

char *analizar_fmt(const char *s, size_t len) {
    char *ret = malloc(len + 1);
    int i = 0, j = 0, let = 0, c = 48;
    for (i = 0; i < ((int)len); i++) {
        switch (s[i]) {
            case '\\': {
                switch (s[i + 1]) {
                    case '/':
                        c = '/';
                        i++;
                        goto save;
                    case '"':
                        c = '\"';
                        i++;
                        goto save;
                    case (int)39:
                        c = '\'';
                        i++;
                        goto save;
                    case 'a':
                        c = '\a';
                        i++;
                        goto save;
                    case 'b':
                        c = '\b';
                        i++;
                        goto save;
                    case 'f':
                        c = '\f';
                        i++;
                        goto save;
                    case 'n':
                        c = '\n';
                        i++;
                        goto save;
                    case 'r':
                        c = '\r';
                        i++;
                        goto save;
                    case 't':
                        c = '\t';
                        i++;
                        goto save;
                    case 'v':
                        c = '\v';
                        i++;
                        goto save;
                    case '\\':
                        c = '\\';
                        i++;
                        goto save;
                    case 'u':
                        c = s[i];
                        ret[j] = c;
                        j++;
                        i++;
                        int k;
                        for (k = 0; k <= 4; k++) {
                            c = s[i];
                            ret[j] = c;
                            j++;
                            i++;
                        }
                    default:
                        if (isdigit(s[i + 1])) {
                            while (isdigit(s[i + 1])) {
                                let = (10 * let) + ((int)s[i + 1] - 48);
                                i += 1;
                            };
                            c = oct(let);
                            let = 0;
                            goto save;
                        } else {
                            c = s[i];
                            break;
                        }
                }
            } break;
            default:
                c = s[i];
                break;
        }
    save:
        ret[j] = c;
        j++;
    }
    ret[j] = '\0';
    return ret;
}

char *analizar(const char *s, size_t len) {
    char *ret = malloc(len + 1);
    int i = 0;
    int j = 0;
    int c = '@';
    for (i = 0; i < ((int)len); i++) {
        switch (s[i]) {
            case '\\': {
                switch (s[i + 1]) {
                    case '\\':
                        c = '\\';
                        i++;
                        ret[j] = c;
                        j++;
                        c = '\\';
                        i++;
                        ret[j] = c;
                        j++;
                    case 'u':
                        c = s[i];
                        ret[j] = c;
                        j++;
                        i++;
                        int k;
                        for (k = 0; k <= 4; k++) {
                            c = s[i];
                            ret[j] = c;
                            j++;
                            i++;
                        }
                    default:
                        c = s[i];
                        break;
                }
            } break;
            default:
                c = s[i];
                break;
        }
        ret[j] = c;
        j++;
    }
    ret[j] = '\0';
    return ret;
}

char *decimal_acadena(double d) {
    char *buffer = calloc(1, 64);
    snprintf(buffer, 64, LAT_NUMERIC_FMT, d);
    return buffer;
}

char *entero_acadena(int i) {
    char *buffer = calloc(1, 64);
    snprintf(buffer, 64, "%i", i);
    return buffer;
}

char *logico_acadena(int i) {
    char *buffer = malloc(10);
    if (i)
        snprintf(buffer, 10, "%s", "verdadero");
    else
        snprintf(buffer, 10, "%s", "falso");
    buffer[9] = '\0';
    return buffer;
}

bool inicia_con(const char *base, const char *str) {
    /* strstr puede devolver NULL: no restar punteros sin coincidencia. */
    return strncmp(base, str, strlen(str)) == 0;
}

bool termina_con(char *base, char *str) {
    int blen = strlen(base);
    int slen = strlen(str);
    return (blen >= slen) && (!strcmp(base + blen - slen, str));
}

int intercambiar_pos(char *base, char *str, int startIndex) {
    int result;
    int baselen = strlen(base);
    if ((int)strlen(str) > baselen || startIndex > baselen) {
        result = -1;
    } else {
        if (startIndex < 0) {
            startIndex = 0;
        }
        char *pos = strstr(base + startIndex, str);
        if (pos == NULL) {
            result = -1;
        } else {
            result = pos - base;
        }
    }
    return result;
}

int pos(char *base, char *str) { return intercambiar_pos(base, str, 0); }

int ultima_pos(char *base, char *str) {
    if (!*str) return (int)strlen(base);
    const char *p = base, *hallada;
    int ultima = -1;
    while ((hallada = strstr(p, str)) != NULL) {
        ultima = (int)(hallada - base);
        p = hallada + 1; /* Conservar coincidencias solapadas. */
    }
    return ultima;
}

char *insertar(char *dest, char *src, int pos) {
    size_t a = strlen(dest), b = strlen(src);
    if (pos < 0) {
        if ((size_t)(-(long long)pos) > a) return NULL;
        pos = (int)(a + pos);
    }
    size_t inicio = (size_t)pos > a ? a : (size_t)pos;
    if (b > SIZE_MAX - a - 1) return NULL;
    char *ret = malloc(a + b + 1);
    if (!ret) return NULL;
    memcpy(ret, dest, inicio);
    memcpy(ret + inicio, src, b);
    memcpy(ret + inicio + b, dest + inicio, a - inicio + 1);
    return ret;
}

static char *rellenar_bytes(char *base, char *c, int n, bool izquierda) {
    if (n < 0) return NULL;
    size_t len = strlen(base), unidad = strlen(c);
    /* La repeticion incluye el desfase historico de uno. */
    size_t rep = (size_t)n >= len ? (size_t)n - len + 1 : 0;
    if (unidad && rep > (SIZE_MAX - len - 1) / unidad) return NULL;
    size_t relleno = rep * unidad;
    char *ret = malloc(len + relleno + 1);
    if (!ret) return NULL;
    size_t offset = izquierda ? 0 : len;
    if (!izquierda) memcpy(ret, base, len);
    if (unidad) for (size_t i = 0; i < rep; ++i) {
        memcpy(ret + offset, c, unidad);
        offset += unidad;
    }
    if (izquierda) memcpy(ret + relleno, base, len);
    ret[len + relleno] = '\0';
    return ret;
}

char *rellenar_izquierda(char *base, char *c, int n) {
    return rellenar_bytes(base, c, n, true);
}

char *rellenar_derecha(char *base, char *c, int n) {
    return rellenar_bytes(base, c, n, false);
}

char *reemplazar(char *o_string, char *s_string, char *r_string) {
    const char *hit = strstr(o_string, s_string);
    size_t original = strlen(o_string), patron = strlen(s_string), nuevo = strlen(r_string);
    size_t prefijo = hit ? (size_t)(hit - o_string) : original;
    size_t resto = hit ? original - prefijo - patron : 0;
    if (hit && nuevo > SIZE_MAX - prefijo - resto - 1) return NULL;
    size_t len = hit ? prefijo + nuevo + resto : original;
    char *ret = malloc(len + 1);
    if (!ret) return NULL;
    memcpy(ret, o_string, prefijo);
    if (hit) {
        memcpy(ret + prefijo, r_string, nuevo);
        memcpy(ret + prefijo + nuevo, hit + patron, resto);
    }
    ret[len] = '\0';
    return ret;
}

char *subcadena(const char *str, int beg, int n) {
    size_t len = strlen(str);
    if (beg < 0 || n < 0 || (size_t)beg > len) return NULL;
    size_t cantidad = (size_t)n < len - (size_t)beg ? (size_t)n : len - (size_t)beg;
    char *ret = malloc(cantidad + 1);
    if (!ret) return NULL;
    memcpy(ret, str + beg, cantidad);
    ret[cantidad] = '\0';
    return ret;
}

char *minusculas(const char *str) {
    size_t len = strlen(str);
    char *ret = malloc(len + 1);
    if (!ret) return NULL;
    for (size_t i = 0; i < len; ++i) ret[i] = (char)tolower((unsigned char)str[i]);
    ret[len] = '\0';
    return ret;
}

char *mayusculas(const char *str) {
    size_t len = strlen(str);
    char *ret = malloc(len + 1);
    if (!ret) return NULL;
    for (size_t i = 0; i < len; ++i) ret[i] = (char)toupper((unsigned char)str[i]);
    ret[len] = '\0';
    return ret;
}

char *quitar_espacios(const char *str) {
    const char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    const char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)end[-1])) end--;
    size_t len = (size_t)(end - start);
    char *ret = malloc(len + 1);
    if (ret) {
        memcpy(ret, start, len);
        ret[len] = '\0';
    }
    return ret;
}

lat_cadena *latO_cadenaNueva(lat_mv *mv, const char *str, size_t l);

/* Validar antes de reservar: latC_error abandona el marco mediante longjmp. */
static int cadena_entero(lat_mv *mv, lat_objeto *o) {
    double valor = latC_checar_numerico(mv, o);
    if (!isfinite(valor) || valor < INT_MIN || valor > INT_MAX)
        latC_error(mv, "Numero fuera de rango para cadena");
    return (int)valor; /* Truncamiento heredado hacia cero. */
}

static void cadena_publicar(lat_mv *mv, char *buffer) {
    if (!buffer) { latC_error(mv, "Memoria virtual agotada"); return; }
    latC_apilar_string(mv, buffer); /* La VM interna una copia, no toma el buffer. */
    free(buffer);
}

static const char *cadena_texto_prestado(const lat_objeto *o, char **propio,
                                         const char **error) {
    if (o->tipo == T_STR || o->tipo == T_LABEL) return getstr(getCadena(o));
    *propio = latP_texto(o, error);
    return *propio;
}

void str_concatenar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    char *propio_a = NULL, *propio_b = NULL;
    const char *error = NULL;
    const char *s1 = cadena_texto_prestado(a, &propio_a, &error);
    const char *s2 = s1 ? cadena_texto_prestado(b, &propio_b, &error) : NULL;
    if (!s1 || !s2) {
        free(propio_a); free(propio_b);
        latC_error(mv, "%s", error ? error : "Error al representar objeto");
        return;
    }
    size_t n1 = strlen(s1), n2 = strlen(s2);
    if (n2 > SIZE_MAX - n1 - 1) {
        free(propio_a); free(propio_b);
        latC_error(mv, "Resultado de concatenacion demasiado largo"); return;
    }
    char *ret = malloc(n1 + n2 + 1);
    if (ret) { memcpy(ret, s1, n1); memcpy(ret + n1, s2, n2 + 1); }
    free(propio_a); free(propio_b);
    cadena_publicar(mv, ret);
}

void str_comparar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    lat_objeto *tmp = latC_crear_numerico(
        mv, strcmp(latC_checar_cadena(mv, a), latC_checar_cadena(mv, b)));
    latC_apilar(mv, tmp);
}

void str_contiene(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    char *result = strstr(latC_checar_cadena(mv, a), latC_checar_cadena(mv, b));
    if (result != NULL) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

void str_termina_con(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    if (termina_con(latC_checar_cadena(mv, a), latC_checar_cadena(mv, b))) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

void str_es_igual(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    if (!strcmp(latC_checar_cadena(mv, a), latC_checar_cadena(mv, b))) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

void str_indice(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    char *cb = latC_checar_cadena(mv, b);
    char *ca = latC_checar_cadena(mv, a);
    lat_objeto *r = NULL;
    if (a->tipo == T_STR && b->tipo == T_STR) {
        char *cpos = strstr(ca, cb);
        int pos;
        if (cpos == NULL) {
            pos = -1;
        } else {
            pos = cpos - ca;
        }
        r = latC_crear_numerico(mv, pos);
    }
    latC_apilar(mv, r);
    return;
}

void str_insertar(lat_mv *mv) {
    lat_objeto *c = latC_desapilar(mv), *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    const char *base = latC_checar_cadena(mv, a), *texto = latC_checar_cadena(mv, b);
    int posicion = cadena_entero(mv, c);
    if (posicion < 0 && (size_t)(-(long long)posicion) > strlen(base))
        latC_error(mv, "Indice de cadena fuera de rango");
    cadena_publicar(mv, insertar((char *)base, (char *)texto, posicion));
}

void str_ultimo_indice(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    lat_objeto *tmp = latC_crear_numerico(
        mv, ultima_pos(latC_checar_cadena(mv, a), latC_checar_cadena(mv, b)));
    latC_apilar(mv, tmp);
}

void str_rellenar_izquierda(lat_mv *mv) {
    lat_objeto *c = latC_desapilar(mv), *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    char *base = latC_checar_cadena(mv, a), *texto = latC_checar_cadena(mv, b);
    int n = cadena_entero(mv, c);
    if (n < 0) latC_error(mv, "Longitud de cadena fuera de rango");
    cadena_publicar(mv, rellenar_izquierda(base, texto, n));
}

void str_rellenar_derecha(lat_mv *mv) {
    lat_objeto *c = latC_desapilar(mv), *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    char *base = latC_checar_cadena(mv, a), *texto = latC_checar_cadena(mv, b);
    int n = cadena_entero(mv, c);
    if (n < 0) latC_error(mv, "Longitud de cadena fuera de rango");
    cadena_publicar(mv, rellenar_derecha(base, texto, n));
}

void str_eliminar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    char *base = latC_checar_cadena(mv, a), *patron = latC_checar_cadena(mv, b);
    cadena_publicar(mv, reemplazar(base, patron, ""));
}

void str_esta_vacia(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    if (!strcmp(latC_checar_cadena(mv, a), "")) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

void str_longitud(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    size_t len = 0;
    if (a->tipo == T_STR) len = strlen(latC_checar_cadena(mv, a));
    else if (a->tipo == T_DIC) len = latH_longitud(latC_checar_dic(mv, a));
    /* Conserva el contrato por bytes, sin reservar un objeto temporal. */
    latC_apilar_double(mv, (double)len);
}

static char *reemplazar_lat(char *orig, char *rep, char *with, int veces) {
    size_t len = strlen(orig), patron = strlen(rep), nuevo = strlen(with), count = 0;
    if (!patron) return NULL;
    const char *p = orig, *hit;
    while ((hit = strstr(p, rep)) != NULL && (veces <= 0 || count < (size_t)veces)) {
        ++count;
        p = hit + patron;
    }
    size_t total;
    if (nuevo >= patron) {
        size_t delta = nuevo - patron;
        if (delta && count > (SIZE_MAX - len - 1) / delta) return NULL;
        total = len + count * delta;
    } else total = len - count * (patron - nuevo);
    char *ret = malloc(total + 1), *dest = ret;
    if (!ret) return NULL;
    p = orig;
    for (size_t i = 0; i < count; ++i) {
        hit = strstr(p, rep);
        size_t prefijo = (size_t)(hit - p);
        memcpy(dest, p, prefijo); dest += prefijo;
        memcpy(dest, with, nuevo); dest += nuevo;
        p = hit + patron;
    }
    memcpy(dest, p, strlen(p) + 1);
    return ret;
}

/* El lector historico de escapes forma parte del parser. No se cambia aqui:
   se excluyen sus entradas sin contrato seguro antes de reservar temporales. */
static void cadena_validar_escapes(lat_mv *mv, const char *s) {
    for (size_t i = 0; s[i]; ++i) {
        if (s[i] != '\\' || !s[i + 1]) continue;
        ++i;
        if (s[i] == 'u') latC_error(mv, "Escape Unicode no admitido en reemplazar");
        if (s[i] >= '0' && s[i] <= '9') {
            unsigned valor = 0;
            do {
                unsigned d = (unsigned)(s[i] - '0');
                if (valor > ((unsigned)INT_MAX - d) / 10)
                    latC_error(mv, "Escape numerico fuera de rango");
                valor = valor * 10 + d;
                ++i;
            } while (s[i] >= '0' && s[i] <= '9');
            --i;
        }
    }
}

void str_reemplazar(lat_mv *mv) {
    lat_objeto *d = latC_desapilar(mv), *c = latC_desapilar(mv);
    lat_objeto *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    char *base = latC_checar_cadena(mv, a), *patron = latC_checar_cadena(mv, b);
    char *nuevo = latC_checar_cadena(mv, c);
    int veces = d->tipo == T_NULL ? 0 : cadena_entero(mv, d);
    cadena_validar_escapes(mv, patron);
    cadena_validar_escapes(mv, nuevo);
    /* Conservar la segunda interpretacion historica de escapes. */
    char *p = analizar_fmt(patron, strlen(patron));
    char *n = analizar_fmt(nuevo, strlen(nuevo));
    if (!p || !n) { free(p); free(n); latC_error(mv, "Memoria virtual agotada"); return; }
    if (!*p) { free(p); free(n); latC_error(mv, "Patron de reemplazo vacio"); return; }
    char *ret = reemplazar_lat(base, p, n, veces);
    free(p); free(n);
    cadena_publicar(mv, ret);
}

void str_inicia_con(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    bool emp = inicia_con(latC_checar_cadena(mv, a), latC_checar_cadena(mv, b));
    if (emp) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

void str_subcadena(lat_mv *mv) {
    lat_objeto *c = latC_desapilar(mv), *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    const char *base = latC_checar_cadena(mv, a);
    int inicio = cadena_entero(mv, b), cantidad = cadena_entero(mv, c);
    if (inicio < 0 || (size_t)inicio > strlen(base)) latC_error(mv, "Indice de cadena fuera de rango");
    if (cantidad < 0) latC_error(mv, "Longitud de cadena fuera de rango");
    cadena_publicar(mv, subcadena(base, inicio, cantidad));
}

void str_minusculas(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    cadena_publicar(mv, minusculas(latC_checar_cadena(mv, a)));
}

void str_mayusculas(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    cadena_publicar(mv, mayusculas(latC_checar_cadena(mv, a)));
}

void str_recortar(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    char *rec = quitar_espacios(latC_checar_cadena(mv, a));
    if (!rec) { latC_error(mv, "Memoria virtual agotada"); return; }
    latC_apilar_string(mv, rec);
    free(rec);
}

void str_es_numerico(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    char *ptr;
  latC_strtod_c(latC_checar_cadena(mv, a), &ptr);
    if (!strcmp(ptr, "")) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

void str_esalfa(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    char *cad = latC_checar_cadena(mv, a);
    bool res = true;
    size_t len = strlen(cad);
    for (size_t i = 0; i < len; i++) {
        if (!isalnum((unsigned char)cad[i])) {
            res = false;
            break;
        }
    }
    if (res) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

void str_separar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    const char *sep = " ";
    if (b->tipo != T_NULL) {
        sep = latC_checar_cadena(mv, b);
        if (!*sep) sep = " ";
    }
    const char *str = latC_checar_cadena(mv, a);
    char *copia = malloc(strlen(str) + 1);
    if (!copia) { latC_error(mv, "Memoria virtual agotada"); return; }
    strcpy(copia, str);
    lista *lst = latL_crear(mv);
    /* Semantica historica: conjunto de bytes separadores, sin tokens vacios.
       strspn/strcspn son C estandar y no tienen estado global como strtok. */
    char *p = copia;
    while (*p) {
        p += strspn(p, sep);
        if (!*p) break;
        size_t len = strcspn(p, sep);
        char *fin = p + len;
        char siguiente = *fin;
        *fin = '\0';
        latL_agregar(mv, lst, latC_crear_cadena(mv, p));
        if (!siguiente) break;
        p = fin + 1;
    }
    free(copia);
    lat_objeto *tmp = latC_crear_lista(mv, lst);
    latC_apilar(mv, tmp);
#if !HABILITAR_GC
    /* La pila recibe una copia; la lista no se destruye aqui. */
    latM_liberar(mv, tmp);
#endif
}

void str_invertir(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    const char *orig = latC_checar_cadena(mv, a);
    size_t len = strlen(orig);
    char *dest = malloc(len + 1);
    if (!dest) { latC_error(mv, "Memoria virtual agotada"); return; }
    for (size_t i = 0; i < len; ++i) dest[i] = orig[len - i - 1];
    dest[len] = '\0';
    cadena_publicar(mv, dest);
}

void str_ejecutar(lat_mv *mv) {
    int status;
    lat_objeto *o = latC_desapilar(mv);
    char *codigo = latC_checar_cadena(mv, o);
    char *tmp_name = mv->nombre_archivo;
    mv->nombre_archivo = "CADENA";
    ast *nodo = latA_analizar_exp(codigo, &status);
    if (status == 0 && nodo != NULL) {
        lat_objeto *func = latC_analizar(mv, nodo);
        status = latC_llamar_funcion(mv, func);
        latO_destruir(mv, func);
    } else {
        latA_destruir(nodo);
        mv->nombre_archivo = tmp_name;
        latC_error(mv, "Error al ejecutar cadena");
        return;
    }
    mv->nombre_archivo = tmp_name;
    latA_destruir(nodo);
}

void str_regex(lat_mv *mv) {
    lat_objeto *patron = latC_desapilar(mv), *valor = latC_desapilar(mv);
    const char *exp = latC_checar_cadena(mv, patron), *texto;
    char numero[64];
    if (valor->tipo == T_NUMERIC) {
        snprintf(numero, sizeof numero, "%.16g", latC_checar_numerico(mv, valor));
        texto = numero;
    } else if (valor->tipo == T_STR) texto = latC_checar_cadena(mv, valor);
    else { latC_error(mv, "Error se esperaba una cadena ó númerico."); return; }
    regex_t regex;
    if (regcomp(&regex, exp, REG_EXTENDED)) { latC_error(mv, "Error al compilar regex."); return; }
    int estado = regexec(&regex, texto, 0, NULL, 0);
    regfree(&regex);
    if (estado != 0 && estado != REG_NOMATCH) { latC_error(mv, "Error en el match regex."); return; }
    latC_apilar(mv, estado == 0 ? latO_verdadero : latO_falso);
}

void str_match(lat_mv *mv) {
    lat_objeto *regexString = latC_desapilar(mv);
    lat_objeto *source = latC_desapilar(mv);
    const char *texto_fuente = latC_checar_cadena(mv, source);
    char maxMatches = 50;
    int maxGroups = 50;
    regex_t regexCompiled;
    regmatch_t groupArray[50];
    char *cursor;
    int retorno = regcomp(&regexCompiled, latC_checar_cadena(mv, regexString),
                          REG_EXTENDED);
    if (retorno) {
        switch (retorno) {
            case REG_BADPAT:
                latC_error(mv,
                           "error en el regex: expresión regular inválida.");
                break;
            case REG_ECOLLATE:
                latC_error(
                    mv, "error en el regex: elemento de referencia inválido.");
                break;
            case REG_ECTYPE:
                latC_error(mv,
                           "error en el regex: tipo de carácteres inválidos.");
                break;
            case REG_EESCAPE:
                latC_error(mv,
                           "error en el regex: número en \\ dígito no válido.");
                break;
            case REG_EBRACK:
                latC_error(mv, "error en el regex: [] sin balance.");
                break;
            case REG_EPAREN:
                latC_error(mv, "error en el regex: \\( \\) ó ( ) sin balance.");
                break;
            case REG_EBRACE:
                latC_error(mv, "error en el regex: \\{ \\} sin balance.");
                break;
            case REG_BADBR:
                latC_error(mv, "error en el regex: contenido de \\{\\} "
                               "inválido, no es un número, o es un número "
                               "demasiado grande, o más de dos números, o "
                               "quizá el primero es mayor que el segundo.");
                break;
            case REG_ERANGE:
                latC_error(mv, "error en el regex: el final no es válido con "
                               "el rango de la expresión.");
                break;
            case REG_ESPACE:
                latC_error(mv, "error en el regex: sin memoria.");
                break;
            case REG_BADRPT:
                latC_error(mv, "error en el regex: ?, * o + no está siendo "
                               "usado en una expresión regular válida.");
                break;
#ifdef _XOPEN_SOURCE
            case REG_ENOSYS:
                latC_error(mv, "error en el regex: la implementación no admite "
                               "esta función.");
#endif
                break;
            default:
                latC_error(mv, "error en el regex: error desconocido.");
                break;
        }
    };
    cursor = (char *)texto_fuente;
    lat_objeto *l_matches = latC_crear_lista(mv, latL_crear(mv));
    for (int m = 0; m < maxMatches; m++) {
        if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0)) {
            break; // No more matches
        }
        unsigned int offset = 0;
        lat_objeto *l_groups = latC_crear_lista(mv, latL_crear(mv));
        for (int g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == -1) {
                break; // No more groups
            }
            if (g == 0) {
                offset = groupArray[g].rm_eo;
            }
            size_t len = (size_t)(groupArray[g].rm_eo - groupArray[g].rm_so);
            if (len) {
                char *parte = malloc(len + 1);
                if (!parte) { regfree(&regexCompiled); latC_error(mv, "Memoria virtual agotada"); return; }
                memcpy(parte, cursor + groupArray[g].rm_so, len);
                parte[len] = '\0';
                latL_agregar(mv, latC_checar_lista(mv, l_groups), latC_crear_cadena(mv, parte));
                free(parte);
            }
        }
        cursor += offset;
        if (latL_longitud(getLista(l_groups)) > 0) {
            latL_agregar(mv, latC_checar_lista(mv, l_matches), l_groups);
        }
    }
    latC_apilar(mv, l_matches);
    regfree(&regexCompiled);
}

void imprimir_pila(lat_mv *mv);

static bool formato_agregar(char **texto, size_t *longitud, size_t *capacidad,
                            const char *parte, size_t n, const char **error) {
    if (n > (size_t)MAX_STR_LENGTH - 1 - *longitud) {
        *error = "Resultado de formato demasiado largo";
        return false;
    }
    size_t necesaria = *longitud + n + 1;
    if (necesaria > *capacidad) {
        size_t nueva = *capacidad ? *capacidad : 128;
        while (nueva < necesaria) nueva *= 2;
        char *ampliada = realloc(*texto, nueva);
        if (!ampliada) {
            *error = "Memoria agotada al dar formato";
            return false;
        }
        *texto = ampliada;
        *capacidad = nueva;
    }
    memcpy(*texto + *longitud, parte, n);
    *longitud += n;
    (*texto)[*longitud] = '\0';
    return true;
}

static bool formato_numerico(char *destino, size_t capacidad, char opcion,
                             lat_mv *mv, lat_objeto *o, const char **error) {
    /* latC_adouble puede lanzar para los tipos que no se enumeran aqui. */
    switch (o->tipo) {
        case T_NULL: case T_BOOL: case T_NUMERIC: case T_STR: case T_LIST: case T_DIC:
            break;
        default:
            *error = "Conversion de tipo de dato incompatible";
            return false;
    }
    double valor = latC_adouble(mv, o);
    if (opcion == 'c' || opcion == 'i' || opcion == 'o' || opcion == 'x') {
        if (!isfinite(valor) || valor < INT_MIN || valor > INT_MAX) {
            *error = "Numero fuera de rango para el formato";
            return false;
        }
        int entero = (int)valor;
        if (opcion == 'c') snprintf(destino, capacidad, "%c", entero);
        if (opcion == 'i') snprintf(destino, capacidad, "%i", entero);
        if (opcion == 'o') snprintf(destino, capacidad, "%o", (unsigned)entero);
        if (opcion == 'x') snprintf(destino, capacidad, "%x", (unsigned)entero);
    } else if (opcion == 'f') {
        snprintf(destino, capacidad, "%f", valor);
    } else if (opcion == 'd') {
        snprintf(destino, capacidad, LAT_NUMERIC_FMT, valor);
    } else {
        snprintf(destino, capacidad, "%e", valor);
    }
    return true;
}

void str_formato(lat_mv *mv) {
    double cantidad = latC_checar_numerico(mv, latC_desapilar(mv));
    if (!isfinite(cantidad) || cantidad < 0 || cantidad > mv->ptrpila ||
        cantidad != floor(cantidad)) {
        latC_error(mv, "Numero de argumentos invalido para el formato");
        return;
    }
    if (cantidad == 0) {
        latC_apilar(mv, latO_nulo);
        return;
    }
    int total = (int)cantidad;
    lat_objeto *argumentos = mv->tope - total;
    const char *formato = latC_checar_cadena(mv, argumentos);
    char *texto = NULL;
    size_t longitud = 0, capacidad = 0;
    const char *error = NULL;
    int siguiente = 1;
    for (const char *p = formato; *p && !error; ++p) {
        if (*p != '%') {
            formato_agregar(&texto, &longitud, &capacidad, p, 1, &error);
            continue;
        }
        ++p;
        if (*p == '%') {
            formato_agregar(&texto, &longitud, &capacidad, p, 1, &error);
            continue;
        }
        if (!*p || !strchr("cifdoxes", *p)) {
            error = "Opcion de formato invalida";
            break;
        }
        if (siguiente == total) {
            error = "Numero de argumentos invalido para el formato";
            break;
        }
        lat_objeto *valor = &argumentos[siguiente++];
        if (*p == 's') {
            char *parte = latP_texto(valor, &error);
            if (parte) {
                formato_agregar(&texto, &longitud, &capacidad, parte, strlen(parte), &error);
                free(parte);
            }
        } else {
            /* El maximo decimal finito de double ocupa menos de 330 bytes. */
            char parte[384];
            if (formato_numerico(parte, sizeof(parte), *p, mv, valor, &error))
                formato_agregar(&texto, &longitud, &capacidad, parte, strlen(parte), &error);
        }
    }
    if (error) {
        free(texto);
        latC_error(mv, "%s", error);
        return;
    }
    /* Los argumentos siguen en celdas activas durante el formato. */
    mv->ptrpila -= total;
    mv->tope = argumentos;
    latC_apilar_string(mv, texto ? texto : "");
    free(texto);
}

void str_char(lat_mv *mv) {
    lat_objeto *arg = latC_desapilar(mv);
    if (arg->tipo == T_LIST) {
        lista *lst = latC_checar_lista(mv, arg);
        /* Validar todo antes de reservar; conservar ceros omitidos de la lista. */
        { LIST_FOREACH(lst, primero, siguiente, cur) (void)cadena_entero(mv, cur->valor); }
        size_t len = (size_t)latL_longitud(lst), pos = 0;
        char *ret = malloc(len + 1);
        if (!ret) { latC_error(mv, "Memoria virtual agotada"); return; }
        LIST_FOREACH(lst, primero, siguiente, cur) {
            char c = (char)(int)latC_checar_numerico(mv, cur->valor);
            if (c) ret[pos++] = c;
        }
        ret[pos] = '\0';
        cadena_publicar(mv, ret);
    } else if (arg->tipo == T_NUMERIC) {
        char ret[2] = {(char)cadena_entero(mv, arg), '\0'};
        latC_apilar_string(mv, ret);
    }
}

void str_bytes(lat_mv *mv) {
    lat_objeto *str = latC_desapilar(mv);
    if (str->tipo == T_NULL) {
        latC_apilar(mv, latO_nulo);
    } else {
        char *stringp = latC_checar_cadena(mv, str);
        lat_objeto *decs = latC_crear_lista(mv, latL_crear(mv));
        size_t len = strlen(stringp);
        for (size_t i = 0; i < len; i++) {
            latL_agregar(mv, latC_checar_lista(mv, decs),
                         latC_crear_numerico(mv, (int)stringp[i]));
        }
        latC_apilar(mv, decs);
    }
}

#include "latstr_utf8.inc"

static const lat_CReg libstr[] = {
    {"longitud_bytes", str_longitud_bytes, 1},
    {"longitud_unicode", str_longitud_unicode, 1},
    {"invertir_unicode", str_invertir_unicode, 1},
    {"subcadena_unicode", str_subcadena_unicode, 3},
    {"indice_unicode", str_indice_unicode, 2},
    {"separar_texto", str_separar_texto, 2},
    {"separar_delimitadores", str_separar, 2},
    {"mayusculas_ascii", str_mayusculas_ascii, 1},
    {"minusculas_ascii", str_minusculas_ascii, 1},
    {"char", str_char, 1},
    {"bytes", str_bytes, 1},
    {"esta_vacia", str_esta_vacia, 1},
    {"longitud", str_longitud, 1},
    {"minusculas", str_minusculas, 1},
    {"mayusculas", str_mayusculas, 1},
    {"recortar", str_recortar, 1},
    {"es_numerico", str_es_numerico, 1},
    {"es_numero", str_es_numerico, 1},
    {"es_alfa", str_esalfa, 1},
    {"invertir", str_invertir, 1},
    {"ejecutar", str_ejecutar, 1},
    {"concatenar", str_concatenar, 2},
    {"comparar", str_comparar, 2},
    {"contiene", str_contiene, 2},
    {"termina_con", str_termina_con, 2},
    {"es_igual", str_es_igual, 2},
    {"indice", str_indice, 2},
    {"encontrar", str_indice, 2},
    {"ultimo_indice", str_ultimo_indice, 2},
    {"eliminar", str_eliminar, 2},
    {"separar", str_separar, 2},
    {"inicia_con", str_inicia_con, 2},
    {"regexl", str_regex, 2},
    {"regex", str_match, 2},
    {"insertar", str_insertar, 3},
    {"rellenar_izquierda", str_rellenar_izquierda, 3},
    {"rellenar_derecha", str_rellenar_derecha, 3},
    {"reemplazar", str_reemplazar, 4},
    {"subcadena", str_subcadena, 3},
    {"formato", str_formato, FUNCION_VAR_ARGS}, // para funciones var_arg
    {NULL, NULL, 0}};

void latC_abrir_liblatino_strlib(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_CADENA_NAME, libstr);
}
