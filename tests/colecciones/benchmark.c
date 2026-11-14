#define _POSIX_C_SOURCE 200809L
#include "latino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
static double reloj(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec/1e9;}
static long rss(void){struct rusage r;return getrusage(RUSAGE_SELF,&r)?-1:r.ru_maxrss;}
int main(int argc,char **argv){
 if(argc!=4)return 2;
 const char *nombre=argv[1];int n=atoi(argv[2]),vueltas=atoi(argv[3]);if(n<1||n>10000||vueltas<1||vueltas>100000)return 2;
 int dic=!strcmp(nombre,"dic_contiene");
 lat_mv *mv=latC_crear_mv();mv->nombre_archivo="medicion-colecciones";
 lat_objeto *origen=dic?latC_crear_dic(mv,latH_crear(mv)):latC_crear_lista(mv,latL_crear(mv));
 char clave[32] = "";
 for(int i=0;i<n;++i){lat_objeto *v=latC_crear_numerico(mv,i%10);if(dic){snprintf(clave,sizeof clave,"k_%d",i);latH_asignar(mv,getDic(origen),clave,v);}else latL_agregar(mv,getLista(origen),v);}
 lat_objeto *mod=latO_obtener_contexto(mv,mv->contexto[0],dic?"dic":"lista");
 lat_objeto *f=latH_obtener(getDic(mod),dic?"contiene":nombre);if(!f)return 77;
 lat_objeto *separador=latC_crear_cadena(mv,",");lat_objeto *llave=latC_crear_cadena(mv,clave);
 size_t inicio=0,fin;long r0=0;double t0=0;unsigned long control=0;
 for(int i=0;i<=vueltas;++i){
  if(i==1){inicio=mv->memoria_usada;r0=rss();t0=reloj();}
  latC_apilar(mv,origen);
  if(dic)latC_apilar(mv,llave);
  else if(!strcmp(nombre,"concatenar")||!strcmp(nombre,"comparar"))latC_apilar(mv,origen);
  else if(!strcmp(nombre,"separar"))latC_apilar(mv,separador);
  if(latC_llamar_funcion(mv,f))return 1;
  lat_objeto *o=latC_desapilar(mv);
  if(dic){if(o->tipo!=T_BOOL||!getLogico(o))return 1;control++;}
  else if(!strcmp(nombre,"comparar")){if(o->tipo!=T_NUMERIC||getNumerico(o)!=0)return 1;control++;}
  else if(o->tipo==T_LIST){int esperada=!strcmp(nombre,"concatenar")?n*2:n;if(getLista(o)->longitud!=esperada||getLista(origen)->longitud!=n)return 1;control+=(unsigned long)getLista(o)->longitud;}
  else if(o->tipo==T_STR){size_t bytes=strlen(latC_checar_cadena(mv,o));if(bytes!=(size_t)n*2-1)return 1;control+=(unsigned long)bytes;}
  else return 1;
 }
 double tiempo=reloj()-t0;fin=mv->memoria_usada;
 printf("%s,%d,%d,%.9f,%zu,%zu,%lld,%ld,%ld,%lu\n",nombre,n,vueltas,tiempo,inicio,fin,(long long)fin-(long long)inicio,r0,rss(),control);
 latC_destruir_mv(mv);return 0;
}
