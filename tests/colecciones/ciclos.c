#include "latino.h"
#include <stdio.h>
#include <stdlib.h>
#define C(c) do{if(!(c)){fprintf(stderr,"Ciclos B2 linea %d: %s\n",__LINE__,#c);return 1;}}while(0)
static lat_objeto *f(lat_mv *mv,const char *n){lat_objeto *l=latO_obtener_contexto(mv,mv->contexto[0],"lista");return latH_obtener(getDic(l),n);}
int main(void){
 lat_mv *mv=latC_crear_mv();mv->nombre_archivo="ciclos-b2";
 lat_objeto *a=latC_crear_lista(mv,latL_crear(mv));latL_agregar(mv,getLista(a),a);
 latC_apilar_double(mv,777);latC_apilar(mv,a);latC_apilar(mv,a);
 C(latC_llamar_funcion(mv,f(mv,"comparar"))==LAT_ERRRUN);
 C(mv->ptrpila==1 && getNumerico(latC_tope(mv))==777 && !mv->error);
 latC_apilar(mv,a);C(latC_llamar_funcion(mv,f(mv,"invertir"))==0);
 lat_objeto copia=*latC_desapilar(mv);C(getLista(&copia)!=getLista(a));
 C(latL_obtener_elemento(mv,getLista(&copia),0)==a);
 latC_apilar(mv,a);latC_apilar(mv,a);C(latC_llamar_funcion(mv,f(mv,"concatenar"))==0);
 C(getLista(latC_desapilar(mv))->longitud==2);
 latC_apilar(mv,a);latC_apilar_double(mv,0);C(latC_llamar_funcion(mv,f(mv,"eliminar_indice"))==0);
 C(getLista(a)->longitud==0 && getLista(&copia)->longitud==1);
 C(mv->ptrctx==0 && mv->numejec==0 && mv->ptrpila==1);
 (void)latC_desapilar(mv);latC_destruir_mv(mv);return 0;
}
