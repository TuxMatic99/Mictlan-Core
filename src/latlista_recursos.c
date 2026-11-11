
#include "latlista_recursos.h"

#include "latino.h"
#include "latmem.h"

/*
 * Contenedor interno.
 * Cada nodo almacena un lat_recurso *,
 * no un lat_objeto *.
 */
struct lat_lista_recursos {
    lista *nodos;
};

lat_lista_recursos *latLR_crear(lat_mv *mv)
{
    lat_lista_recursos *c =
        latM_asignar(mv, sizeof(*c));

    c->nodos = latL_crear(mv);
    return c;
}

int latLR_longitud(const lat_lista_recursos *c)
{
    return c != NULL ? latL_longitud(c->nodos) : 0;
}

bool latLR_agregar(
    lat_mv *mv,
    lat_lista_recursos *c,
    lat_recurso *recurso
) {
    if (c == NULL || recurso == NULL) {
        return false;
    }

    /* Cada posicion adquiere su propia referencia. */
    lat_recurso *propia = latR_adquirir(recurso);

    if (propia == NULL) {
        return false;
    }

    latL_agregar(mv, c->nodos, propia);
    return true;
}

lat_objeto *latLR_obtener(
    const lat_lista_recursos *c,
    int indice
) {
    if (c == NULL ||
        indice < 0 ||
        indice >= latL_longitud(c->nodos)) {
        return NULL;
    }

    nodo_lista *nodo =
        latL_obtener_nodo(NULL, c->nodos, indice);

    lat_recurso *recurso =
        (lat_recurso *)nodo->valor;

    /* Referencia prestada: no incrementa propietarios. */
    return (lat_objeto *)latR_dato(recurso);
}

bool latLR_reemplazar(
    lat_mv *mv,
    lat_lista_recursos *c,
    int indice,
    lat_recurso *nuevo
) {
    if (c == NULL ||
        nuevo == NULL ||
        indice < 0 ||
        indice >= latL_longitud(c->nodos)) {
        return false;
    }

    nodo_lista *nodo = latL_obtener_nodo(mv, c->nodos, indice);
    if (nodo == NULL) {
        return false;
    }

    lat_recurso *anterior = (lat_recurso *)nodo->valor;

    /* No modificar contadores si el objeto no cambia. */
    if (anterior == nuevo) {
        return true;
    }

    /* Adquirir primero, para mantener una referencia valida. */
    lat_recurso *propia = latR_adquirir(nuevo);
    if (propia == NULL) {
        return false;
    }

    nodo->valor = propia;
    latR_soltar(mv, &anterior);
    return true;
}

bool latLR_eliminar(
    lat_mv *mv,
    lat_lista_recursos *c,
    int indice
) {
    if (c == NULL ||
        indice < 0 ||
        indice >= latL_longitud(c->nodos)) {
        return false;
    }

    nodo_lista *nodo =
        latL_obtener_nodo(mv, c->nodos, indice);

    lat_recurso *recurso =
        (lat_recurso *)latL_eliminar_nodo(
            mv, c->nodos, nodo
        );

    /* Soltar exclusivamente la referencia del nodo. */
    latR_soltar(mv, &recurso);
    return true;
}

void latLR_destruir(
    lat_mv *mv,
    lat_lista_recursos **referencia
) {
    if (referencia == NULL || *referencia == NULL) {
        return;
    }

    lat_lista_recursos *c = *referencia;
    *referencia = NULL;

    /* Liberar cada referencia almacenada. */
    while (latL_longitud(c->nodos) > 0) {
        lat_recurso *recurso =
            (lat_recurso *)latL_desapilar(
                mv, c->nodos
            );

        latR_soltar(mv, &recurso);
    }

    /* Liberar el contenedor generico ya vacio. */
    latL_destruir(mv, c->nodos);
    latM_liberar(mv, c);
}