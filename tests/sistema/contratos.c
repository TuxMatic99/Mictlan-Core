#define _GNU_SOURCE
#include "latino.h"
#include "latmem.h"
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
#define C(x) do { if (!(x)) { fprintf(stderr,"A0_ASSERT %d: %s\n",__LINE__,#x); bien=0; } } while(0)
int parse_silent=0;char *filename=NULL;
static int bien=1,armado,fallo,activado,abiertos,cerrados,nsleep,nsenal,nsystem,salidas;
static int fila,columna,ultimo_signal;static struct timespec pedidos[4];
static void (*manejador)(int);static FILE *tuberia;static void *reserva;
static const unsigned char *datos;static size_t cantidad,posicion;static int interrumpido;
static const char *release="6.8.12-sintetico";
enum { NADA, RELOJ, LOCAL, UTC, FORMATO, ABRIR, LEER, EINTR_LEER,
       CERRAR, RESERVAR, PUBLICAR, HIJO, PAUSA, EINTR_PAUSA, SENAL, SISTEMA, UNAME, CWD };
static time_t reloj(time_t *p){time_t n=1709251199;if(armado&&fallo==RELOJ){n=-1;activado=1;errno=EIO;}if(p)*p=n;return n;}
static struct tm *local_r(const time_t *t,struct tm *o){if(armado&&fallo==LOCAL){memset(o,0,sizeof *o);activado=1;errno=EOVERFLOW;return NULL;}return localtime_r(t,o);}
static struct tm *local(const time_t *t){static struct tm o;return local_r(t,&o);}
static struct tm *utc_r(const time_t *t,struct tm *o){if(armado&&fallo==UTC){activado=1;errno=EOVERFLOW;return NULL;}return gmtime_r(t,o);}
static struct tm *utc(const time_t *t){static struct tm o;return utc_r(t,&o);}
static size_t formato(char *s,size_t n,const char *f,const struct tm *t){if(armado&&fallo==FORMATO){activado=1;if(n){memset(s,'?',n-1);s[n-1]=0;}return 0;}return strftime(s,n,f,t);}
static ssize_t recibir(void *c,char *buf,size_t n){(void)c;if(posicion==2&&!interrumpido&&(fallo==LEER||fallo==EINTR_LEER)){activado=1;interrumpido=1;errno=fallo==LEER?EIO:EINTR;return -1;}if(posicion==cantidad)return 0;size_t m=cantidad-posicion;if(m>n)m=n;if(fallo==LEER||fallo==EINTR_LEER)m=1;memcpy(buf,datos+posicion,m);posicion+=m;return (ssize_t)m;}
static FILE *abrir(const char *s,const char *m){C(!strcmp(s,"productor_controlado")&&!strcmp(m,"r"));if(fallo==ABRIR){activado=1;errno=EMFILE;return NULL;}cookie_io_functions_t io={.read=recibir};tuberia=fopencookie(NULL,"r",io);if(tuberia)++abiertos;return tuberia;}
static int cerrar(FILE *p){C(p==tuberia);++cerrados;int r=fclose(p);tuberia=NULL;if(fallo==CERRAR){activado=1;errno=ECHILD;return -1;}return fallo==HIJO?7<<8:r;}
static void *asignar(lat_mv *m,size_t n){if(armado&&fallo==RESERVAR){activado=1;latC_error(m,"Reserva inyectada");}void *p=latM_asignar(m,n);if(armado)reserva=p;return p;}
static void liberar(lat_mv *m,void *p){if(p==reserva)reserva=NULL;latM_liberar(m,p);}
static lat_objeto *cadena(lat_mv *m,const char *s){if(armado&&fallo==PUBLICAR){activado=1;latC_error(m,"Publicacion inyectada");}return latC_crear_cadena(m,s);}
static void apilar_texto(lat_mv *m,const char *s){if(armado&&fallo==PUBLICAR){activado=1;latC_error(m,"Publicacion inyectada");}latC_apilar_string(m,s);}
static int pausa(const struct timespec *p,struct timespec *r){if(nsleep<4)pedidos[nsleep]=*p;++nsleep;if(fallo==EINTR_PAUSA&&nsleep==1){activado=1;if(r){r->tv_sec=0;r->tv_nsec=250000000;}errno=EINTR;return -1;}if(fallo==PAUSA){activado=1;errno=EINVAL;return -1;}return 0;}
static void (*senal(int s,void (*h)(int)))(int){++nsenal;ultimo_signal=s;manejador=h;if(fallo==SENAL){activado=1;errno=EINVAL;return SIG_ERR;}return SIG_DFL;}
static int sistema(const char *s){C(!strcmp(s,"productor_controlado"));++nsystem;if(fallo==SISTEMA){activado=1;errno=EAGAIN;return -1;}return fallo==HIJO?7<<8:0;}
static void salir(int s){C(s==0);++salidas;}
static int cursor(const char *f,...){C(!strcmp(f,"%c[%d;%df"));va_list a;va_start(a,f);C(va_arg(a,int)==27);fila=va_arg(a,int);columna=va_arg(a,int);va_end(a);return 0;}
static int version(struct utsname *o){if(fallo==UNAME){activado=1;errno=EIO;memset(o,0x7f,sizeof *o);return -1;}memset(o,0,sizeof *o);snprintf(o->release,sizeof o->release,"%s",release);return 0;}
static char *directorio(char *p,size_t n){if(fallo==CWD){activado=1;errno=ENOENT;return NULL;}return getcwd(p,n);}
#define time reloj
#define localtime local
#define localtime_r local_r
#define gmtime utc
#define gmtime_r utc_r
#define strftime formato
#define popen abrir
#define pclose cerrar
#define latM_asignar asignar
#define latM_liberar liberar
#define latC_crear_cadena cadena
#define latC_apilar_string apilar_texto
#define nanosleep pausa
#define signal senal
#define system sistema
#define exit salir
#define printf cursor
#define uname version
#define getcwd directorio
#include "../../src/latsyslib.c"
#undef time
#undef localtime
#undef localtime_r
#undef gmtime
#undef gmtime_r
#undef strftime
#undef popen
#undef pclose
#undef latM_asignar
#undef latM_liberar
#undef latC_crear_cadena
#undef latC_apilar_string
#undef nanosleep
#undef signal
#undef system
#undef exit
#undef printf
#undef uname
#undef getcwd
static lat_mv *mv;
static int llamar(const char *n){lat_objeto *mod=latO_obtener_contexto(mv,mv->contexto[0],"sis");lat_objeto *f=latH_obtener(latC_checar_dic(mv,mod),n);C(f&&f->tipo==T_CFUN);return f?latC_llamar_funcion(mv,f):LAT_ERRRUN;}
static void numero(double x){latC_apilar_double(mv,x);}
static void texto(const char *s){latC_apilar_string(mv,s);}
static void nulo(void){latC_apilar(mv,latO_nulo);}
static void resultado_texto(const char *s){lat_objeto *o=latC_desapilar(mv);C(o&&o->tipo==T_STR);if(o&&o->tipo==T_STR)C(!strcmp(latC_checar_cadena(mv,o),s));}
static void resultado_numero(double x){lat_objeto *o=latC_desapilar(mv);C(o&&o->tipo==T_NUMERIC);if(o&&o->tipo==T_NUMERIC)C(getNumerico(o)==x);}
static double valor(const char *s){if(!strcmp(s,"nan"))return NAN;if(!strcmp(s,"inf"))return INFINITY;if(!strcmp(s,"menos_inf"))return -INFINITY;if(!strcmp(s,"grande"))return 1e100;return strtod(s,NULL);}
static void estado_vm(void){C(mv->ptrpila==1&&getNumerico(latC_tope(mv))==777);C(mv->ptrctx==0&&mv->numejec==0&&!mv->error);C(!strcmp(mv->nombre_archivo,"sistema-sintetico.mict"));}
int main(int argc,char **argv){
 if(argc!=3)return 2;char cwd[4096];const char *raiz=getenv("MICTLAN_ARCHIVOS_TMP");if(!raiz||!getcwd(cwd,sizeof cwd)||strcmp(cwd,raiz))return 2;
 setenv("TZ","UTC0",1);tzset();setenv("USER","usuario_sintetico",1);mv=latC_crear_mv();mv->nombre_archivo="sistema-sintetico.mict";numero(777);
 const char *familia=argv[1],*caso=argv[2];int r=0,error=0;unsigned char *grande=NULL;
 if(!strcmp(familia,"tiempo")){
  const char *fmt="%Y-%m-%d %H:%M:%S";double t=0;int ahora=0,sin_formato=0,espera_nulo=0;
  if(!strcmp(caso,"epoch")){}
  else if(!strcmp(caso,"negativo"))t=-1;
  else if(!strcmp(caso,"fraccion"))t=1.9;
  else if(!strcmp(caso,"fraccion_negativa"))t=-1.9;
  else if(!strcmp(caso,"vacio")){fmt="";espera_nulo=1;}
  else if(!strcmp(caso,"largo")){fmt="1234567890123456789012345678901234567890123456789012345678901234567890";espera_nulo=1;}
  else if(!strcmp(caso,"ahora")){ahora=1;}
  else if(!strcmp(caso,"sin_formato")){sin_formato=1;}
  else if(!strcmp(caso,"reloj")){sin_formato=1;fallo=RELOJ;error=1;}
  else if(!strcmp(caso,"reloj_explicito")){fallo=RELOJ;}
  else if(!strcmp(caso,"local")){fallo=LOCAL;error=1;}
  else if(!strcmp(caso,"formato")){fallo=FORMATO;espera_nulo=1;}
  else if(!strcmp(caso,"tipo")){error=1;}
  else if(!strcmp(caso,"tipo_fecha")){error=1;}
  else if(!strcmp(caso,"limite_alto")){t=ldexp(1.,63);error=1;}
  else if(!strcmp(caso,"limite_bajo")){t=nextafter(-ldexp(1.,63),-INFINITY);error=1;}
  else if(!strcmp(caso,"no_representable")){t=nextafter(ldexp(1.,63),0);error=1;}
  else {t=valor(caso);error=1;}
  if(sin_formato)nulo();else if(!strcmp(caso,"tipo"))numero(1);else texto(fmt);
  if(sin_formato||!strcmp(caso,"tipo_fecha"))texto("ignorado");else if(ahora)nulo();else numero(t);
  armado=1;r=llamar("tiempo");armado=0;C(r==(error?LAT_ERRRUN:0));
  if(!r){if(error){(void)latC_desapilar(mv);}else if(sin_formato)resultado_numero(1709251199);else if(espera_nulo)C(latC_desapilar(mv)->tipo==T_NULL);else resultado_texto(ahora?"2024-02-29 23:59:59":t<0?"1969-12-31 23:59:59":t>=1?"1970-01-01 00:00:01":"1970-01-01 00:00:00");}
 }else if(!strcmp(familia,"fecha")){
  const char *form=caso;int tipo=0;
  if(!strcmp(caso,"reloj")){fallo=RELOJ;form="seg";error=1;}
  else if(!strcmp(caso,"local")){fallo=LOCAL;form="seg";error=1;}
  else if(!strcmp(caso,"utc")){fallo=UTC;form="iso";error=1;}
  else if(!strcmp(caso,"formato_sem")){fallo=FORMATO;form="sem";error=1;}
  else if(!strcmp(caso,"formato_iso")){fallo=FORMATO;form="iso";error=1;}
  else if(!strcmp(caso,"invalido"))error=1;
  else if(!strcmp(caso,"tipo")){tipo=1;error=1;}
  if(tipo)numero(1);else texto(form);armado=1;r=llamar("fecha");armado=0;C(r==(error?LAT_ERRRUN:0));
  if(!r){if(error)(void)latC_desapilar(mv);else if(!strcmp(form,"iso"))resultado_texto("2024-02-29T23:59:59Z");else if(!strcmp(form,"horario_de_verano")){lat_objeto *o=latC_desapilar(mv);C(o->tipo==T_BOOL&&!getLogico(o));}else {const char *f[]={"seg","min","hora","d_mes","mes","sem","año","d_sem","d_año"};int v[]={59,59,23,29,2,9,2024,5,60};int encontrado=0;for(int i=0;i<9;i++)if(!strcmp(f[i],form)){resultado_numero(v[i]);encontrado=1;}C(encontrado);}}
 }else if(!strcmp(familia,"pipe")){
  datos=(const unsigned char *)"hola\n";cantidad=5;const char *esperado="hola";
  if(!strcmp(caso,"vacio")){datos=(const unsigned char *)"";cantidad=0;esperado="";}
  else if(!strcmp(caso,"sin_lf")){cantidad=4;}
  else if(!strcmp(caso,"crlf")){datos=(const unsigned char *)"hola\r\n";cantidad=6;}
  else if(!strcmp(caso,"doble_lf")){datos=(const unsigned char *)"hola\n\n";cantidad=6;esperado="hola\n";}
  else if(!strcmp(caso,"nul")){datos=(const unsigned char *)"ab\0cd";cantidad=5;error=1;}
  else if(!strcmp(caso,"limite")||!strcmp(caso,"exceso")){cantidad=MAX_STR_LENGTH-1+(!strcmp(caso,"exceso")?65536:0);grande=malloc(cantidad);if(!grande)return 2;memset(grande,'x',cantidad);datos=grande;error=!strcmp(caso,"exceso");}
  else if(!strcmp(caso,"abrir")){fallo=ABRIR;error=1;}
  else if(!strcmp(caso,"leer")){fallo=LEER;error=1;}
  else if(!strcmp(caso,"eintr"))fallo=EINTR_LEER;
  else if(!strcmp(caso,"cerrar")){fallo=CERRAR;error=1;}
  else if(!strcmp(caso,"reserva")){fallo=RESERVAR;error=1;}
  else if(!strcmp(caso,"publicar")){fallo=PUBLICAR;error=1;}
  else if(!strcmp(caso,"hijo"))fallo=HIJO;
  else if(!strcmp(caso,"tipo"))error=1;
  if(!strcmp(caso,"tipo"))numero(1);else texto("productor_controlado");armado=1;r=llamar("pipe");armado=0;C(r==(error?LAT_ERRRUN:0));
  if(!r){if(error)(void)latC_desapilar(mv);else if(!strcmp(caso,"limite")){lat_objeto *o=latC_desapilar(mv);C(o->tipo==T_STR);if(o->tipo==T_STR)C(strlen(latC_checar_cadena(mv,o))==MAX_STR_LENGTH-1);}else resultado_texto(esperado);}
  C(abiertos==cerrados);C(!reserva);if(fallo&&fallo!=HIJO)C(activado);if(!strcmp(caso,"exceso"))C(posicion==cantidad);
  printf("pipe abiertos=%d cerrados=%d buffer=%d bytes=%zu/%zu estado=%d\n",abiertos,cerrados,reserva!=NULL,posicion,cantidad,r);
 }else if(!strcmp(familia,"dormir")){
  double v=valor(caso);if(!strcmp(caso,"eintr")){v=1.5;fallo=EINTR_PAUSA;}else if(!strcmp(caso,"error")){v=1.5;fallo=PAUSA;error=1;}else if(!strcmp(caso,"tipo"))error=1;else if(!strcmp(caso,"limite"))v=(double)INT_MAX/1000.;else error=!isfinite(v)||v<0||v*1000>INT_MAX;
  if(!strcmp(caso,"tipo"))texto("no numero");else numero(v);armado=1;r=llamar("dormir");armado=0;C(r==(error?LAT_ERRRUN:0));
  if(!error){C(nsleep==(!strcmp(caso,"eintr")?2:1));C(pedidos[0].tv_sec==(int)(v*1000)/1000);C(pedidos[0].tv_nsec==((int)(v*1000)%1000)*1000000L);if(nsleep==2)C(pedidos[1].tv_sec==0&&pedidos[1].tv_nsec==250000000);}else if(fallo!=PAUSA)C(nsleep==0);
 }else if(!strcmp(familia,"avisar")){
  double v=!strcmp(caso,"normal")||!strcmp(caso,"recibida")||!strcmp(caso,"fallo")?9.9:valor(caso);error=!isfinite(v)||v<INT_MIN||v>INT_MAX||!strcmp(caso,"tipo")||!strcmp(caso,"fallo");if(!strcmp(caso,"fallo"))fallo=SENAL;
  if(!strcmp(caso,"tipo"))texto("x");else numero(v);armado=1;r=llamar("avisar");armado=0;C(r==(error?LAT_ERRRUN:0));if(!r)C(latC_desapilar(mv)->tipo==T_BOOL);if(!error){C(nsenal==1&&ultimo_signal==9);if(!strcmp(caso,"recibida")){manejador(9);numero(9);C(llamar("avisar")==0);resultado_numero(9);}}
 }else if(!strcmp(familia,"iraxy")){
  double v=!strcmp(caso,"normal")?2.9:valor(caso);error=!isfinite(v)||v<INT_MIN||v>INT_MAX||!strcmp(caso,"tipo");if(!strcmp(caso,"tipo"))texto("x");else numero(v);numero(3.9);armado=1;r=llamar("iraxy");armado=0;C(r==(error?LAT_ERRRUN:0));if(!error)C(fila==2&&columna==3);
 }else if(!strcmp(familia,"version")){
  unsigned long a=6,b=8,c=12;if(!strcmp(caso,"fallo")){fallo=UNAME;a=b=c=0;}else if(!strcmp(caso,"sufijo")){release="6.8-12";c=0;}else if(!strcmp(caso,"corta")){release="6";b=c=0;}else if(!strcmp(caso,"vacia")){release="";a=b=c=0;}else if(!strcmp(caso,"negativa")){release="-6.8.12";a=b=c=0;}else if(!strcmp(caso,"desborde")){release="9999999999999999999999999999999999999.2.3";a=b=c=0;}
  armado=1;Struct v=buscar_os_version();armado=0;C(v.osMayor==a&&v.osMenor==b&&v.osBuild==c);
 }else if(!strcmp(familia,"aux")){
  if(!strcmp(caso,"ejecutar_error")||!strcmp(caso,"ejecutar_hijo")){fallo=!strcmp(caso,"ejecutar_error")?SISTEMA:HIJO;texto("productor_controlado");armado=1;r=llamar("ejecutar");armado=0;C(r==(fallo==SISTEMA?LAT_ERRRUN:0));C(nsystem==1);}
  else if(!strcmp(caso,"salir")){C(llamar("salir")==0);C(salidas==1);}
  else if(!strcmp(caso,"cwd_error")){fallo=CWD;C(llamar("cwd")==0);C(latC_desapilar(mv)->tipo==T_NULL);}
  else if(!strcmp(caso,"cwd")){C(llamar("cwd")==0);resultado_texto(raiz);}
  else if(!strcmp(caso,"usuario")){C(llamar("usuario")==0);resultado_texto("usuario_sintetico");}
  else if(!strcmp(caso,"usuario_ausente")){unsetenv("USER");C(llamar("usuario")==0);C(latC_desapilar(mv)->tipo==T_NULL);}
  else if(!strcmp(caso,"os")){texto("LINUX");C(llamar("os")==0);lat_objeto *o=latC_desapilar(mv);C(o->tipo==T_BOOL&&getLogico(o));texto("otra_plataforma");C(llamar("operativo")==0);o=latC_desapilar(mv);C(o->tipo==T_BOOL&&!getLogico(o));}
  else return 2;
 }else return 2;
 /* Registrar antes de la limpieza auxiliar del arnés: no ocultar fugas de base. */
 if(tuberia){fclose(tuberia);tuberia=NULL;}if(reserva){latM_liberar(mv,reserva);reserva=NULL;}free(grande);fallo=NADA;armado=0;estado_vm();
 texto("%Y");numero(0);C(llamar("tiempo")==0);resultado_texto("1970");estado_vm();latC_destruir_mv(mv);return bien?0:1;
}
