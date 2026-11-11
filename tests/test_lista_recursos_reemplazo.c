#include <assert.h>
#include <stdio.h>

#include "latino.h"
#include "latlista_recursos.h"

static int destrucciones = 0;

static void destruir_objeto(lat_mv *mv, void *dato)
{
    destrucciones++;
    latO_destruir(mv, (lat_objeto *)dato);
}

static lat_recurso *crear_recurso(lat_mv *mv, int valor)
{
    lat_objeto *objeto = latC_crear_numerico(mv, valor);
    assert(objeto != NULL);
    lat_recurso *recurso = latR_crear(mv, objeto, destruir_objeto);
    assert(recurso != NULL);
    return recurso;
}

int main(void)
{
    lat_mv mv = {0};
    lat_recurso *a = crear_recurso(&mv, 11);
    lat_recurso *b = crear_recurso(&mv, 22);
    lat_recurso *c = crear_recurso(&mv, 33);
    lat_lista_recursos *lista = latLR_crear(&mv);
    assert(lista != NULL);

    assert(latLR_agregar(&mv, lista, a));
    assert(latLR_agregar(&mv, lista, a));
    assert(latLR_agregar(&mv, lista, b));
    assert(latLR_longitud(lista) == 3);
    assert(latR_propietarios(a) == 3);
    assert(latR_propietarios(b) == 2);
    assert(latR_propietarios(c) == 1);

    /* Los errores no modifican ni el contenido ni la propiedad. */
    assert(!latLR_reemplazar(&mv, NULL, 0, b));
    assert(!latLR_reemplazar(&mv, lista, -1, b));
    assert(!latLR_reemplazar(&mv, lista, 3, b));
    assert(!latLR_reemplazar(&mv, lista, 0, NULL));
    assert(latLR_longitud(lista) == 3);
    assert(latR_propietarios(a) == 3);
    assert(latR_propietarios(b) == 2);
    assert(latR_propietarios(c) == 1);
    assert(latC_checar_numerico(&mv, latLR_obtener(lista, 0)) == 11);
    assert(destrucciones == 0);

    /* A -> B; A sigue vivo en otra posicion. */
    assert(latLR_reemplazar(&mv, lista, 0, b));
    assert(latR_propietarios(a) == 2);
    assert(latR_propietarios(b) == 3);
    assert(latLR_obtener(lista, 0) == latLR_obtener(lista, 2));
    assert(latC_checar_numerico(&mv, latLR_obtener(lista, 1)) == 11);

    /* B -> B no cambia los contadores. */
    assert(latLR_reemplazar(&mv, lista, 0, b));
    assert(latR_propietarios(b) == 3);
    assert(destrucciones == 0);

    /* Sustituir la otra posicion y soltar el A exterior. */
    assert(latLR_reemplazar(&mv, lista, 1, b));
    assert(latR_propietarios(a) == 1);
    assert(latR_propietarios(b) == 4);
    latR_soltar(&mv, &a);
    assert(a == NULL);
    assert(destrucciones == 1);

    /* Sustituir B por C, luego C por B: liberar C una vez. */
    assert(latLR_reemplazar(&mv, lista, 2, c));
    assert(latR_propietarios(b) == 3);
    assert(latR_propietarios(c) == 2);
    latR_soltar(&mv, &c);
    assert(c == NULL);
    assert(latC_checar_numerico(&mv, latLR_obtener(lista, 2)) == 33);
    assert(latLR_reemplazar(&mv, lista, 2, b));
    assert(latR_propietarios(b) == 4);
    assert(destrucciones == 2);

    latR_soltar(&mv, &b);
    assert(b == NULL);
    latLR_destruir(&mv, &lista);
    assert(lista == NULL);
    assert(destrucciones == 3);
    assert(mv.memoria_usada == 0);

    /* Caso critico: auto-reemplazo siendo la lista el unico dueño. */
    lat_mv mv_unico = {0};
    lat_recurso *unico = crear_recurso(&mv_unico, 44);
    lat_lista_recursos *otra = latLR_crear(&mv_unico);
    assert(otra != NULL);
    assert(latLR_agregar(&mv_unico, otra, unico));
    lat_recurso *prestado = unico;
    latR_soltar(&mv_unico, &unico);
    assert(unico == NULL);
    assert(latR_propietarios(prestado) == 1);
    assert(latLR_reemplazar(&mv_unico, otra, 0, prestado));
    assert(latR_propietarios(prestado) == 1);
    assert(destrucciones == 3);
    assert(latC_checar_numerico(&mv_unico, latLR_obtener(otra, 0)) == 44);
    latLR_destruir(&mv_unico, &otra);
    assert(otra == NULL);
    assert(destrucciones == 4);
    assert(mv_unico.memoria_usada == 0);

    puts("PRUEBA DE REEMPLAZO DE RECURSOS SUPERADA");
    return 0;
}
