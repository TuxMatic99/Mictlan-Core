/* Único productor de las pruebas reales de tubería: datos fijos y fichero propio.
   No interpreta órdenes, no lee datos externos ni ejecuta otros programas. */
#include <stdio.h>
#include <string.h>
int main(int argc,char **argv){
 if(argc!=2)return 2;const char *s="hola\n";size_t n=5;int salida=0;
 if(!strcmp(argv[1],"vacio")){s="";n=0;}
 else if(!strcmp(argv[1],"crlf")){s="hola\r\n";n=6;}
 else if(!strcmp(argv[1],"doble")){s="hola\n\n";n=6;}
 else if(!strcmp(argv[1],"sinlf")){n=4;}
 else if(!strcmp(argv[1],"utf8")){s="María ñ 😀\n";n=strlen(s);}
 else if(!strcmp(argv[1],"nul")){s="ab\0cd";n=5;}
 else if(!strcmp(argv[1],"limite")||!strcmp(argv[1],"exceso")){
  size_t cantidad=1024u*1024u-1u+(!strcmp(argv[1],"exceso")?262144u:0);char b[4096];memset(b,'x',sizeof b);
  while(cantidad){size_t m=cantidad<sizeof b?cantidad:sizeof b;if(fwrite(b,1,m,stdout)!=m)return 3;cantidad-=m;}n=0;
 }else if(!strcmp(argv[1],"hijo"))salida=7;
 else if(strcmp(argv[1],"normal"))return 2;
 if(n&&fwrite(s,1,n,stdout)!=n)return 3;
 if(fflush(stdout))return 3;
 FILE *f=fopen("productor.fin","wb");if(!f)return 4;
 if(fputs("terminado\n",f)<0||fclose(f))return 4;
 return salida;
}
