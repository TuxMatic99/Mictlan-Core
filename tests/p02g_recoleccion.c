#include "latino.h"
#include "latast.h"
#include "latmem.h"
#include "latmemoria.h"
#include "latobj.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "P02G_ASSERT linea %d: %s\n", __LINE__, #x); exit(1); \
} } while (0)

static lat_mv *nueva(void) {
    lat_mv *mv = latC_crear_mv();
    CHECK(mv);
    mv->nombre_archivo = "p02g_recoleccion";
    mv->global->REPL = false;
    CHECK(latG_recolectar(mv) == LAT_G_INACTIVO);
    CHECK(latG_habilitar(mv) == LAT_G_OK);
    return mv;
}

static latG_medicion medir(lat_mv *mv) {
    latG_medicion m;
    CHECK(latG_medir(mv, &m) == LAT_G_OK);
    return m;
}

static lat_objeto *lista_uno(lat_mv *mv, lat_objeto *valor) {
    lat_objeto *o = latC_crear_lista(mv, latL_crear(mv));
    latL_agregar(mv, getLista(o), valor);
    return o;
}

static void ciclos(void) {
    lat_mv *mv = nueva();
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    latG_medicion base = medir(mv);
    lat_objeto *l = latC_crear_lista(mv, latL_crear(mv));
    lat_objeto *d = latC_crear_dic(mv, latH_crear(mv));
    lat_objeto *numero = latC_crear_numerico(mv, 42);
    latL_agregar(mv, getLista(l), numero);
    latL_agregar(mv, getLista(l), d);
    latL_agregar(mv, getLista(l), d);
    latH_asignar(mv, getDic(d), "vuelta", l);
    CHECK(latG_retener(mv, l) == LAT_G_OK);
    CHECK(latG_retener(mv, l) == LAT_G_OK);
    CHECK(latG_soltar(mv, l) == LAT_G_OK);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    CHECK(medir(mv).fijados == 1);
    CHECK(latL_obtener_elemento(mv, getLista(l), 1) == d);
    CHECK(latL_obtener_elemento(mv, getLista(l), 2) == d);
    lat_objeto *vuelta = latH_obtener(getDic(d), "vuelta");
    CHECK(getLista(vuelta) == getLista(l));
    CHECK(getNumerico(numero) == 42);
    CHECK(latG_soltar(mv, l) == LAT_G_OK);
    CHECK(latG_soltar(mv, l) == LAT_G_REFERENCIA);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    latG_medicion fin = medir(mv);
    CHECK(fin.reservas_registradas == base.reservas_registradas);
    CHECK(fin.bytes_registrados == base.bytes_registrados);
    CHECK(fin.reservas_recuperadas >= 7 && fin.bytes_recuperados > 0);
    latC_destruir_mv(mv);
}

static void raices(void) {
    lat_mv *mv = nueva();
    lat_mv *otra = nueva();
    lat_objeto *l = lista_uno(mv, latC_crear_numerico(mv, 19));
    lista *identidad = getLista(l);
    latC_apilar(mv, l);
    CHECK(latG_retener(otra, l) == LAT_G_REFERENCIA);
    CHECK(latG_retener(mv, mv->tope - 1) == LAT_G_REFERENCIA);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    CHECK(getLista(latC_tope(mv)) == identidad);
    lat_objeto *elemento = latL_desapilar(mv, identidad);
    CHECK(latG_retener(mv, elemento) == LAT_G_OK);
    latC_desapilar(mv);
    CHECK(latC_tope(mv) == NULL);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    CHECK(getNumerico(elemento) == 19);
    CHECK(latG_soltar(mv, elemento) == LAT_G_OK);
    CHECK(latG_recolectar(mv) == LAT_G_OK);

    lat_objeto *d = latC_crear_dic(mv, latH_crear(mv));
    lat_objeto valor = {0};
    setNumerico(&valor, 88);
    latH_asignar(mv, getDic(d), "x", &valor);
    lat_objeto *entrada = latH_obtener(getDic(d), "x");
    lista *alias = getLista(lista_uno(mv, entrada));
    CHECK(latG_retener(mv, alias) == LAT_G_OK);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    CHECK(latL_obtener_elemento(mv, alias, 0) == entrada);
    CHECK(getNumerico(entrada) == 88);
    CHECK(latG_soltar(mv, alias) == LAT_G_OK);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    CHECK(latG_recolectar(otra) == LAT_G_OK);
    latC_destruir_mv(otra);
    latC_destruir_mv(mv);
}

