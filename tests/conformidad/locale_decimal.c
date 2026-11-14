/* El punto decimal del lenguaje es independiente del locale del anfitrion. */
#include "latino.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *nombre = argc > 1 ? argv[1] : "C";
    if (!setlocale(LC_NUMERIC, nombre)) {
        fprintf(stderr, "Locale no disponible: %s\n", nombre);
        return 77;
    }
    char *anterior = strdup(setlocale(LC_NUMERIC, NULL));
    if (!anterior) return EXIT_FAILURE;
    char fuente[] =
        "decimal_a = 5.25\n"
        "decimal_b = .25\n"
        "decimal_c = 5.25e1\n"
        "decimal_d = 525e-2\n"
        "decimal_e = 5.25 + .75\n";
    static const struct { const char *nombre; double esperado; } casos[] = {
        {"decimal_a", 21.0 / 4.0}, {"decimal_b", 1.0 / 4.0},
        {"decimal_c", 105.0 / 2.0}, {"decimal_d", 21.0 / 4.0},
        {"decimal_e", 6.0}
    };
    lat_mv *mv = latC_crear_mv();
    if (!mv) { free(anterior); return EXIT_FAILURE; }
    mv->nombre_archivo = "locale_decimal.mict";
    int estado = -1, fallos = 0;
    ast *arbol = latA_analizar_exp(fuente, &estado);
    if (!arbol || estado) {
        fprintf(stderr, "Fallo del parser en locale %s\n", nombre);
        if (arbol) latA_destruir(arbol);
        latC_destruir_mv(mv);
        free(anterior);
        return EXIT_FAILURE;
    }
    lat_objeto *programa = latC_analizar(mv, arbol);
    if (!programa || latC_llamar_funcion(mv, programa)) {
        fprintf(stderr, "Fallo al ejecutar en locale %s\n", nombre);
        fallos++;
    } else {
        for (size_t i = 0; i < sizeof casos / sizeof casos[0]; ++i) {
            lat_objeto *valor = latO_obtener_contexto(mv, mv->contexto[0], casos[i].nombre);
            if (!valor || valor->tipo != T_NUMERIC || getNumerico(valor) != casos[i].esperado) {
                fprintf(stderr, "%s incorrecto en locale %s\n", casos[i].nombre, nombre);
                fallos++;
            }
        }
    }
    if (strcmp(anterior, setlocale(LC_NUMERIC, NULL))) {
        fprintf(stderr, "El parser cambio el locale del anfitrion\n");
        fallos++;
    }
    if (mv->ptrpila || mv->ptrctx || mv->numejec) fallos++;
    latA_destruir(arbol);
    latC_destruir_mv(mv);
    free(anterior);
    return fallos ? EXIT_FAILURE : EXIT_SUCCESS;
}
