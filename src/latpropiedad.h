/* Registro privado: no cambia estructuras ni funciones de la ABI publica. */
#ifndef LAT_PROPIEDAD_H
#define LAT_PROPIEDAD_H
#include "latino.h"
#if defined(__GNUC__) && !defined(_WIN32)
#define LAT_P_INTERNO __attribute__((visibility("hidden")))
#else
#define LAT_P_INTERNO
#endif
typedef enum { LAT_P_OBJETO, LAT_P_LISTA, LAT_P_MAPA, LAT_P_CADENA,
               LAT_P_TIPOS } lat_propiedad_tipo;
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
LAT_P_INTERNO void latP_lista_datos(lat_mv *mv, lista *l);
LAT_P_INTERNO void latP_lista_cubeta(lat_mv *mv, lista *l);
LAT_P_INTERNO void latP_proteger_modulo(lat_mv *mv, hash_map *mapa);
LAT_P_INTERNO void latP_contexto(lat_mv *mv, hash_map *mapa);
LAT_P_INTERNO bool latP_recoleccion_habilitada(lat_mv *mv);
LAT_P_INTERNO void latP_inhibir(lat_mv *mv);
LAT_P_INTERNO unsigned latP_profundidad(lat_mv *mv);
LAT_P_INTERNO void latP_restaurar_profundidad(lat_mv *mv, unsigned nivel);
LAT_P_INTERNO void latP_llamada_nativa(lat_mv *mv, lat_CFuncion funcion);
LAT_P_INTERNO void latP_registrar_cadena(lat_mv *mv, lat_cadena *cadena);
LAT_P_INTERNO void latP_fuente_compilada(lat_mv *mv);
LAT_P_INTERNO lat_bytecode *latP_reducir_codigo(lat_mv *mv, lat_bytecode *codigo,
                                               int celdas);
LAT_P_INTERNO lat_objeto *latP_funcion_compilada(lat_mv *mv, lat_bytecode *codigo,
                                                int ninst);
LAT_P_INTERNO void latP_registrar_funcion_compilada(lat_mv *mv, lat_funcion *fun);
LAT_P_INTERNO bool latP_funcion_registrada(lat_mv *mv, const lat_funcion *fun);
/* El resultado pertenece al llamador. Si falla, devuelve NULL y un mensaje
   estatico, de modo que el llamador pueda limpiar antes de lanzar el error. */
LAT_P_INTERNO char *latP_texto(const lat_objeto *o, const char **error);
#endif
