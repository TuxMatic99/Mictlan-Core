#include "latino.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int parse_silent=0;char *filename=0;
static int vivos;static int fallar;
static void *reservar(size_t n){if(fallar){fallar=0;return NULL;}void *p=malloc(n);if(p)++vivos;return p;}
static void liberar(void *p){if(p){--vivos;free(p);}}
#define malloc reservar
#define free liberar
#include "../../src/latlistlib.c"
#undef malloc
#undef free
int main(int argc,char **argv){
 if(argc!=2)return 2;
 lat_mv *mv=latC_crear_mv();mv->nombre_archivo="union-b2";
 lat_objeto *llamada=latC_crear_cfuncion(mv,lista_separar);
 latC_apilar_double(mv,777);
 if(!strcmp(argv[1],"argumentos"))latC_apilar_double(mv,3);
 else {lat_objeto *l=latC_crear_lista(mv,latL_crear(mv));latL_agregar(mv,getLista(l),latC_crear_cadena(mv,"a"));latC_apilar(mv,l);fallar=1;}
 latC_apilar_string(mv,",");int estado=latC_llamar_funcion(mv,llamada);
 int correcto=estado==LAT_ERRRUN && vivos==0 && !fallar && mv->ptrpila==1 && getNumerico(latC_tope(mv))==777;
 if(!correcto)fprintf(stderr,"Temporal: vivos=%d, estado=%d\n",vivos,estado);
 latC_destruir_mv(mv);return correcto?0:1;
}
