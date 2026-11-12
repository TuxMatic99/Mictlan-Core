/* Contratos reales; CHECK no desaparece con NDEBUG. */
#include "latino.h"
#include "latast.h"
#include "latobj.h"
#include "latmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "P02F_ASSERT %d: %s\n", __LINE__, #x); exit(1); } } while (0)

static lat_objeto *lista_uno(lat_mv *mv, lat_objeto *valor) {
    lista *l = latL_crear(mv);
    latL_agregar(mv, l, valor);
    return latC_crear_lista(mv, l);
}
static lat_objeto *elemento(lat_mv *mv, lat_objeto *l) {
    return latL_obtener_elemento(mv, latC_checar_lista(mv, l), 0);
}
static void compartidos(lat_mv *mv, int anidado) {
    lat_objeto *valor = latC_crear_numerico(mv, 8);
    if (anidado) valor = lista_uno(mv, valor);
    lat_objeto *a = lista_uno(mv, valor), *b = lista_uno(mv, valor);
    latL_agregar(mv, getLista(b), valor);
    CHECK(elemento(mv, a) == elemento(mv, b));
    latO_destruir(mv, a);
    CHECK(elemento(mv, b) == valor);
    CHECK(latL_obtener_elemento(mv, getLista(b), 1) == valor);
    CHECK(getNumerico(anidado ? elemento(mv, valor) : valor) == 8);
    latO_destruir(mv, b);
}
static void diccionarios(lat_mv *mv) {
    lat_objeto *l = lista_uno(mv, latC_crear_numerico(mv, 8));
    lat_objeto *a = latC_crear_dic(mv, latH_crear(mv));
    lat_objeto *b = latC_crear_dic(mv, latH_crear(mv));
    latH_asignar(mv, getDic(a), "valor", l);
    latH_asignar(mv, getDic(b), "valor", l);
    lista *identidad = getLista(l);
    latM_liberar(mv, l); /* La tabla tiene su propio envoltorio. */
    latO_destruir(mv, a);
    lat_objeto *alias = latH_obtener(getDic(b), "valor");
    CHECK(getLista(alias) == identidad && getNumerico(elemento(mv, alias)) == 8);
    latO_destruir(mv, b);
}
static lat_objeto *ciclo(lat_mv *mv, int mixto) {
    lat_objeto *a = latC_crear_lista(mv, latL_crear(mv));
    if (mixto) {
        lat_objeto *b = latC_crear_dic(mv, latH_crear(mv));
        latL_agregar(mv, getLista(a), b);
        latH_asignar(mv, getDic(b), "lista", a);
        CHECK(getLista((lat_objeto *)latH_obtener(getDic(b), "lista")) == getLista(a));
    } else {
        latL_agregar(mv, getLista(a), a);
        CHECK(elemento(mv, a) == a);
    }
    return a;
}
static void extraer(lat_mv *mv) {
    lat_objeto *a = lista_uno(mv, latC_crear_numerico(mv, 8));
    lat_objeto *valor = latL_desapilar(mv, getLista(a));
    CHECK(getLista(a)->longitud == 0);
    latO_destruir(mv, a);
    CHECK(getNumerico(valor) == 8);
    latO_destruir(mv, valor);
}
static void reasignar(lat_mv *mv) {
    /* Calentar la capacidad del registro antes de comparar el saldo. */
    lat_objeto *o = latC_crear_numerico(mv, 8);
    latO_destruir(mv, o);
    size_t base = mv->memoria_usada;
    o = latC_crear_numerico(mv, 8);
    o = latM_reasignar(mv, o, 8192);
    CHECK(o && getNumerico(o) == 8);
    CHECK(mv->memoria_usada >= base + 8192);
    o = latM_reasignar(mv, o, sizeof(*o));
    CHECK(o && getNumerico(o) == 8);
    latO_destruir(mv, o);
    CHECK(mv->memoria_usada == base);
    o = latC_crear_numerico(mv, 16);
    CHECK(latM_reasignar(mv, o, 0) == NULL);
    CHECK(mv->memoria_usada == base);
}
static void clonar_mixto(lat_mv *mv) {
    lat_objeto *dic = latC_crear_dic(mv, latH_crear(mv));
    lat_objeto numero = {0}; setNumerico(&numero, 8);
    latH_asignar(mv, getDic(dic), "numero", &numero);
    lat_objeto *a = lista_uno(mv, dic), *b = latO_clonar(mv, a);
    CHECK(elemento(mv, a) == elemento(mv, b)); /* Semantica existente. */
    latO_destruir(mv, a);
    CHECK(getNumerico((lat_objeto *)latH_obtener(getDic(elemento(mv, b)), "numero")) == 8);
    latO_destruir(mv, b);
}
static void fallar(lat_mv *mv) {
    latC_checar_numerico(mv, latC_desapilar(mv));
}
static void devolver(lat_mv *mv) {
    lat_objeto *o = lista_uno(mv, latC_crear_numerico(mv, 8));
    latL_agregar(mv, getLista(o), latC_crear_numerico(mv, 16));
    latC_apilar(mv, o);
    latM_liberar(mv, o); /* Contrato usado por el SDK publicado. */
}
static void clonar_ciclo(lat_mv *mv) {
    lat_objeto *o = ciclo(mv, 0);
    (void)latO_clonar(mv, o); /* Debe producir LAT_ERRRUN, sin stack-overflow. */
}
static const lat_CReg funciones[] = {
    {"p02f_fallar", fallar, 1}, {"p02f_vector", devolver, 0},
    {"p02f_clonar_ciclo", clonar_ciclo, 0}, {NULL, NULL, 0}
};
static void ejecutar(lat_mv *mv, const char *source, int esperado, int ciclos) {
    int estado = -1;
    ast *arbol = latA_analizar_exp((char *)source, &estado);
    CHECK(arbol && estado == 0);
    lat_objeto *f = latC_analizar(mv, arbol);
    CHECK(f);
    for (int i = 0; i < ciclos; ++i) {
        estado = latC_llamar_funcion(mv, f);
        CHECK(estado == esperado);
        CHECK(mv->ptrctx == 0 && mv->numejec == 0);
        CHECK(mv->ptrpila == 0 && mv->tope == mv->pila);
    }
    latO_destruir(mv, f);
    latA_destruir(arbol);
}
int main(int argc, char **argv) {
    CHECK(argc == 2 && LAT_ERRRUN == 2);
    setvbuf(stdout, NULL, _IOLBF, 0);
    lat_mv *mv = latC_crear_mv(); CHECK(mv);
    mv->nombre_archivo = "p02f_ciclo_vida";
    mv->global->REPL = false;
    latC_abrir_liblatino(mv, "", funciones);
    const char *modo = argv[1];
    if (!strcmp(modo, "compartidos")) compartidos(mv, 0);
    else if (!strcmp(modo, "anidados")) compartidos(mv, 1);
    else if (!strcmp(modo, "diccionarios")) diccionarios(mv);
    else if (!strcmp(modo, "ciclo_lista")) latO_destruir(mv, ciclo(mv, 0));
    else if (!strcmp(modo, "ciclo_mixto")) latO_destruir(mv, ciclo(mv, 1));
    else if (!strcmp(modo, "extraer")) extraer(mv);
    else if (!strcmp(modo, "reasignar")) reasignar(mv);
    else if (!strcmp(modo, "clonar_mixto")) clonar_mixto(mv);
    else if (!strcmp(modo, "error")) {
        ejecutar(mv, "datos = [8,16]\np02f_fallar(\"texto\")\n", LAT_ERRRUN, 200);
        ejecutar(mv, "correcto = 42\n", 0, 1);
        CHECK(getNumerico(latO_obtener_contexto(mv, mv->contexto[0], "correcto")) == 42);
    } else if (!strcmp(modo, "ffi")) {
        ejecutar(mv, "original = p02f_vector()\nalias = original\noriginal = [7,14]\nalias[1] = 77\n", 0, 200);
        lat_objeto *alias = latO_obtener_contexto(mv, mv->contexto[0], "alias");
        CHECK(getNumerico(elemento(mv, alias)) == 8);
        CHECK(getNumerico(latL_obtener_elemento(mv, getLista(alias), 1)) == 77);
    } else if (!strcmp(modo, "clonar_ciclo")) {
        ejecutar(mv, "p02f_clonar_ciclo()\n", LAT_ERRRUN, 1);
    } else CHECK(0);
    printf("{\"case\":\"%s\",\"before_close_bytes\":%zu}\n", modo, mv->memoria_usada);
    latC_destruir_mv(mv);
    printf("{\"case\":\"%s\",\"status\":\"PASS_FUNCTIONAL\"}\n", modo);
    return 0;
}
