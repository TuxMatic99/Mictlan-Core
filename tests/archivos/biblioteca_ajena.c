/* Biblioteca sintética que permite demostrar la detección de un enlace ajeno.
   Nunca debe ejecutarse su función: el comprobador sólo inspecciona la dirección. */
#include <stdlib.h>
void *latC_crear_mv(void){abort();}
