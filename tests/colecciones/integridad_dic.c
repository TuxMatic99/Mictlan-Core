#include "latino.h"
#include "latmemoria.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define C(c) do { if(!(c)){fprintf(stderr,"Dic B2 linea %d: %s\n",__LINE__,#c); fallos=1;goto cerrar;} }while(0)
static lat_objeto *funcion(lat_mv *mv,const char *lib,const char *n){
 lat_objeto *l=latO_obtener_contexto(mv,mv->contexto[0],lib);return latH_obtener(getDic(l),n);
}
static int borrar(lat_mv *mv,lat_objeto *f,lat_objeto *d,const char *k){latC_apilar(mv,d);latC_apilar_string(mv,k);return latC_llamar_funcion(mv,f);}
static size_t entradas(hash_map *m){size_t n=0;for(size_t i=0;i<m->capacity;++i)if(m->buckets[i])n+=(size_t)m->buckets[i]->longitud;return n;}
int main(int argc,char **argv){
 if(argc!=2)return 2;
 int fallos=0;lat_mv *mv=latC_crear_mv();mv->nombre_archivo="integridad-dic";
 lat_objeto *f=funcion(mv,"dic","eliminar");
 lat_objeto *d=latC_crear_dic(mv,latH_crear(mv));hash_map *m=getDic(d);
 if(!strcmp(argv[1],"colisiones")){
  const char *k[]={"b2_32","b2_87","b2_128"};
  for(int i=0;i<3;++i){lat_objeto n={0};setNumerico(&n,11*(i+1));latH_asignar(mv,m,k[i],&n);}
  C(m->capacity==256 && m->buckets[127] && m->buckets[127]->longitud==3);
  C(borrar(mv,f,d,k[1])==0);C(m->longitud==2 && entradas(m)==2);
  C(latH_obtener(m,k[0]) && latH_obtener(m,k[2]) && !latH_obtener(m,k[1]));
  C(borrar(mv,f,d,k[1])==0 && m->longitud==2);
  C(borrar(mv,f,d,k[0])==0 && m->longitud==1);
  C(borrar(mv,f,d,k[2])==0 && m->longitud==0 && entradas(m)==0);
 }else if(!strcmp(argv[1],"crecimiento")){
  enum{N=1300};char k[48];
  for(int i=0;i<N;++i){snprintf(k,sizeof k,"clave_%d",i);lat_objeto n={0};setNumerico(&n,i);latH_asignar(mv,m,k,&n);}
  C(m->capacity>=2048 && m->longitud==N);
  for(int i=0;i<N;i+=3){snprintf(k,sizeof k,"clave_%d",i);C(borrar(mv,f,d,k)==0);}
  size_t esperadas=N-(N+2)/3;C(m->longitud==esperadas && entradas(m)==esperadas);
  for(int i=0;i<N;++i){snprintf(k,sizeof k,"clave_%d",i);lat_objeto *v=latH_obtener(m,k);C((i%3==0)?v==NULL:(v && getNumerico(v)==i));}
  for(int i=0;i<2000;++i){snprintf(k,sizeof k,"nuevo_%d",i);lat_objeto n={0};setNumerico(&n,i+N);latH_asignar(mv,m,k,&n);}
  C(m->longitud==esperadas+2000 && entradas(m)==m->longitud && m->capacity>=4096);
  for(int i=0;i<N;++i)if(i%3){snprintf(k,sizeof k,"clave_%d",i);lat_objeto *v=latH_obtener(m,k);C(v && getNumerico(v)==i);}
 }else if(!strcmp(argv[1],"compartidos")||!strcmp(argv[1],"recoleccion")){
  bool g=!strcmp(argv[1],"recoleccion");
  if(g){C(latG_habilitar(mv)==LAT_G_OK);C(latG_autorizar(mv,getCfun(f))==LAT_G_OK);C(latG_autorizar(mv,getCfun(funcion(mv,"dic","valores")))==LAT_G_OK);}
  lat_objeto *l=latC_crear_lista(mv,latL_crear(mv));latL_agregar(mv,getLista(l),latC_crear_numerico(mv,7));
  latH_asignar(mv,m,"dato",l);
  latC_apilar(mv,d);C(latC_llamar_funcion(mv,funcion(mv,"dic","valores"))==0);
  lat_objeto vista=*latC_desapilar(mv);lat_objeto *celda=latL_obtener_elemento(mv,getLista(&vista),0);
  hash_map *otro=latH_clonar(mv,m);lat_objeto *alias=latH_obtener(otro,"dato");
  C(getLista(celda)==getLista(l) && getLista(alias)==getLista(l));
  /* Raices reales conservadas durante el recolector explicito. */
  latC_apilar(mv,&vista);latC_apilar(mv,alias);
  if(g)C(latG_retener(mv,alias)==LAT_G_OK);
  C(borrar(mv,f,d,"dato")==0 && m->longitud==0);
  if(g)C(latG_recolectar(mv)==LAT_G_OK);
  C(getLista(celda)==getLista(alias));
  latL_agregar(mv,getLista(alias),latC_crear_numerico(mv,8));C(getLista(celda)->longitud==2);
  C(getNumerico(latL_obtener_elemento(mv,getLista(celda),0))==7);
  if(g)C(latG_soltar(mv,alias)==LAT_G_OK);
  (void)latC_desapilar(mv);(void)latC_desapilar(mv);
 }else return 2;
 C(mv->ptrpila==0 && mv->ptrctx==0 && mv->numejec==0 && !mv->error);
 cerrar:latC_destruir_mv(mv);return fallos;
}
