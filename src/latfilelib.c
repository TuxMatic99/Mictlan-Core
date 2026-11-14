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

#include "latino.h"
#include "latgc.h"
#include "latmem.h"
#include "laterror.h"
#include "latdo.h"
#include <stdint.h>
#ifdef __linux__
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#define LIB_ARCHIVO_NAME "archivo"

/* Operaciones de texto: maximo 8 MiB por lectura, sin NUL embebido.
   Todos los errores de E/S cierran el descriptor antes de volver a Latino. */
#define LAT_FILE_MAX_TEXTO (8u * 1024u * 1024u)
/* estado: -1 error, 0 no existe, 1 lectura completa. */
static char *file_leer_texto_estado(const char *path, int *estado) {
    if (estado) *estado = -1;
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        if (estado && errno == ENOENT) *estado = 0;
        return NULL;
    }
    size_t usados = 0, capacidad = 4096;
    char *buf = malloc(capacidad + 1);
    if (!buf) { fclose(fp); return NULL; }
    for (;;) {
        errno = 0;
        size_t n = fread(buf + usados, 1, capacidad - usados, fp);
        if (memchr(buf + usados, '\0', n)) goto error;
        usados += n;
        if (ferror(fp)) {
            if (errno != EINTR) goto error;
            clearerr(fp);
        }
        if (feof(fp)) break;
        if (usados == capacidad) {
            if (capacidad == LAT_FILE_MAX_TEXTO) {
                int c;
                for (;;) {
                    errno = 0;
                    c = fgetc(fp);
                    if (c != EOF || !ferror(fp) || errno != EINTR) break;
                    clearerr(fp);
                }
                if (c != EOF || ferror(fp)) goto error;
                break;
            }
            size_t nueva = capacidad * 2;
            char *tmp = realloc(buf, nueva + 1);
            if (!tmp) goto error;
            buf = tmp;
            capacidad = nueva;
        }
    }
    if (fclose(fp) != 0) { free(buf); return NULL; }
    buf[usados] = '\0';
    if (estado) *estado = 1;
    return buf;
error:
    fclose(fp);
    free(buf);
    return NULL;
}

static char *file_leer_texto(const char *path) {
    return file_leer_texto_estado(path, NULL);
}

#ifdef __linux__
static int file_sincronizar(int fd) {
    int r;
    do { r = fsync(fd); } while (r < 0 && errno == EINTR);
    return r;
}
#endif

/* Linux, directorio privado/estable y sistema de archivos local.
   0: fallo antes del reemplazo; 1: reemplazo y fsync confirmados;
   2: reemplazado, pero no se confirmo el cierre/sync del directorio;
  -1: plataforma no implementada. No hay fallback con truncado. */
