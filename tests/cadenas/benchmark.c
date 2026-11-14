#define _POSIX_C_SOURCE 200809L
#include "latino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef __linux__
#include <sys/resource.h>
#endif
static double reloj(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec/1e9; }
static long rss(void) {
#ifdef __linux__
    struct rusage r; return getrusage(RUSAGE_SELF,&r)==0?r.ru_maxrss:-1;
#else
    return -1;
#endif
}
int main(int argc,char **argv) {
    if(argc!=5) return 2;
    const char *caso=argv[1];size_t len=(size_t)strtoul(argv[2],NULL,10);
    unsigned vueltas=(unsigned)strtoul(argv[3],NULL,10);bool unicode=!strcmp(argv[4],"utf8");
    if(!len || len>1048576 || !vueltas || vueltas>100000) return 2;
    if(unicode) len=len/5*5;
    char *texto=malloc(len+1); if(!texto)return 2;
    if(unicode) for(size_t i=0;i<len;i+=5)memcpy(texto+i,"niño",5);
    else memset(texto,'a',len);
    texto[len]='\0';
    lat_mv *mv=latC_crear_mv();mv->nombre_archivo="medicion-cadenas";
    lat_objeto *lib=latO_obtener_contexto(mv,mv->contexto[0],"cadena");
    lat_objeto *fun=latH_obtener(getDic(lib),caso); if(!fun) {latC_destruir_mv(mv);free(texto);return 77;}
    /* Calentar el internado y medir la misma salida reiterada. */
    size_t inicio=0,fin=0;double tiempo=0;unsigned long suma=0;long rss_inicio=0;
    for(unsigned i=0;i<=vueltas;++i) {
        if(i==1) {inicio=mv->memoria_usada;rss_inicio=rss();tiempo=reloj();}
        latC_apilar_string(mv,texto);
        if(!strcmp(caso,"concatenar")) latC_apilar_string(mv,"X");
        if(!strcmp(caso,"insertar")) {latC_apilar_string(mv,"X");latC_apilar_double(mv,1);}
        if(!strcmp(caso,"subcadena")) {latC_apilar_double(mv,0);latC_apilar_double(mv,32);}
        if(!strcmp(caso,"reemplazar")) {latC_apilar_string(mv,"a");latC_apilar_string(mv,"b");latC_apilar_double(mv,1);}
        if(latC_llamar_funcion(mv,fun)) {latC_destruir_mv(mv);free(texto);return 1;}
        lat_objeto *o=latC_desapilar(mv);
        if(o->tipo==T_STR) suma+=(unsigned long)strlen(latC_checar_cadena(mv,o));
        else if(o->tipo==T_NUMERIC) suma+=(unsigned long)getNumerico(o);
        else return 1;
    }
    tiempo=reloj()-tiempo;fin=mv->memoria_usada;
    printf("%s,%s,%zu,%u,%.9f,%zu,%zu,%lld,%ld,%ld,%lu\n",caso,unicode?"utf8":"ascii",len,vueltas,tiempo,inicio,fin,(long long)fin-(long long)inicio,rss_inicio,rss(),suma);
    latC_destruir_mv(mv);free(texto);return 0;
}
