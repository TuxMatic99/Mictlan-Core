/* Fallos deterministas sobre archivos propios. Se observa antes de limpiar los
   recursos que la implementación defectuosa pudiera dejar abiertos. */
#define _GNU_SOURCE
#include "latino.h"
#include <stdarg.h>
#include <stdint.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
int parse_silent=0;char *filename=NULL;
enum fallo {BIEN,ABRIR_ORIGEN,ABRIR_DESTINO,LEER_ERROR,LEER_EINTR,LEER_CORTO,
 ESCRIBIR_ERROR,ESCRIBIR_EINTR,ESCRIBIR_CORTO,ESCRIBIR_CERO,CERRAR_ORIGEN,
 CERRAR_DESTINO,SYNC_ARCHIVO,SYNC_DIRECTORIO,RENOMBRAR,RESERVA,REDIMENSION,
 PUBLICAR,LISTA,COMPILAR};
static enum fallo fallo;
static FILE *flujos[32],*fuente,*salida;
static int fds[32],nfds,lecturas,escrituras,error_lectura,error_escritura;
static void *reservas[4096];static size_t vivos;static int activado;
static FILE *abrir(const char *p,const char *modo){
 if((fallo==ABRIR_ORIGEN&&strchr(modo,'r'))||(fallo==ABRIR_DESTINO&&!strchr(modo,'r'))){activado=1;errno=EACCES;return NULL;}
 FILE *f=fopen(p,modo);if(f){int i;for(i=0;i<32&&flujos[i];++i);if(i==32)abort();flujos[i]=f;if(strchr(modo,'r'))fuente=f;else salida=f;}return f;
}
static int cerrar(FILE *f){
 int es_fuente=f==fuente,es_salida=f==salida;
 for(int i=0;i<32;++i)if(flujos[i]==f)flujos[i]=NULL;
 int r=fclose(f);
 if((fallo==CERRAR_ORIGEN&&es_fuente)||(fallo==CERRAR_DESTINO&&es_salida)){activado=1;errno=EIO;return EOF;}return r;
}
static size_t leer(void *p,size_t t,size_t n,FILE *f){
 if(f==fuente){++lecturas;if(fallo==LEER_EINTR&&lecturas==1){activado=1;error_lectura=1;errno=EINTR;return 0;}
  if(fallo==LEER_ERROR){activado=1;error_lectura=1;errno=EIO;return 0;}
  if(fallo==LEER_CORTO&&n>1){activado=1;n=1;}}
 return fread(p,t,n,f);
}
static int caracter(FILE *f){
 if(f==fuente&&fallo==LEER_CORTO)activado=1;
 if(f==fuente&&(fallo==LEER_ERROR||fallo==LEER_EINTR)){++lecturas;if(fallo==LEER_ERROR||lecturas==1){activado=1;error_lectura=1;errno=fallo==LEER_ERROR?EIO:EINTR;return EOF;}}
 return fgetc(f);
}
static int error_flujo(FILE *f){return (f==fuente&&error_lectura)||(f==salida&&error_escritura)||ferror(f);}
static void limpiar_error(FILE *f){if(f==fuente)error_lectura=0;if(f==salida)error_escritura=0;clearerr(f);}
static size_t escribir(const void *p,size_t t,size_t n,FILE *f){
 ++escrituras;
 if(fallo==ESCRIBIR_ERROR){activado=1;error_escritura=1;errno=EIO;return 0;}
 if(fallo==ESCRIBIR_EINTR&&escrituras==1){activado=1;error_escritura=1;errno=EINTR;return 0;}
 if(fallo==ESCRIBIR_CERO){activado=1;return 0;}
 if(fallo==ESCRIBIR_CORTO&&n>1){activado=1;n=1;}
 return fwrite(p,t,n,f);
}
static int poner_caracter(int c,FILE *f){
 ++escrituras;
 if(fallo==ESCRIBIR_CORTO)activado=1;
 if(fallo==ESCRIBIR_EINTR&&escrituras==1){activado=1;error_escritura=1;errno=EINTR;return EOF;}
 if(fallo==ESCRIBIR_ERROR||fallo==ESCRIBIR_CERO){activado=1;error_escritura=1;errno=EIO;return EOF;}return fputc(c,f);}
