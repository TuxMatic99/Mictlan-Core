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
#include "latast.h"
#include "latmem.h"
#include "latmv.h"
#include "latrepl.h"
#include "mictlan_version.h"

/*
Para depurar en visual studio:
Menu propiedades del proyecto-> Debugging -> Command Arguments. Agregar
$(SolutionDir)..\ejemplos\debug.lat
----------------------------------
Para depurar en Netbeans ir a propiedades del proyecto -> Run command y Agregar
"${OUTPUT_PATH}" ejemplos/debug.lat
 */

#if YYDEBUG
extern int yydebug = 1; /* 1 para debuguear analizador lexico/sintactico */
#endif

int parse_silent = 0;
char *filename = NULL;

static void mostrar_ayuda(void) {
    puts(MICTLAN_IDENTIDAD "\n"
         "Lenguaje de programación en español derivado de Latino (MIT).\n"
         "\n"
         "Uso: mictlan [--] archivo.mict [argumentos...]\n"
         "     mictlan -e 'código'\n"
         "     mictlan\n"
         "\n"
         ".mict es la extensión principal. También se admite .lat.\n"
         "Sin argumentos abre la consola interactiva. Ctrl+D finaliza la entrada.\n"
         "-h, -a, --help, --ayuda   Mostrar esta ayuda\n"
         "-v, --version            Mostrar la versión\n"
         "LATINO_LIB               Directorio de bibliotecas de código fuente");
}

int main(int argc, char *argv[]) {
    #if (defined __WIN32__) || (defined _WIN32)
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    #endif
    if (!setlocale(LC_ALL, "C.UTF-8")) {
        if (!setlocale(LC_ALL, "C")) {
            setlocale(LC_ALL, "");
        }
    }
    if (!setlocale(LC_CTYPE, "C.UTF-8")) {
        if (!setlocale(LC_CTYPE, "C")) {
            setlocale(LC_CTYPE, "");
        }
    }
    setlocale(LC_NUMERIC, "C");
    const char *opcion = argc > 1 ? argv[1] : "";
    bool expresion = !strcmp(opcion, "-e");
    int archivo = !strcmp(opcion, "--") ? 2 : 1;
    int codigo_salida = EXIT_SUCCESS;
    if (!strcmp(opcion, "-v") || !strcmp(opcion, "--version")) {
        puts(MICTLAN_IDENTIDAD);
        return EXIT_SUCCESS;
    }
    if (!strcmp(opcion, "-h") || !strcmp(opcion, "-a") ||
        !strcmp(opcion, "--ayuda") || !strcmp(opcion, "--help")) {
        mostrar_ayuda();
        return EXIT_SUCCESS;
    }
    if (expresion) {
        if (argc != 3) {
            fputs("mictlan: -e requiere una cadena de código. Consulta --help.\n", stderr);
            return EXIT_FAILURE;
        }
    } else if (archivo == 2 && argc == 2) {
        fputs("mictlan: falta el archivo después de --. Consulta --help.\n", stderr);
        return EXIT_FAILURE;
    } else if (archivo == 1 && opcion[0] == '-') {
        fprintf(stderr, "mictlan: opción desconocida '%s'. Consulta --help.\n", opcion);
        return EXIT_FAILURE;
    }
    lat_mv *mv = latC_crear_mv();
    if (!mv) {
        fputs("No se pudo crear la VM.\n", stderr);
        return EXIT_FAILURE;
    }
    if (expresion || argc > 1) {
        mv->nombre_archivo = expresion ? "<-e>" : argv[archivo];
        mv->global->REPL = false;
        if (!expresion) {
            mv->global->argc = argc - archivo;
            for (int j = archivo; j < argc; j++) {
                latL_agregar(mv, latC_checar_lista(mv, mv->global->argv),
                             latC_crear_cadena(mv, argv[j]));
            }
        }
        int status = EXIT_FAILURE;
        ast *nodo;
        if (expresion) {
            filename = mv->nombre_archivo;
            nodo = latA_analizar_exp(argv[2], &status);
            if (status == 0 && nodo == NULL)
                fputs("mictlan: <-e>: la entrada no contiene instrucciones.\n", stderr);
        } else {
            nodo = latA_analizar_arch(argv[archivo], &status);
        }
        if (status == 0 && nodo != NULL) {
            lat_objeto *main_func = latC_analizar(mv, nodo);
            if (!expresion && mv->global->menu) {
                // inicio instrucciones para llamar a menu
                lat_funcion *fval = (lat_funcion *)main_func->val.gc;
                lat_bytecode *codigo = (lat_bytecode *)fval->codigo;
                int ninst = main_func->ninst;
                lat_bytecode *bc = (lat_bytecode *)latM_asignar(
                    mv, sizeof(lat_bytecode) * (ninst + 2));
                memcpy(bc, codigo, sizeof(*bc) * (ninst - 1));
#if DEPURAR_MEM
                printf("main.bc: %p\n", bc);
#endif
                bc[ninst - 1] = latMV_bytecode_crear(
                    LOAD_NAME, 0, 0, latO_retener_constante(mv, latC_crear_cadena(mv, "menu")), 0, 0,
                    mv->nombre_archivo);
                bc[ninst] = latMV_bytecode_crear(CALL_FUNCTION, 2, 0, NULL, 0,
                                                 0, mv->nombre_archivo);
                bc[ninst + 1] = latMV_bytecode_crear(HALT, 0, 0, NULL, 0, 0,
                                                     mv->nombre_archivo);
                lat_objeto *num = latC_crear_numerico(mv, mv->global->argc);
                latC_apilar(mv, num);
                latC_apilar(mv, mv->global->argv);
                // fin instrucciones
                lat_objeto *new_main = latC_crear_funcion(mv, bc, ninst + 2);
                new_main->es_vararg = 0;
                new_main->nparams = 2;
                /* debug eliminado: Ejecutando menú */
                status = latC_llamar_funcion(mv, new_main);
                /* debug eliminado: Menú finalizado */
                latO_destruir(mv, new_main);
            } else {
                /* debug eliminado: Ejecutando función principal */
                status = latC_llamar_funcion(mv, main_func);
                /* debug eliminado: Ejecución finalizada */
            }
            latO_destruir(mv, main_func);
        }


        if (status != 0 || nodo == NULL) {
            codigo_salida = EXIT_FAILURE;
        }

        latA_destruir(nodo);
    } else {
        puts(MICTLAN_IDENTIDAD " · derivado de Latino (MIT)");
        latR_REPL(mv);
    }
    latC_destruir_mv(mv);
    return codigo_salida;

}
