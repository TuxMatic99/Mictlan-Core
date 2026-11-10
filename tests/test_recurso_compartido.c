
#include <assert.h>
#include <stdio.h>

#include "latino.h"
#include "latmem.h"
#include "latrecurso.h"

/* Contador para verificar cuantas veces se destruye el dato. */
static int destrucciones = 0;

/* Funcion que libera el dato almacenado. */
static void destruir_entero(lat_mv *mv, void *dato)
{
    destrucciones++;
    latM_liberar(mv, dato);
}

int main(void)
{
    lat_mv mv = {0};

    /* Crear un entero utilizando el asignador de Core. */
    int *numero = latM_asignar(&mv, sizeof(*numero));
    *numero = 8;

    /* Crear un recurso con un propietario. */
    lat_recurso *original =
        latR_crear(&mv, numero, destruir_entero);

    assert(original != NULL);
    assert(latR_propietarios(original) == 1);

    /* Adquirir un segundo propietario. */
    lat_recurso *alias = latR_adquirir(original);

    assert(alias == original);
    assert(latR_propietarios(original) == 2);

    /* Ambos propietarios acceden al mismo dato. */
    *(int *)latR_dato(alias) = 16;

    /* Liberar el primer propietario. */
    latR_soltar(&mv, &original);

    assert(original == NULL);
    assert(destrucciones == 0);
    assert(latR_propietarios(alias) == 1);

    /* Comprobar que el segundo sigue siendo valido. */
    int valor = *(int *)latR_dato(alias);
    assert(valor == 16);

    printf("Valor conservado: %d\n", valor);

    /* Liberar el ultimo propietario. */
    latR_soltar(&mv, &alias);

    assert(alias == NULL);
    assert(destrucciones == 1);

    /* Comprobar la contabilidad interna de Core. */
    assert(mv.memoria_usada == 0);

    puts("Destructor ejecutado una sola vez");
    puts("Memoria contabilizada: 0");
    puts("PRUEBA DE RECURSOS SUPERADA");

    return 0;
}
