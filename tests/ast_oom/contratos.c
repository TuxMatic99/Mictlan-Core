#define _GNU_SOURCE
#include "latino.h"
#include "latast.h"
#include "latmem.h"
#include "latdo.h"
#include "latmv.h"
#include "laterror.h"
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
int parse_silent=0;char *filename=NULL;
static int fase,armado,objetivo,ordinal,inyectado,bien=1;
static void *vivos[65536];static size_t nvivos;
#define C(x) do{if(!(x)){fprintf(stderr,"A1_ASSERT %d: %s\n",__LINE__,#x);bien=0;}}while(0)
void *__real_malloc(size_t);void *__real_calloc(size_t,size_t);void *__real_realloc(void*,size_t);char *__real_strdup(const char*);void __real_free(void*);
static int fallar(const char *api,size_t n){if(!fase||!armado)return 0;++ordinal;if(ordinal!=objetivo)return 0;++inyectado;fprintf(stderr,"A1_INYECTADO api=%s ordinal=%d bytes=%zu fase=%d\n",api,ordinal,n,fase);fflush(stderr);errno=ENOMEM;return 1;}
static void observar(void *p){if(!p||!fase)return;for(size_t i=0;i<65536;i++)if(!vivos[i]){vivos[i]=p;++nvivos;return;}abort();}
static void olvidar(void *p){if(!p)return;for(size_t i=0;i<65536;i++)if(vivos[i]==p){vivos[i]=NULL;--nvivos;return;}}
void *__wrap_malloc(size_t n){if(fallar("malloc",n))return NULL;void *p=__real_malloc(n);observar(p);return p;}
void *__wrap_calloc(size_t n,size_t z){if(fallar("calloc",n*z))return NULL;void *p=__real_calloc(n,z);observar(p);return p;}
void *__wrap_realloc(void *p,size_t n){if(fallar("realloc",n))return NULL;void *q=__real_realloc(p,n);if(q){olvidar(p);observar(q);}return q;}
char *__wrap_strdup(const char *s){if(fallar("strdup",strlen(s)+1))return NULL;char *p=__real_strdup(s);observar(p);return p;}
void __wrap_free(void *p){olvidar(p);__real_free(p);}
static void *ast_malloc(size_t n){int f=fase;fase=1;void *p=malloc(n);fase=f;return p;}
static char *ast_dup(const char *s){int f=fase;fase=1;char *p=strdup(s);fase=f;return p;}
char *analizar_fmt(const char*,size_t);
static char *ast_fmt(const char *s,size_t n){int f=fase;fase=1;char *p=analizar_fmt(s,n);fase=f;return p;}
#define malloc ast_malloc
#define strdup ast_dup
#define analizar_fmt ast_fmt
#include "../../src/latast.c"
#undef malloc
#undef strdup
#undef analizar_fmt
static void *codigo_asignar(lat_mv *m,size_t n){int f=fase;fase=2;void *p=latM_asignar(m,n);fase=f;return p;}
#define latM_asignar codigo_asignar
#include "../../src/latdo.c"
#undef latM_asignar
static ast *ins[8];static int ni;
static ast *num(double x){return latA_numerico(x,2,3);}
static ast *id(void){return latA_var("x",2,3,false);}
static ast *bloque(void){return latA_nodo(NODO_BLOQUE,num(9),NULL,2,3);}
static ast *crear(const char *s){
 if(!strcmp(s,"nodo"))return latA_nodo(NODO_SUMA,ins[0],ins[1],2,3);
 if(!strcmp(s,"logico"))return latA_logico(1,2,3);
 if(!strcmp(s,"nulo"))return latA_nulo(NULL,2,3);
 if(!strcmp(s,"numerico"))return num(4.25);
 if(!strcmp(s,"cadena"))return latA_cadena("María\\nñ",2,3);
 if(!strcmp(s,"literal"))return latA_literal("hola",2,3);
 if(!strcmp(s,"var"))return id();
 if(!strcmp(s,"asign"))return latA_asign(ins[0],ins[1]);
 if(!strcmp(s,"compuesta"))return latA_asign(ins[0],ins[1]);
 if(!strcmp(s,"asign_le"))return latA_asign_le(ins[0],ins[1],ins[2]);
 if(!strcmp(s,"si"))return latA_si(ins[0],ins[1],ins[2]);
 if(!strcmp(s,"mientras"))return latA_mientras(ins[0],ins[1]);
 if(!strcmp(s,"hacer"))return latA_hacer(ins[0],ins[1]);
 if(!strcmp(s,"desde"))return latA_desde(ins[0],ins[1],ins[2],ins[3]);
 if(!strcmp(s,"para"))return latA_para(ins[0],ins[1],ins[2],ins[3],ins[4]);
 if(!strcmp(s,"funcion"))return latA_funcion(ins[0],ins[1],ins[2],2,3);
 if(!strcmp(s,"clase"))return latA_clase(ins[0],ins[1],ins[2],2,3);
 if(!strcmp(s,"nueva"))return latA_nueva(ins[0],ins[1],2,3);
 if(!strcmp(s,"mi"))return latA_mi(2,3);
 if(!strcmp(s,"super"))return latA_super(ins[0],2,3);
 abort();
}
static void entradas(const char *s){
 if(!strcmp(s,"nodo")||!strcmp(s,"asign")||!strcmp(s,"mientras")||!strcmp(s,"hacer")||!strcmp(s,"nueva"))ni=2;
 else if(!strcmp(s,"asign_le")||!strcmp(s,"si")||!strcmp(s,"funcion")||!strcmp(s,"clase"))ni=3;
 else if(!strcmp(s,"desde"))ni=4;else if(!strcmp(s,"para"))ni=5;else if(!strcmp(s,"super"))ni=1;
 for(int i=0;i<ni;i++)ins[i]=num(i+1);
 if(!strcmp(s,"desde")){latA_destruir(ins[3]);ins[3]=bloque();}
 if(!strcmp(s,"para")){latA_destruir(ins[0]);ins[0]=id();latA_destruir(ins[4]);ins[4]=bloque();}
 if(!strcmp(s,"compuesta")){ins[1]=id();ins[0]=latA_nodo(NODO_SUMA,ins[1],num(2),2,3);ni=1;}
}
static char *fuente(const char *p){FILE *f=fopen(p,"rb");if(!f)return NULL;char *s=malloc(32768);size_t n=fread(s,1,32767,f);s[n]=0;fclose(f);return s;}
struct compilacion{ast *a;lat_objeto *f;};
static void compilar(lat_mv *m,void *v){struct compilacion *c=v;c->f=latC_analizar(m,c->a);}
static int proteger_compilar(lat_mv *m,struct compilacion *c){struct lat_longjmp g;int r=latE_proteger(m,&g,compilar,c);fase=0;return r;}
static void estado(lat_mv *m){C(m->ptrpila==1&&getNumerico(latC_tope(m))==777);C(m->ptrctx==0&&m->numejec==0&&!m->error);C(m->enBucle==0&&!m->enClase);}
int main(int argc,char **argv){
 if(argc!=4)return 2;const char *tmp=getenv("MICTLAN_ARCHIVOS_TMP");char cwd[4096];if(!tmp||!getcwd(cwd,sizeof cwd)||strcmp(tmp,cwd))return 2;
 const char *modo=argv[1],*caso=argv[2];objetivo=atoi(argv[3]);
 if(!strcmp(modo,"constructor")){
  entradas(caso);size_t antes=nvivos;errno=0;armado=1;ast *a=crear(caso);armado=0;int e=errno;
  if(objetivo){C(inyectado==1);C(!a&&e==ENOMEM);C(nvivos==antes);if(!strcmp(caso,"desde"))C(ins[3]->der==NULL);if(!a)for(int i=0;i<ni;i++)latA_destruir(ins[i]);}
  else C(a!=NULL);
  if(a)latA_destruir(a);C(nvivos==0);
 }else{
  char *s=fuente(caso);if(!s)return 2;filename="sintetico.mict";
  lat_mv *m=latC_crear_mv();m->nombre_archivo=filename;latC_apilar_double(m,777);
  int st=0;ast *a=NULL;
  if(!strcmp(modo,"parser")||!strcmp(modo,"sintaxis")){
   armado=1;a=latA_analizar_exp(s,&st);armado=0;C(st==(objetivo?2:!strcmp(modo,"sintaxis")?1:0));if(st)C(!a);if(objetivo)C(inyectado==1);if(a)latA_destruir(a);C(nvivos==0);estado(m);
  }else if(!strcmp(modo,"compilar")){
   a=latA_analizar_exp(s,&st);C(!st&&a);size_t ast_vivos=nvivos;struct compilacion c={a,NULL};armado=1;int r=proteger_compilar(m,&c);armado=0;C(r==(objetivo?LAT_ERRRUN:0));if(objetivo){C(inyectado==1);C(nvivos==ast_vivos);}latA_destruir(a);estado(m);
  }else return 2;
  /* Recuperar en la misma VM: analizar, compilar y ejecutar un resultado fijo. */
  a=latA_analizar_exp("resultado_a1 = 6 * 7\n",&st);C(!st&&a);struct compilacion c={a,NULL};C(proteger_compilar(m,&c)==0);latA_destruir(a);if(c.f)C(latC_llamar_funcion(m,c.f)==0);
  lat_objeto *o=latO_obtener_contexto(m,m->contexto[0],"resultado_a1");C(o&&o->tipo==T_NUMERIC&&getNumerico(o)==42);estado(m);latC_destruir_mv(m);C(nvivos==0);free(s);
 }
 printf("A1_RESULTADO reservas=%d inyectado=%d vivos=%zu estado=%s\n",ordinal,inyectado,nvivos,bien?"OK":"FALLO");return bien?0:1;
}
