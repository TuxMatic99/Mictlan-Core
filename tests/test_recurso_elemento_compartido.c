
#include <assert.h>
#include <stdio.h>

#include "latino.h"
#include "latrecurso.h"

static int destrucciones = 0;

/* Destructor de un objeto real de Mictlan. */
static void destruir_objeto(lat_mv *mv, void *dato)
{
    destrucciones++;
    latO_destruir(mv, (lat_objeto *)dato);
}

int main(void)
{
    lat_mv mv = {0};

    /* Crear un objeto numerico real. */
    lat_objeto *numero =
        latC_crear_numerico(&mv, 8.0);

    /* Su primera referencia propietaria. */
    lat_recurso *original =
        latR_crear(&mv, numero, destruir_objeto);

    assert(original != NULL);

    /* Crear una lista generica para guardar referencias. */
    lista *contenedor = latL_crear(&mv);

    /* Cada posicion adquiere su propia referencia. */
    lat_recurso *primera = latR_adquirir(original);
    lat_recurso *segunda = latR_adquirir(original);

    assert(primera != NULL);
    assert(segunda != NULL);
    assert(primera == segunda);
    assert(latR_propietarios(original) == 3);

    latL_agregar(&mv, contenedor, primera);
    latL_agregar(&mv, contenedor, segunda);

    /* Soltar al propietario exterior. */
    latR_soltar(&mv, &original);

    assert(original == NULL);
    assert(latR_propietarios(primera) == 2);

    /* Recuperar ambas referencias desde los nodos. */
    nodo_lista *nodo1 = contenedor->primero;
    nodo_lista *nodo2 = contenedor->ultimo;

    lat_recurso *ref1 = (lat_recurso *)nodo1->valor;
    lat_recurso *ref2 = (lat_recurso *)nodo2->valor;

    assert(ref1 == ref2);

    /* Modificar el objeto desde la primera posicion. */
    lat_objeto *objeto1 = latR_dato(ref1);
    objeto1->val.numerico = 99.0;

    /* La segunda posicion observa el mismo cambio. */
    lat_objeto *objeto2 = latR_dato(ref2);

    assert(latC_checar_numerico(&mv, objeto2) == 99.0);

    puts("Ambas posiciones comparten el valor: 99");

    /* Liberar solamente la primera referencia. */
    nodo1->valor = NULL;
    latR_soltar(&mv, &ref1);

    assert(destrucciones == 0);
    assert(latR_propietarios(ref2) == 1);

    /* El objeto sigue vivo desde la segunda posicion. */
    assert(latC_checar_numerico(&mv, objeto2) == 99.0);

    puts("Primera referencia liberada; objeto vivo");

    /* Liberar la ultima referencia. */
    nodo2->valor = NULL;
    latR_soltar(&mv, &ref2);

    assert(destrucciones == 1);

    /* Destruir los nodos del contenedor. */
    latL_destruir(&mv, contenedor);

    assert(mv.memoria_usada == 0);

    puts("Objeto destruido exactamente una vez");
    puts("PRUEBA DE ELEMENTO COMPARTIDO SUPERADA");

    return 0;
}
