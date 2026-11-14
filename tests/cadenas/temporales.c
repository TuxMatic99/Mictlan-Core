/* Incluye las funciones reales y cuenta sus buffers C sin simular resultados.
   La VM sigue siendo Core real enlazado estaticamente. LSan valida ademas en CI. */
#include "latino.h"
#include <stdlib.h>
#include <string.h>
int parse_silent = 0;
char *filename = NULL;
static size_t vivos;
static void *reservar(size_t n) { void *p=malloc(n); if(p) ++vivos; return p; }
static void liberar(void *p) { if(p) { --vivos; free(p); } }
#define malloc reservar
#define free liberar
#include "../../src/latstrlib.c"
#undef malloc
#undef free

int main(void) {
    lat_mv *mv=latC_crear_mv();
    mv->nombre_archivo="temporales-cadena";
    for(int i=0;i<100;++i) {
        latC_apilar_string(mv,"abc");str_invertir(mv);(void)latC_desapilar(mv);
    }
    latC_destruir_mv(mv);
    if(vivos) { fprintf(stderr,"Buffers C sin liberar: %zu\n",vivos);return 1; }
    return 0;
}
