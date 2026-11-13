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

#include "latrepl.h"
#include "latino.h"

bool inicia_con(const char *base, const char *str);
extern char *filename;

static void completion(const char *buf, linenoiseCompletions *lc) {
    if (inicia_con(buf, "pon")) {
        linenoiseAddCompletion(lc, "poner");
    }
}

int leer_linea(char *buffer) {
    const char *prompt = "mictlan> ";
    size_t usados = 0;
    buffer[0] = '\0';
    for (;;) {
        char *input = linenoise(prompt);
        if (!input) return -1;
        size_t n = strlen(input);
        if (n > MAX_INPUT_SIZE - 1 - usados) {
            free(input);
            fputs("mictlan: entrada interactiva demasiado larga. Usa un archivo .mict.\n", stderr);
            buffer[0] = '\0';
            return 0;
        }
        memcpy(buffer + usados, input, n + 1);
        free(input);
        usados += n;
        if (usados == 0 || buffer[usados - 1] != '\\') return 0;
        /* Continuación explícita para recuperar tras una línea inválida. */
        buffer[usados - 1] = '\n';
        prompt = "    >>> ";
    }
}

void latR_REPL(lat_mv *mv) {
    int silencio_previo = parse_silent;
    char *archivo_previo = filename;
    parse_silent = 0;
    mv->global->REPL = true;
    mv->nombre_archivo = "<consola>";
    filename = mv->nombre_archivo;
    char buf[MAX_INPUT_SIZE];
    char historial[MAX_PATH_LENGTH];
    const char *usuario = getenv("HOME");
    int longitud = usuario && *usuario
        ? snprintf(historial, sizeof(historial), "%s/%s", usuario, HISTORY_FILE)
        : snprintf(historial, sizeof(historial), "%s", HISTORY_FILE);
    bool guardar = longitud > 0 && (size_t)longitud < sizeof(historial);
    if (guardar) linenoiseHistoryLoad(historial);
    linenoiseSetCompletionCallback(completion);
    while (leer_linea(buf) != -1) {
        int status;
        ast *nodo = latA_analizar_exp(buf, &status);
        if (status == 0 && nodo != NULL) {
            lat_objeto *curexpr = latC_analizar(mv, nodo);
            status = latC_llamar_funcion(mv, curexpr);
            latO_destruir(mv, curexpr);
        }
        latA_destruir(nodo);
        if (guardar && *buf) {
            linenoiseHistoryAdd(buf);
            linenoiseHistorySave(historial);
        }
    }
    linenoiseHistoryFree();
    filename = archivo_previo;
    parse_silent = silencio_previo;
}
