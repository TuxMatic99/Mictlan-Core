
#include <stdio.h>
#include "latino.h"
#include "latobj.h"

int main(void)
{
    lat_mv mv = {0};

    lista *datos = latL_crear(&mv);

    latL_agregar(
        &mv, datos,
        latC_crear_numerico(&mv, 10)
    );

    latL_agregar(
        &mv, datos,
        latC_crear_numerico(&mv, 30)
    );

    /* Insertar el numero 20 entre 10 y 30. */
    latL_insertar_elemento(
        &mv,
        datos,
        latC_crear_numerico(&mv, 20),
        1
    );

    double a = latC_checar_numerico(
        &mv, latL_obtener_elemento(&mv, datos, 0)
    );

    double b = latC_checar_numerico(
        &mv, latL_obtener_elemento(&mv, datos, 1)
    );

    double c = latC_checar_numerico(
        &mv, latL_obtener_elemento(&mv, datos, 2)
    );

    printf("Resultado: %.0f %.0f %.0f\n", a, b, c);

    int correcto =
        latL_longitud(datos) == 3 &&
        a == 10 && b == 20 && c == 30;

    lat_objeto *contenedor =
        latC_crear_lista(&mv, datos);

    latO_destruir(&mv, contenedor);

    if (!correcto) {
        puts("ERROR: insercion incorrecta");
        return 2;
    }

    puts("Insercion correcta");
    return 0;
}
