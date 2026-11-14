/* Ayudantes privados: no agregan simbolos a la ABI publica. */
#ifndef LAT_COLECCIONES_H
#define LAT_COLECCIONES_H
#include "latpropiedad.h"
LAT_P_INTERNO bool latP_dic_eliminar(lat_mv *mv, hash_map *m, const char *clave);
/* Solo para operaciones de colecciones: el comparador escalar sigue intacto. */
static inline int latP_numero_lista(lat_mv *mv, double a, double b) {
  if (a != a || b != b) {
    latC_error(mv, "NaN no comparable en listas");
    return 0;
  }
  return a == b ? 0 : a < b ? -1 : 1;
}
#endif
