#include "latino.h"
#include "latmemoria.h"

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "P02G_ASSERT recompilacion linea %d: %s\n", __LINE__, #x); \
    exit(EXIT_FAILURE); \
} } while (0)

static latG_medicion compilar(lat_mv *mv, unsigned vueltas, bool distintas) {
    char fuente[120];
    for (unsigned i = 0; i < vueltas; ++i) {
        snprintf(fuente, sizeof(fuente), "valor = [\"muestra_%u\", %u]\n",
                 distintas ? i : 0, i);
        int estado;
        ast *arbol = latA_analizar_exp(fuente, &estado);
        CHECK(arbol && estado == 0);
        lat_objeto *programa = latC_analizar(mv, arbol);
        CHECK(programa && latC_llamar_funcion(mv, programa) == 0);
        CHECK(mv->ptrctx == 0 && mv->ptrpila == 0 && mv->numejec == 0 && !mv->error);
        latO_destruir(mv, programa);
        latA_destruir(arbol);
        if (i % 50 == 0) CHECK(latG_recolectar(mv) == LAT_G_OK);
    }
    CHECK(latG_recolectar(mv) == LAT_G_OK);
    lat_objeto *valor = latO_obtener_contexto(mv, mv->contexto[0], "valor");
    CHECK(valor && valor->tipo == T_LIST);
    lista *lista_valor = getLista(valor);
    CHECK(latL_longitud(lista_valor) == 2);
    lat_objeto *elemento = latL_obtener_elemento(mv, lista_valor, 1);
    CHECK(elemento && elemento->tipo == T_NUMERIC && getNumerico(elemento) == vueltas - 1);
    latG_medicion datos;
    CHECK(latG_medir(mv, &datos) == LAT_G_OK);
    return datos;
}

int main(void) {
    const unsigned vueltas = 500;
    lat_mv *iguales = latC_crear_mv(), *distintas = latC_crear_mv();
    CHECK(iguales && distintas);
    iguales->nombre_archivo = distintas->nombre_archivo = "alpha_recompilacion";
    CHECK(latG_habilitar(iguales) == LAT_G_OK && latG_habilitar(distintas) == LAT_G_OK);
    int cadenas_base = iguales->global->strt.nuse;
    CHECK(cadenas_base == distintas->global->strt.nuse);
    latG_medicion estable = compilar(iguales, vueltas, false);
    latG_medicion variado = compilar(distintas, vueltas, true);
    int cadenas_iguales = iguales->global->strt.nuse - cadenas_base;
    int cadenas_distintas = distintas->global->strt.nuse - cadenas_base;
    CHECK(cadenas_distintas - cadenas_iguales == (int)vueltas - 1);
    CHECK(variado.bytes_vm > estable.bytes_vm);
    latC_destruir_mv(distintas);
    CHECK(latG_recolectar(iguales) == LAT_G_OK);
    printf("{\"compilaciones_por_vm\":%u,\"cadenas_repetidas\":%d,"
           "\"cadenas_distintas\":%d,\"bytes_vm_repetidas\":%zu,"
           "\"bytes_vm_distintas\":%zu,\"pico_repetidas\":%zu,\"pico_distintas\":%zu}\n",
           vueltas, cadenas_iguales, cadenas_distintas, estable.bytes_vm,
           variado.bytes_vm, estable.pico_bytes_vm, variado.pico_bytes_vm);
    latC_destruir_mv(iguales);
    return EXIT_SUCCESS;
}
