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
#include "latpropiedad.h"
#include "latnumerico.h"
#include <stdint.h>

#undef PI
#define PI 3.141592653589793

#undef TAU
#define TAU 6.283185307179586

#undef Eu
#define Eu 2.718281828459045


#define LIB_MATE_NAME "mate"

static void mate_abs(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, fabs(latC_checar_numerico(mv, a)));
}

static void mate_acos(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, acos(latC_checar_numerico(mv, a)));
}

static void mate_acosh(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, acosh(latC_checar_numerico(mv, a)));
}

static void mate_asin(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, asin(latC_checar_numerico(mv, a)));
}

static void mate_asinh(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, asinh(latC_checar_numerico(mv, a)));
}

static void mate_atan(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, atan(latC_checar_numerico(mv, a)));
}

static void mate_atanh(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, atanh(latC_checar_numerico(mv, a)));
}

static void mate_atan2(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, atan2(latC_checar_numerico(mv, a), latC_checar_numerico(mv, b)));
}

static void mate_cos(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, cos(latC_checar_numerico(mv, a)));
}

static void mate_cosh(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, cosh(latC_checar_numerico(mv, a)));
}

static void mate_cbrt(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, cbrt(latC_checar_numerico(mv, a)));
}

static void mate_sin(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, sin(latC_checar_numerico(mv, a)));
}

static void mate_sinh(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, sinh(latC_checar_numerico(mv, a)));
}

static void mate_tan(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, tan(latC_checar_numerico(mv, a)));
}

static void mate_tanh(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, tanh(latC_checar_numerico(mv, a)));
}

static void mate_exp(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, exp(latC_checar_numerico(mv, a)));
}

static void mate_log(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, log(latC_checar_numerico(mv, a)));
}

static void mate_log10(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, log10(latC_checar_numerico(mv, a)));
}

static void mate_sqrt(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, sqrt(latC_checar_numerico(mv, a)));
}

static void mate_ceil(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, ceil(latC_checar_numerico(mv, a)));
}

static void mate_floor(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, floor(latC_checar_numerico(mv, a)));
}

static void mate_pow(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, pow(latC_checar_numerico(mv, a), latC_checar_numerico(mv, b)));
}

static void mate_frexp(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    (void)latC_checar_numerico(mv, b); /* Argumento historico ignorado. */
    int e;
    latC_apilar_double(mv, frexp(latC_checar_numerico(mv, a), &e));
}

static void mate_ldexp(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    int e = latN_entero(mv, latC_checar_numerico(mv, b),
                       "Exponente fuera de rango");
    latC_apilar_double(mv, ldexp(latC_checar_numerico(mv, a), e));
}

/* Consumir todo el marco antes de validar tipos: un error no deja argumentos
   sobre el prefijo del llamador C. Los descriptores siguen en celdas prestadas
   hasta publicar el resultado; no hay reservas ni llamadas que los sobrescriban. */
static lat_objeto *mate_argumentos(lat_mv *mv, int *cantidad) {
    double n = latC_checar_numerico(mv, latC_desapilar(mv));
    if (!isfinite(n) || n < 0 || n > mv->ptrpila || n != trunc(n))
        latC_error(mv, "Numero invalido de argumentos");
    *cantidad = (int)n;
    lat_objeto *args = mv->tope - *cantidad;
    for (int i = 0; i < *cantidad; ++i) (void)latC_desapilar(mv);
    return args;
}

static void mate_random(lat_mv *mv) {
    int cantidad;
    lat_objeto *args = mate_argumentos(mv, &cantidad);
    if (cantidad > 2) latC_error(mv, "Numero invalido de argumentos");
    int64_t minimo = 0, maximo = 1;
    if (cantidad) {
        maximo = latN_entero(mv, latC_checar_numerico(mv, args),
                            "Limite aleatorio fuera de rango");
        if (cantidad == 1 && maximo < 0)
            latC_error(mv, "Limite aleatorio fuera de rango");
        if (cantidad == 2) {
            minimo = latN_entero(mv, latC_checar_numerico(mv, args + 1),
                                "Limite aleatorio fuera de rango");
            if (minimo > maximo) {
                int64_t temporal = minimo; minimo = maximo; maximo = temporal;
            }
        }
    }
    uint64_t amplitud = (uint64_t)(maximo - minimo) + 1;
    /* Conservar rand y su secuencia/sesgo historicos. Para uniformidad y
       reproducibilidad entre plataformas usar aleatorio_entero. */
    int64_t resultado = minimo + (int64_t)((uint64_t)rand() % amplitud);
    latC_apilar_double(mv, (double)resultado);
}

