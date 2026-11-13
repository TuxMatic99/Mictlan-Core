#include "latino.h"
#include "latobj.h"
#include <stdlib.h>
#include <string.h>

extern void str_formato(lat_mv *mv);
extern void str_ejecutar(lat_mv *mv);

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "P02G_ASSERT linea %d: %s\n", __LINE__, #x); exit(1); \
} } while (0)

int main(void) {
    lat_mv *mv = latC_crear_mv();
    CHECK(mv);
    mv->nombre_archivo = "p02g_formato";
    lat_objeto *funcion = latC_crear_cfuncion(mv, str_formato);
    char largo[2049];
    memset(largo, 'x', sizeof(largo) - 1);
    largo[sizeof(largo) - 1] = '\0';
    for (int i = 0; i < 20; ++i) {
        latC_apilar_string(mv, "%s %% %i");
        latC_apilar_string(mv, largo);
        latC_apilar_double(mv, 42);
        latC_apilar_double(mv, 3);
        CHECK(latC_llamar_funcion(mv, funcion) == 0);
        CHECK(mv->ptrpila == 1);
        const char *salida = latC_checar_cadena(mv, latC_desapilar(mv));
        CHECK(strlen(salida) == 2053 && !strncmp(salida, largo, 2048));
        CHECK(!strcmp(salida + 2048, " % 42"));
    }
    const char *fallos[] = {"%", "%s", "%Q", "%i"};
    for (size_t i = 0; i < sizeof(fallos) / sizeof(fallos[0]); ++i) {
        latC_apilar_string(mv, fallos[i]);
        if (i == 3) latC_apilar_double(mv, 1e100);
        latC_apilar_double(mv, i == 3 ? 2 : 1);
        CHECK(latC_llamar_funcion(mv, funcion) == LAT_ERRRUN);
        /* Los argumentos ya estaban en la pila al entrar al manejador. */
        while (mv->ptrpila) latC_desapilar(mv);
        CHECK(mv->numejec == 0 && mv->ptrctx == 0 && mv->tope == mv->pila);
    }
    latC_apilar_string(mv, "correcto");
    latC_apilar_double(mv, 1);
    CHECK(latC_llamar_funcion(mv, funcion) == 0);
    CHECK(!strcmp(latC_checar_cadena(mv, latC_desapilar(mv)), "correcto"));
    lat_objeto *evaluar = latC_crear_cfuncion(mv, str_ejecutar);
    const char *archivo = mv->nombre_archivo;
    latC_apilar_string(mv, "");
    CHECK(latC_llamar_funcion(mv, evaluar) == LAT_ERRRUN);
    CHECK(mv->ptrpila == 0 && mv->nombre_archivo == archivo);
    latC_destruir_mv(mv);
    return 0;
}
