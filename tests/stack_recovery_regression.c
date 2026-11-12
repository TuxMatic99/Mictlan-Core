/* P0.2-E: VM, compilador, FFI y longjmp reales; sin sustituciones de Core.
 * Cada caso reutiliza una VM. Las comprobaciones siguen activas con NDEBUG.
 */
#include "latino.h"
#include "latast.h"
#include "latobj.h"
#include "latlist.h"
#include "latdic.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(LAT_ERRRUN == 2, "LAT_ERRRUN debe ser el estado interno 2");

typedef struct { ast *tree; lat_objeto *function; } Program;
typedef struct {
    int stack, context, executions, previous, returns;
    lat_objeto *context_object;
    struct lat_longjmp *handler;
} Frame;

static unsigned failures, protected_calls;
static Program nested_error;
static lat_objeto *valid_function;
static lat_objeto *list_owner, *dict_owner;

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "FAIL linea %d: %s\n", __LINE__, #test); ++failures; \
} } while (0)

static Frame capture(lat_mv *mv)
{
    Frame frame = {mv->ptrpila, mv->ptrctx, mv->numejec, mv->ptrprevio,
                   mv->prev_args, mv->contexto_actual, mv->error};
    return frame;
}

static int call(lat_mv *mv, lat_objeto *function)
{
    ++protected_calls;
    return latC_llamar_funcion(mv, function);
}

static void check_frame(lat_mv *mv, Frame frame, int stack, int failed_call)
{
    CHECK(mv->ptrpila == stack);
    CHECK(mv->tope == mv->pila + mv->ptrpila);
    CHECK(mv->ptrctx == frame.context);
    CHECK(mv->contexto_actual == frame.context_object);
    CHECK(mv->numejec == frame.executions);
    CHECK(mv->error == frame.handler);
    if (failed_call) {
        CHECK(mv->ptrprevio == frame.previous);
        CHECK(mv->prev_args == frame.returns);
    }
}

static Program compile(lat_mv *mv, const char *source)
{
    int status = -1;
    Program p = {0};
    p.tree = latA_analizar_exp((char *)source, &status);
    if (p.tree && status == 0) p.function = latC_analizar(mv, p.tree);
    if (!p.function) { fprintf(stderr, "FAIL compilacion de la regresion\n"); exit(1); }
    return p;
}

static void check_owners(lat_mv *mv)
{
    CHECK(list_owner->tipo == T_LIST && latL_longitud(getLista(list_owner)) == 1);
    lat_objeto *element = latL_obtener_elemento(mv, getLista(list_owner), 0);
    CHECK(element && element->tipo == T_NUMERIC && getNumerico(element) == 73);
    lat_objeto *entry = latH_obtener(getDic(dict_owner), "valor");
    CHECK(entry && entry->tipo == T_NUMERIC && getNumerico(entry) == 91);
}

static void check_prefix(lat_mv *mv, int depth)
{
    if (depth == 0) return;
    CHECK(mv->pila[0].tipo == T_LIST && getLista(&mv->pila[0]) == getLista(list_owner));
    CHECK(mv->pila[1].tipo == T_DIC && getDic(&mv->pila[1]) == getDic(dict_owner));
}

static void valid_after_error(lat_mv *mv, int depth)
{
    Frame frame = capture(mv);
    int status = call(mv, valid_function);
    CHECK(status == 0);
    CHECK(mv->ptrpila == depth + 1);
    /* En la base defectuosa aun observamos la llamada posterior, sin reparar
       los argumentos abandonados. Solo consumimos el retorno normal. */
    if (status == 0 && mv->ptrpila > 0) {
        lat_objeto *value = latC_desapilar(mv);
        CHECK(value->tipo == T_NUMERIC && getNumerico(value) == 42);
    }
    check_frame(mv, frame, depth, 0);
}

static void numeric_four(lat_mv *mv)
{
    /* El mismo orden de consumo que ciencia.sumar2 del SDK. */
    for (int i = 0; i < 4; ++i) (void)latC_checar_numerico(mv, latC_desapilar(mv));
    latC_apilar_double(mv, 42);
}

static void consume_and_fail(lat_mv *mv)
{
    (void)latC_desapilar(mv);
    latC_error(mv, "P02E argumento de entrada consumido");
}

static void two_returns(lat_mv *mv)
{
    (void)latC_desapilar(mv);
    latC_apilar_double(mv, 42);
    latC_apilar_double(mv, 84);
}

static void inner_handler(lat_mv *mv, int propagate)
{
    /* Se entra desde un CALL_FUNCTION real: profundidad/contexto no nulos. */
    CHECK(mv->numejec > 0 && mv->ptrctx > 0 && mv->error != NULL);
    latC_apilar_double(mv, 777);
    Frame frame = capture(mv);
    CHECK(call(mv, nested_error.function) == LAT_ERRRUN);
    check_frame(mv, frame, frame.stack, 1);
    valid_after_error(mv, frame.stack);
    if (mv->ptrpila == frame.stack) {
        lat_objeto *marker = latC_desapilar(mv);
        CHECK(marker->tipo == T_NUMERIC && getNumerico(marker) == 777);
    }
    if (propagate) latC_error(mv, "P02E propagacion al manejador exterior");
    else latC_apilar_double(mv, 42);
}

static void protected_inner(lat_mv *mv) { inner_handler(mv, 0); }
static void propagated_inner(lat_mv *mv) { inner_handler(mv, 1); }

static const lat_CReg natives[] = {
    {"cuatro", numeric_four, 4}, {"proteger", protected_inner, 0},
    {"propagar", propagated_inner, 0}, {"consumir", consume_and_fail, 1},
    {"dos", two_returns, 1}, {NULL, NULL, 0}
};

