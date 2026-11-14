#ifndef LAT_NUMERICO_H
#define LAT_NUMERICO_H
#include "latino.h"
#include <limits.h>
#include <math.h>
static inline int latN_entero(lat_mv *mv, double x, const char *mensaje) {
    double t = trunc(x);
    if (!isfinite(x) || t < INT_MIN || t > INT_MAX) latC_error(mv, "%s", mensaje);
    return (int)t;
}
static inline char latN_caracter(lat_mv *mv, double x) {
    double t = trunc(x);
    if (!isfinite(x) || t < CHAR_MIN || t > CHAR_MAX)
        latC_error(mv, "Conversion a caracter fuera de rango");
    return (char)t;
}
#endif
