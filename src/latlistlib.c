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
#include "latmem.h"
#include <limits.h>
#include <stdint.h>
#include <math.h>

#define LIB_LISTA_NAME "lista"

/* Latino trunca fracciones finitas hacia cero. Validar antes del cast evita
   comportamiento indefinido para NaN, infinito o cantidades fuera de int. */
static int lista_entero(lat_mv *mv, lat_objeto *o) {
    double n = trunc(latC_checar_numerico(mv, o));
    if (!isfinite(n) || n < INT_MIN || n > INT_MAX) {
        latC_error(mv, "Numero fuera de rango para lista");
        return 0;
    }
    return (int)n;
}

static void lista_publicar(lat_mv *mv, lista *l) {
    lat_objeto *o = latC_crear_lista(mv, l);
    latC_apilar(mv, o);
#if !HABILITAR_GC
    /* La pila conserva el payload; este descriptor no es un elemento. */
    latM_liberar(mv, o);
#endif
}

static void lista_agregar(lat_mv *mv) {
    lat_objeto *elem = latC_desapilar(mv);
    lat_objeto *lst = latC_desapilar(mv);
    if (!latO_comparar(mv, lst, elem)) {
        latC_error(mv, "Referencia circular detectada");
    }
    latL_agregar(mv, latC_checar_lista(mv, lst), latO_clonar(mv, elem));
}

static void lista_extender(lat_mv *mv) {
    lat_objeto *l2 = latC_desapilar(mv);
    lat_objeto *lst = latC_desapilar(mv);
    if (lst->tipo != T_LIST) {
        latC_error(mv, "El objeto no es una lista");
    }
    if (l2->tipo != T_LIST) {
        latC_error(mv, "El objeto no es una lista");
    }
    lista *_lst2 = latC_checar_lista(mv, l2);
    lista *_lst = latC_checar_lista(mv, lst);
    latL_extender(mv, _lst, _lst2);
}

static void lista_eliminar_indice(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    lista *lst = latC_checar_lista(mv, a);
    int pos = lista_entero(mv, b);
    if (pos < 0 || pos >= latL_longitud(lst)) {
        latC_error(mv, "Indice fuera de rango");
    }
    if (pos >= 0) {
        nodo_lista *nt = latL_obtener_nodo(mv, lst, pos);
        latL_eliminar_nodo(mv, lst, nt);
    }
}

static void lista_invertir(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    lista *lst = latC_checar_lista(mv, a);
    lista *nl = latL_crear(mv);
    for (nodo_lista *n = lst->ultimo; n; n = n->anterior) {
        latL_agregar(mv, nl, n->valor);
    }
    lista_publicar(mv, nl);
}

static void lista_longitud(lat_mv *mv) {
    lat_objeto *o = latC_desapilar(mv);
    latC_apilar_double(mv, latL_longitud(latC_checar_lista(mv, o)));
}

static void lista_comparar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, latL_comparar(mv, latC_checar_lista(mv, a),
                                             latC_checar_lista(mv, b)));
}

static void lista_insertar(lat_mv *mv) {
    lat_objeto *c = latC_desapilar(mv);
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    lista *lst = latC_checar_lista(mv, a);
    int pos = lista_entero(mv, c);
    if (pos < 0 || pos > latL_longitud(lst)) {
        latC_error(mv, "Indice fuera de rango");
        return;
    }
    latL_insertar_elemento(mv, lst, latO_clonar(mv, b), pos);
}

static void lista_eliminar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    lista *lst = latC_checar_lista(mv, a);
    int i = latL_obtener_indice(mv, lst, b);
    if (i >= 0) {
        nodo_lista *nt = latL_obtener_nodo(mv, lst, i);
        latL_eliminar_nodo(mv, lst, nt);
    }
}

static void lista_indice(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    lista *lst = latC_checar_lista(mv, a);
    int i = latL_obtener_indice(mv, lst, b);
    latC_apilar_double(mv, i);
}