int main(int argc, char **argv)
{
    const char *modes[] = {"directa", "anidada", "prefijo", "anidada_prefijo",
                          "prestados", "reentrada", "propagar", "consumido", "exito"};
    int mode = -1;
    if (argc == 3) for (int i = 0; i < 9; ++i) if (!strcmp(argv[1], modes[i])) mode = i;
    if (mode < 0) { fprintf(stderr, "Uso: %s caso ciclos(1..1000)\n", argv[0]); return 3; }
    char *end = NULL;
    errno = 0;
    long cycles = strtol(argv[2], &end, 10);
    if (errno || !*argv[2] || *end || cycles < 1 || cycles > 1000) return 3;
    setvbuf(stdout, NULL, _IOLBF, 0);
    lat_mv *mv = latC_crear_mv();
    if (!mv) return 1;
    mv->nombre_archivo = "p02e_stack_recovery";
    mv->global->REPL = false;
    latC_abrir_liblatino(mv, "pila", natives);
    /* Estos payloads pertenecen a la prueba. Pila y contextos los toman
       prestados; destruir una celda de pila seria invalid-free/doble-free. */
    lista *list = latL_crear(mv);
    latL_agregar(mv, list, latC_crear_numerico(mv, 73));
    list_owner = latC_crear_lista(mv, list);
    hash_map *dict = latH_crear(mv);
    lat_objeto number = {0}; setNumerico(&number, 91);
    latH_asignar(mv, dict, "valor", &number);
    dict_owner = latC_crear_dic(mv, dict);
    latO_asignar_ctx(mv, mv->contexto[0], "p02e_lista", list_owner);
    latO_asignar_ctx(mv, mv->contexto[0], "p02e_dic", dict_owner);

    Program programs[6];
    programs[0] = compile(mv,
        "funcion valida_interna()\n retorno pila.cuatro(1, 2, 3, 4)\nfin\n"
        "funcion valida()\n retorno valida_interna()\nfin\n"
        "funcion falla_interna()\n retorno pila.cuatro(3, 4, 5, \"texto\")\nfin\n"
        "funcion falla()\n retorno falla_interna()\nfin\n");
    CHECK(call(mv, programs[0].function) == 0);
    valid_function = latO_obtener_contexto(mv, mv->contexto[0], "valida");
    if (!valid_function) return 1;
    programs[1] = compile(mv, "resultado = pila.cuatro(3, 4, 5, \"texto\")\n");
    programs[2] = compile(mv, "resultado = falla()\n");
    nested_error = programs[2];
    programs[3] = compile(mv, "resultado = pila.cuatro(p02e_lista, p02e_dic, 5, \"texto\")\n");
    programs[4] = compile(mv, "resultado = pila.proteger()\n");
    programs[5] = compile(mv, "resultado = pila.propagar()\n");
    lat_objeto *module = latO_obtener_contexto(mv, mv->contexto[0], "pila");
    lat_objeto *consumer = latH_obtener(getDic(module), "consumir");
    lat_objeto *success = latH_obtener(getDic(module), "dos");
    /* Calentar el retorno normal antes de medir el saldo interno. */
    valid_after_error(mv, 0);
    int depth = mode >= 2 ? 2 : 0;
    if (depth) { latC_apilar(mv, list_owner); latC_apilar(mv, dict_owner); }
    size_t steady_memory = 0;
    unsigned completed = 0;
    for (long cycle = 0; cycle < cycles && !failures; ++cycle) {
        int expected = LAT_ERRRUN;
        lat_objeto *target = mode == 1 || mode == 3 ? programs[2].function : programs[1].function;
        if (mode == 4) target = programs[3].function;
        if (mode == 5) { target = programs[4].function; expected = 0; }
        if (mode == 6) target = programs[5].function;
        if (mode >= 7) { latC_apilar_double(mv, 99); target = mode == 7 ? consumer : success; }
        if (mode == 8) expected = 0;
        Frame frame = capture(mv);
        int state = call(mv, target);
        CHECK(state == expected);
        if (mode == 8) {
            CHECK(mv->ptrpila == depth + 2);
            CHECK(getNumerico(latC_desapilar(mv)) == 84);
            CHECK(getNumerico(latC_desapilar(mv)) == 42);
        }
        check_frame(mv, frame, depth, expected != 0);
        check_prefix(mv, depth);
        check_owners(mv);
        valid_after_error(mv, depth);
        check_prefix(mv, depth);
        check_owners(mv);
        /* La primera vuelta puede crear 'dummy'/'resultado'. No sustituye LSan,
           pero detecta crecimiento de asignaciones contabilizadas por latM. */
        if (cycle == 0) steady_memory = mv->memoria_usada;
        else CHECK(mv->memoria_usada == steady_memory);
        ++completed;
    }
    printf("P02E caso=%s ciclos=%u/%ld llamadas=%u ptrpila=%d ptrctx=%d numejec=%d fallos=%u\n",
           modes[mode], completed, cycles, protected_calls, mv->ptrpila, mv->ptrctx, mv->numejec, failures);
    /* Soltar una sola vez a los propietarios; el destructor de contextos solo
       libera sus envoltorios prestados. La pila no se destruye celda por celda. */
    for (int i = 0; i < 6; ++i) { latO_destruir(mv, programs[i].function); latA_destruir(programs[i].tree); }
    latO_destruir(mv, list_owner);
    latO_destruir(mv, dict_owner);
    latC_destruir_mv(mv);
    return failures || completed != (unsigned)cycles ? 1 : 0;
}
