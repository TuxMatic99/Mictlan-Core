#include "latino.h"
#include "latast.h"
#include "latobj.h"
#include <stdio.h>
#include <stdlib.h>

#define COMPROBAR(c) do { if (!(c)) { \
    fprintf(stderr, "P1.1 pila: %s (línea %d)\n", #c, __LINE__); \
    return EXIT_FAILURE; \
} } while (0)

static unsigned efectos;
static void n_dos(lat_mv *mv)
{
    double x = latC_checar_numerico(mv, latC_desapilar(mv));
    if (x < 0) latC_error(mv, "P1.1 rango");
    ++efectos;
    latC_apilar_double(mv, x);
    latC_apilar_double(mv, x + 1);
}

static void n_cero(lat_mv *mv)
{
    (void)mv;
    ++efectos;
}

int main(void)
{
    static const lat_CReg biblioteca[] = {
        {"dos", n_dos, 1}, {"cero", n_cero, 0}, {NULL, NULL, 0}
    };
    char fuente[] =
        "funcion suma_p11(n)\n retorno n + 1\nfin\n"
        "funcion bloque_p11()\n"
        "  para i en rango(0, 8)\n"
        "    ensayo.dos(i)\n    suma_p11(i)\n    ensayo.cero()\n"
        "  fin\n"
        "  si verdadero ensayo.dos(suma_p11(4)) fin\n"
        "  a11, b11 = ensayo.dos(40)\n"
        "  retorno a11 + b11\nfin\n"
        "funcion tipo_p11()\n ensayo.dos(\"texto\")\nfin\n"
        "funcion rango_p11()\n ensayo.dos(-1)\nfin\n"
        "ensayo.dos(2)\n";
    lat_mv *mv = latC_crear_mv();
    COMPROBAR(mv);
    mv->nombre_archivo = "p11_pila.lat";
    latC_abrir_liblatino(mv, "ensayo", biblioteca);
    int estado = -1;
    ast *arbol = latA_analizar_exp(fuente, &estado);
    COMPROBAR(arbol && estado == 0);
    lat_objeto *programa = latC_analizar(mv, arbol);
    latC_apilar_double(mv, 777);
    COMPROBAR(latC_llamar_funcion(mv, programa) == 0 && mv->ptrpila == 1);
    lat_objeto *ctx = mv->contexto[0];
    lat_objeto *bloque = latO_obtener_contexto(mv, ctx, "bloque_p11");
    lat_objeto *tipos = latO_obtener_contexto(mv, ctx, "tipo_p11");
    lat_objeto *rangos = latO_obtener_contexto(mv, ctx, "rango_p11");
    COMPROBAR(bloque && tipos && rangos);
    for (int i = 0; i < 40; ++i) {
        const int previo = mv->ptrprevio, retornos = mv->prev_args;
        COMPROBAR(latC_llamar_funcion(mv, i % 2 ? tipos : rangos) == LAT_ERRRUN);
        COMPROBAR(mv->ptrprevio == previo && mv->prev_args == retornos && mv->ptrpila == 1);
        COMPROBAR(latC_llamar_funcion(mv, bloque) == 0 && mv->ptrpila == 2);
        lat_objeto *valor = latC_desapilar(mv);
        COMPROBAR(valor->tipo == T_NUMERIC && getNumerico(valor) == 81);
        COMPROBAR(getNumerico(latC_tope(mv)) == 777 && mv->tope == mv->pila + 1);
        COMPROBAR(mv->ptrctx == 0 && mv->contexto_actual == ctx && mv->numejec == 0 && !mv->error);
    }
    COMPROBAR(efectos == 1 + 40 * 18);
    (void)latC_desapilar(mv);
    latO_destruir(mv, programa);
    latC_destruir_mv(mv);
    latA_destruir(arbol);
    return EXIT_SUCCESS;
}
