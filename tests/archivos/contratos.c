#define _GNU_SOURCE
#include "latino.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define C(x) do{if(!(x)){fprintf(stderr,"B4A_ASSERT %d: %s\n",__LINE__,#x);return 1;}}while(0)
static lat_mv *mv;
static lat_objeto *f(const char *n){return latH_obtener(getDic(latO_obtener_contexto(mv,mv->contexto[0],"archivo")),n);}
static int uno(const char *n,const char *a){latC_apilar_string(mv,a);return latC_llamar_funcion(mv,f(n));}
static int dos(const char *n,const char *a,const char *b){latC_apilar_string(mv,a);latC_apilar_string(mv,b);return latC_llamar_funcion(mv,f(n));}
static int guardar(const char *p,const void *s,size_t n){FILE *fp=fopen(p,"wb");if(!fp)return 0;int bien=fwrite(s,1,n,fp)==n;return fclose(fp)==0&&bien;}
static int contenido(const char *p,const void *s,size_t n){FILE *fp=fopen(p,"rb");if(!fp)return 0;int bien=1;const unsigned char *e=s;for(size_t i=0;i<n;++i)if(fgetc(fp)!=e[i])bien=0;if(fgetc(fp)!=EOF||ferror(fp))bien=0;return fclose(fp)==0&&bien;}
static int copia(const char *caso){
 C(guardar("ajeno","INTACTO",7));
 if(!strcmp(caso,"ausente_nuevo")){C(dos("duplicar","ausente","destino")==LAT_ERRRUN);C(access("destino",F_OK)!=0);}
 else if(!strcmp(caso,"ausente_existente")){C(guardar("destino","ANTERIOR",8));C(dos("duplicar","ausente","destino")==LAT_ERRRUN);C(contenido("destino","ANTERIOR",8));}
 else if(!strcmp(caso,"misma_ruta")){C(guardar("origen","ORIGEN",6));C(dos("duplicar","origen","origen")==LAT_ERRRUN);C(contenido("origen","ORIGEN",6));}
 else if(!strcmp(caso,"mismo_inode")){C(guardar("origen","ORIGEN",6));C(link("origen","alias")==0);C(dos("duplicar","origen","alias")==LAT_ERRRUN);C(contenido("origen","ORIGEN",6));C(contenido("alias","ORIGEN",6));}
 else if(!strcmp(caso,"enlace_destino")){C(guardar("origen","ORIGEN",6));C(symlink("ajeno","destino")==0);C(dos("duplicar","origen","destino")==LAT_ERRRUN);}
 else if(!strcmp(caso,"origen_directorio")){C(mkdir("directorio",0700)==0);C(guardar("destino","ANTERIOR",8));C(dos("duplicar","directorio","destino")==LAT_ERRRUN);C(contenido("destino","ANTERIOR",8));}
 else if(!strcmp(caso,"destino_invalido")){C(guardar("origen","ORIGEN",6));C(dos("duplicar","origen","no-existe/destino")==LAT_ERRRUN);C(contenido("origen","ORIGEN",6));}
 else if(!strcmp(caso,"vacio")){C(guardar("origen","",0));C(guardar("destino","ANTERIOR",8));C(dos("duplicar","origen","destino")==0);C(contenido("destino","",0));}
 else if(!strcmp(caso,"binario")){unsigned char b[4096];for(size_t i=0;i<sizeof b;++i)b[i]=(unsigned char)i;C(guardar("origen",b,sizeof b));C(guardar("destino","ANTERIOR",8));C(dos("duplicar","origen","destino")==0);C(contenido("origen",b,sizeof b));C(contenido("destino",b,sizeof b));}
 else return 2;
 C(contenido("ajeno","INTACTO",7));C(mv->ptrpila==1&&getNumerico(latC_tope(mv))==777);return 0;
}
static int ejecutar(const char *caso){
 const char *ruta=strstr(caso,"mict")?"hijo.mict":"hijo.lat";
 const char *programa="archivo.escribir(\"marca\", acadena(6*7))\n";
 int error=0;if(strstr(caso,"error")){programa="archivo.escribir(3,\"fallo\")\n";error=1;}
 if(strstr(caso,"sintaxis")){programa="fin\n";error=1;}
 if(strstr(caso,"vacio")){programa="";error=1;}
 C(guardar(ruta,programa,strlen(programa)));
 char *anterior=mv->nombre_archivo;mv->global->REPL=true;mv->nlin=17;mv->ncol=23;
 C(uno("ejecutar",ruta)==(error?LAT_ERRRUN:0));
 C(mv->nombre_archivo==anterior&&mv->global->REPL&&mv->nlin==17&&mv->ncol==23);
 if(!error)C(contenido("marca","42",2));else C(access("marca",F_OK)!=0);
 C(mv->ptrpila==1&&getNumerico(latC_tope(mv))==777);C(mv->ptrctx==0&&mv->numejec==0&&!mv->error);
 /* Recuperación: la misma VM vuelve a escribir correctamente. */
 C(dos("escribir","recuperado","bien")==0);C(contenido("recuperado","bien",4));return 0;
}
static int lectura(void){
 int estado;char *s;
 s=latF_leer_texto_seguro("ausente",&estado);C(!s&&estado==0);
 C(guardar("vacio","",0));s=latF_leer_texto_seguro("vacio",&estado);C(s&&!*s&&estado==1);free(s);
 const char texto[]="\nMaría ñ\n\nfin\r\n";C(guardar("texto",texto,sizeof texto-1));
 C(uno("lineas","texto")==0);lat_objeto l=*latC_desapilar(mv);C(l.tipo==T_LIST&&getLista(&l)->longitud==4);
 const char *esperadas[]={"","María ñ","","fin\r"};for(int i=0;i<4;++i)C(!strcmp(latC_checar_cadena(mv,latL_obtener_elemento(mv,getLista(&l),i)),esperadas[i]));
 C(guardar("binario","a\0b",3));s=latF_leer_texto_seguro("binario",&estado);C(!s&&estado==-1);
 C(uno("leer","binario")==0);lat_objeto *o=latC_desapilar(mv);C(o->tipo==T_BOOL&&!getLogico(o));
 C(uno("leer_texto_seguro","ausente")==0);C(latC_desapilar(mv)->tipo==T_NULL);
 C(getCfun(f("leer"))==getCfun(f("intentar_leer")));C(getCfun(f("eliminar"))==getCfun(f("borrar")));return 0;
}
static int limites(void){
 size_t n=8u*1024u*1024u;char *datos=malloc(n+1);C(datos);memset(datos,'x',n+1);C(guardar("grande",datos,n));
 int estado;char *s=latF_leer_texto_seguro("grande",&estado);int bien=s&&estado==1&&strlen(s)==n&&!memcmp(s,datos,n);free(s);if(!bien){free(datos);C(bien);}
 C(guardar("grande",datos,n+1));s=latF_leer_texto_seguro("grande",&estado);free(datos);C(!s&&estado==-1);return 0;
}
static int auxiliares(void){
 C(uno("crear","nuevo")==0);C(getLogico(latC_desapilar(mv)));C(contenido("nuevo","",0));
 C(guardar("nuevo","NO BORRAR",9));C(uno("crear","nuevo")==0);C(!getLogico(latC_desapilar(mv)));C(contenido("nuevo","NO BORRAR",9));
 C(dos("renombrar","nuevo","renombrado")==0);C(!strcmp(latC_checar_cadena(mv,latC_desapilar(mv)),"renombrado"));C(access("nuevo",F_OK)!=0);C(contenido("renombrado","NO BORRAR",9));
 C(uno("eliminar","renombrado")==0);C(getLogico(latC_desapilar(mv)));C(uno("borrar","renombrado")==0);C(!getLogico(latC_desapilar(mv)));return 0;
}
int main(int argc,char **argv){
 if(argc!=2)return 2;char cwd[4096];const char *raiz=getenv("MICTLAN_ARCHIVOS_TMP");if(!raiz||!getcwd(cwd,sizeof cwd)||strcmp(cwd,raiz))return 2;
 mv=latC_crear_mv();mv->nombre_archivo="padre.mict";latC_apilar_double(mv,777);int r;
 if(!strncmp(argv[1],"ejecutar_",9))r=ejecutar(argv[1]);else if(!strcmp(argv[1],"lectura"))r=lectura();else if(!strcmp(argv[1],"limites"))r=limites();else if(!strcmp(argv[1],"auxiliares"))r=auxiliares();else r=copia(argv[1]);
 latC_destruir_mv(mv);return r;
}
