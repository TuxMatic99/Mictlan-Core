/*
The MIT License (MIT)

Copyright (c) Latino - Lenguaje de Programacion

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "latcompat.h"
#include "latdo.h"
#include "laterror.h"
#include "latnumerico.h"
#include "latgc.h"
#include "latino.h"
#include "latlist.h"
#include "latmem.h"
#include "latmv.h"

#define LIB_SISTEMA_NOMBRE "sis"

struct OS_VERSION_LAT { // estructura variable OS.Major, OS.Minor, OS.Build
  unsigned long osMayor, osMenor, osBuild;
};

typedef struct OS_VERSION_LAT Struct;

Struct
buscar_os_version() { // Busca y asigna el valor del Build del sistema operativo
  Struct os_v = {0, 0, 0};

#ifdef WIN32
  OSVERSIONINFO osvi;

  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

  GetVersionEx(&osvi);

  os_v.osMayor = osvi.dwMajorVersion;
  os_v.osMenor = osvi.dwMinorVersion;
  os_v.osBuild = osvi.dwBuildNumber;

#elif __APPLE__
  char cmd[64];
  for (int i = 0; i <= 3; i++) {
    sprintf(cmd, "sw_vers -productVersion | awk -F '.' '{print $%d}'", i);

    FILE *stdoutFile = popen(cmd, "r");

    int resp = 0;
    if (stdoutFile) {
      char buff[16];
      char *stdout = fgets(buff, sizeof(buff), stdoutFile);
      pclose(stdoutFile);
      sscanf(stdout, "%d", &resp);
    }
    switch (i) {
    case 1:
      os_v.osMayor = resp;
    case 2:
      os_v.osMenor = resp;
    case 3:
      os_v.osBuild = resp;
      break;
    }
  }
#elif __CYGWIN__
  // TODO: Implementar para MSYS2
#else
  struct utsname buffer;
  if (uname(&buffer) < 0) return os_v;
  /* Sólo segmentos decimales separados por puntos. Un sufijo del kernel
     no es otro componente; tampoco se avanza más allá del terminador. */
  const char *p = buffer.release;
  unsigned long *partes[] = {&os_v.osMayor, &os_v.osMenor, &os_v.osBuild};
  for (size_t i = 0; i < 3; ++i) {
    unsigned long valor = 0;
    if (*p < '0' || *p > '9') break;
    do {
      unsigned long digito = (unsigned long)(*p - '0');
      if (valor > (ULONG_MAX - digito) / 10) {
        errno = ERANGE;
        return os_v;
      }
      valor = valor * 10 + digito;
      ++p;
    } while (*p >= '0' && *p <= '9');
    *partes[i] = valor;
    if (*p != '.') break;
    ++p;
  }
#endif

  return os_v;
}

volatile sig_atomic_t senial;

/* Devuelve el fallo al llamador nativo sin cambiar la firma C histórica. */
static int sistema_pausar_ms(int milliseconds) {
  if (milliseconds < 0) { errno = EINVAL; return -1; }
#ifdef WIN32
  Sleep((DWORD)milliseconds);
  return 0;
#elif _POSIX_C_SOURCE >= 199309L
  struct timespec pedido, resto;
  pedido.tv_sec = milliseconds / 1000;
  pedido.tv_nsec = (milliseconds % 1000) * 1000000L;
  while (nanosleep(&pedido, &resto) != 0) {
    if (errno != EINTR) return -1;
    pedido = resto;
  }
  return 0;
#else
  /* El fallback no multiplica un int grande por 1000. */
  while (milliseconds > 0) {
    int bloque = milliseconds > 999 ? 999 : milliseconds;
    if (usleep((unsigned int)bloque * 1000U) != 0) return -1;
    milliseconds -= bloque;
  }
  return 0;
#endif
}

void sleep_ms(int milliseconds) {
  (void)sistema_pausar_ms(milliseconds);
}

static void latSO_ejecutar(lat_mv *mv) {
  lat_objeto *cmd = latC_desapilar(mv);
  if (system(latC_checar_cadena(mv, cmd)) == -1)
    latC_error(mv, "No se pudo ejecutar el comando");
  /* El estado no cero del hijo sigue sin ser un error del lenguaje. */
}

