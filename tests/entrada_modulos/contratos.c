#define _GNU_SOURCE
#include "latino.h"
#include "latmemoria.h"
#include <locale.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#define C(x) do {if(!(x)){fprintf(stderr,"B4B_ASSERT %d: %s\n",__LINE__,#x);return 1;}}while(0)
static lat_mv *mv;
static lat_objeto *funcion(const char *n){return latO_obtener_contexto(mv,mv->contexto[0],n);}
static int invocar(const char *n){lat_objeto *f=funcion(n);if(!f||f->tipo!=T_CFUN&&f->tipo!=T_FUN){fprintf(stderr,"B4B_API ausente: %s\n",n);return -1;}return latC_llamar_funcion(mv,f);}
static int guardar(const char *p,const void *t,size_t n){FILE *f=fopen(p,"wb");if(!f)return 0;int ok=fwrite(t,1,n,f)==n;return fclose(f)==0&&ok;}
static int texto(const char *p,const char *t){return guardar(p,t,strlen(t));}
static int contenido(const char *p,const char *t){FILE *f=fopen(p,"rb");if(!f)return 0;int ok=1;for(size_t i=0;i<strlen(t);++i)if(fgetc(f)!=(unsigned char)t[i])ok=0;if(fgetc(f)!=EOF||ferror(f))ok=0;return fclose(f)==0&&ok;}
static int pila(void){return mv->ptrpila==1&&getNumerico(latC_tope(mv))==777&&mv->ptrctx==0&&mv->numejec==0&&!mv->error;}
static int limites(const char *api,const char *caso){
 FILE *f=fopen("entrada","wb");C(f);int error=0;
 if(!strcmp(caso,"nul")){C(fwrite("ab\0cd\n42\n",1,9,f)==9);error=1;}
 else if(!strcmp(caso,"consecutivas")){C(fputs(!strcmp(api,"leer_numero")?"invalido\n42\n":"12\n42\n",f)>=0);error=!strcmp(api,"leer_numero");}
 else {size_t n=8u*1024u*1024u+(!strcmp(caso,"exceso"));for(size_t i=0;i<n;++i)C(fputc(!strcmp(api,"leer_numero")?'0':'x',f)!=EOF);C(fputs("\n42\n",f)>=0);error=!strcmp(caso,"exceso");}
 C(fclose(f)==0);C(freopen("entrada","rb",stdin));int e=invocar(api);C(e==(error&&strcmp(api,"leer")?LAT_ERRRUN:0));
 if(!e){lat_objeto *o=latC_desapilar(mv);if(error)C(o->tipo==T_NULL);else if(!strcmp(api,"leer_texto"))C(o->tipo==T_STR&&(!strcmp(caso,"limite")?strlen(latC_checar_cadena(mv,o))==8u*1024u*1024u:!strcmp(latC_checar_cadena(mv,o),"12")));else C(o->tipo==T_NUMERIC&&getNumerico(o)==(!strcmp(caso,"limite")?0:12));}
 C(pila());C(invocar(api)==0);lat_objeto *o=latC_desapilar(mv);C(!strcmp(api,"leer_texto")?o->tipo==T_STR&&!strcmp(latC_checar_cadena(mv,o),"42"):o->tipo==T_NUMERIC&&getNumerico(o)==42);C(pila());return 0;
}
static int entrada(const char *api,const char *caso,const char *locale){
 if(!strcmp(caso,"nul")||!strcmp(caso,"consecutivas")||!strcmp(caso,"limite")||!strcmp(caso,"exceso"))return limites(api,caso);
 C(setlocale(LC_NUMERIC,locale));char previo[128];snprintf(previo,sizeof previo,"%s",setlocale(LC_NUMERIC,NULL));
 const char *in="",*esperado="";double numero=0;int tipo=T_STR,error=0;
 if(!strcmp(caso,"vacio"))in="\n";
 else if(!strcmp(caso,"eof")){in="";tipo=T_NULL;}
 else if(!strcmp(caso,"texto")){in="hola\n";esperado="hola";}
 else if(!strcmp(caso,"utf8")){in="María ñ 😀\n";esperado="María ñ 😀";}
 else if(!strcmp(caso,"espacios")){in="   \n";esperado="   ";}
 else if(!strcmp(caso,"iniciales")){in="  12\n";esperado="  12";numero=12;tipo=T_NUMERIC;}
 else if(!strcmp(caso,"finales")){in="12 \n";esperado="12 ";numero=12;}
 else if(!strcmp(caso,"cero")){in="0\n";esperado="0";tipo=T_NUMERIC;}
 else if(!strcmp(caso,"negcero")){in="-0\n";esperado="-0";tipo=T_NUMERIC;numero=-0.;}
 else if(!strcmp(caso,"ceros")){in="0012\n";esperado="0012";tipo=T_NUMERIC;numero=12;}
 else if(!strcmp(caso,"decimal")){in="1.5\n";esperado="1.5";tipo=!strcmp(locale,"C")?T_NUMERIC:T_STR;numero=1.5;}
 else if(!strcmp(caso,"coma")){in="1,5\n";esperado="1,5";tipo=strcmp(locale,"C")?T_NUMERIC:T_STR;numero=1.5;}
 else if(!strcmp(caso,"cientifico")){in="1e3\n";esperado="1e3";tipo=T_NUMERIC;numero=1000;}
 else if(!strcmp(caso,"hex")){in="0x10\n";esperado="0x10";tipo=T_NUMERIC;numero=16;}
 else if(!strcmp(caso,"nan")){in="nan\n";esperado="nan";tipo=T_NUMERIC;numero=NAN;}
 else if(!strcmp(caso,"inf")){in="-inf\n";esperado="-inf";tipo=T_NUMERIC;numero=-INFINITY;}
 else if(!strcmp(caso,"desborde")){in="1e9999\n";esperado="1e9999";tipo=T_NUMERIC;numero=INFINITY;}
 else if(!strcmp(caso,"subdesborde")){in="1e-9999\n";esperado="1e-9999";tipo=T_NUMERIC;numero=0;}
 else if(!strcmp(caso,"crlf")){in="12\r\n";esperado="12";numero=12;tipo=T_NUMERIC;}
 else if(!strcmp(caso,"sin_lf")){in="12";esperado="12";tipo=T_NUMERIC;numero=12;}
 else if(!strcmp(caso,"larga")){char b[4097];memset(b,'x',4096);b[4096]=0;C(texto("entrada",b));C(freopen("entrada","rb",stdin));C(invocar(api)==0);lat_objeto *o=latC_desapilar(mv);C(o->tipo==T_STR&&strlen(latC_checar_cadena(mv,o))==4096);return pila()?0:1;}
 else return 2;
 if(!strcmp(api,"leer_texto")){if(tipo!=T_NULL)tipo=T_STR;if(!strcmp(caso,"crlf"))esperado="12";}
 if(!strcmp(api,"leer_numero")&&tipo!=T_NULL){
  error=!strcmp(caso,"vacio")||!strcmp(caso,"texto")||!strcmp(caso,"utf8")||!strcmp(caso,"espacios")||!strcmp(caso,"coma")||!strcmp(caso,"hex")||!strcmp(caso,"nan")||!strcmp(caso,"inf")||!strcmp(caso,"desborde")||!strcmp(caso,"subdesborde");
  tipo=T_NUMERIC;
 }
 C(texto("entrada",in));C(freopen("entrada","rb",stdin));C(invocar(api)==(error?LAT_ERRRUN:0));
 if(!error){lat_objeto *o=latC_desapilar(mv);C(o->tipo==tipo);
  if(tipo==T_STR)C(!strcmp(latC_checar_cadena(mv,o),esperado));
  if(tipo==T_NUMERIC){double x=getNumerico(o);C(isnan(numero)?isnan(x):x==numero);if(numero==0)C(!!signbit(x)==!!signbit(numero));}}
 C(pila());C(!strcmp(previo,setlocale(LC_NUMERIC,NULL)));
 /* EOF y una nueva entrada en la misma VM tras cualquier resultado/error. */
 C(invocar(api)==0);C(latC_desapilar(mv)->tipo==T_NULL);C(pila());
 C(texto("recuperacion","42\n"));C(freopen("recuperacion","rb",stdin));C(invocar(api)==0);lat_objeto *o=latC_desapilar(mv);C(!strcmp(api,"leer_texto")?!strcmp(latC_checar_cadena(mv,o),"42"):getNumerico(o)==42);C(pila());return 0;
}
static int incluir(const char *n){latC_apilar_string(mv,n);return invocar("incluir");}
static int modulos(const char *caso){
 C(mkdir("src",0700)==0);C(mkdir("lib",0700)==0);C(mkdir("a",0700)==0);C(mkdir("b",0700)==0);C(setenv("LATINO_LIB","lib",1)==0);
 const char *nombre="mod",*esperado="X";int error=0;
 const char *x="archivo.anexar(\"traza\",\"X\")\n";
 if(!strcmp(caso,"cwd")){C(texto("mod.lat",x));C(texto("src/mod.lat","error(\"no elegir src\")\n"));C(texto("lib/mod.lat","error(\"no elegir lib\")\n"));}
 else if(!strcmp(caso,"prioridad_ext")){C(texto("mod.mict","error(\"no elegir mict\")\n"));C(texto("src/mod.lat",x));}
 else if(!strcmp(caso,"latino_lib")){C(texto("mod.mict","error(\"no elegir cwd mict\")\n"));C(texto("lib/mod.lat",x));}
 else if(!strcmp(caso,"explicito_lat")){C(texto("mod.lat",x));nombre="mod.lat";}
 else if(!strcmp(caso,"explicito_mict")){C(texto("mod.mict",x));C(texto("mod.lat","error(\"no elegir lat\")\n"));nombre="mod.mict";}
 else if(!strcmp(caso,"relativa")){C(texto("src/mod.mict",x));nombre="./src/mod.mict";}
 else if(!strcmp(caso,"absoluta")){static char ruta[4096];C(getcwd(ruta,sizeof ruta));strcat(ruta,"/src/mod.mict");C(texto("src/mod.mict",x));nombre=ruta;}
 else if(!strcmp(caso,"anidado")||!strcmp(caso,"abc")){C(texto("src/mod.mict","incluir(\"dep\")\narchivo.anexar(\"traza\",\"A\")\n"));C(texto("src/dep.mict",!strcmp(caso,"abc")?"incluir(\"tercero.mict\")\narchivo.anexar(\"traza\",\"B\")\n":"archivo.anexar(\"traza\",\"B\")\n"));C(texto("src/tercero.mict","archivo.anexar(\"traza\",\"C\")\n"));esperado=!strcmp(caso,"abc")?"CBA":"BA";}
 else if(!strcmp(caso,"repetido")){C(texto("mod.lat",x));C(incluir(nombre)==0);esperado="XX";}
 else if(!strcmp(caso,"homonimos")){C(texto("a/mod.mict","incluir(\"dep.mict\")\n"));C(texto("b/mod.mict","incluir(\"dep.mict\")\n"));C(texto("a/dep.mict","archivo.anexar(\"traza\",\"A\")\n"));C(texto("b/dep.mict","archivo.anexar(\"traza\",\"B\")\n"));C(incluir("a/mod.mict")==0);nombre="b/mod.mict";esperado="AB";}
 else if(!strcmp(caso,"sintaxis")){nombre="mod.mict";C(texto(nombre,"fin\n"));error=1;}
 else if(!strcmp(caso,"ejecucion")){nombre="mod.lat";C(texto(nombre,"error(\"error sintetico\")\n"));error=1;}
 else if(!strcmp(caso,"vacio")){nombre="mod.mict";C(texto(nombre,""));error=1;}
 else if(!strcmp(caso,"ausente")){nombre="ausente.mict";error=1;}
 else if(!strcmp(caso,"ruta_larga")){static char largo[MAX_PATH_LENGTH+1];memset(largo,'x',sizeof largo-1);largo[sizeof largo-1]=0;nombre=largo;error=1;}
 else if(!strcmp(caso,"lib_larga")){char largo[MAX_PATH_LENGTH+1];memset(largo,'x',sizeof largo-1);largo[sizeof largo-1]=0;C(setenv("LATINO_LIB",largo,1)==0);C(texto("mod.lat",x));error=1;}
 else if(!strcmp(caso,"ciclo")){nombre="ciclo.mict";C(texto(nombre,"incluir(\"ciclo.mict\")\n"));mv->numejec=MAX_CALL_FUNCTION-4;error=1;}
 else if(!strcmp(caso,"vida_ruta")){nombre="mod.mict";C(texto(nombre,"global funcion retenida() retorno 42 fin\n"));esperado="";}
 else return 2;
 char *anterior=mv->nombre_archivo;mv->nlin=17;mv->ncol=23;mv->global->REPL=true;int nivel=mv->numejec;
 int e=incluir(nombre);C(e==(error?LAT_ERRRUN:0));C(mv->nombre_archivo==anterior&&mv->nlin==17&&mv->ncol==23&&mv->global->REPL);C(mv->numejec==nivel);mv->numejec=0;C(pila());
 if(!error&&*esperado)C(contenido("traza",esperado));else C(access("traza",F_OK)!=0);
 C(setenv("LATINO_LIB","lib",1)==0);C(texto("correcto.mict",x));C(incluir("correcto.mict")==0);C(pila());
 if(!strcmp(caso,"vida_ruta")){C(latG_habilitar(mv)==LAT_G_OK);C(latG_recolectar(mv)==LAT_G_OK);C(invocar("retenida")==0);C(getNumerico(latC_desapilar(mv))==42);C(pila());}
 return 0;
}
int main(int argc,char **argv){
 char cwd[4096];const char *raiz=getenv("MICTLAN_ARCHIVOS_TMP");if(!raiz||!getcwd(cwd,sizeof cwd)||strcmp(cwd,raiz))return 2;
 if(argc<3)return 2;setenv("HOME",raiz,1);setenv("INPUTRC","inputrc",1);if(!texto("inputrc",""))return 2;unsetenv("LATINO_LIB");mv=latC_crear_mv();mv->nombre_archivo="src/padre.mict";latC_apilar_double(mv,777);
 int r=!strcmp(argv[1],"modulos")?modulos(argv[2]):argc==4?entrada(argv[1],argv[2],argv[3]):2;
 latC_destruir_mv(mv);return r;
}
