
#ifndef MICTLAN_LATLISTA_RECURSOS_H
#define MICTLAN_LATLISTA_RECURSOS_H

#include <stdbool.h>
#include "latrecurso.h"

typedef struct _lat_objeto lat_objeto;
typedef struct lat_lista_recursos lat_lista_recursos;

/* Crear un contenedor de referencias propietarias. */
lat_lista_recursos *latLR_crear(lat_mv *mv);

/*
 * Agregar un recurso existente.
 * Adquiere una referencia propia del contenedor.
 * No transfiere la referencia del llamador.
 */
bool latLR_agregar(
    lat_mv *mv,
    lat_lista_recursos *contenedor,
    lat_recurso *recurso
);

/*
 * Obtener un objeto prestado.
 * No usarlo despues de liberar su ultima referencia.
 */
lat_objeto *latLR_obtener(
    const lat_lista_recursos *contenedor,
    int indice
);

/* Cantidad de elementos. */
int latLR_longitud(const lat_lista_recursos *contenedor);

/* Eliminar una posicion y liberar su referencia. */
bool latLR_eliminar(
    lat_mv *mv,
    lat_lista_recursos *contenedor,
    int indice
);

/* Liberar todas las referencias y el contenedor. */
void latLR_destruir(
    lat_mv *mv,
    lat_lista_recursos **contenedor
);

#endif