static void *reservar(size_t n){
 if(fallo==RESERVA){activado=1;return NULL;}void *p=malloc(n);if(p){size_t i=0;while(i<4096&&reservas[i])++i;if(i==4096)abort();reservas[i]=p;++vivos;}return p;
}
static void liberar(void *p){for(size_t i=0;i<4096;++i)if(p&&reservas[i]==p){reservas[i]=NULL;--vivos;break;}free(p);}
static void *redimensionar(void *p,size_t n){
 if(fallo==REDIMENSION){activado=1;return NULL;}size_t i=0;while(i<4096&&reservas[i]!=p)++i;
 void *q=realloc(p,n);if(q&&i<4096)reservas[i]=q;return q;
}
static int abrir_fd(const char *p,int flags,...){
 if(fallo==ABRIR_DESTINO){activado=1;errno=EACCES;return -1;}
 int fd=open(p,flags);if(fd>=0){if(nfds==32)abort();fds[nfds++]=fd;}return fd;
}
static int crear_temporal_inyectado(char *p,int flags){int fd=mkostemp(p,flags);if(fd>=0){if(nfds==32)abort();fds[nfds++]=fd;}return fd;}
static int cerrar_fd(int fd){
 struct stat st;int regular=fstat(fd,&st)==0&&S_ISREG(st.st_mode);
 for(int i=0;i<nfds;++i)if(fds[i]==fd)fds[i]=-1;
 int r=close(fd);if(fallo==CERRAR_DESTINO&&regular){activado=1;errno=EIO;return -1;}return r;
}
static ssize_t escribir_fd(int fd,const void *p,size_t n){
 ++escrituras;if(fallo==ESCRIBIR_ERROR){activado=1;errno=EIO;return -1;}
 if(fallo==ESCRIBIR_EINTR&&escrituras==1){activado=1;errno=EINTR;return -1;}
 if(fallo==ESCRIBIR_CERO){activado=1;return 0;}
 if(fallo==ESCRIBIR_CORTO&&n>1){activado=1;n=1;}return write(fd,p,n);
}
static int sincronizar(int fd){struct stat st;if(fstat(fd,&st))abort();if((fallo==SYNC_ARCHIVO&&S_ISREG(st.st_mode))||(fallo==SYNC_DIRECTORIO&&S_ISDIR(st.st_mode))){activado=1;errno=EIO;return -1;}return fsync(fd);}
static int renombrar(const char *a,const char *b){if(fallo==RENOMBRAR){activado=1;errno=EACCES;return -1;}return rename(a,b);}
static void publicar(lat_mv *m,const char *s){if(fallo==PUBLICAR){activado=1;latC_error(m,"Fallo inyectado al publicar texto");}latC_apilar_string(m,s);}
static lista *lista_crear(lat_mv *m){if(fallo==LISTA){activado=1;latC_error(m,"Fallo inyectado al crear lista");}return latL_crear(m);}
static lat_objeto *compilar(lat_mv *m,ast *n){if(fallo==COMPILAR){activado=1;latC_error(m,"Fallo inyectado al compilar archivo");}return latC_analizar(m,n);}
#define fopen abrir
#define fclose cerrar
#define fread leer
#define fgetc caracter
#define ferror error_flujo
#define clearerr limpiar_error
#define fwrite escribir
#define fputc poner_caracter
#define malloc reservar
#define realloc redimensionar
#define free liberar
#define open abrir_fd
#define mkostemp crear_temporal_inyectado
#define close cerrar_fd
#define write escribir_fd
#define fsync sincronizar
#define rename renombrar
#define latC_apilar_string publicar
#define latL_crear lista_crear
#define latC_analizar compilar
#include "../../src/latfilelib.c"
#undef latC_analizar
#undef latL_crear
#undef latC_apilar_string
#undef rename
#undef fsync
#undef write
#undef close
#undef mkostemp
#undef open
/* El cargador de fuente también es una interfaz de E/S de archivo.ejecutar.
   Sus reservas y cierres se observan; la gramática no se modifica. */
#include "../../src/latast.c"
#undef fopen
#undef fclose
#undef fread
#undef fgetc
#undef ferror
#undef clearerr
#undef fwrite
#undef fputc
#undef malloc
#undef realloc
#undef free

