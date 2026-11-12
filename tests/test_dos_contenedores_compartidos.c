/* P0.2-D: dos contenedores, una sola identidad, un solo destructor. */
#include <assert.h>
#include <stdio.h>
#include "latino.h"
#include "latrecurso.h"
#include "latlista_recursos.h"

static int destrucciones = 0;

static void destruir_numero(lat_mv *mv, void *dato)
{
    ++destrucciones;
    latO_destruir(mv, (lat_objeto *)dato);
}

int main(void)
{
    lat_mv mv = {0};
    lat_objeto *numero = latC_crear_numerico(&mv, 8.0);
    lat_recurso *recurso = latR_crear(&mv, numero, destruir_numero);
    lat_lista_recursos *a = latLR_crear(&mv);
    lat_lista_recursos *b = latLR_crear(&mv);

    assert(numero != NULL && recurso != NULL && a != NULL && b != NULL);
    assert(latLR_agregar(&mv, a, recurso));
    assert(latLR_agregar(&mv, b, recurso));
    assert(latR_propietarios(recurso) == 3);

    lat_objeto *desde_a = latLR_obtener(a, 0);
    lat_objeto *desde_b = latLR_obtener(b, 0);
    assert(desde_a == numero && desde_b == numero);
    puts("[OK] Identidad compartida entre contenedores");

    latR_soltar(&mv, &recurso);
    assert(recurso == NULL);
    latLR_destruir(&mv, &a);
    assert(a == NULL && destrucciones == 0);
    assert(latLR_obtener(b, 0) == numero);
    assert(latC_checar_numerico(&mv, numero) == 8.0);
    puts("[OK] El objeto sobrevive al primer contenedor");

    latLR_destruir(&mv, &b);
    assert(b == NULL && destrucciones == 1);
    assert(mv.memoria_usada == 0);
    puts("[OK] Ultimo propietario: una destruccion, cero bytes pendientes");
    return 0;
}
