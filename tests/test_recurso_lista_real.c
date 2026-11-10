
#include <assert.h>
#include <stdio.h>

#include "latino.h"
#include "latrecurso.h"

static int destrucciones = 0;

/* Destruir la lista mediante la API real de Core. */
static void destruir_lista(lat_mv *mv, void *dato)
{
    destrucciones++;
    latO_destruir(mv, (lat_objeto *)dato);
}

int main(void)
{
    lat_mv mv = {0};

    /* Crear una lista real con el numero 8. */
    lista *datos = latL_crear(&mv);

    latL_agregar(
        &mv, datos, latC_crear_numerico(&mv, 8)
    );

    lat_objeto *objeto =
        latC_crear_lista(&mv, datos);

    /* Registrar el objeto como recurso compartido. */
    lat_recurso *original =
        latR_crear(&mv, objeto, destruir_lista);

    assert(original != NULL);

    /* Crear un segundo propietario. */
    lat_recurso *alias = latR_adquirir(original);

    assert(alias != NULL);
    assert(latR_propietarios(alias) == 2);

    /* El primer propietario deja de usar la lista. */
    latR_soltar(&mv, &original);

    assert(original == NULL);
    assert(destrucciones == 0);

    /* El segundo propietario puede seguir usandola. */
    lat_objeto *lista_viva = latR_dato(alias);

    lista *contenido = latC_checar_lista(
        &mv, lista_viva
    );

    latL_agregar(
        &mv, contenido, latC_crear_numerico(&mv, 16)
    );

    assert(latL_longitud(contenido) == 2);

    lat_objeto *segundo =
        latL_obtener_elemento(&mv, contenido, 1);

    assert(latC_checar_numerico(&mv, segundo) == 16);

    puts("La lista sigue viva: [8, 16]");

    /* El ultimo propietario libera el objeto completo. */
    latR_soltar(&mv, &alias);

    assert(alias == NULL);
    assert(destrucciones == 1);
    assert(mv.memoria_usada == 0);

    puts("Lista destruida una sola vez");
    puts("PRUEBA DE LISTA REAL SUPERADA");

    return 0;
}