static void nativa_falla(lat_mv *mv) {
    CHECK(latG_recolectar(mv) == LAT_G_OCUPADO);
    lat_objeto *l = lista_uno(mv, latC_crear_numerico(mv, 7));
    latC_apilar(mv, l);
    latC_error(mv, "error esperado de recoleccion");
}

static void nativa_valida(lat_mv *mv) {
    CHECK(latG_recolectar(mv) == LAT_G_OCUPADO);
    lat_objeto *l = lista_uno(mv, latC_crear_numerico(mv, 42));
    latC_apilar(mv, l);
    latM_liberar(mv, l);
}

static void reentrada(lat_mv *mv) {
    CHECK(latG_recolectar(mv) == LAT_G_OCUPADO);
    lat_objeto *falla = latO_obtener_contexto(mv, mv->contexto[0], "falla");
    CHECK(latC_llamar_funcion(mv, falla) == LAT_ERRRUN);
    CHECK(latG_recolectar(mv) == LAT_G_OCUPADO);
    nativa_valida(mv);
}

static void errores(void) {
    lat_mv *mv = nueva();
    static const lat_CReg funciones[] = {{"falla", nativa_falla, 0},
        {"reentrada", reentrada, 0}, {NULL, NULL, 0}};
    latC_abrir_liblatino(mv, "", funciones);
    CHECK(latG_autorizar(mv, nativa_falla) == LAT_G_OK);
    CHECK(latG_autorizar(mv, reentrada) == LAT_G_OK);
    lat_objeto *falla = latO_obtener_contexto(mv, mv->contexto[0], "falla");
    lat_objeto *valida = latO_obtener_contexto(mv, mv->contexto[0], "reentrada");
    for (int i = 0; i < 20; ++i) {
        CHECK(latC_llamar_funcion(mv, falla) == LAT_ERRRUN);
        CHECK(latC_llamar_funcion(mv, valida) == 0);
        CHECK(mv->ptrpila == 1 && mv->numejec == 0 && mv->ptrctx == 0 && !mv->error);
        CHECK(latG_recolectar(mv) == LAT_G_OK);
        lat_objeto *l = latC_desapilar(mv);
        CHECK(getNumerico(latL_obtener_elemento(mv, getLista(l), 0)) == 42);
    }
    latC_destruir_mv(mv);
}

static void opacos(void) {
    lat_mv *mv = nueva();
    lista *generica = latL_crear(mv);
    CHECK(latG_recolectar(mv) == LAT_G_OPACO);
    latL_destruir(mv, generica);
    lat_objeto *externo = latC_crear_numerico(NULL, 17);
    lat_objeto *l = lista_uno(mv, externo);
    latC_apilar(mv, l);
    latG_medicion antes = medir(mv);
    CHECK(latG_recolectar(mv) == LAT_G_REFERENCIA);
    CHECK(medir(mv).reservas_registradas == antes.reservas_registradas);
    CHECK(latL_desapilar(mv, getLista(l)) == externo);
    latM_liberar(NULL, externo);
    latC_desapilar(mv);
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    lat_objeto *no_auditada = latC_crear_cfuncion(mv, nativa_valida);
    CHECK(latC_llamar_funcion(mv, no_auditada) == 0);
    CHECK(latG_recolectar(mv) == LAT_G_OPACO);
    CHECK(getNumerico(latL_obtener_elemento(mv, getLista(latC_tope(mv)), 0)) == 42);
    latC_destruir_mv(mv);

    mv = nueva();
    (void)latC_crear_cdato(mv, &mv);
    CHECK(latG_recolectar(mv) == LAT_G_OPACO);
    latC_destruir_mv(mv);
}

int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!strcmp(argv[1], "ciclos")) ciclos();
    else if (!strcmp(argv[1], "raices")) raices();
    else if (!strcmp(argv[1], "errores")) errores();
    else if (!strcmp(argv[1], "opacos")) opacos();
    else CHECK(0);
    return 0;
}
