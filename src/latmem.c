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

#include <stdlib.h>
#include <stdint.h>

#include "latdo.h"
#include "latino.h"
#include "latmem.h"
#include "latmv.h"
#include "latpropiedad.h"

typedef struct lat_propiedad_entrada {
    void *direccion;
    lat_propiedad_tipo tipo;
    bool constante;
    struct lat_propiedad_entrada *hash_sig, *anterior, *siguiente;
} lat_propiedad_entrada;

/* El puntero ya era opaco en lat_mv. La representacion privada se amplia
   para conservar constantes y objetos de la region, sin tocar la ABI. */
struct lat_constante_registro {
    size_t capacidad, cantidad;
    bool general;
    lat_propiedad_entrada **cubetas;
    lat_propiedad_entrada *primero[LAT_P_TIPOS];
};

static size_t indice(struct lat_constante_registro *r, const void *p) {
    uintptr_t h = (uintptr_t)p >> 3;
    h ^= h >> 17;
    return h & (r->capacidad - 1);
}

static lat_propiedad_entrada *buscar(lat_mv *mv, const void *p) {
    struct lat_constante_registro *r = mv ? mv->constantes_propias : NULL;
    if (!r || !p) return NULL;
    for (lat_propiedad_entrada *e = r->cubetas[indice(r, p)]; e; e = e->hash_sig)
        if (e->direccion == p) return e;
    return NULL;
}

static void desvincular_hash(struct lat_constante_registro *r, lat_propiedad_entrada *e) {
    lat_propiedad_entrada **lugar = &r->cubetas[indice(r, e->direccion)];
    while (*lugar != e) lugar = &(*lugar)->hash_sig;
    *lugar = e->hash_sig;
}

static void vincular_hash(struct lat_constante_registro *r, lat_propiedad_entrada *e) {
    size_t i = indice(r, e->direccion);
    e->hash_sig = r->cubetas[i];
    r->cubetas[i] = e;
}

static void iniciar(lat_mv *mv, bool general) {
    if (!mv || mv->constantes_propias) return;
    struct lat_constante_registro *r = latM_asignar(mv, sizeof(*r));
    r->capacidad = 64;
    r->cubetas = latM_asignar(mv, r->capacidad * sizeof(*r->cubetas));
    r->general = general;
    mv->constantes_propias = r;
}

void latP_iniciar(lat_mv *mv) { iniciar(mv, true); }

static void registrar(lat_mv *mv, void *ptr, lat_propiedad_tipo tipo, bool constante) {
    if (!mv || !ptr) return;
    if (constante) iniciar(mv, false); /* Compatibilidad con los ensayos mv={0}. */
    struct lat_constante_registro *r = mv->constantes_propias;
    if (!r || (!r->general && !constante)) return;
    lat_propiedad_entrada *e = buscar(mv, ptr);
    if (e) {
        if (e->tipo != tipo) latC_error(mv, "Registro de propiedad incoherente");
        e->constante |= constante;
        return;
    }
    if (r->cantidad >= r->capacidad) {
        if (r->capacidad > SIZE_MAX / 2 / sizeof(*r->cubetas))
            latC_error(mv, "Registro de propiedad fuera de rango");
        lat_propiedad_entrada **viejas = r->cubetas;
        size_t nueva = r->capacidad * 2;
        lat_propiedad_entrada **cubetas = latM_asignar(mv, nueva * sizeof(*cubetas));
        r->capacidad = nueva;
        r->cubetas = cubetas;
        for (int k = 0; k < LAT_P_TIPOS; ++k)
            for (e = r->primero[k]; e; e = e->siguiente) vincular_hash(r, e);
        latM_liberar(mv, viejas);
    }
    e = latM_asignar(mv, sizeof(*e));
    e->direccion = ptr;
    e->tipo = tipo;
    e->constante = constante;
    e->siguiente = r->primero[tipo];
    if (e->siguiente) e->siguiente->anterior = e;
    r->primero[tipo] = e;
    vincular_hash(r, e);
    ++r->cantidad;
}

