/* Se comprueba el objeto realmente cargado, antes de crear ninguna VM. */
#define _GNU_SOURCE
#include "latino.h"
#include <dlfcn.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;Dl_info info;char real[PATH_MAX],esperada[PATH_MAX];
 if(!dladdr((void *)latC_crear_mv,&info)||!realpath(info.dli_fname,real)||!realpath(argv[1],esperada))return 2;
 printf("ABI=%s biblioteca=%s esperada=%s\n",LAT_VERSION,real,esperada);
 if(strcmp(real,esperada)){fprintf(stderr,"Biblioteca Core inesperada\n");return 86;}return 0;
}
