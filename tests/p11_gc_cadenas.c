#include "latino.h"
#include "latmemoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPROBAR(c) do { if (!(c)) { \
    fprintf(stderr, "P1.1 cadenas: %s (línea %d)\n", #c, __LINE__); \
    return EXIT_FAILURE; \
} } while (0)

static void n_opaca(lat_mv *mv) { (void)mv; }

int main(void)
{
    char fuente[] = "funcion leer_p11()\n retorno datos_p11.a[0] .. \"_literal\"\nfin\n";
    lat_mv *mv = latC_crear_mv(), *otra = latC_crear_mv();
    COMPROBAR(mv && otra);
    lat_objeto *temprano = latC_crear_cadena(mv, "antes_de_habilitar");
    latC_apilar(mv, temprano);
    COMPROBAR(latG_habilitar(mv) == LAT_G_OK && latG_habilitar_cadenas(mv) == LAT_G_OK);
    lat_cadena *fijada = getCadena(temprano);
    COMPROBAR(latG_retener(mv, fijada) == LAT_G_OK && latG_retener(mv, fijada) == LAT_G_OK);
    COMPROBAR(latG_habilitar(otra) == LAT_G_OK && latG_habilitar_cadenas(otra) == LAT_G_OK);
    COMPROBAR(latG_retener(otra, fijada) == LAT_G_REFERENCIA);
    (void)latC_desapilar(mv);
    latO_destruir(mv, temprano);
    COMPROBAR(latG_soltar(mv, fijada) == LAT_G_OK);

    char ruta[] = "cadenas-p11.mict";
    mv->nombre_archivo = ruta;
    int estado = -1;
    ast *arbol = latA_analizar_exp(fuente, &estado);
    COMPROBAR(arbol && estado == 0);
    lat_objeto *programa = latC_analizar(mv, arbol);
    COMPROBAR(programa && latG_retener(mv, programa) == LAT_G_OK);
    ruta[0] = 'X'; /* La ruta del bytecode tiene almacenamiento propio. */
    COMPROBAR(latC_llamar_funcion(mv, programa) == 0 && mv->ptrpila == 0);
    latA_destruir(arbol);

    lista *l = latL_crear(mv);
    lat_objeto *alias = latC_crear_lista(mv, l);
    latL_agregar(mv, l, latC_crear_cadena(mv, "raíz española"));
    hash_map *mapa = latH_crear(mv);
    lat_objeto *dic = latC_crear_dic(mv, mapa);
    latH_asignar(mv, mapa, "a", alias);
    latL_agregar(mv, l, dic); /* ciclo lista → diccionario → lista */
    latO_asignar_ctx(mv, mv->contexto[0], "datos_p11", dic);
    latO_asignar_ctx(mv, mv->contexto[0], "alias_p11", alias);

    latC_apilar_string(mv, "sólo en la pila");
    unsigned antes = mv->global->strt.nuse;
    for (unsigned i = 0; i < 4000; ++i) {
        char texto[48];
        snprintf(texto, sizeof texto, "temporal-%u", i);
        latC_apilar_string(mv, texto);
        (void)latC_desapilar(mv);
    }
    COMPROBAR(mv->global->strt.nuse == antes + 4000);
    COMPROBAR(latG_recolectar(mv) == LAT_G_OK && mv->global->strt.nuse == antes);
    COMPROBAR(!strcmp(latC_checar_cadena(mv, latC_desapilar(mv)), "sólo en la pila"));
    COMPROBAR(!strcmp(getstr(fijada), "antes_de_habilitar"));
    COMPROBAR(latG_soltar(mv, fijada) == LAT_G_OK);
    COMPROBAR(latG_recolectar(mv) == LAT_G_OK && mv->global->strt.nuse == antes - 2);
    COMPROBAR(latG_recolectar(otra) == LAT_G_OK);
    COMPROBAR(latC_llamar_funcion(mv, programa) == LAT_ERRRUN); /* la función ya existe */
    COMPROBAR(!strcmp(mv->nombre_archivo, "cadenas-p11.mict"));
    COMPROBAR(latG_soltar(mv, programa) == LAT_G_OK);
    latO_destruir(mv, programa);
    lat_objeto *leer = latO_obtener_contexto(mv, mv->contexto[0], "leer_p11");
    COMPROBAR(leer && latC_llamar_funcion(mv, leer) == 0 && mv->ptrpila == 1);
    COMPROBAR(!strcmp(latC_checar_cadena(mv, latC_desapilar(mv)), "raíz española_literal"));
    COMPROBAR(getLista((lat_objeto *)latH_obtener(mapa, "a")) ==
              getLista(latO_obtener_contexto(mv, mv->contexto[0], "alias_p11")));
    COMPROBAR(getDic((lat_objeto *)latL_obtener_elemento(mv, l, 1)) == mapa);
    lat_objeto *opaca = latC_crear_cfuncion(mv, n_opaca);
    COMPROBAR(latC_llamar_funcion(mv, opaca) == 0);
    COMPROBAR(latG_recolectar(mv) == LAT_G_OPACO);
    latC_destruir_mv(otra);
    latC_destruir_mv(mv);
    /* El nuevo modo exige activación antes de compilar y es independiente de G. */
    mv = latC_crear_mv();
    COMPROBAR(mv && latG_habilitar(mv) == LAT_G_OK);
    mv->nombre_archivo = "sin-barrido-cadenas";
    arbol = latA_analizar_exp(fuente, &estado);
    COMPROBAR(arbol && estado == 0 && latC_analizar(mv, arbol));
    COMPROBAR(latG_habilitar_cadenas(mv) == LAT_G_OCUPADO);
    latA_destruir(arbol);
    latC_destruir_mv(mv);
    return EXIT_SUCCESS;
}