void latP_registrar(lat_mv *mv, void *ptr, lat_propiedad_tipo tipo) {
    registrar(mv, ptr, tipo, false);
}
void latP_retener_constante(lat_mv *mv, lat_objeto *o) {
    registrar(mv, o, LAT_P_OBJETO, true);
}
bool latP_es_constante(lat_mv *mv, lat_objeto *o) {
    lat_propiedad_entrada *e = buscar(mv, o);
    return e && e->constante;
}
bool latP_contiene(lat_mv *mv, void *ptr, lat_propiedad_tipo tipo) {
    lat_propiedad_entrada *e = buscar(mv, ptr);
    return e && e->tipo == tipo;
}
void *latP_primero(lat_mv *mv, lat_propiedad_tipo tipo) {
    struct lat_constante_registro *r = mv ? mv->constantes_propias : NULL;
    return r && r->primero[tipo] ? r->primero[tipo]->direccion : NULL;
}
void latP_olvidar(lat_mv *mv, void *ptr) {
    lat_propiedad_entrada *e = buscar(mv, ptr);
    if (!e) return;
    struct lat_constante_registro *r = mv->constantes_propias;
    desvincular_hash(r, e);
    if (e->anterior) e->anterior->siguiente = e->siguiente;
    else r->primero[e->tipo] = e->siguiente;
    if (e->siguiente) e->siguiente->anterior = e->anterior;
    --r->cantidad;
    /* La entrada no esta registrada: este liberar no vuelve a retirarla. */
    latM_liberar(mv, e);
}
void latP_cerrar(lat_mv *mv) {
    struct lat_constante_registro *r = mv ? mv->constantes_propias : NULL;
    if (!r) return;
    if (r->cantidad) {
        latC_error(mv, "Registro de propiedad no vacio al cerrar");
        return;
    }
    mv->constantes_propias = NULL;
    latM_liberar(mv, r->cubetas);
    latM_liberar(mv, r);
}
void latP_soltar_constantes(lat_mv *mv) {
    struct lat_constante_registro *r = mv ? mv->constantes_propias : NULL;
    if (!r) return;
    lat_propiedad_entrada *e = r->primero[LAT_P_OBJETO];
    while (e) {
        lat_propiedad_entrada *siguiente = e->siguiente;
        if (e->constante) latM_liberar(mv, e->direccion);
        e = siguiente;
    }
    if (!r->general && !r->cantidad) latP_cerrar(mv);
}

void *latM_asignar(lat_mv *mv, size_t size) {
    void *ptr = calloc(1, size);
    if (ptr == NULL) {
        latC_error(mv, "Memoria virtual agotada");
    }
    if (mv) {
#if DEPURAR_MEM
        size_t tam = latM_tamanio(ptr);
        printf("+++ memoria asignada: %zu, %p\n", tam, ptr);
#endif
        mv->memoria_usada += latM_tamanio(ptr);
    }
    return ptr;
}

size_t latM_tamanio(void *ptr) {
#ifdef __APPLE__
    return 0;
#else
    return malloc_size(ptr);
#endif
}

void *latM_reasignar(lat_mv *mv, void *ptr, size_t size) {
    if (!size) { latM_liberar(mv, ptr); return NULL; }
    size_t anterior = ptr ? latM_tamanio(ptr) : 0;
    lat_propiedad_entrada *e = buscar(mv, ptr);
    if (e) desvincular_hash(mv->constantes_propias, e);
    void *value = realloc(ptr, size);
    if (value == NULL) {
        if (e) vincular_hash(mv->constantes_propias, e);
        latC_error(mv, "Memoria virtual agotada");
        return NULL;
    }
    if (e) { e->direccion = value; vincular_hash(mv->constantes_propias, e); }
    if (mv) mv->memoria_usada = mv->memoria_usada - anterior + latM_tamanio(value);
#if DEPURAR_MEM
    printf("memoria inicial: %zu memoria nueva: %zu, %p\n", anterior,
           latM_tamanio(value), &value);
#endif
    return value;
}

void latM_liberar(lat_mv *mv, void *ptr) {
    if (ptr != NULL) {
        latP_olvidar(mv, ptr);
        if (mv != NULL) {
#if DEPURAR_MEM
            size_t tam = latM_tamanio(ptr);
            printf("--- memoria liberada: %zu, %p\n", tam, ptr);
#endif
            mv->memoria_usada -= latM_tamanio(ptr);
        }
        free(ptr);
        ptr = NULL;
    }
}