static int file_reemplazar_atomico(const char *path, const char *texto, FILE *origen) {
#ifdef __linux__
    /* Se consume origen incluso si la preparación falla. texto y origen son
       alternativas: el límite de texto no se aplica a una copia binaria. */
    int resultado = 0;
    if (!path || !*path || (!origen &&
        (!texto || strlen(texto) > LAT_FILE_MAX_TEXTO))) goto cerrar_origen;
    const char *barra = strrchr(path, '/');
    const char *nombre = barra ? barra + 1 : path;
    if (!*nombre || !strcmp(nombre, ".") || !strcmp(nombre, "..")) goto cerrar_origen;
    struct stat st;
    if (lstat(path, &st) == 0) {
        if (!S_ISREG(st.st_mode)) goto cerrar_origen;
        if (origen) {
            struct stat fuente;
            if (fstat(fileno(origen), &fuente) != 0 ||
                (fuente.st_dev == st.st_dev && fuente.st_ino == st.st_ino))
                goto cerrar_origen;
        }
    } else if (errno != ENOENT) goto cerrar_origen;

    size_t ndir = barra ? (size_t)(barra - path) : 1;
    if (barra == path) ndir = 1;
    static const char sufijo[] = "/.latino-XXXXXX";
    if (ndir > SIZE_MAX - sizeof(sufijo)) goto cerrar_origen;
    char *directorio = malloc(ndir + 1);
    char *temporal = malloc(ndir + sizeof(sufijo));
    if (!directorio || !temporal) {
        free(directorio); free(temporal); goto cerrar_origen;
    }
    if (barra) memcpy(directorio, path, ndir);
    else directorio[0] = '.';
    directorio[ndir] = '\0';
    memcpy(temporal, directorio, ndir);
    memcpy(temporal + ndir, sufijo, sizeof(sufijo));
    int dirfd = open(directorio, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    int fd = -1;
    bool creado = false;
    if (dirfd < 0) goto salir;
    fd = mkostemp(temporal, O_CLOEXEC); /* exclusivo, permisos 0600 */
    if (fd < 0) goto salir;
    creado = true;
    if (origen) {
        unsigned char bloque[16384];
        for (;;) {
            errno = 0;
            size_t n = fread(bloque, 1, sizeof bloque, origen);
            int error_lectura = ferror(origen), causa = errno;
            size_t usados = 0;
            while (usados < n) {
                ssize_t escritos = write(fd, bloque + usados, n - usados);
                if (escritos < 0 && errno == EINTR) continue;
                if (escritos <= 0) goto salir;
                usados += (size_t)escritos;
            }
            if (error_lectura) {
                if (causa != EINTR) goto salir;
                clearerr(origen);
            } else if (feof(origen)) break;
            else if (!n) goto salir; /* no progreso, sin EOF ni EINTR */
        }
        int cierre_origen = fclose(origen);
        origen = NULL;
        if (cierre_origen != 0) goto salir;
    } else {
        size_t n = strlen(texto), usados = 0;
        while (usados < n) {
            ssize_t escritos = write(fd, texto + usados, n - usados);
            if (escritos < 0 && errno == EINTR) continue;
            if (escritos <= 0) goto salir;
            usados += (size_t)escritos;
        }
    }
    if (file_sincronizar(fd) < 0) goto salir;
    int cierre = close(fd);
    fd = -1; /* en Linux no reintentar close tras un error */
    if (cierre < 0) goto salir;
    if (rename(temporal, path) < 0) goto salir;
    creado = false;
    resultado = file_sincronizar(dirfd) == 0 ? 1 : 2;
salir:
    if (fd >= 0) (void)close(fd);
    if (creado) (void)unlink(temporal);
    if (dirfd >= 0 && close(dirfd) < 0 && resultado == 1) resultado = 2;
    free(temporal);
    free(directorio);
cerrar_origen:
    if (origen) (void)fclose(origen);
    return resultado;
#else
    (void)path; (void)texto;
    if (origen) (void)fclose(origen);
    return -1;
#endif
}

/* Adaptador histórico: un único motor de reemplazo para texto y copia. */
static int file_guardar_atomico(const char *path, const char *texto) {
    return file_reemplazar_atomico(path, texto, NULL);
}

/* API C compartida con los consumidores; conserva el contrato de Entrega11. El llamador libera con free() la lectura completa. */
LATINO_API char *latF_leer_texto_seguro(const char *path, int *estado) {
    if (estado) *estado = -1;
    if (!path || !*path) return NULL;
    return file_leer_texto_estado(path, estado);
}

LATINO_API int latF_escribir_atomico(const char *path, const char *texto) {
    return file_guardar_atomico(path, texto);
}

/* El guard y los datos pertenecen al llamador, fuera del marco de setjmp:
   siguen definidos aunque una operación lance longjmp. No se modifica la VM
   ni se registra una segunda función nativa para realizar la limpieza. */
static int file_proteger(lat_mv *mv, struct lat_longjmp *guard,
                         void (*accion)(lat_mv *, void *), void *datos) {
    guard->previo = mv->error;
    guard->status = 0;
    mv->error = guard;
    LAT_TRY(mv, guard, accion(mv, datos););
    mv->error = guard->previo;
    return guard->status;
}

static void file_publicar_texto(lat_mv *mv, void *buf) {
    latC_apilar_string(mv, buf);
}

static void file_entregar_texto(lat_mv *mv, char *buf) {
    struct lat_longjmp guard;
    int estado = file_proteger(mv, &guard, file_publicar_texto, buf);
    free(buf);
    if (estado) latD_lanzar(mv, estado);
}

static void file_leer_texto_seguro(lat_mv *mv) {
    const char *path = latC_checar_cadena(mv, latC_desapilar(mv));
    int estado;
    char *buf = latF_leer_texto_seguro(path, &estado);
    if (buf) file_entregar_texto(mv, buf);
    else latC_apilar(mv, estado == 0 ? latO_nulo : latO_falso);
}

static void file_escribir_atomico(lat_mv *mv) {
    lat_objeto *valor = latC_desapilar(mv);
    lat_objeto *ruta = latC_desapilar(mv);
    const char *path = latC_checar_cadena(mv, ruta);
    const char *texto = latC_checar_cadena(mv, valor);
    int resultado = latF_escribir_atomico(path, texto);
    if (resultado == 2)
        fprintf(stderr, "Latino: '%s' reemplazado; no se confirmo la sincronizacion/cierre del directorio.\n", path);
    latC_apilar_double(mv, (double)resultado);
}

static bool file_escribir_texto(const char *path, const char *texto,
                                const char *modo) {
    FILE *fp = fopen(path, modo);
    if (!fp) return false;
    size_t n = strlen(texto);
    size_t usados = 0;
    bool ok = true;
    while (usados < n) {
        errno = 0;
        size_t escritos = fwrite(texto + usados, 1, n - usados, fp);
        usados += escritos;
        if (ferror(fp)) {
            if (errno == EINTR) { clearerr(fp); continue; }
            ok = false;
            break;
        }
        if (!escritos) { ok = false; break; }
    }
    /* fclose tambien puede informar un error de escritura diferida. */
    if (fclose(fp) != 0) ok = false;
    return ok;
}

static void file_publicar_lineas(lat_mv *mv, void *datos) {
    char *buf = datos;
    lista *lst = latL_crear(mv);
    char *p = buf;
    while (*p) {
        char *fin = strchr(p, '\n');
        if (fin) *fin = '\0';
        latL_agregar(mv, lst, latC_crear_cadena(mv, p));
        if (!fin) break;
        p = fin + 1;
    }
    lat_objeto *lineas = latC_crear_lista(mv, lst);
    latC_apilar(mv, lineas);
#if !HABILITAR_GC
    latM_liberar(mv, lineas);
#endif
}

static void file_lineas(lat_mv *mv) {
    const char *path = latC_checar_cadena(mv, latC_desapilar(mv));
    char *buf = file_leer_texto(path);
    if (!buf) {
        latC_error(mv, "No se pudo leer el archivo '%s'", path);
        return;
    }
    struct lat_longjmp guard;
    int estado = file_proteger(mv, &guard, file_publicar_lineas, buf);
    /* Los objetos/listas parciales pertenecen al registro de la VM. Sólo
       este buffer crudo pertenece a la biblioteca de archivos. */
    free(buf);
    if (estado) latD_lanzar(mv, estado);
}

static void file_leer(lat_mv *mv) {
    const char *path = latC_checar_cadena(mv, latC_desapilar(mv));
    char *buf = file_leer_texto(path);
    if (!buf) { latC_apilar(mv, latO_falso); return; }
    file_entregar_texto(mv, buf);
}

/* API optativa: los fallos operativos devuelven falso, sin longjmp.
   Los errores de tipos siguen siendo errores de programacion. */
static void file_intentar_escribir(lat_mv *mv) {
    lat_objeto *s = latC_desapilar(mv);
    lat_objeto *o = latC_desapilar(mv);
    const char *path = latC_checar_cadena(mv, o);
    const char *cad = latC_checar_cadena(mv, s);
    latC_apilar(mv, file_escribir_texto(path, cad, "wb") ? latO_verdadero : latO_falso);
}

static void file_escribir(lat_mv *mv) {
    lat_objeto *s = latC_desapilar(mv);
    lat_objeto *o = latC_desapilar(mv);
    const char *path = latC_checar_cadena(mv, o);
    const char *cad = latC_checar_cadena(mv, s);
    if (!file_escribir_texto(path, cad, "w"))
        latC_error(mv, "No se pudo escribir el archivo '%s'", path);
}

struct file_programa {
    ast *nodo;
    lat_objeto *func;
    int estado;
};

static void file_correr_programa(lat_mv *mv, void *datos) {
    struct file_programa *p = datos;
    p->func = latC_analizar(mv, p->nodo);
    p->estado = latC_llamar_funcion(mv, p->func);
}

static void file_ejecutar(lat_mv *mv) {
    char *input = latC_checar_cadena(mv, latC_desapilar(mv));
    const char *dot = strrchr(input, '.');
    if (!dot || (strcmp(dot, ".lat") && strcmp(dot, ".mict"))) return;
    int estado;
    ast *nodo = latA_analizar_arch(input, &estado);
    if (!nodo || estado != 0) {
        if (nodo) latA_destruir(nodo);
        latC_error(mv, "Error al leer el archivo: '%s'", input);
        return;
    }
    char *archivo_previo = mv->nombre_archivo;
    bool repl_previo = mv->global->REPL;
    int linea_previa = mv->nlin, columna_previa = mv->ncol;
    mv->global->REPL = false;
    mv->nombre_archivo = input;
    struct file_programa p = {nodo, NULL, 0};
    struct lat_longjmp guard;
    estado = file_proteger(mv, &guard, file_correr_programa, &p);
    if (p.func) latO_destruir(mv, p.func);
    latA_destruir(nodo);
    mv->nombre_archivo = archivo_previo;
    mv->global->REPL = repl_previo;
    mv->nlin = linea_previa;
    mv->ncol = columna_previa;
    if (estado || p.estado) latD_lanzar(mv, estado ? estado : p.estado);
}

static void file_duplicar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    const char *origen = latC_checar_cadena(mv, a);
    const char *destino = latC_checar_cadena(mv, b);
    if (!strcmp(origen, destino)) {
        latC_error(mv, "Error al duplicar archivo '%s': origen y destino deben ser distintos", origen);
        return;
    }
#ifdef __linux__
    struct stat st;
    /* En el contrato de rutas estables, rechazar FIFO/dispositivo antes de
       fopen evita esperar a un escritor. Volver a comprobar el descriptor. */
    if (stat(origen, &st) != 0) {
        latC_error(mv, "No se pudo abrir el archivo '%s'", origen);
        return;
    }
    if (!S_ISREG(st.st_mode)) {
        latC_error(mv, "No se pudo copiar el archivo regular '%s'", origen);
        return;
    }
    FILE *fp = fopen(origen, "rb");
    if (!fp) {
        latC_error(mv, "No se pudo abrir el archivo '%s'", origen);
        return;
    }
    if (fstat(fileno(fp), &st) != 0 || !S_ISREG(st.st_mode)) {
        (void)fclose(fp);
        latC_error(mv, "No se pudo copiar el archivo regular '%s'", origen);
        return;
    }
    /* El motor consume y cierra fp, incluso si no puede preparar el destino. */
    int resultado = file_reemplazar_atomico(destino, NULL, fp);
    if (resultado == 0)
        latC_error(mv, "No se pudo duplicar '%s' en '%s'; destino sin reemplazar", origen, destino);
    else if (resultado == 2)
        fprintf(stderr, "Latino: '%s' reemplazado; no se confirmo la sincronizacion/cierre del directorio.\n", destino);
#else
    latC_error(mv, "Copia protegida no implementada en esta plataforma");
#endif
}

