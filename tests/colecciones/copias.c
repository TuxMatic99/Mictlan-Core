#include "latino.h"
#include <stdio.h>
#include <string.h>
#define C(c) do { if(!(c)){fprintf(stderr,"Copias B2 linea %d: %s\n",__LINE__,#c);fallos=1;goto cerrar;} } while(0)
static lat_objeto *fun(lat_mv *mv,const char *mod,const char *nombre){
 return latH_obtener(getDic(latO_obtener_contexto(mv,mv->contexto[0],mod)),nombre);
}
int main(void){
 int fallos=0;lat_mv *mv=latC_crear_mv();mv->nombre_archivo="copias-b2";
 lat_objeto *n=latC_crear_numerico(mv,1);
 lat_objeto *interno=latC_crear_lista(mv,latL_crear(mv));latL_agregar(mv,getLista(interno),n);
 lat_objeto *d=latC_crear_dic(mv,latH_crear(mv));latH_asignar(mv,getDic(d),"dato",interno);
 lat_objeto *a=latC_crear_lista(mv,latL_crear(mv));
 latL_agregar(mv,getLista(a),n);latL_agregar(mv,getLista(a),interno);latL_agregar(mv,getLista(a),d);
 lat_objeto *c=latO_clonar(mv,a);
 lat_objeto *cn=latL_obtener_elemento(mv,getLista(c),0),*ci=latL_obtener_elemento(mv,getLista(c),1);
 C(getLista(a)!=getLista(c) && cn!=n && getNumerico(cn)==1);
 C(getLista(ci)!=getLista(interno));
 C(latL_obtener_elemento(mv,getLista(ci),0)!=n);
 C(latL_obtener_elemento(mv,getLista(c),2)==d);
 setNumerico(n,9);C(getNumerico(cn)==1 && getNumerico(latL_obtener_elemento(mv,getLista(ci),0))==1);
 hash_map *dc=latH_clonar(mv,getDic(d));lat_objeto *v=latH_obtener(dc,"dato");
 C(v!=latH_obtener(getDic(d),"dato") && getLista(v)==getLista(interno));
 latH_asignar(mv,dc,"dato",cn);C(getLista((lat_objeto *)latH_obtener(getDic(d),"dato"))==getLista(interno));
 /* Repetir una referencia y retirar un nodo no destruye los otros. */
 lista *repetidos=latL_crear(mv);latL_agregar(mv,repetidos,d);latL_agregar(mv,repetidos,d);
 C(latL_eliminar_nodo(mv,repetidos,repetidos->primero)==d);
 C(repetidos->longitud==1 && latL_obtener_elemento(mv,repetidos,0)==d);
 /* Valores es una lista de celdas existentes: actualizar la clave la cambia. */
 latC_apilar(mv,d);C(latC_llamar_funcion(mv,fun(mv,"dic","valores"))==0);
 lat_objeto vista=*latC_desapilar(mv);lat_objeto *celda=latL_obtener_elemento(mv,getLista(&vista),0);
 latH_asignar(mv,getDic(d),"dato",cn);C(celda==latH_obtener(getDic(d),"dato") && getNumerico(celda)==1);
 latC_apilar(mv,d);latC_apilar_string(mv,"dato");C(latC_llamar_funcion(mv,fun(mv,"dic","eliminar"))==0);
 latH_asignar(mv,getDic(d),"dato",n);C(celda!=latH_obtener(getDic(d),"dato") && getNumerico(celda)==1);
 /* Diccionarios anidados y ciclo: clonar comparte el payload, eliminar no lo destruye. */
 latH_asignar(mv,getDic(d),"yo",d);hash_map *ciclo=latH_clonar(mv,getDic(d));
 C(getDic((lat_objeto *)latH_obtener(ciclo,"yo"))==getDic(d));
 latC_apilar(mv,d);latC_apilar_string(mv,"yo");C(latC_llamar_funcion(mv,fun(mv,"dic","eliminar"))==0);
 C(getDic((lat_objeto *)latH_obtener(ciclo,"yo"))==getDic(d) && getDic(d)->longitud==1);
 C(getDic(latO_obtener_contexto(mv,mv->contexto[0],"dic"))==getDic(latO_obtener_contexto(mv,mv->contexto[0],"diccionario")));
 C(getCfun(fun(mv,"lista","separar"))==getCfun(fun(mv,"lista","unir")));
 C(mv->ptrpila==0 && mv->ptrctx==0 && mv->numejec==0 && !mv->error);
 cerrar:latC_destruir_mv(mv);return fallos;
}