static void latSO_dormir(lat_mv *mv) {
  double segundos = latC_checar_numerico(mv, latC_desapilar(mv));
  if (!isfinite(segundos) || segundos < 0)
    latC_error(mv, "Duracion fuera de rango");
#ifdef WIN32
  double ms = trunc(segundos * 1000.0);
  if (!isfinite(ms) || ms > (double)MAXDWORD)
    latC_error(mv, "Duracion fuera de rango");
  Sleep((DWORD)ms);
#else
  int ms = latN_entero(mv, segundos * 1000.0, "Duracion fuera de rango");
  if (sistema_pausar_ms(ms) != 0)
    latC_error(mv, "Error al esperar la duracion indicada");
#endif
}

static void sistema_publicar_texto(lat_mv *mv, void *p) {
  latC_apilar_string(mv, p);
}

static void latSO_pipe(lat_mv *mv) {
  const char *comando = latC_checar_cadena(mv, latC_desapilar(mv));
  if (MAX_STR_LENGTH < 2) {
    latC_error(mv, "Capacidad de lectura insuficiente");
    return;
  }
  /* Reservar antes de abrir: latM_asignar puede lanzar un error. */
  char *p = latM_asignar(mv, MAX_STR_LENGTH);
  FILE *fp = latC_popen(mv, comando, "r");
  if (!fp) {
    latM_liberar(mv, p);
    latC_error(mv, "No se pudo ejecutar el comando");
    return;
  }
  const size_t capacidad = MAX_STR_LENGTH - 1;
  size_t usados = 0;
  int error_lectura = 0, exceso = 0, contiene_nul = 0;
  unsigned char bloque[8192];
  for (;;) {
    errno = 0;
    size_t n = fread(bloque, 1, sizeof(bloque), fp);
    int error = errno;
    if (memchr(bloque, 0, n)) contiene_nul = 1;
    size_t copiar = n < capacidad - usados ? n : capacidad - usados;
    memcpy(p + usados, bloque, copiar);
    usados += copiar;
    if (n > copiar) exceso = 1;
    /* Aun con exceso/NUL se consume la salida finita. Nunca se entrega un
       prefijo incompleto ni se espera al hijo dejando su salida bloqueada. */
    if (ferror(fp)) {
      if (error == EINTR) { clearerr(fp); continue; }
      error_lectura = 1;
      break;
    }
    if (feof(fp)) break;
    if (!n) { error_lectura = 1; break; }
  }
  int estado_cierre = latC_pclose(mv, fp);
  const char *error = error_lectura || estado_cierre == -1
      ? "Error al leer o cerrar la tuberia"
      : exceso ? "Salida de tuberia demasiado larga"
      : contiene_nul ? "Salida de tuberia contiene NUL" : NULL;
  if (error) {
    latM_liberar(mv, p);
    latC_error(mv, "%s", error);
    return;
  }
  if (usados && p[usados - 1] == '\n') {
    --usados;
    if (usados && p[usados - 1] == '\r') --usados;
  }
  p[usados] = '\0';
  struct lat_longjmp guard;
  int estado = latE_proteger(mv, &guard, sistema_publicar_texto, p);
  latM_liberar(mv, p);
  if (estado) latD_lanzar(mv, estado);
}

static time_t sistema_reloj(lat_mv *mv) {
  time_t ahora = time(NULL);
  if (ahora == (time_t)-1) latC_error(mv, "No se pudo consultar el reloj");
  return ahora;
}

static void sistema_calendario(lat_mv *mv, const time_t *raw,
                               struct tm *salida, int utc) {
#ifndef _WIN32
  struct tm *r = utc ? gmtime_r(raw, salida) : localtime_r(raw, salida);
#else
  struct tm *r = utc ? gmtime(raw) : localtime(raw);
  if (r) *salida = *r;
#endif
  if (!r) latC_error(mv, "Tiempo fuera de rango del calendario");
}

