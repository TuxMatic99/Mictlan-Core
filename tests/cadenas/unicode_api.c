#include "latino.h"
#include <stdio.h>
#include <string.h>
#define EXIGIR(c) do { if (!(c)) { fprintf(stderr,"Unicode API: %s linea %d\n",#c,__LINE__); return 1; } } while(0)
static lat_objeto *funcion(lat_mv *mv, const char *nombre) {
    lat_objeto *lib=latO_obtener_contexto(mv,mv->contexto[0],"cadena");
    return latH_obtener(getDic(lib),nombre);
}
int main(void) {
    lat_mv *mv=latC_crear_mv();
    mv->nombre_archivo="unicode-api";
    const char *invalidas[]={"\x80","\xc0\xaf","\xc1\xbf","\xe0\x9f\xbf",
      "\xed\xa0\x80","\xf0\x8f\xbf\xbf","\xf4\x90\x80\x80","\xf5\x80\x80\x80",
      "\xff","\xc2","\xe2\x82","\xf0\x90\x80","\xc2\x41","\xe2\x41\x80"};
    const char *validas[]={"", "mañana", "e\xcc\x81", "👩‍💻", "\xf4\x8f\xbf\xbf"};
    const int puntos[]={0,6,2,3,1};
    lat_objeto *medir=funcion(mv,"longitud_unicode"), *invertir=funcion(mv,"invertir_unicode");
    EXIGIR(medir && invertir);
    latC_apilar_double(mv,777);
    for(size_t i=0;i<sizeof validas/sizeof *validas;++i) {
        latC_apilar_string(mv,validas[i]);
        EXIGIR(latC_llamar_funcion(mv,medir)==0);
        EXIGIR(latC_checar_numerico(mv,latC_desapilar(mv))==puntos[i]);
        latC_apilar_string(mv,validas[i]);
        EXIGIR(latC_llamar_funcion(mv,invertir)==0);
        EXIGIR(latC_llamar_funcion(mv,invertir)==0);
        EXIGIR(!strcmp(latC_checar_cadena(mv,latC_desapilar(mv)),validas[i]));
    }
    for(size_t i=0;i<sizeof invalidas/sizeof *invalidas;++i) {
        latC_apilar_string(mv,invalidas[i]);
        EXIGIR(latC_llamar_funcion(mv,medir)==LAT_ERRRUN);
        EXIGIR(mv->ptrpila==1 && mv->tope==mv->pila+1 && getNumerico(latC_tope(mv))==777);
        EXIGIR(mv->ptrctx==0 && mv->numejec==0 && !mv->error);
        latC_apilar_string(mv,"ñ");
        EXIGIR(latC_llamar_funcion(mv,medir)==0);
        EXIGIR(latC_checar_numerico(mv,latC_desapilar(mv))==1);
    }
    (void)latC_desapilar(mv);
    latC_destruir_mv(mv);
    return 0;
}
