/* Registro privado: no cambia estructuras ni funciones de la ABI publica. */
#ifndef LAT_PROPIEDAD_H
#define LAT_PROPIEDAD_H
#include "latino.h"
#if defined(__GNUC__) && !defined(_WIN32)
#define LAT_P_INTERNO __attribute__((visibility("hidden")))
#else
#define LAT_P_INTERNO
#endif
typedef enum { LAT_P_OBJETO, LAT_P_LISTA, LAT_P_MAPA, LAT_P_TIPOS } lat_propiedad_tipo;
LAT_P_INTERNO void latP_iniciar(lat_mv *mv);
LAT_P_INTERNO void latP_registrar(lat_mv *mv, void *ptr, lat_propiedad_tipo tipo);
LAT_P_INTERNO void latP_olvidar(lat_mv *mv, void *ptr);
LAT_P_INTERNO bool latP_contiene(lat_mv *mv, void *ptr, lat_propiedad_tipo tipo);
LAT_P_INTERNO void *latP_primero(lat_mv *mv, lat_propiedad_tipo tipo);
LAT_P_INTERNO void latP_retener_constante(lat_mv *mv, lat_objeto *o);
LAT_P_INTERNO bool latP_es_constante(lat_mv *mv, lat_objeto *o);
LAT_P_INTERNO void latP_soltar_constantes(lat_mv *mv);
LAT_P_INTERNO void latP_cerrar(lat_mv *mv);
LAT_P_INTERNO void latP_liberar_objetos(lat_mv *mv);
#endif
