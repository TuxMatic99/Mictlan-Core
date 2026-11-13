#include "latino.h"
#include "latmem.h"
#include "latmemoria.h"

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "P02G_ASSERT agotamiento linea %d: %s\n", __LINE__, #x); \
    exit(EXIT_FAILURE); \
} } while (0)

static bool falla_calloc, falla_realloc;
static void *reserva;
/* Las variables del parser que normalmente proporciona la consola. */
int parse_silent = 0;
char *filename = NULL;

void *__real_calloc(size_t cantidad, size_t tamanio);
void *__real_realloc(void *ptr, size_t tamanio);

void *__wrap_calloc(size_t cantidad, size_t tamanio) {
    if (falla_calloc) {
        falla_calloc = false;
        return NULL;
    }
    return __real_calloc(cantidad, tamanio);
}

void *__wrap_realloc(void *ptr, size_t tamanio) {
    if (falla_realloc) {
        falla_realloc = false;
        return NULL;
    }
    return __real_realloc(ptr, tamanio);
}

static void reservar(lat_mv *mv) {
    latC_apilar_double(mv, 7);
    falla_calloc = true;
    (void)latM_asignar(mv, 40);
    CHECK(false);
}

static void ampliar(lat_mv *mv) {
    falla_realloc = true;
    (void)latM_reasignar(mv, reserva, 128);
    CHECK(false);
}

static void comprobar(lat_mv *mv, size_t bytes) {
    CHECK(!falla_calloc && !falla_realloc && mv->memoria_usada == bytes);
    CHECK(mv->ptrpila == 1 && mv->tope == mv->pila + 1 &&
          mv->ptrctx == 0 && mv->numejec == 0 && mv->error == NULL);
    CHECK(getNumerico(latC_tope(mv)) == 42);
}

int main(void) {
    lat_mv *mv = latC_crear_mv();
    CHECK(mv && latG_habilitar(mv) == LAT_G_OK);
    mv->nombre_archivo = "alpha_agotamiento";
    CHECK(latG_autorizar(mv, reservar) == LAT_G_OK);
    CHECK(latG_autorizar(mv, ampliar) == LAT_G_OK);
    lat_objeto *llamada_reservar = latC_crear_cfuncion(mv, reservar);
    lat_objeto *llamada_ampliar = latC_crear_cfuncion(mv, ampliar);
    CHECK(latG_retener(mv, llamada_reservar) == LAT_G_OK);
    CHECK(latG_retener(mv, llamada_ampliar) == LAT_G_OK);
    reserva = latM_asignar(mv, 16);
    memset(reserva, 23, 16);
    latC_apilar_double(mv, 42);
    size_t bytes = mv->memoria_usada;
    CHECK(latC_llamar_funcion(mv, llamada_reservar) == LAT_ERRRUN);
    comprobar(mv, bytes);
    CHECK(latC_llamar_funcion(mv, llamada_ampliar) == LAT_ERRRUN);
    comprobar(mv, bytes);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    unsigned char *ampliada = latM_reasignar(mv, reserva, 128);
    for (size_t i = 0; i < 16; ++i) CHECK(ampliada[i] == 23);
    CHECK(getNumerico(latC_desapilar(mv)) == 42);
    latM_liberar(mv, ampliada);
    CHECK(latG_soltar(mv, llamada_reservar) == LAT_G_OK);
    CHECK(latG_soltar(mv, llamada_ampliar) == LAT_G_OK);
    latC_destruir_mv(mv);
    puts("Dos fallos de asignacion recuperados. Pila, memoria y VM validas.");
    return EXIT_SUCCESS;
}
