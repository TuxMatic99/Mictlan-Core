#include "latino.h"
#include <stdio.h>
int main(void){
 lat_mv *mv=latC_crear_mv();mv->nombre_archivo="profundidad-b2";
 lat_objeto *a=latC_crear_numerico(mv,5);
 for(int i=0;i<256;++i){lat_objeto *l=latC_crear_lista(mv,latL_crear(mv));latL_agregar(mv,getLista(l),a);a=l;}
 lat_objeto *mod=latO_obtener_contexto(mv,mv->contexto[0],"lista");lat_objeto *f=latH_obtener(getDic(mod),"comparar");
 latC_apilar(mv,a);latC_apilar(mv,a);
 int e=latC_llamar_funcion(mv,f);if(e || getNumerico(latC_desapilar(mv))!=0)return 1;
 lat_objeto *l=latC_crear_lista(mv,latL_crear(mv));latL_agregar(mv,getLista(l),a);
 latC_apilar_double(mv,777);latC_apilar(mv,l);latC_apilar(mv,l);
 e=latC_llamar_funcion(mv,f);int ok=e==LAT_ERRRUN && mv->ptrpila==1 && getNumerico(latC_tope(mv))==777 && mv->ptrctx==0 && !mv->error;
 latC_destruir_mv(mv);return ok?0:1;
}