static void mate_pi(lat_mv *mv) {
    latC_apilar_double(mv, PI);
}

static void mate_tau(lat_mv *mv) {
    latC_apilar_double(mv, TAU);
}

static void mate_e(lat_mv *mv) {
    latC_apilar_double(mv, Eu);
}

static void mate_base(lat_mv *mv) {
    lat_objeto *ob = latC_desapilar(mv);
    lat_objeto *oa = latC_desapilar(mv);
    double a = latC_checar_numerico(mv, oa), b = latC_checar_numerico(mv, ob);
    latC_apilar_double(mv, a / (b / 100));
}

static void mate_parte(lat_mv *mv) {
    lat_objeto *ob = latC_desapilar(mv);
    lat_objeto *oa = latC_desapilar(mv);
    double a = latC_checar_numerico(mv, oa), b = latC_checar_numerico(mv, ob);
    latC_apilar_double(mv, (a / 100) * b);
}

static void mate_porc(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv);
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, (latC_checar_numerico(mv, a) / latC_checar_numerico(mv, b)));
}

static void mate_trunc(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, trunc(latC_checar_numerico(mv, a)));
}

static void mate_round(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, round(latC_checar_numerico(mv, a)));
}

static void mate_extremo(lat_mv *mv, bool maximo) {
    int cantidad;
    lat_objeto *args = mate_argumentos(mv, &cantidad);
    if (!cantidad) latC_error(mv, "Numero invalido de argumentos");
    double valor = NAN;
    for (int i = 0; i < cantidad; ++i) {
        double x = latC_checar_numerico(mv, args + i);
        if (!isnan(x) && (isnan(valor) || (maximo ? x > valor : x < valor)))
            valor = x;
    }
    latC_apilar_double(mv, valor);
}
static void mate_max(lat_mv *mv) { mate_extremo(mv, true); }
static void mate_min(lat_mv *mv) { mate_extremo(mv, false); }

static double mate_finito(lat_mv *mv, lat_objeto *o) {
    double x = latC_checar_numerico(mv, o);
    if (!isfinite(x)) latC_error(mv, "Se requiere un numero finito");
    return x;
}
static void mate_descomponer_binario(lat_mv *mv) {
    double x = mate_finito(mv, latC_desapilar(mv));
    int exponente = 0;
    double fraccion = frexp(x, &exponente);
    lista *partes = latL_crear(mv);
    latL_agregar(mv, partes, latC_crear_numerico(mv, fraccion));
    latL_agregar(mv, partes, latC_crear_numerico(mv, exponente));
    latC_apilar(mv, latC_crear_lista(mv, partes));
}
static void mate_es_finito(lat_mv *mv) {
    double x = latC_checar_numerico(mv, latC_desapilar(mv));
    latC_apilar(mv, isfinite(x) ? latO_verdadero : latO_falso);
}
static void mate_es_infinito(lat_mv *mv) {
    double x = latC_checar_numerico(mv, latC_desapilar(mv));
    latC_apilar(mv, isinf(x) ? latO_verdadero : latO_falso);
}
static void mate_es_nan(lat_mv *mv) {
    double x = latC_checar_numerico(mv, latC_desapilar(mv));
    latC_apilar(mv, isnan(x) ? latO_verdadero : latO_falso);
}
#define MATE_ENTERO_EXACTO 9007199254740991.0
static int64_t mate_entero_exacto(lat_mv *mv, lat_objeto *o, double minimo) {
    double x = latC_checar_numerico(mv, o);
    if (!isfinite(x) || x < minimo || x > MATE_ENTERO_EXACTO || x != trunc(x))
        latC_error(mv, "Se requiere un entero exacto dentro del rango permitido");
    return (int64_t)x;
}
static void mate_semilla_aleatoria(lat_mv *mv) {
    uint64_t s = (uint64_t)mate_entero_exacto(mv, latC_desapilar(mv), 0);
    latP_semilla_aleatoria(mv, s);
    latC_apilar(mv, latO_nulo);
}
static void mate_aleatorio_entero(lat_mv *mv) {
    lat_objeto *ob = latC_desapilar(mv), *oa = latC_desapilar(mv);
    int64_t a = mate_entero_exacto(mv, oa, -MATE_ENTERO_EXACTO);
    int64_t b = mate_entero_exacto(mv, ob, -MATE_ENTERO_EXACTO);
    if (a > b) latC_error(mv, "Intervalo aleatorio invertido");
    uint64_t amplitud = (uint64_t)(b - a) + 1;
    /* 2^64 mod amplitud; se descarta el prefijo incompleto. El intervalo
       restante contiene exactamente el mismo numero de residuos de cada clase. */
    uint64_t umbral = (uint64_t)(-amplitud) % amplitud, x;
    do { x = latP_aleatorio64(mv); } while (x < umbral);
    latC_apilar_double(mv, (double)(a + (int64_t)(x % amplitud)));
}
static void mate_raiz_segura(lat_mv *mv) {
    double x = mate_finito(mv, latC_desapilar(mv));
    if (x < 0) latC_error(mv, "Dominio invalido para raiz_segura");
    latC_apilar_double(mv, sqrt(x));
}
static void mate_log_seguro(lat_mv *mv) {
    double x = mate_finito(mv, latC_desapilar(mv));
    if (x <= 0) latC_error(mv, "Dominio invalido para log_seguro");
    latC_apilar_double(mv, log(x));
}
static void mate_potencia_segura(lat_mv *mv) {
    lat_objeto *ob = latC_desapilar(mv), *oa = latC_desapilar(mv);
    double a = mate_finito(mv, oa), b = mate_finito(mv, ob);
    if ((a < 0 && b != trunc(b)) || (a == 0 && b <= 0))
        latC_error(mv, "Dominio invalido para potencia_segura");
    double r = pow(a, b);
    if (!isfinite(r) || (r == 0 && a != 0))
        latC_error(mv, "Resultado fuera de rango");
    latC_apilar_double(mv, r);
}

