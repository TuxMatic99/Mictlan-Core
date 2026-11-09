
/*
 * Mictlan Core
 * Diagnostico de un objeto compartido por dos posiciones.
 * Ejecutar exclusivamente con AddressSanitizer.
 */

#include <stdio.h>
#include "latino.h"
#include "latobj.h"

int main(void)
{
    lat_mv mv = {0};

    lat_objeto *numero =
        latC_crear_numerico(&mv, 8.0);

    lista *datos = latL_crear(&mv);

    /* Ambas posiciones apuntan al mismo objeto. */
    latL_agregar(&mv, datos, numero);
    latL_agregar(&mv, datos, numero);

    lat_objeto *contenedor =
        latC_crear_lista(&mv, datos);

    puts("=== ELEMENTO COMPARTIDO ===");
    puts("Destruyendo el contenedor...");
    fflush(stdout);

    latO_destruir(&mv, contenedor);

    puts("=== FIN ===");
    return 0;
}
