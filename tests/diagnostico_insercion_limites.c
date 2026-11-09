
#include <assert.h>
#include <stdio.h>
#include "latino.h"

/* Insertar un numero en la posicion indicada. */
static void insertar(lat_mv *mv, lista *datos,
                     int numero, int posicion)
{
    lat_objeto *valor =
        latC_crear_numerico(mv, numero);

    latL_insertar_elemento(
        mv, datos, valor, posicion
    );
}

int main(void)
{
    lat_mv mv = {0};
    lista *datos = latL_crear(&mv);

    /* Insertar en una lista vacia. */
    insertar(&mv, datos, 20, 0);

    /* Insertar al principio. */
    insertar(&mv, datos, 10, 0);

    /* Insertar al final. */
    insertar(&mv, datos, 40, 2);

    /* Insertar dos veces en medio. */
    insertar(&mv, datos, 30, 2);
    insertar(&mv, datos, 25, 2);

    double esperado[] = {10, 20, 25, 30, 40};

    assert(latL_longitud(datos) == 5);

    nodo_lista *anterior = NULL;
    nodo_lista *actual = datos->primero;

    for (int i = 0; i < 5; i++) {
        assert(actual != NULL);

        /* Comprobar los enlaces hacia atras. */
        assert(actual->anterior == anterior);

        lat_objeto *objeto = actual->valor;

        assert(latC_checar_numerico(&mv, objeto)
               == esperado[i]);

        anterior = actual;
        actual = actual->siguiente;
    }

    /* Comprobar que la cadena termina correctamente. */
    assert(actual == NULL);
    assert(datos->ultimo == anterior);

    lat_objeto *contenedor =
        latC_crear_lista(&mv, datos);

    latO_destruir(&mv, contenedor);

    puts("Inserciones y enlaces correctos");
    return 0;
}
