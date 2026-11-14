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
#endif
