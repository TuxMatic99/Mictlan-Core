
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Un recurso con un contador de propietarios. */
typedef struct {
    unsigned referencias;
    int valor;
} Recurso;

/* Crear un recurso con un propietario inicial. */
static Recurso *nuevo(int valor) {
    Recurso *r = malloc(sizeof(*r));
    if (r == NULL) {
        perror("Error de memoria");
        exit(EXIT_FAILURE);
    }

    r->valor = valor;
    r->referencias = 1;
    return r;
}

/* Adquirir otra referencia al mismo recurso. */
static Recurso *retener(Recurso *r) {
    assert(r != NULL && r->referencias > 0);
    r->referencias++;
    return r;
}

/* Liberar una referencia, no necesariamente el recurso. */
static void soltar(Recurso *r) {
    assert(r != NULL && r->referencias > 0);

    r->referencias--;

    if (r->referencias == 0) {
        puts("Recurso liberado una sola vez");
        free(r);
    }
}

int main(void) {
    Recurso *original = nuevo(8);

    Recurso *primera = retener(original);
    Recurso *segunda = retener(original);

    printf("Referencias iniciales: %u\n",
           original->referencias);

    soltar(original);

    printf("Valor conservado: %d\n",
           primera->valor);

    soltar(primera);

    printf("Ultima referencia conserva: %d\n",
           segunda->valor);

    soltar(segunda);

    puts("PRUEBA COMPLETADA");
    return 0;
}
