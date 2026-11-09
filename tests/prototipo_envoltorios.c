
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* Recurso real que puede ser compartido. */
typedef struct {
    unsigned propietarios;
    int valor;
} Recurso;

/* Envoltorio que apunta al recurso. */
typedef struct {
    Recurso *recurso;
} Objeto;

/* Crear un recurso con un propietario inicial. */
static Objeto crear_objeto(int valor) {
    Recurso *r = malloc(sizeof(*r));

    if (r == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    r->propietarios = 1;
    r->valor = valor;

    Objeto obj = {r};
    return obj;
}

/* Crear otra referencia propietaria. */
static Objeto compartir_objeto(const Objeto *original) {
    assert(original != NULL && original->recurso != NULL);
    assert(original->recurso->propietarios < UINT_MAX);

    original->recurso->propietarios++;

    Objeto copia = {original->recurso};
    return copia;
}

/* Retirar una referencia propietaria. */
static void soltar_objeto(Objeto *obj) {
    assert(obj != NULL && obj->recurso != NULL);

    Recurso *r = obj->recurso;
    obj->recurso = NULL;

    assert(r->propietarios > 0);
    r->propietarios--;

    if (r->propietarios == 0) {
        puts("Recurso liberado una sola vez");
        free(r);
    }
}

int main(void) {
    Objeto original = crear_objeto(8);

    Objeto a = compartir_objeto(&original);
    Objeto b = compartir_objeto(&original);

    printf("Propietarios: %u\n",
           original.recurso->propietarios);

    assert(original.recurso->propietarios == 3);

    /* Modificar el recurso desde A. */
    a.recurso->valor = 16;

    /* B debe observar el mismo cambio. */
    printf("Valor desde B: %d\n",
           b.recurso->valor);

    assert(b.recurso->valor == 16);

    /* Liberar dos referencias. */
    soltar_objeto(&original);
    soltar_objeto(&a);

    /* B sigue teniendo acceso valido. */
    printf("Ultimo propietario lee: %d\n",
           b.recurso->valor);

    assert(b.recurso->valor == 16);

    soltar_objeto(&b);

    puts("PRUEBA COMPLETADA");
    return 0;
}
