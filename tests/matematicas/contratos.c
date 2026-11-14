/* Oraculos: identidades matematicas, limites de <float.h>, contratos publicados.
   No se generan resultados esperados ejecutando Mictlan. */
#include "latino.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static lat_mv *vm;
static int fallo;
#define C(c) do { if (!(c)) { fprintf(stderr,"P12B3_ASSERT %d: %s\n",__LINE__,#c); return 1; } } while(0)
static lat_objeto *funcion(lat_mv *m, const char *nombre) {
    if (!strncmp(nombre,"anumero",7)) return latO_obtener_contexto(m,m->contexto[0],nombre);
    return latH_obtener(getDic(latO_obtener_contexto(m,m->contexto[0],"mate")),nombre);
}
static int variable(const char *s) { return !strcmp(s,"max")||!strcmp(s,"min")||!strcmp(s,"aleatorio")||!strcmp(s,"alt"); }
static lat_objeto llamar(lat_mv *m,const char *s,const double *v,int n) {
    lat_objeto r={0}; lat_objeto *f=funcion(m,s);
    if (!f || f->tipo!=T_CFUN) { fprintf(stderr,"Funcion ausente: %s\n",s); fallo=1; return r; }
    for(int i=0;i<n;++i) latC_apilar_double(m,v[i]);
    if(variable(s)) latC_apilar_double(m,n);
    if(latC_llamar_funcion(m,f)) { fallo=1; return r; }
    r=*latC_desapilar(m); return r;
}
static double uno(const char *s,double x) { lat_objeto r=llamar(vm,s,&x,1);if(r.tipo!=T_NUMERIC)fallo=1;return r.tipo==T_NUMERIC?getNumerico(&r):NAN; }
static double dos(const char *s,double x,double y) { double v[]={x,y};lat_objeto r=llamar(vm,s,v,2);if(r.tipo!=T_NUMERIC)fallo=1;return r.tipo==T_NUMERIC?getNumerico(&r):NAN; }
static double texto(const char *s,const char *v,int *estado) {
    lat_objeto *f=funcion(vm,s);if(!f||f->tipo!=T_CFUN){fallo=1;*estado=-1;return NAN;}
    latC_apilar_string(vm,v);*estado=latC_llamar_funcion(vm,f);if(*estado)return NAN;
    lat_objeto *r=latC_desapilar(vm);if(r->tipo!=T_NUMERIC){fallo=1;return NAN;}return getNumerico(r);
}
static int cerca(double a,double b,double tol) {return isfinite(a)&&fabs(a-b)<=tol;}
static int extremos(void) {
    C(dos("max",-1e20,-2e20)==-1e20); C(dos("min",1e20,2e20)==1e20);
    C(dos("max",-INFINITY,-INFINITY)==-INFINITY); C(dos("min",INFINITY,INFINITY)==INFINITY);
    C(dos("max",NAN,3)==3); C(dos("min",3,NAN)==3);
    C(isnan(dos("max",NAN,NAN)));C(isnan(dos("min",NAN,NAN)));
    C(signbit(dos("max",-0.,0.)));C(!signbit(dos("min",0.,-0.)));
    C(dos("max",DBL_MAX,-DBL_MAX)==DBL_MAX);C(dos("min",DBL_MAX,-DBL_MAX)==-DBL_MAX);
    return fallo;
}
static int conversion(void) {
    C(uno("anumero",0)==0); C(signbit(uno("anumero",-0.)));
    int e;C(texto("anumero","0",&e)==0&&e==0);C(signbit(texto("anumero","-0",&e))&&e==0);
    C(texto("anumero","abc",&e)==97);C(texto("anumero","12x",&e)==49);C(texto("anumero"," 0 ",&e)==32);
    C(isinf(uno("anumero",INFINITY)));C(isnan(uno("anumero",NAN)));return fallo;
}
static int herencia(void) {
    C(uno("redondear",2.5)==3);C(uno("rnd",-2.5)==-3);C(uno("trunc",-2.9)==-2);
    C(uno("techo",-2.5)==-2);C(uno("piso",-2.5)==-3);
    C(dos("frexp",8,123)==.5);C(dos("frexp",8,INFINITY)==.5);C(dos("frexp",8,NAN)==.5);
    C(dos("ldexp",.5,4.9)==8);C(dos("ldexp",.5,-1.9)==.25);
    C(dos("base",25,50)==50);C(dos("parte",50,25)==12.5);
    const char *p[]={"porc","porciento","porcentaje"};for(int i=0;i<3;++i)C(dos(p[i],25,50)==.5);
    C(isnan(uno("raiz",-1)));C(isnan(uno("log",-1)));C(uno("log",0)==-INFINITY);
    C(isinf(dos("porc",1,0)));C(isnan(dos("porc",0,0)));C(dos("pot",0,0)==1);
    C(signbit(uno("raiz",-0.)));C(!signbit(uno("abs",-0.)));return fallo;
}
static int precision(void) {
    C(FLT_RADIX==2&&DBL_MANT_DIG==53&&DBL_MAX_EXP==1024);
    const double xs[]={-2,-1,-.5,0,.5,1,2};
    for(size_t i=0;i<sizeof xs/sizeof *xs;++i){double x=xs[i],s=uno("sen",x),c=uno("cos",x);
      C(fabs(s)<=1&&fabs(c)<=1);C(cerca(s*s+c*c,1,8*DBL_EPSILON));
      C(cerca(uno("log",uno("exp",x)),x,16*DBL_EPSILON));
      C(cerca(uno("tan",uno("atan",x)),x,16*DBL_EPSILON));
    }
    C(uno("raiz",DBL_MAX)>1e154);C(uno("raiz",DBL_TRUE_MIN)>0);
    volatile double a=.1,b=.2;C(a+b!=.3);C(cerca(a+b,.3,DBL_EPSILON));
    C(uno("abs",9007199254740992.)+1==9007199254740992.);return fallo;
}
static int binario(void) {
    const double xs[]={0.,-0.,1.,-1.,.125,123.45,DBL_MAX,DBL_MIN,DBL_TRUE_MIN};
    for(size_t i=0;i<sizeof xs/sizeof *xs;++i){lat_objeto r=llamar(vm,"descomponer_binario",xs+i,1);
      C(r.tipo==T_LIST&&getLista(&r)->longitud==2);
      double f=getNumerico((lat_objeto *)latL_obtener_elemento(vm,getLista(&r),0));
      double e=getNumerico((lat_objeto *)latL_obtener_elemento(vm,getLista(&r),1));
      C(e==trunc(e));if(xs[i])C(fabs(f)>=.5&&fabs(f)<1);else C(e==0&&signbit(f)==signbit(xs[i]));
      double v=dos("ldexp",f,e);C(v==xs[i]&&signbit(v)==signbit(xs[i]));
    }return fallo;
}
static int azar(void) {
    /* Vector de referencia SplitMix64, estado inicial 0, bits bajos de 32. */
    const uint32_t esperados[]={0x7b1dcdaf,0xa1b965f4,0x8009454f,0x724c81ec,0x51a8749b};
    double cero=0;lat_mv *otra=latC_crear_mv();otra->nombre_archivo="azar-otra";
    for(int ronda=0;ronda<2;++ronda){(void)llamar(vm,"semilla_aleatoria",&cero,1);
      for(int i=0;i<5;++i)C(dos("aleatorio_entero",0,4294967295.)==esperados[i]);}
    double limites[]={0,4294967295.};lat_objeto r=llamar(otra,"aleatorio_entero",limites,2);
    int ok=r.tipo==T_NUMERIC&&getNumerico(&r)==esperados[0];latC_destruir_mv(otra);C(ok);
    C(dos("aleatorio_entero",-9007199254740991.,-9007199254740991.)==-9007199254740991.);
    C(dos("aleatorio_entero",9007199254740991.,9007199254740991.)==9007199254740991.);
    for(int i=0;i<1000;++i){double x=dos("aleatorio_entero",-7,13);C(x>=-7&&x<=13&&x==trunc(x));
      x=dos("aleatorio",5,-5);C(x>=-5&&x<=5);x=uno("alt",3.9);C(x>=0&&x<=3);}
    for(int i=0;i<20;++i){r=llamar(vm,"aleatorio",NULL,0);C(r.tipo==T_NUMERIC&&(getNumerico(&r)==0||getNumerico(&r)==1));}
    C(dos("aleatorio",INT_MAX,INT_MAX)==INT_MAX);C(dos("aleatorio",INT_MIN,INT_MIN)==INT_MIN);return fallo;
}
static int estricto(void) {
    int e;C(texto("anumero_estricto","  +12.5e-1\t",&e)==1.25&&e==0);
    C(texto("anumero_estricto",".5",&e)==.5);C(texto("anumero_estricto","1.",&e)==1);
    C(signbit(texto("anumero_estricto","-0",&e)));C(texto("anumero_estricto","0e-99999",&e)==0);
    C(texto("anumero_estricto","4.9406564584124654e-324",&e)==DBL_TRUE_MIN);
    C(texto("anumero_estricto","1.7976931348623157e308",&e)==DBL_MAX);
    const char *malos[]={""," ","abc","12x","nan","inf","0x1p2","1,2","1e","--1","1e309","1e-9999"};
    for(size_t i=0;i<sizeof malos/sizeof *malos;++i){(void)texto("anumero_estricto",malos[i],&e);C(e==LAT_ERRRUN&&vm->ptrpila==0);C(uno("abs",-2)==2);}
    return fallo;
}
static int dominio(void) {
    C(uno("raiz_segura",4)==2);C(signbit(uno("raiz_segura",-0.)));C(uno("log_seguro",1)==0);
    C(dos("potencia_segura",-2,3)==-8);C(dos("potencia_segura",2,-1074)==DBL_TRUE_MIN);
    const char *pred[]={"es_finito","es_infinito","es_nan"};double xs[]={0,DBL_MAX,INFINITY,-INFINITY,NAN};
    for(int p=0;p<3;++p)for(int i=0;i<5;++i){lat_objeto r=llamar(vm,pred[p],xs+i,1);int v=p==0?i<2:p==1?i==2||i==3:i==4;C(r.tipo==T_BOOL&&!!getLogico(&r)==v);}
    return fallo;
}
static int recuperacion(void) {
    const char *ns[]={"max","min","aleatorio","aleatorio","aleatorio","ldexp","raiz_segura","log_seguro","potencia_segura","aleatorio_entero","semilla_aleatoria","descomponer_binario","anumero_estricto"};
    const double aa[][3]={{1,2,3},{1,2,3},{1,2,3},{-1},{NAN},{1,INFINITY},{-1},{0},{0,0},{9,2},{-1},{INFINITY},{NAN}};
    const int nn[]={3,3,3,1,1,2,1,1,2,2,1,1,1};
    for(int k=0;k<13;++k){lat_objeto *f=funcion(vm,ns[k]);C(f&&f->tipo==T_CFUN);latC_apilar_double(vm,777);
      for(int i=0;i<nn[k];++i){if(k<2&&i==2)latC_apilar_string(vm,"tipo");else latC_apilar_double(vm,aa[k][i]);}
      if(variable(ns[k]))latC_apilar_double(vm,nn[k]);
      C(latC_llamar_funcion(vm,f)==LAT_ERRRUN);C(vm->ptrpila==1&&getNumerico(latC_tope(vm))==777);
      C(uno("abs",-3)==3);C(vm->ptrpila==1&&vm->ptrctx==0&&vm->numejec==0&&!vm->error);(void)latC_desapilar(vm);
    }
    /* Una semilla invalida no altera el estado previo. */
    double z=0;(void)llamar(vm,"semilla_aleatoria",&z,1);lat_objeto *f=funcion(vm,"semilla_aleatoria");latC_apilar_double(vm,1.5);C(latC_llamar_funcion(vm,f)==LAT_ERRRUN);
    C(dos("aleatorio_entero",0,4294967295.)==0x7b1dcdaf);return fallo;
}
static void a_entero(lat_mv *m){lat_objeto o=*latC_desapilar(m);latC_apilar_double(m,latC_aint(m,&o));}
static void a_caracter(lat_mv *m){lat_objeto o=*latC_desapilar(m);latC_apilar_double(m,latC_achar(m,&o));}
static int sdk(void) {
    lat_objeto *fs[]={latC_crear_cfuncion(vm,a_entero),latC_crear_cfuncion(vm,a_caracter)};
    for(int k=0;k<2;++k){latC_apilar_string(vm,"65.9");C(latC_llamar_funcion(vm,fs[k])==0);C(getNumerico(latC_desapilar(vm))==65);
      const char *malos[]={"nan","inf","1e100","-1e100"};for(int i=0;i<4;++i){latC_apilar_string(vm,malos[i]);C(latC_llamar_funcion(vm,fs[k])==LAT_ERRRUN);C(vm->ptrpila==0);}}
    return fallo;
}
static int temporales(void) {
    (void)uno("abs",1);(void)dos("max",1,2);size_t inicio=vm->memoria_usada;
    for(int i=0;i<1000;++i){C(uno("abs",-2)==2);C(dos("max",1,2)==2);C(dos("min",1,2)==1);C(dos("parte",100,25)==25);}
    fprintf(stdout,"bytes_vm_inicio=%zu bytes_vm_final=%zu\n",inicio,vm->memoria_usada);
    C(vm->memoria_usada==inicio);return fallo;
}
int main(int argc,char **argv) {
    if(argc!=2)return 2;vm=latC_crear_mv();vm->nombre_archivo="contratos-p12b3";int r=2;
#define CASO(x) if(!strcmp(argv[1],#x)) r=x();
    CASO(extremos) CASO(conversion) CASO(herencia) CASO(precision) CASO(binario) CASO(azar) CASO(estricto) CASO(dominio) CASO(recuperacion) CASO(sdk) CASO(temporales)
    latC_destruir_mv(vm);return r||fallo;
}
