/* Generacion determinista de programas validos y comprobacion por valores.
   Cada caso usa parser, compilador, VM, contenedores y cierre reales. */
#include "latino.h"
#include "latast.h"
#include "latmemoria.h"
#include "latobj.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "P02G_ASSERT caso %u linea %d: %s\n", caso, __LINE__, #x); \
    return 1; \
} } while (0)

static unsigned siguiente(uint32_t *semilla) {
    *semilla = *semilla * UINT32_C(1664525) + UINT32_C(1013904223);
    return (*semilla >> 16) % 100u;
}

static int ejecutar(unsigned caso, unsigned a, unsigned b, unsigned c) {
    char fuente[384];
    int n = snprintf(fuente, sizeof(fuente),
        "funcion calcular(x, y)\n retorno x * 3 + y\nfin\n"
        "v = [calcular(%u, %u), %u]\n"
        "alias = v\nv[1] = %u\n"
        "d = {\"vector\": alias}\n"
        "v = [%u]\nresultado = d.vector[0] + alias[1]\n", a, b, b, c, a);
    CHECK(n > 0 && (size_t)n < sizeof(fuente));
    lat_mv *mv = latC_crear_mv();
    CHECK(mv);
    mv->nombre_archivo = "p02g_generativo";
    CHECK(latG_habilitar(mv) == LAT_G_OK);
    int estado = -1;
    ast *arbol = latA_analizar_exp(fuente, &estado);
    CHECK(arbol && estado == 0);
    lat_objeto *programa = latC_analizar(mv, arbol);
    CHECK(programa && latC_llamar_funcion(mv, programa) == 0);
    CHECK(mv->ptrpila == 0 && mv->ptrctx == 0 && mv->numejec == 0);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    lat_objeto *resultado = latO_obtener_contexto(mv, mv->contexto[0], "resultado");
    lat_objeto *alias = latO_obtener_contexto(mv, mv->contexto[0], "alias");
    lat_objeto *dic = latO_obtener_contexto(mv, mv->contexto[0], "d");
    CHECK(resultado && resultado->tipo == T_NUMERIC && getNumerico(resultado) == a * 3 + b + c);
    CHECK(alias && alias->tipo == T_LIST && dic && dic->tipo == T_DIC);
    lat_objeto *vector = latH_obtener(getDic(dic), "vector");
    CHECK(vector && vector->tipo == T_LIST && getLista(vector) == getLista(alias));
    CHECK(getNumerico(latL_obtener_elemento(mv, getLista(alias), 1)) == c);
    latA_destruir(arbol);
    latC_destruir_mv(mv);
    return 0;
}

int main(void) {
    uint32_t semilla = UINT32_C(1729);
    for (unsigned caso = 0; caso < 64; ++caso) {
        unsigned a = siguiente(&semilla), b = siguiente(&semilla), c = siguiente(&semilla);
        if (ejecutar(caso, a, b, c)) return 1;
    }
    puts("{\"semilla\":1729,\"programas\":64,\"ok\":true}");
    return 0;
}
