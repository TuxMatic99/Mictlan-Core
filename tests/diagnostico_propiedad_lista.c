
/*
 * Mictlan Core
 * Diagnostico de propiedad de elementos compartidos.
 *
 * Prueba intencionadamente peligrosa.
 * Ejecutar solamente con AddressSanitizer.
 */

#include <stdio.h>
#include "latino.h"
#include "latobj.h"

int main(void)
{
    lat_mv mv = {0};

    /* Crear una lista con un numero. */
    lista *datos = latL_crear(&mv);

    latL_agregar(
        &mv,
        datos,
        latC_crear_numerico(&mv, 8.0)
    );

    lat_objeto *original =
        latC_crear_lista(&mv, datos);

    /* Clonar la lista con el mecanismo actual. */
    lat_objeto *copia =
        latO_clonar(&mv, original);

    puts("=== PRUEBA DE PROPIEDAD ===");
    puts("Destruyendo la lista original...");
    fflush(stdout);

    /* Destruir la primera lista. */
    latO_destruir(&mv, original);

    /*
     * La copia debe conservar datos validos.
     * Si comparte objetos ya liberados,
     * AddressSanitizer deberia detectarlo.
     */
    lat_objeto *elemento =
        latL_obtener_elemento(
            &mv,
            latC_checar_lista(&mv, copia),
            0
        );

    
double valor = latC_checar_numerico(&mv, elemento);

printf("Valor conservado: %.0f\n", valor);

/* Comprobar que la clonacion conserva el valor correcto. */
if (valor != 8.0) {
    fprintf(stderr,
            "ERROR: valor numerico incorrecto\n");

    latO_destruir(&mv, copia);
    return 2;
}

/* Destruir la copia y comprobar su liberacion con ASan. */
latO_destruir(&mv, copia);

return 0;

}
