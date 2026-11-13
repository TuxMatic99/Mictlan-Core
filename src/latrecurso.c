
#include "latrecurso.h"
#include "latmem.h"
#include "latpropiedad.h"

#include <stdint.h>

/* Identidad del recurso y contador de propietarios. */
struct lat_recurso {
    size_t propietarios;
    void *dato;
    latR_destructor destruir;
};

lat_recurso *latR_crear(
    lat_mv *mv,
    void *dato,
    latR_destructor destruir
) {
    if (dato == NULL || destruir == NULL) {
        return NULL;
    }

    latP_inhibir(mv);
    lat_recurso *r = latM_asignar(mv, sizeof(*r));

    r->propietarios = 1;
    r->dato = dato;
    r->destruir = destruir;

    return r;
}

lat_recurso *latR_adquirir(lat_recurso *r) {
    if (r == NULL || r->propietarios == SIZE_MAX) {
        return NULL;
    }

    r->propietarios++;
    return r;
}

void *latR_dato(const lat_recurso *r) {
    return r != NULL ? r->dato : NULL;
}

size_t latR_propietarios(const lat_recurso *r) {
    return r != NULL ? r->propietarios : 0;
}

void latR_soltar(lat_mv *mv, lat_recurso **referencia) {
    if (referencia == NULL || *referencia == NULL) {
        return;
    }

    lat_recurso *r = *referencia;

    /* La variable del propietario deja de apuntar al recurso. */
    *referencia = NULL;

    r->propietarios--;

    /* Otros propietarios siguen utilizando el recurso. */
    if (r->propietarios != 0) {
        return;
    }

    /* El ultimo propietario libera el dato y su control. */
    r->destruir(mv, r->dato);
    latM_liberar(mv, r);
}
