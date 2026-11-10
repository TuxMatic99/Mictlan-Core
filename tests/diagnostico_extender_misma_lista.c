
#include <assert.h>
#include <stdio.h>
#include "latino.h"

int main(void)
{
    lat_mv mv = {0};

    int a = 8;
    int b = 16;

    lista *datos = latL_crear(&mv);

    latL_agregar(&mv, datos, &a);
    latL_agregar(&mv, datos, &b);

    /* Extender la lista consigo misma. */
    latL_extender(&mv, datos, datos);

    assert(latL_longitud(datos) == 4);

    nodo_lista *n = datos->primero;

    assert(n->valor == &a);
    n = n->siguiente;

    assert(n->valor == &b);
    n = n->siguiente;

    assert(n->valor == &a);
    n = n->siguiente;

    assert(n->valor == &b);
    assert(n == datos->ultimo);
    assert(n->siguiente == NULL);

    /* Liberar nodos, no los enteros de la pila C. */
    latL_destruir(&mv, datos);

    assert(mv.memoria_usada == 0);

    puts("Autoextension correcta: [8, 16, 8, 16]");
    return 0;
}