static void lista_contiene(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    lista *lst = latC_checar_lista(mv, a);
    bool contiene = latL_contiene_valor(mv, lst, b);
    if (contiene) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

static void lista_crear(lat_mv *mv) {
    lat_objeto *num = latC_desapilar(mv);
    int i = 0;
    int cant = lista_entero(mv, num);
    lista *lst = latL_crear(mv);
    if (cant > 0) {
        while (i < cant) {
            latL_agregar(mv, lst, latO_nulo);
            i++;
        }
    }
    lista_publicar(mv, lst);
}

void lista_concatenar(lat_mv *mv) {
    lat_objeto *l2 = latC_desapilar(mv);
    lat_objeto *l1 = latC_desapilar(mv);

    lista *lt1 = latC_checar_lista(mv, l1);
    lista *lt2 = latC_checar_lista(mv, l2);
    lista *nl = latL_crear(mv);
    latL_extender(mv, nl, lt1);
    latL_extender(mv, nl, lt2);
    lista_publicar(mv, nl);
}

/* Texto prestado o buffer numerico local: no hay conversiones que reserven. */
static const char *lista_texto(lat_objeto *o, char numero[64]) {
    if (o->tipo == T_STR) return getstr(getCadena(o));
    if (o->tipo == T_NUMERIC) {
        snprintf(numero, 64, "%.16g", getNumerico(o));
        return numero;
    }
    return NULL;
}

void lista_separar(lat_mv *mv) {
    lat_objeto *separador = latC_desapilar(mv);
    lat_objeto *list_ = latC_desapilar(mv);
    lista *lst = latC_checar_lista(mv, list_);
    const char *sep = separador->tipo == T_NULL ? " "
                      : latC_checar_cadena(mv, separador);
    size_t nsep = strlen(sep), total = 0;
    bool primero = true;
    for (nodo_lista *n = lst->primero; n; n = n->siguiente) {
        char numero[64];
        const char *texto = lista_texto((lat_objeto *)n->valor, numero);
        if (texto) {
            size_t bytes = strlen(texto), extra = primero ? 0 : nsep;
            if (bytes > SIZE_MAX - total - 1 ||
                extra > SIZE_MAX - total - bytes - 1) {
                latC_error(mv, "Resultado de union demasiado largo");
                return;
            }
            total += bytes + extra;
        }
        /* Compatibilidad: posicion original, incluso si se omiten valores. */
        primero = false;
    }
    if (total == 0) {
        latC_apilar(mv, latO_nulo);
        return;
    }
    char *buffer = malloc(total + 1);
    if (!buffer) {
        latC_error(mv, "Memoria agotada al unir lista");
        return;
    }
    char *destino = buffer;
    primero = true;
    for (nodo_lista *n = lst->primero; n; n = n->siguiente) {
        char numero[64];
        const char *texto = lista_texto((lat_objeto *)n->valor, numero);
        if (texto) {
            if (!primero) { memcpy(destino, sep, nsep); destino += nsep; }
            size_t bytes = strlen(texto);
            memcpy(destino, texto, bytes);
            destino += bytes;
        }
        primero = false;
    }
    *destino = '\0';
    latC_apilar_string(mv, buffer);
    free(buffer);
}

static const lat_CReg liblist[] = {
    {"invertir", lista_invertir, 1},
    {"agregar", lista_agregar, 2},
    {"extender", lista_extender, 2},
    {"eliminar_indice", lista_eliminar_indice, 2},
    {"longitud", lista_longitud, 1},
    {"indice", lista_indice, 2},
    {"encontrar", lista_indice, 2},
    {"comparar", lista_comparar, 2},
    {"insertar", lista_insertar, 3},
    {"eliminar", lista_eliminar, 2},
    {"contiene", lista_contiene, 2},
    {"concatenar", lista_concatenar, 2},
    {"crear", lista_crear, 1},
    {"separar", lista_separar, 2},
    {"unir", lista_separar, 2},
    {NULL, NULL, 0}};

void latC_abrir_liblatino_listlib(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_LISTA_NAME, liblist);
}
