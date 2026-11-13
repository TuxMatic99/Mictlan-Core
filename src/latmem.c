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
#include "latmemoria.h"
#include "latmv.h"
#include "latpropiedad.h"

typedef enum { LISTA_GENERICA, LISTA_DATOS, LISTA_CUBETA } latP_uso_lista;

typedef struct lat_propiedad_entrada {
    void *direccion;
    lat_propiedad_tipo tipo;
    bool constante, modulo, contexto, marcada;
    latP_uso_lista uso_lista;
    size_t fijaciones, bytes;
    struct lat_propiedad_entrada *hash_sig, *anterior, *siguiente;
    struct lat_propiedad_entrada *pendiente;
} lat_propiedad_entrada;

typedef struct latG_nativa {
    lat_CFuncion funcion;
    struct latG_nativa *siguiente;
} latG_nativa;

/* El puntero ya era opaco en lat_mv. La representacion privada se amplia
   para conservar constantes y objetos de la region, sin tocar la ABI. */
struct lat_constante_registro {
    size_t capacidad, cantidad;
    bool general;
    bool habilitada, opaca;
    unsigned profundidad;
    size_t bytes_registrados, fijados, pico_bytes_vm;
    size_t reservas_recuperadas, bytes_recuperados;
    latG_nativa *nativas;
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
    r->pico_bytes_vm = mv->memoria_usada;
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
    e->bytes = latM_tamanio(ptr);
    r->bytes_registrados += e->bytes;
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
    r->bytes_registrados -= e->bytes;
    if (e->fijaciones) --r->fijados;
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
    while (r->nativas) {
        latG_nativa *nativa = r->nativas;
        r->nativas = nativa->siguiente;
        latM_liberar(mv, nativa);
    }
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
        struct lat_constante_registro *r = mv->constantes_propias;
        if (r && mv->memoria_usada > r->pico_bytes_vm)
            r->pico_bytes_vm = mv->memoria_usada;
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
    lat_propiedad_entrada *e = buscar(mv, ptr);
    if (e && latP_recoleccion_habilitada(mv)) {
        latC_error(mv, "Una reserva administrada no se puede mover con recoleccion activa");
        return NULL;
    }
    if (!size) { latM_liberar(mv, ptr); return NULL; }
    size_t anterior = ptr ? latM_tamanio(ptr) : 0;
    if (e) desvincular_hash(mv->constantes_propias, e);
    void *value = realloc(ptr, size);
    if (value == NULL) {
        if (e) vincular_hash(mv->constantes_propias, e);
        latC_error(mv, "Memoria virtual agotada");
        return NULL;
    }
    if (e) {
        struct lat_constante_registro *r = mv->constantes_propias;
        r->bytes_registrados -= e->bytes;
        e->bytes = latM_tamanio(value);
        r->bytes_registrados += e->bytes;
        e->direccion = value;
        vincular_hash(r, e);
    }
    if (mv) mv->memoria_usada = mv->memoria_usada - anterior + latM_tamanio(value);
    if (mv && mv->constantes_propias && mv->memoria_usada > mv->constantes_propias->pico_bytes_vm)
        mv->constantes_propias->pico_bytes_vm = mv->memoria_usada;
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

static struct lat_constante_registro *region(lat_mv *mv) {
    struct lat_constante_registro *r = mv ? mv->constantes_propias : NULL;
    return r && r->general ? r : NULL;
}

void latP_lista_datos(lat_mv *mv, lista *l) {
    lat_propiedad_entrada *e = buscar(mv, l);
    if (e && e->tipo == LAT_P_LISTA && e->uso_lista != LISTA_CUBETA)
        e->uso_lista = LISTA_DATOS;
    else
        latP_inhibir(mv);
}

void latP_lista_cubeta(lat_mv *mv, lista *l) {
    lat_propiedad_entrada *e = buscar(mv, l);
    if (e && e->tipo == LAT_P_LISTA) e->uso_lista = LISTA_CUBETA;
}

void latP_proteger_modulo(lat_mv *mv, hash_map *mapa) {
    lat_propiedad_entrada *e = buscar(mv, mapa);
    if (e && e->tipo == LAT_P_MAPA) e->modulo = true;
    else latP_inhibir(mv);
}

void latP_contexto(lat_mv *mv, hash_map *mapa) {
    lat_propiedad_entrada *e = buscar(mv, mapa);
    if (e && e->tipo == LAT_P_MAPA) e->contexto = true;
}

bool latP_recoleccion_habilitada(lat_mv *mv) {
    struct lat_constante_registro *r = region(mv);
    return r && r->habilitada;
}

void latP_inhibir(lat_mv *mv) {
    struct lat_constante_registro *r = region(mv);
    if (r) r->opaca = true;
}

unsigned latP_profundidad(lat_mv *mv) {
    struct lat_constante_registro *r = region(mv);
    return r ? r->profundidad : 0;
}

void latP_restaurar_profundidad(lat_mv *mv, unsigned nivel) {
    struct lat_constante_registro *r = region(mv);
    if (r) r->profundidad = nivel;
}

void latP_llamada_nativa(lat_mv *mv, lat_CFuncion funcion) {
    struct lat_constante_registro *r = region(mv);
    if (!r || r->opaca) return;
    for (latG_nativa *n = r->nativas; n; n = n->siguiente)
        if (n->funcion == funcion) return;
    r->opaca = true;
}

LATINO_API latG_estado latG_habilitar(lat_mv *mv) {
    struct lat_constante_registro *r = region(mv);
    if (!r) return LAT_G_INACTIVO;
    if (r->profundidad || mv->error) return LAT_G_OCUPADO;
    if (r->opaca) return LAT_G_OPACO;
    r->habilitada = true;
    return LAT_G_OK;
}

LATINO_API latG_estado latG_autorizar(lat_mv *mv, lat_CFuncion funcion) {
    struct lat_constante_registro *r = region(mv);
    if (!r || !r->habilitada) return LAT_G_INACTIVO;
    if (r->profundidad || mv->error) return LAT_G_OCUPADO;
    if (r->opaca) return LAT_G_OPACO;
    if (!funcion) return LAT_G_REFERENCIA;
    for (latG_nativa *n = r->nativas; n; n = n->siguiente)
        if (n->funcion == funcion) return LAT_G_OK;
    latG_nativa *n = latM_asignar(mv, sizeof(*n));
    n->funcion = funcion;
    n->siguiente = r->nativas;
    r->nativas = n;
    return LAT_G_OK;
}

LATINO_API latG_estado latG_retener(lat_mv *mv, const void *reserva) {
    struct lat_constante_registro *r = region(mv);
    if (!r || !r->habilitada) return LAT_G_INACTIVO;
    if (r->opaca) return LAT_G_OPACO;
    lat_propiedad_entrada *e = buscar(mv, reserva);
    /* Las tablas de contexto y sus entradas pueden retirarse en longjmp.
       En modo G las entradas son descriptores independientes al retirar la
       tabla. Se fijan igual que otros objetos registrados. */
    if (!e || e->contexto || e->fijaciones == SIZE_MAX ||
        (e->tipo == LAT_P_LISTA && e->uso_lista != LISTA_DATOS) ||
        (e->tipo == LAT_P_OBJETO && ((lat_objeto *)e->direccion)->tipo == T_CONTEXT))
        return LAT_G_REFERENCIA;
    if (e->fijaciones == 0) ++r->fijados;
    ++e->fijaciones;
    return LAT_G_OK;
}

LATINO_API latG_estado latG_soltar(lat_mv *mv, const void *reserva) {
    struct lat_constante_registro *r = region(mv);
    if (!r || !r->habilitada) return LAT_G_INACTIVO;
    lat_propiedad_entrada *e = buscar(mv, reserva);
    if (!e || !e->fijaciones) return LAT_G_REFERENCIA;
    if (!--e->fijaciones) --r->fijados;
    return LAT_G_OK;
}

LATINO_API latG_estado latG_medir(lat_mv *mv, latG_medicion *medicion) {
    struct lat_constante_registro *r = region(mv);
    if (!r) return LAT_G_INACTIVO;
    if (!medicion) return LAT_G_REFERENCIA;
    *medicion = (latG_medicion){r->cantidad, r->bytes_registrados, r->fijados,
        mv->memoria_usada, r->pico_bytes_vm, r->reservas_recuperadas,
        r->bytes_recuperados};
    return LAT_G_OK;
}

typedef struct {
    lat_mv *mv;
    lat_propiedad_entrada *pendientes;
    latG_estado estado;
} latG_trazado;

static void encolar(latG_trazado *t, lat_propiedad_entrada *e) {
    if (e->marcada) return;
    e->marcada = true;
    e->pendiente = t->pendientes;
    t->pendientes = e;
}

static void marcar(latG_trazado *t, const void *p, lat_propiedad_tipo tipo) {
    if (t->estado != LAT_G_OK) return;
    if (tipo == LAT_P_OBJETO &&
        (!p || p == latO_nulo || p == latO_falso || p == latO_verdadero)) return;
    lat_propiedad_entrada *e = buscar(t->mv, p);
    if (!e || e->tipo != tipo || (tipo == LAT_P_LISTA && e->uso_lista != LISTA_DATOS)) {
        t->estado = LAT_G_REFERENCIA;
        return;
    }
    encolar(t, e);
}

static void marcar_valor(latG_trazado *t, const lat_objeto *o) {
    switch (o->tipo) {
    case T_LIST: marcar(t, getLista(o), LAT_P_LISTA); break;
    case T_DIC: marcar(t, getDic(o), LAT_P_MAPA); break;
    case T_CONTEXT: marcar(t, getCtx(o), LAT_P_MAPA); break;
    case T_FUN:
        if (!latP_funcion_registrada(t->mv, getFun(o))) t->estado = LAT_G_REFERENCIA;
        break;
    case T_NULL: case T_BOOL: case T_NUMERIC: case T_STR:
    case T_CFUN: case T_INTEGER: case T_CHAR: case T_LABEL:
        break; /* Escalares o contenidos conservados hasta el cierre. */
    case T_CPTR: case T_CLASS: case T_INSTANCE:
        t->estado = LAT_G_OPACO;
        break;
    default:
        t->estado = LAT_G_REFERENCIA;
    }
}

static void marcar_tabla(latG_trazado *t, hash_map *mapa) {
    for (size_t i = 0; i < mapa->capacity && t->estado == LAT_G_OK; ++i) {
        lista *l = mapa->buckets[i];
        if (!l) continue;
        lat_propiedad_entrada *e = buscar(t->mv, l);
        if (!e || e->tipo != LAT_P_LISTA || e->uso_lista != LISTA_CUBETA) {
            t->estado = LAT_G_REFERENCIA;
            return;
        }
        e->marcada = true;
        LIST_FOREACH(l, primero, siguiente, n) {
            hash_val *v = n->valor;
            if (v) marcar(t, v->valor, LAT_P_OBJETO);
        }
    }
}

static latG_estado trazar(lat_mv *mv, struct lat_constante_registro *r) {
    latG_trazado t = {.mv = mv, .estado = LAT_G_OK};
    for (int tipo = 0; tipo < LAT_P_TIPOS; ++tipo) {
        for (lat_propiedad_entrada *e = r->primero[tipo]; e; e = e->siguiente) {
            e->marcada = false;
            if (e->tipo == LAT_P_LISTA && e->uso_lista == LISTA_GENERICA)
                return LAT_G_OPACO;
            if (e->constante || e->modulo || e->fijaciones) encolar(&t, e);
        }
    }
    if (mv->ptrpila < 0 || mv->ptrpila > MAX_STACK_SIZE ||
        mv->ptrctx < 0 || (size_t)mv->ptrctx >= sizeof(mv->contexto) / sizeof(mv->contexto[0]) ||
        mv->tope != mv->pila + mv->ptrpila) return LAT_G_REFERENCIA;
    marcar(&t, mv->global->argv, LAT_P_OBJETO);
    marcar(&t, mv->label_ctx, LAT_P_OBJETO);
    marcar(&t, mv->contexto_actual, LAT_P_OBJETO);
    for (int i = 0; i <= mv->ptrctx; ++i) marcar(&t, mv->contexto[i], LAT_P_OBJETO);
    for (int i = 0; i < mv->ptrpila && t.estado == LAT_G_OK; ++i)
        marcar_valor(&t, &mv->pila[i]);
    while (t.pendientes && t.estado == LAT_G_OK) {
        lat_propiedad_entrada *e = t.pendientes;
        t.pendientes = e->pendiente;
        switch (e->tipo) {
        case LAT_P_OBJETO: marcar_valor(&t, e->direccion); break;
        case LAT_P_MAPA: marcar_tabla(&t, e->direccion); break;
        case LAT_P_LISTA: {
            lista *l = e->direccion;
            LIST_FOREACH(l, primero, siguiente, n) marcar(&t, n->valor, LAT_P_OBJETO);
            break;
        }
        default: return LAT_G_REFERENCIA;
        }
    }
    return t.estado;
}

static void barrer(lat_mv *mv, struct lat_constante_registro *r, lat_propiedad_tipo tipo) {
    lat_propiedad_entrada *e = r->primero[tipo];
    while (e) {
        lat_propiedad_entrada *siguiente = e->siguiente;
        if (!e->marcada) {
            switch (tipo) {
            case LAT_P_MAPA: latH_limpiar(mv, e->direccion); break;
            case LAT_P_LISTA: latL_destruir(mv, e->direccion); break;
            case LAT_P_OBJETO: latM_liberar(mv, e->direccion); break;
            default: break;
            }
        }
        e = siguiente;
    }
}

LATINO_API latG_estado latG_recolectar(lat_mv *mv) {
    struct lat_constante_registro *r = region(mv);
    if (!r || !r->habilitada) return LAT_G_INACTIVO;
    if (r->profundidad || mv->error || mv->numejec || mv->ptrctx) return LAT_G_OCUPADO;
    if (r->opaca) return LAT_G_OPACO;
    latG_estado estado = trazar(mv, r);
    if (estado != LAT_G_OK) return estado;
    size_t cantidad = r->cantidad, bytes = mv->memoria_usada;
    memset(mv->var_cache, 0, sizeof(mv->var_cache));
    mv->var_cache_next = 0;
    barrer(mv, r, LAT_P_MAPA);
    barrer(mv, r, LAT_P_LISTA);
    barrer(mv, r, LAT_P_OBJETO);
    r->reservas_recuperadas = cantidad - r->cantidad;
    r->bytes_recuperados = bytes - mv->memoria_usada;
    return LAT_G_OK;
}
