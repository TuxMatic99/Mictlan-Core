
/*
 * Mictlan Core
 * Diagnostico de propiedad de listas anidadas.
 *
 * Ejecutar exclusivamente con AddressSanitizer.
 */

#include <stdio.h>
#include "latino.h"
#include "latobj.h"

int main(void)
{
    lat_mv mv = {0};

    /* Crear la lista interna: [8]. */
    lista *datos_internos = latL_crear(&mv);

    latL_agregar(
        &mv,
        datos_internos,
        latC_crear_numerico(&mv, 8.0)
    );

    lat_objeto *lista_interna =
        latC_crear_lista(&mv, datos_internos);

    /* Crear la lista exterior: [[8]]. */
    lista *datos_externos = latL_crear(&mv);

    latL_agregar(
        &mv,
        datos_externos,
        lista_interna
    );

    lat_objeto *original =
        latC_crear_lista(&mv, datos_externos);

    /* Clonar la lista exterior. */
    lat_objeto *copia =
        latO_clonar(&mv, original);

    puts("=== PROPIEDAD DE LISTAS ANIDADAS ===");
    puts("Destruyendo la lista original...");
    fflush(stdout);

    latO_destruir(&mv, original);

    /* Intentar acceder al contenido de la copia. */
    lat_objeto *interior =
        latL_obtener_elemento(
            &mv,
            latC_checar_lista(&mv, copia),
            0
        );

    lat_objeto *numero =
        latL_obtener_elemento(
            &mv,
            latC_checar_lista(&mv, interior),
            0
        );

    double valor =
        latC_checar_numerico(&mv, numero);

    printf("Valor conservado: %.0f\n", valor);

    if (valor != 8.0) {
        return 2;
    }

    latO_destruir(&mv, copia);

    return 0;
}
