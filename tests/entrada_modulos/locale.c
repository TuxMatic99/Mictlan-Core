#define _GNU_SOURCE
#include "latino.h"
#include <locale.h>
#include <errno.h>
#include <pthread.h>
static int cambios;
static char *vigilar_locale(int categoria,const char *valor){if(valor)++cambios;return setlocale(categoria,valor);}
#define setlocale vigilar_locale
#include "../../src/latlocale.c"
#undef setlocale
static void *hilo(void *dato){locale_t l=newlocale(LC_NUMERIC_MASK,(const char *)dato,(locale_t)0);if(!l)return (void *)1;locale_t previo=uselocale(l);int error=0;
 for(int i=0;i<500;++i){char *fin;double x=latC_strtod_c("1.25",&fin);if(x!=1.25||*fin||uselocale((locale_t)0)!=l)error=1;}
 uselocale(previo);freelocale(l);return (void *)(long)error;}
int main(int argc,char **argv){if(argc!=2)return 2;if(!setlocale(LC_NUMERIC,"es_ES.UTF-8"))return 77;
 if(!strcmp(argv[1],"sin_mutacion")){char *fin;double x=latC_strtod_c("1.25",&fin);if(x!=1.25||*fin||cambios){fprintf(stderr,"B4B_ASSERT cambios globales=%d\n",cambios);return 1;}return 0;}
 /* No ejecutar una carrera C indefinida: la base se rechaza primero por su
    mutación global. Tras corregirla sí se ejercitan conversiones concurrentes. */
 char *fin;latC_strtod_c("1.25",&fin);if(cambios){fprintf(stderr,"B4B_ASSERT concurrencia bloqueada: cambia locale global\n");return 1;}
 pthread_t h[3];const char *n[]={"C","es_ES.UTF-8","fr_FR.UTF-8"};for(int i=0;i<3;++i)if(pthread_create(&h[i],NULL,hilo,(void *)n[i]))return 2;
 int error=0;for(int i=0;i<3;++i){void *r;pthread_join(h[i],&r);error|=r!=NULL;}return error;
}
