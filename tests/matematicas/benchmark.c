#define _POSIX_C_SOURCE 200809L
#include "latino.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
static double reloj(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec/1e9;}
int main(int argc,char **argv){
 if(argc!=3)return 2;int n=atoi(argv[1]),reps=atoi(argv[2]);if(n<1||n>512||reps<1||reps>10000)return 2;
 lat_mv *mv=latC_crear_mv();mv->nombre_archivo="medicion-matematicas";
 lat_objeto *m=latO_obtener_contexto(mv,mv->contexto[0],"mate"),*f=latH_obtener(getDic(m),"max");
 size_t inicial=0;double t0=0,control=0;
 for(int i=0;i<=reps;++i){if(i==1){inicial=mv->memoria_usada;t0=reloj();}
  for(int j=0;j<n;++j)latC_apilar_double(mv,j);latC_apilar_double(mv,n);
  if(latC_llamar_funcion(mv,f))return 1;double x=getNumerico(latC_desapilar(mv));if(x!=n-1)return 1;control+=x;}
 struct rusage r;getrusage(RUSAGE_SELF,&r);
 printf("%d,%d,%.9f,%zu,%zu,%lld,%ld,%.0f\n",n,reps,reloj()-t0,inicial,mv->memoria_usada,(long long)mv->memoria_usada-(long long)inicial,r.ru_maxrss,control);
 latC_destruir_mv(mv);return 0;
}
