
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

int main(void)
{
    lat_mv mv = {0};

    /* Un objeto numerico real de Mictlan. */
    lat_objeto *numero =
        latC_crear_numerico(&mv, 8);

    /* Su propietario inicial. */
    lat_recurso *original =
        latR_crear(&mv, numero, destruir_objeto);

    assert(original != NULL);

    /* Crear un contenedor administrado. */
    lat_lista_recursos *lista =
        latLR_crear(&mv);

    assert(lista != NULL);

    /* Dos posiciones comparten el mismo recurso. */
    assert(latLR_agregar(&mv, lista, original));
    assert(latLR_agregar(&mv, lista, original));

    assert(latLR_longitud(lista) == 2);
    assert(latR_propietarios(original) == 3);

    /* Liberar la referencia exterior. */
    latR_soltar(&mv, &original);
    assert(original == NULL);

    /* Las dos posiciones conservan la misma identidad. */
    lat_objeto *primero = latLR_obtener(lista, 0);
    lat_objeto *segundo = latLR_obtener(lista, 1);

    assert(primero != NULL);
    assert(primero == segundo);

    primero->val.numerico = 99;

    assert(latC_checar_numerico(&mv, segundo) == 99);
    puts("Identidad compartida: 99");

    /* Un indice invalido no modifica la lista. */
    assert(!latLR_eliminar(&mv, lista, -1));
    assert(latLR_longitud(lista) == 2);

    /* Eliminar una posicion. */
    assert(latLR_eliminar(&mv, lista, 0));

    assert(latLR_longitud(lista) == 1);
    assert(destrucciones == 0);

    lat_objeto *restante = latLR_obtener(lista, 0);

    assert(restante != NULL);
    assert(latC_checar_numerico(&mv, restante) == 99);

    puts("Elemento eliminado; objeto sigue vivo");

    /* Destruir el contenedor con su ultima referencia. */
    latLR_destruir(&mv, &lista);

    assert(lista == NULL);
    assert(destrucciones == 1);
    assert(mv.memoria_usada == 0);

    puts("Contenedor destruido; objeto liberado una vez");
    puts("PRUEBA DE LISTA DE RECURSOS SUPERADA");

    return 0;
}
