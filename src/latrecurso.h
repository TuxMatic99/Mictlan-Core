
#ifndef MICTLAN_LATRECURSO_H
#define MICTLAN_LATRECURSO_H

#include <stddef.h>

/* Declaraciones de tipos internos. */
typedef struct lat_mv lat_mv;
typedef struct lat_recurso lat_recurso;

/* Funcion encargada de destruir el dato. */
typedef void (*latR_destructor)(lat_mv *, void *);

/* Crear un recurso con un propietario. */
lat_recurso *latR_crear(
    lat_mv *mv,
    void *dato,
    latR_destructor destruir
);

/* Adquirir una nueva referencia propietaria. */
lat_recurso *latR_adquirir(lat_recurso *recurso);

/* Consultar el dato y su numero de propietarios. */
void *latR_dato(const lat_recurso *recurso);
size_t latR_propietarios(const lat_recurso *recurso);

/* Liberar una referencia y anular su variable. */
void latR_soltar(lat_mv *mv, lat_recurso **referencia);

#endif
