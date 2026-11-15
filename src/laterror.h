/* Disposicion privada del manejador ya usado por latC_llamar_funcion.
   Compartida para liberar temporales antes de propagar el mismo error. */
#ifndef LAT_ERROR_INTERNO_H
#define LAT_ERROR_INTERNO_H
#include "latino.h"
struct lat_longjmp {
  struct lat_longjmp *previo;
  lat_jmpbuf b;
  volatile int status;
};
/* El guard y los datos pertenecen al llamador, fuera del marco de setjmp:
   siguen definidos aunque una operación lance longjmp. No se modifica la VM
   ni se registra una segunda función nativa para realizar la limpieza. */
static inline int latE_proteger(lat_mv *mv, struct lat_longjmp *guard,
                         void (*accion)(lat_mv *, void *), void *datos) {
    guard->previo = mv->error;
    guard->status = 0;
    mv->error = guard;
    LAT_TRY(mv, guard, accion(mv, datos););
    mv->error = guard->previo;
    return guard->status;
}

#endif