static void latSO_fecha(lat_mv *mv) {
  const char *num = latC_checar_cadena(mv, latC_desapilar(mv));
  time_t raw = sistema_reloj(mv);
  struct tm tipo;
  sistema_calendario(mv, &raw, &tipo, !strcmp(num, "iso"));
  double resultado;
  if (!strcmp(num, "seg")) resultado = tipo.tm_sec;
  else if (!strcmp(num, "min")) resultado = tipo.tm_min;
  else if (!strcmp(num, "hora")) resultado = tipo.tm_hour;
  else if (!strcmp(num, "d_mes")) resultado = tipo.tm_mday;
  else if (!strcmp(num, "mes")) resultado = tipo.tm_mon + 1;
  else if (!strcmp(num, "año")) resultado = (double)tipo.tm_year + 1900;
  else if (!strcmp(num, "d_sem")) resultado = tipo.tm_wday + 1;
  else if (!strcmp(num, "d_año")) resultado = tipo.tm_yday + 1;
  else if (!strcmp(num, "sem")) {
    char semana[3];
    if (!strftime(semana, sizeof(semana), "%V", &tipo))
      latC_error(mv, "No se pudo dar formato de tiempo");
    resultado = atoi(semana);
  } else if (!strcmp(num, "horario_de_verano")) {
    latC_apilar(mv, tipo.tm_isdst ? latO_verdadero : latO_falso);
    return;
  } else if (!strcmp(num, "iso")) {
    char iso[21];
    if (!strftime(iso, sizeof(iso), "%Y-%m-%dT%H:%M:%SZ", &tipo))
      latC_error(mv, "No se pudo dar formato de tiempo");
    latC_apilar_string(mv, iso);
    return;
  } else {
    latC_error(mv, "El formato de tiempo indicado no existe");
    return;
  }
  latC_apilar_double(mv, resultado);
}

/* time_t es entero en las plataformas soportadas. Comparar con potencias
   exactas de dos evita redondear TIME_MAX hacia un valor no representable. */
static time_t sistema_instante(lat_mv *mv, double numero) {
  int bits = (int)(sizeof(time_t) * CHAR_BIT);
  int con_signo = (time_t)-1 < (time_t)0;
  double limite = ldexp(1.0, bits - con_signo);
  double entero = trunc(numero);
  if (!isfinite(numero) || entero < (con_signo ? -limite : 0.0) || entero >= limite)
    latC_error(mv, "Tiempo fuera de rango");
  return (time_t)entero;
}

void latSO_tiempo(lat_mv *mv) {
  lat_objeto *unixt = latC_desapilar(mv);
  lat_objeto *obj_formato = latC_desapilar(mv);
  if (obj_formato->tipo == T_NULL) {
    latC_apilar_double(mv, (double)sistema_reloj(mv));
    return;
  }
  const char *fmt = latC_checar_cadena(mv, obj_formato);
  time_t instante = unixt->tipo == T_NULL ? sistema_reloj(mv)
      : sistema_instante(mv, latC_checar_numerico(mv, unixt));
  struct tm estructura;
  char res[MAX_STR_INTERN];
  sistema_calendario(mv, &instante, &estructura, 0);
  if (!strftime(res, sizeof(res), fmt, &estructura)) latC_apilar(mv, latO_nulo);
  else latC_apilar_string(mv, res);
}

static void proceso_al_activarse(int sig) { senial = sig; }

static void latSO_avisar(lat_mv *mv) {
  lat_objeto *a = latC_desapilar(mv);
  int numero = latN_entero(mv, latC_checar_numerico(mv, a),
                           "Numero de senial fuera de rango");
  if (signal(numero, proceso_al_activarse) == SIG_ERR)
    latC_error(mv, "No se pudo registrar la senial");
  if (!senial) {
    latC_apilar(mv, latO_falso);
    return;
  } else {
    latC_apilar(mv, latC_crear_numerico(mv, senial));
  };
}

static void latSO_salir(lat_mv *mv) { (void)mv; exit(0); }

static void latSO_cwd(lat_mv *mv) {
  char dir[1024];
  if (getcwd(dir, sizeof(dir)) != NULL) {
    latC_apilar_string(mv, dir);
  } else {
    latC_apilar(mv, latO_nulo);
  };
}

