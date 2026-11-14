#include "latino.h"
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;if(!setlocale(LC_NUMERIC,argv[1])){fprintf(stderr,"Locale no instalado: %s\n",argv[1]);return 77;}
 char anterior[128];snprintf(anterior,sizeof anterior,"%s",setlocale(LC_NUMERIC,NULL));
 lat_mv *mv=latC_crear_mv();mv->nombre_archivo="locale-matematicas";
 lat_objeto *f=latO_obtener_contexto(mv,mv->contexto[0],"anumero_estricto");int fallo=0;
 const char *textos[]={" 1.25 ","-0","1,25","1e309"};
 for(int i=0;i<4;++i){latC_apilar_string(mv,textos[i]);int e=latC_llamar_funcion(mv,f);
  if(i<2){if(e)fallo=1;else{lat_objeto *o=latC_desapilar(mv);if(o->tipo!=T_NUMERIC||(i==0?getNumerico(o)!=1.25:!signbit(getNumerico(o))))fallo=1;}}
  else if(e!=LAT_ERRRUN)fallo=1;
  if(strcmp(anterior,setlocale(LC_NUMERIC,NULL))||mv->ptrpila!=0)fallo=1;
 }
 latC_destruir_mv(mv);return fallo;
}
