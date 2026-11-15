#define _GNU_SOURCE
#include "latino.h"
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
int parse_silent=0;char *filename=NULL;
enum {BIEN,LECTURA,INTERRUPCION,PUBLICAR,RESERVA,REDIMENSION,COMPILAR,NATIVO};
static int fallo,activado,interrumpido,publicaciones,nativos;static void *reservas[4096];static size_t vivos;
static void observar(void *p){if(!p)return;for(int i=0;i<4096;++i)if(!reservas[i]){reservas[i]=p;++vivos;return;}abort();}
static void *reservar(size_t n){if(fallo==RESERVA){activado=1;errno=ENOMEM;return NULL;}void *p=malloc(n);observar(p);return p;}
static void *cero(size_t n,size_t t){if(fallo==RESERVA){activado=1;errno=ENOMEM;return NULL;}void *p=calloc(n,t);observar(p);return p;}
static void liberar(void *p){for(int i=0;i<4096;++i)if(p&&reservas[i]==p){reservas[i]=NULL;--vivos;break;}free(p);}
static void *ampliar(void *p,size_t n){if(fallo==REDIMENSION){activado=1;errno=ENOMEM;return NULL;}int i=0;while(i<4096&&reservas[i]!=p)++i;void *q=realloc(p,n);if(q&&i<4096)reservas[i]=q;return q;}
static char *leer_readline(const char *p){if(fallo==RESERVA||fallo==REDIMENSION){activado=1;errno=ENOMEM;return NULL;}char *r=readline(p);observar(r);return r;}
static lat_objeto *cadena(lat_mv *m,const char *s){if(fallo==PUBLICAR){activado=1;++publicaciones;latC_error(m,"Publicacion inyectada");}return latC_crear_cadena(m,s);}
static void apilar_texto(lat_mv *m,const char *s){if(fallo==PUBLICAR){activado=1;latC_error(m,"Publicacion inyectada");}latC_apilar_string(m,s);}
static lat_objeto *numerico(lat_mv *m,double x){if(fallo==PUBLICAR){activado=1;latC_error(m,"Publicacion inyectada");}return latC_crear_numerico(m,x);}
static void apilar_numero(lat_mv *m,double x){if(fallo==PUBLICAR){activado=1;latC_error(m,"Publicacion inyectada");}latC_apilar_double(m,x);}
static lat_objeto *compilar(lat_mv *m,ast *n){if(fallo==COMPILAR){activado=1;latC_error(m,"Compilacion inyectada");}return latC_analizar(m,n);}
static int no_cargar(lat_mv *m,const char *p,const char *s){(void)m;++nativos;if(fallo==NATIVO)activado=1;if(strcmp(p,"liblatino-ausente.so")||strcmp(s,"latC_abrir_liblatino_ausente"))abort();return 1;}
static FILE *entrada_controlada;
static int caracter_inyectado(FILE *p){return fgetc(p==stdin?entrada_controlada:p);}
static int error_inyectado(FILE *p){return ferror(p==stdin?entrada_controlada:p);}
static void limpiar_inyectado(FILE *p){clearerr(p==stdin?entrada_controlada:p);}
#define fgetc caracter_inyectado
#define ferror error_inyectado
#define clearerr limpiar_inyectado
#define malloc reservar
#define calloc cero
#define realloc ampliar
#define free liberar
#define readline leer_readline
#define latC_crear_cadena cadena
#define latC_crear_numerico numerico
#define latC_apilar_double apilar_numero
#define latC_apilar_string apilar_texto
#define latC_analizar compilar
#define latC_cargarlib no_cargar
#include "../../src/latbaselib.c"
#undef latC_cargarlib
#undef latC_analizar
#undef latC_crear_cadena
#undef latC_crear_numerico
#undef latC_apilar_string
#undef latC_apilar_double
#undef readline
/* Se observa el AST que pertenece al importador, sin alterar gramática. */
#include "../../src/latast.c"
#undef fgetc
#undef ferror
#undef clearerr
#undef malloc
#undef calloc
#undef realloc
#undef free
struct entrada {const char *texto;size_t pos,n;};
static ssize_t recibir(void *datos,char *buf,size_t n){struct entrada *e=datos;(void)n;
 if(e->pos==2&&!interrumpido&&(fallo==LECTURA||fallo==INTERRUPCION)){interrumpido=1;activado=1;errno=fallo==LECTURA?EIO:EINTR;return -1;}
 if(e->pos==e->n)return 0;*buf=e->texto[e->pos++];return 1;}
static FILE *flujo(struct entrada *e){cookie_io_functions_t f={.read=recibir};FILE *p=fopencookie(e,"r",f);if(p)setvbuf(p,NULL,_IONBF,0);return p;}
/* Readline admite un lector de caracteres propio. El FILE de inyección no
   tiene descriptor; evitar que el backend por defecto intente select(-1).
   La interrupción del backend se reintenta como en el lector normal. */