static void latSO_iraxy(lat_mv *mv) {
  lat_objeto *x = latC_desapilar(mv);
  lat_objeto *y = latC_desapilar(mv);
  int yx = latN_entero(mv, latC_checar_numerico(mv, y), "Coordenada fuera de rango");
  int xx = latN_entero(mv, latC_checar_numerico(mv, x), "Coordenada fuera de rango");
#ifdef _WIN32
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD pos = {yx, xx};
  SetConsoleCursorPosition(hConsole, pos);
  WriteConsole(hConsole, NULL, 0, NULL, NULL);
#else
  printf("%c[%d;%df", 0x1B, yx, xx);
#endif
  fflush(stdout);
}

static void latSO_usuario(lat_mv *mv) {
#ifdef _WIN32
  char *user = getenv("username");
#else
  char *user = getenv("USER");
#endif
  lat_objeto *tmp;
  if (user != NULL || user) {
    tmp = latC_crear_cadena(mv, user);
  } else {
    tmp = latO_nulo;
  }
  latC_apilar(mv, tmp);
}

static void latSO_operativo(lat_mv *mv) {
  lat_objeto *o = latC_desapilar(mv);
  lat_objeto *v = latO_falso;
  char *os = latC_checar_cadena(mv, o);
  char *ox = SISTEMAOPERATIVO;
  if (strcmp(ox, os) == 0) {
    v = latO_verdadero;
  }
  latC_apilar(mv, v);
}

void latOS_veriones(lat_mv *mv, int i) {
  unsigned long n = 0;
  Struct os_version;
  os_version = buscar_os_version();
  switch (i) {
  case 1:
    n = os_version.osMayor;
    break;
  case 2:
    n = os_version.osMenor;
    break;
  case 3:
    n = os_version.osBuild;
    break;
  default:
    latC_apilar(mv, latO_nulo);
    return;
  }
  latC_apilar_double(mv, (double)n);
}

static void latSO_ver_mayor(lat_mv *mv) { latOS_veriones(mv, 1); }

static void latSO_ver_menor(lat_mv *mv) { latOS_veriones(mv, 2); }

static void latSO_ver_build(lat_mv *mv) { latOS_veriones(mv, 3); }

/*
void latSO_fork(lat_mv *mv) {
    bool pid_ok = false;
    pid_t pid = fork();
    if (pid == 0) {
        pid_ok = true;
    } else if (pid > 0) {
        pid_ok = true;
    }
    if (!pid_ok) {
        latC_error(mv, "error en fork");
    } else {
        lat_objeto *datos = latC_crear_lista(mv, latL_crear(mv));
        latL_agregar(mv, latC_checar_lista(mv, datos),
                     latC_crear_numerico(mv, pid));
        latL_agregar(mv, latC_checar_lista(mv, datos),
                     latC_crear_numerico(mv, getpid()));
        latC_apilar(mv, datos);
    }
}
*/

static const lat_CReg libsistema[] = {{"dormir", latSO_dormir, 1},
                                      {"ejecutar", latSO_ejecutar, 1},
                                      {"pipe", latSO_pipe, 1},
                                      {"fecha", latSO_fecha, 1},
                                      {"salir", latSO_salir, 0},
                                      {"avisar", latSO_avisar, 1},
                                      {"cwd", latSO_cwd, 0},
                                      {"iraxy", latSO_iraxy, 2},
                                      {"tiempo", latSO_tiempo, 2},
                                      {"usuario", latSO_usuario, 0},
                                      {"operativo", latSO_operativo, 1},
                                      {"os", latSO_operativo, 1},
                                      {"os_version_mayor", latSO_ver_mayor, 0},
                                      {"os_version_menor", latSO_ver_menor, 0},
                                      {"os_version_parche", latSO_ver_build, 0},
                                      {NULL, NULL, 0}};

void latC_abrir_liblatino_syslib(lat_mv *mv) {
  latC_abrir_liblatino(mv, LIB_SISTEMA_NOMBRE, libsistema);
}