static const lat_CReg libmate_[] = {
    {"abs", mate_abs, 1},
    {"acos", mate_acos, 1},
    {"acosh", mate_acosh, 1},
    {"asen", mate_asin, 1},
    {"asenh", mate_asinh, 1},
    {"atan", mate_atan, 1},
    {"atanh", mate_atanh, 1},
    {"atan2", mate_atan2, 2},
    {"cos", mate_cos, 1},
    {"cosh", mate_cosh, 1},
    {"sen", mate_sin, 1},
    {"senh", mate_sinh, 1},
    {"tan", mate_tan, 1},
    {"tanh", mate_tanh, 1},
    {"exp", mate_exp, 1},
    {"log", mate_log, 1},
    {"log10", mate_log10, 1},
    {"max", mate_max, FUNCION_VAR_ARGS},
    {"min", mate_min, FUNCION_VAR_ARGS},
    {"raiz", mate_sqrt, 1},
    {"raizc", mate_cbrt, 1},
    {"techo", mate_ceil, 1},
    {"piso", mate_floor, 1},
    {"pot", mate_pow, 2},
    {"frexp", mate_frexp, 2},
    {"ldexp", mate_ldexp, 2},
    {"trunc", mate_trunc, 1},
    {"aleatorio", mate_random, FUNCION_VAR_ARGS},
    {"alt", mate_random, FUNCION_VAR_ARGS},
    {"pi", mate_pi, 0},
    {"tau", mate_tau, 0},
    {"e", mate_e, 0},
    {"redondear", mate_round, 1},
    {"rnd", mate_round, 1},
    {"base", mate_base, 2},
    {"parte", mate_parte, 2},
    {"porciento", mate_porc, 2},
    {"porcentaje", mate_porc, 2},
    {"porc", mate_porc, 2},
    {"descomponer_binario", mate_descomponer_binario, 1},
    {"es_finito", mate_es_finito, 1},
    {"es_infinito", mate_es_infinito, 1},
    {"es_nan", mate_es_nan, 1},
    {"semilla_aleatoria", mate_semilla_aleatoria, 1},
    {"aleatorio_entero", mate_aleatorio_entero, 2},
    {"raiz_segura", mate_raiz_segura, 1},
    {"log_seguro", mate_log_seguro, 1},
    {"potencia_segura", mate_potencia_segura, 2},
    {NULL, NULL, 0}};

void latC_abrir_liblatino_mathlib(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_MATE_NAME, libmate_);
}
