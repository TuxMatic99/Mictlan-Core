/* Se inyectan palabras del generador, no resultados del algoritmo probado.
   Ejercita de forma determinista el rechazo, extremos y consumo del estado. */
#include "latino.h"
#include "../../src/latpropiedad.h"
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
int parse_silent=0;char *filename=NULL;
static uint64_t palabras[4];static size_t leidas,cantidad;
static int historico;
static uint64_t fuente(lat_mv *mv){(void)mv;if(leidas==cantidad)abort();return palabras[leidas++];}
static int fuente_c(void){return historico;}
#define latP_aleatorio64 fuente
#define rand fuente_c
#include "../../src/latmathlib.c"
#undef latP_aleatorio64
#undef rand
#define C(c) do {if(!(c)){fprintf(stderr,"P12B3_ASSERT %d: %s\n",__LINE__,#c);r=1;goto cerrar;}}while(0)
int main(void){
 int r=0;lat_mv *mv=latC_crear_mv();mv->nombre_archivo="rechazo-p12b3";
 palabras[0]=0;palabras[1]=5;palabras[2]=6;cantidad=3;
 latC_apilar_double(mv,0);latC_apilar_double(mv,9);mate_aleatorio_entero(mv);
 C(getNumerico(latC_desapilar(mv))==6&&leidas==3);
 palabras[0]=10;palabras[1]=9;cantidad=2;leidas=0;
 for(int i=0;i<2;++i){latC_apilar_double(mv,-5);latC_apilar_double(mv,4);mate_aleatorio_entero(mv);C(getNumerico(latC_desapilar(mv))==(i?4:-5));}
 C(leidas==2);
 const int64_t limite=INT64_C(9007199254740991);
 const uint64_t ancho=UINT64_C(18014398509481983);
 palabras[0]=ancho;palabras[1]=ancho-1;cantidad=2;leidas=0;
 for(int i=0;i<2;++i){latC_apilar_double(mv,-(double)limite);latC_apilar_double(mv,(double)limite);mate_aleatorio_entero(mv);C(getNumerico(latC_desapilar(mv))==(i?(double)limite:-(double)limite));}
 C(leidas==2);
 /* Entradas historicamente validas conservan exactamente rand()%amplitud. */
 const double extremos[][2]={{2,10},{10,2},{-8,-2},{-2,-8},{-2.9,4.9},{INT_MIN,INT_MAX}};
 for(int i=0;i<6;++i)for(int k=0;k<2;++k){historico=k?RAND_MAX:0;
  int64_t a=(int64_t)extremos[i][0],b=(int64_t)extremos[i][1];if(a>b){int64_t t=a;a=b;b=t;}
  latC_apilar_double(mv,extremos[i][0]);latC_apilar_double(mv,extremos[i][1]);latC_apilar_double(mv,2);mate_random(mv);
  C(getNumerico(latC_desapilar(mv))==(double)(a+historico%(b-a+1)));
 }
 C(mv->ptrpila==0);
 cerrar:latC_destruir_mv(mv);return r;
}