static int caracter_readline(FILE *p){int c;for(;;){errno=0;c=caracter_inyectado(p);if(c!=EOF||!error_inyectado(p)||errno!=EINTR)return c;limpiar_inyectado(p);}}
int main(int argc,char **argv){
 char cwd[4096];const char *raiz=getenv("MICTLAN_ARCHIVOS_TMP");if(argc!=3||!raiz||!getcwd(cwd,sizeof cwd)||strcmp(cwd,raiz))return 2;
 setenv("HOME",raiz,1);unsetenv("LATINO_LIB");FILE *rc=fopen("inputrc","w");if(!rc)return 2;fclose(rc);setenv("INPUTRC","inputrc",1);
 lat_mv *mv=latC_crear_mv();mv->nombre_archivo="padre.mict";mv->nlin=17;mv->ncol=23;latC_apilar_double(mv,777);
 const char *api=argv[1];int seleccionado=atoi(argv[2]);lat_objeto *fn=latO_obtener_contexto(mv,mv->contexto[0],api);
 if(!fn||fn->tipo!=T_CFUN){fprintf(stderr,"B4B_API ausente %s\n",api);latC_destruir_mv(mv);return 1;}
 char largo[6001];memset(largo,'x',6000);largo[6000]=0;
 struct entrada e={seleccionado==REDIMENSION?largo:seleccionado==PUBLICAR&&strcmp(api,"leer_numero")?"hola\n":"12\n",0,0};e.n=strlen(e.texto);
 FILE *anterior=stdin,*p=fopen("entrada","w+b");entrada_controlada=flujo(&e);if(!p||!entrada_controlada)return 2;fputs(e.texto,p);rewind(p);stdin=p;rl_instream=p;rl_outstream=stdout;rl_getc_function=caracter_readline;
 if(!strcmp(api,"incluir")){FILE *f=fopen("mod.mict","wb");if(!f)return 2;fputs("a = 42\n",f);fclose(f);latC_apilar_string(mv,seleccionado==NATIVO?"ausente":"mod.mict");}
 fallo=seleccionado;int estado=latC_llamar_funcion(mv,fn);fallo=BIEN;
 int bien=activado&&vivos==0;
 if(!strcmp(api,"incluir"))bien&=estado==LAT_ERRRUN&&!strcmp(mv->nombre_archivo,"padre.mict")&&mv->nlin==17&&mv->ncol==23;
 else {
  int exito=seleccionado==INTERRUPCION;
  int salto=seleccionado==PUBLICAR||(!exito&&strcmp(api,"leer"));
  bien&=estado==(salto?LAT_ERRRUN:0);
  if(!estado&&mv->ptrpila==2){lat_objeto *o=latC_desapilar(mv);bien&=exito?(!strcmp(api,"leer_texto")?o->tipo==T_STR&&!strcmp(latC_checar_cadena(mv,o),"12"):o->tipo==T_NUMERIC&&getNumerico(o)==12):o->tipo==T_NULL;}else if(!estado)bien=0;
 }
 bien&=mv->ptrpila==1&&getNumerico(latC_tope(mv))==777&&mv->ptrctx==0&&mv->numejec==0&&!mv->error;
 printf("api=%s fallo=%d activado=%d estado=%d buffers=%zu nativos_interceptados=%d\n",api,seleccionado,activado,estado,vivos,nativos);
 /* Todos los buffers observados son del arnés; limpiar tras registrar la base. */
 for(int i=0;i<4096;++i)if(reservas[i]){free(reservas[i]);reservas[i]=NULL;}vivos=0;
 fclose(p);fclose(entrada_controlada);struct entrada correcta={"42\n",0,3};p=fopen("recuperacion","w+b");if(!p)return 2;fputs("42\n",p);rewind(p);entrada_controlada=flujo(&correcta);stdin=p;rl_instream=p;
 if(strcmp(api,"incluir")){int r=latC_llamar_funcion(mv,fn);bien&=r==0;if(!r){lat_objeto *o=latC_desapilar(mv);bien&=!strcmp(api,"leer_texto")?o->tipo==T_STR&&!strcmp(latC_checar_cadena(mv,o),"42"):o->tipo==T_NUMERIC&&getNumerico(o)==42;}}
 else {latC_apilar_string(mv,"mod.mict");bien&=latC_llamar_funcion(mv,fn)==0;}
 fclose(p);fclose(entrada_controlada);stdin=anterior;rl_instream=NULL;latC_destruir_mv(mv);if(!bien)fprintf(stderr,"B4B_ASSERT recursos/recuperacion\n");return bien?0:1;
}