static int guardar(const char *p,const char *s){FILE *f=fopen(p,"wb");if(!f)return 0;int b=fwrite(s,1,strlen(s),f)==strlen(s);return fclose(f)==0&&b;}
static int contenido(const char *p,const char *s){FILE *f=fopen(p,"rb");if(!f)return 0;int b=1;for(size_t i=0;i<strlen(s);++i)if(fgetc(f)!=(unsigned char)s[i])b=0;if(fgetc(f)!=EOF)b=0;return fclose(f)==0&&b;}
static int temporales(void){DIR *d=opendir(".");if(!d)return -1;int n=0;struct dirent *e;while((e=readdir(d)))if(!strncmp(e->d_name,".latino-",8))++n;closedir(d);return n;}
int main(int argc,char **argv){
 if(argc!=3)return 2;char cwd[4096];const char *raiz=getenv("MICTLAN_ARCHIVOS_TMP");if(!raiz||!getcwd(cwd,sizeof cwd)||strcmp(cwd,raiz))return 2;
 const char *op=argv[1];int seleccionado=atoi(argv[2]);
 if(!guardar("origen","ORIGEN ñ\n")||!guardar("destino","ANTERIOR")||!guardar("ajeno","INTACTO")||!guardar("hijo.lat","archivo.escribir(\"marca\",\"42\")\n"))return 2;
 if(seleccionado==REDIMENSION){FILE *g=fopen("origen","wb");if(!g)return 2;for(int i=0;i<5000;++i)fputc('x',g);if(fclose(g))return 2;}
 lat_mv *mv=latC_crear_mv();mv->nombre_archivo="padre.mict";mv->global->REPL=true;
 lat_CFuncion native=!strcmp(op,"copia")?file_duplicar:!strcmp(op,"leer")?file_leer:!strcmp(op,"seguro")?file_leer_texto_seguro:!strcmp(op,"lineas")?file_lineas:!strcmp(op,"escribir")?file_intentar_escribir:file_ejecutar;
 lat_objeto *fn=latC_crear_cfuncion(mv,native);latC_apilar_double(mv,777);
 if(!strcmp(op,"copia")){latC_apilar_string(mv,"origen");latC_apilar_string(mv,"destino");}
 else if(!strcmp(op,"escribir")){latC_apilar_string(mv,"destino");latC_apilar_string(mv,"NUEVO");}
 else latC_apilar_string(mv,!strcmp(op,"ejecutar")?"hijo.lat":"origen");
 fallo=(enum fallo)seleccionado;int estado=latC_llamar_funcion(mv,fn);fallo=BIEN;
 int nflujo=0,nfd=0;for(int i=0;i<32;++i)if(flujos[i])++nflujo;for(int i=0;i<nfds;++i)if(fds[i]>=0)++nfd;
 int bien=activado&&nflujo==0&&nfd==0&&vivos==0&&temporales()==0&&contenido("ajeno","INTACTO");
 if(!strcmp(op,"copia")){
  int exito=seleccionado==LEER_EINTR||seleccionado==LEER_CORTO||seleccionado==ESCRIBIR_EINTR||seleccionado==ESCRIBIR_CORTO||seleccionado==SYNC_DIRECTORIO;
  bien&=estado==(exito?0:LAT_ERRRUN);bien&=contenido("destino",exito?"ORIGEN ñ\n":"ANTERIOR");
 } else if(!strcmp(op,"escribir")){
  int exito=seleccionado==ESCRIBIR_EINTR||seleccionado==ESCRIBIR_CORTO;
  bien&=estado==0&&mv->ptrpila==2;
  if(mv->ptrpila==2){lat_objeto *o=latC_desapilar(mv);bien&=o->tipo==T_BOOL&&!!getLogico(o)==exito;}
  if(exito)bien&=contenido("destino","NUEVO");
 } else if(!strcmp(op,"ejecutar")){
  bien&=estado==LAT_ERRRUN&&!strcmp(mv->nombre_archivo,"padre.mict")&&mv->global->REPL&&access("marca",F_OK)!=0;
 } else {
  int exito=seleccionado==LEER_EINTR||seleccionado==LEER_CORTO;
  int salto=seleccionado==PUBLICAR||seleccionado==LISTA||!strcmp(op,"lineas");
  bien&=estado==(!exito&&salto?LAT_ERRRUN:0);
  if(estado==0&&mv->ptrpila==2){lat_objeto *o=latC_desapilar(mv);bien&=exito?o->tipo==T_STR:o->tipo==T_BOOL&&!getLogico(o);if(exito&&o->tipo==T_STR)bien&=!strcmp(latC_checar_cadena(mv,o),"ORIGEN ñ\n");}
  else if(estado==0)bien=0;
 }
 bien&=mv->ptrpila==1&&getNumerico(latC_tope(mv))==777&&mv->ptrctx==0&&mv->numejec==0&&!mv->error;
 printf("operacion=%s fallo=%d activado=%d estado=%d flujos=%d fd=%d buffers=%zu temporales=%d destino_anterior=%d\n",op,seleccionado,activado,estado,nflujo,nfd,vivos,temporales(),contenido("destino","ANTERIOR"));
 /* Limpiar sólo recursos del arnés, tras registrar el defecto, aun en la base. */
 for(int i=0;i<32;++i)if(flujos[i])fclose(flujos[i]);for(int i=0;i<nfds;++i)if(fds[i]>=0)close(fds[i]);
 for(size_t i=0;i<4096;++i)if(reservas[i])free(reservas[i]);
 latC_destruir_mv(mv);if(!bien)fprintf(stderr,"B4A_ASSERT fallo o recurso no resuelto\n");return bien?0:1;
}
