/* Recoleccion explicita de objetos para anfitriones C que gestionan raices. */
#ifndef LAT_MEMORIA_H
#define LAT_MEMORIA_H

#include "latino.h"

typedef enum {
    LAT_G_OK = 0,
    LAT_G_INACTIVO,
    LAT_G_OCUPADO,
    LAT_G_OPACO,
    LAT_G_REFERENCIA
} latG_estado;

typedef struct {
    size_t reservas_registradas;
    size_t bytes_registrados;
    size_t fijados;
    size_t bytes_vm;
    size_t pico_bytes_vm;
    size_t reservas_recuperadas;
    size_t bytes_recuperados;
} latG_medicion;

/* Activar antes de ejecutar la carga. El anfitrion conserva explicitamente
   todo puntero a objetos, listas o tablas que use despues de recolectar. */
LATINO_API latG_estado latG_habilitar(lat_mv *mv);
/* Activa también el barrido de cadenas internadas. Llamar después de
   latG_habilitar y antes de compilar o ejecutar. Los punteros prestados a
   texto sobreviven a una recolección solo con una raíz o fijación. */
#define LAT_G_CADENAS_EXPLICITAS 1
LATINO_API latG_estado latG_habilitar_cadenas(lat_mv *mv);
/* Una funcion autorizada respeta el contrato de raices y usa esta VM en
   forma sincronica. Autorizar antes de la primera llamada. */
LATINO_API latG_estado latG_autorizar(lat_mv *mv, lat_CFuncion funcion);
/* Fijar una direccion registrada de esta VM. Cada exito requiere soltar.
   En modo cadenas puede fijar getCadena(objeto), nunca getstr(...).
   Las celdas de pila y los contextos de llamada no son fijables. */
LATINO_API latG_estado latG_retener(lat_mv *mv, const void *reserva);
LATINO_API latG_estado latG_soltar(lat_mv *mv, const void *reserva);
/* Solo entre ejecuciones, en el hilo propietario. Un estado distinto de
   LAT_G_OK deja intactas las reservas. No invoca destructores externos. */
LATINO_API latG_estado latG_recolectar(lat_mv *mv);
/* Bytes de latM y del registro segun el asignador. Los ultimos dos campos
   corresponden a la ultima recoleccion exitosa. Disponible sin activar GC. */
LATINO_API latG_estado latG_medir(lat_mv *mv, latG_medicion *medicion);

#endif
