/* Incluye las funciones reales y cuenta sus buffers C sin simular resultados.
   La VM sigue siendo Core real enlazado estaticamente. LSan valida ademas en CI. */
#include "latino.h"
#include <stdlib.h>
#include <string.h>
int parse_silent = 0;
char *filename = NULL;
static size_t vivos;
static void *reservas[2048];
static void *reservar(size_t n) {
    void *p=malloc(n);
    if(p) { size_t i=0; while(i<2048 && reservas[i]) ++i;
        if(i==2048) abort(); reservas[i]=p; ++vivos; }
    return p;
}
static void liberar(void *p) {
    if(!p) return;
    for(size_t i=0;i<2048;++i) if(reservas[i]==p) { reservas[i]=NULL; --vivos; break; }
    free(p);
}
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
        latC_apilar_string(mv,"abc");str_mayusculas(mv);(void)latC_desapilar(mv);
        latC_apilar_string(mv,"ABC");str_minusculas(mv);(void)latC_desapilar(mv);
        latC_apilar_string(mv,"abc");latC_apilar_string(mv,"b");str_eliminar(mv);(void)latC_desapilar(mv);
        latC_apilar_string(mv,"abc");latC_apilar_string(mv,"X");latC_apilar_double(mv,1);str_insertar(mv);(void)latC_desapilar(mv);
        latC_apilar_string(mv,"abc");latC_apilar_double(mv,1);latC_apilar_double(mv,1);str_subcadena(mv);(void)latC_desapilar(mv);
        latC_apilar_string(mv,"abc");latC_apilar_string(mv,"a");latC_apilar_string(mv,"x");latC_apilar(mv,latO_nulo);str_reemplazar(mv);(void)latC_desapilar(mv);
        latC_apilar_string(mv,"ab");latC_apilar_string(mv,"XX");latC_apilar_double(mv,4);str_rellenar_derecha(mv);(void)latC_desapilar(mv);
        latC_apilar_string(mv,"ab");latC_apilar_string(mv,"ñ");latC_apilar_double(mv,4);str_rellenar_izquierda(mv);(void)latC_desapilar(mv);
        latC_apilar_string(mv,"aba");latC_apilar_string(mv,"a");str_match(mv);(void)latC_desapilar(mv);
    }
    for(unsigned cp=0;cp<=0x10ffff;++cp) {
        if(cp>=0xd800 && cp<=0xdfff) continue;
        unsigned char bytes[4]; size_t n;
        if(cp<0x80) { n=1; bytes[0]=(unsigned char)cp; }
        else if(cp<0x800) { n=2; bytes[0]=(unsigned char)(0xc0|cp>>6); bytes[1]=(unsigned char)(0x80|(cp&63)); }
        else if(cp<0x10000) { n=3; bytes[0]=(unsigned char)(0xe0|cp>>12); bytes[1]=(unsigned char)(0x80|((cp>>6)&63)); bytes[2]=(unsigned char)(0x80|(cp&63)); }
        else { n=4; bytes[0]=(unsigned char)(0xf0|cp>>18); bytes[1]=(unsigned char)(0x80|((cp>>12)&63)); bytes[2]=(unsigned char)(0x80|((cp>>6)&63)); bytes[3]=(unsigned char)(0x80|(cp&63)); }
        if(cadena_utf8_paso(bytes,n)!=n || (n>1 && cadena_utf8_paso(bytes,n-1))) return 2;
    }
    latC_destruir_mv(mv);
    if(vivos) { fprintf(stderr,"Buffers C sin liberar: %zu\n",vivos);return 1; }
    return 0;
}