static void file_eliminar(lat_mv *mv) {
    int status;
    lat_objeto *a = latC_desapilar(mv);
    status = remove(latC_checar_cadena(mv, a));
    if (status == 0) {
        latC_apilar(mv, latO_verdadero);
    } else {
        latC_apilar(mv, latO_falso);
    }
}

static void file_crear(lat_mv *mv) {
    const char *path = latC_checar_cadena(mv, latC_desapilar(mv));
    FILE *fp = fopen(path, "rb");
    if (fp) { fclose(fp); latC_apilar(mv, latO_falso); return; }
    /* C11: "x" evita truncar un archivo creado entre ambas aperturas. */
    fp = fopen(path, "wbx");
    bool ok = fp != NULL;
    if (fp && fclose(fp) != 0) ok = false;
    latC_apilar(mv, ok ? latO_verdadero : latO_falso);
}

static void file_renombrar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    const char *origen = latC_checar_cadena(mv, a);
    const char *nuevo = latC_checar_cadena(mv, b);
    if (rename(origen, nuevo) == 0) latC_apilar_string(mv, nuevo);
    else latC_apilar(mv, latO_falso);
}

static void file_anexar(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    const char *path = latC_checar_cadena(mv, a);
    const char *texto = latC_checar_cadena(mv, b);
    if (!file_escribir_texto(path, texto, "a"))
        latC_error(mv, "No se pudo anexar al archivo '%s'", path);
}

static const lat_CReg libfile[] = {{"leer", file_leer, 1},
                                   {"leer_texto_seguro", file_leer_texto_seguro, 1},
                                   {"escribir_atomico", file_escribir_atomico, 2},
                                   {"intentar_leer", file_leer, 1},
                                   {"intentar_escribir", file_intentar_escribir, 2},
                                   {"lineas", file_lineas, 1},
                                   {"ejecutar", file_ejecutar, 1},
                                   {"escribir", file_escribir, 2},
                                   {"duplicar", file_duplicar, 2},
                                   {"anexar", file_anexar, 2},
                                   {"eliminar", file_eliminar, 1},
                                   {"borrar", file_eliminar, 1},
                                   {"crear", file_crear, 1},
                                   {"renombrar", file_renombrar, 2},
    {NULL, NULL, 0}};

void latC_abrir_liblatino_filelib(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_ARCHIVO_NAME, libfile);
}
//agregar funcion de elegiar donde escribir
// {"escribir", file_escribir, 3},
// archivo.escribir("archivo1.lat", "a�adir", 10)
//https://medium.com/10-goto-10/manejo-de-archivos-en-c-19f85b31ddd
