/* Ruta de error: conservar y liberar la primera conversion si falla la segunda. */
#include "latino.h"
#include <stdlib.h>
#include <stdio.h>
int parse_silent=0;
char *filename=0;
static char *prestado;
static char *observar_texto(lat_mv *mv,lat_objeto *o) {
    char *p=latC_astring(mv,o);
    if(!prestado)prestado=p;
    return p;
}
static void observar_free(void *p) {if(p==prestado)prestado=NULL;free(p);}
#define latC_astring observar_texto
#define free observar_free
#include "../../src/latstrlib.c"
#undef latC_astring
#undef free
int main(void) {
    lat_mv *mv=latC_crear_mv();mv->nombre_archivo="concat-error";
    lista *l=latL_crear(mv);lat_objeto *ciclo=latC_crear_lista(mv,l);
    latL_agregar(mv,l,ciclo);
    lat_objeto *llamada=latC_crear_cfuncion(mv,str_concatenar);
    int estado=0;
    for(int i=0;i<2;++i) {
        if(i) latC_apilar_double(mv,42); else latC_apilar_string(mv,"prefijo");
        latC_apilar(mv,ciclo);
        estado=latC_llamar_funcion(mv,llamada);
        if(estado!=LAT_ERRRUN) break;
    }
    latC_destruir_mv(mv);
    if(estado!=LAT_ERRRUN || prestado) {
        fprintf(stderr,"Conversion sin liberar tras error: %d\n",prestado!=NULL);
        return 1;
    }
    return 0;
}
