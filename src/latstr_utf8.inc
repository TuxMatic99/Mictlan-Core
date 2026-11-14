/* Mictlan, MIT. Implementacion privada: escalares Unicode, no grafemas UAX #29.
   Sin tablas ni dependencia externa; valida la codificacion completa de UTF-8. */
static size_t cadena_utf8_paso(const unsigned char *s, size_t n) {
    if (!n) return 0;
    unsigned c = s[0];
    if (c < 0x80) return 1;
    size_t ancho;
    if (c >= 0xc2 && c <= 0xdf) ancho = 2;
    else if (c >= 0xe0 && c <= 0xef) ancho = 3;
    else if (c >= 0xf0 && c <= 0xf4) ancho = 4;
    else return 0;
    if (ancho > n) return 0;
    for (size_t i = 1; i < ancho; ++i) if ((s[i] & 0xc0) != 0x80) return 0;
    if ((c == 0xe0 && s[1] < 0xa0) || (c == 0xed && s[1] >= 0xa0) ||
        (c == 0xf0 && s[1] < 0x90) || (c == 0xf4 && s[1] >= 0x90)) return 0;
    return ancho;
}

static size_t cadena_utf8_contar(lat_mv *mv, const char *s, size_t len) {
    size_t i = 0, puntos = 0;
    while (i < len) {
        size_t ancho = cadena_utf8_paso((const unsigned char *)s + i, len - i);
        if (!ancho) { latC_error(mv, "UTF-8 invalido en byte %zu", i); return 0; }
        i += ancho;
        ++puntos;
    }
    return puntos;
}

static size_t cadena_indice_unicode(lat_mv *mv, lat_objeto *o) {
    double n = latC_checar_numerico(mv, o);
    if (!isfinite(n) || n < 0 || n > INT_MAX || floor(n) != n)
        latC_error(mv, "Indice Unicode debe ser entero no negativo");
    return (size_t)n;
}

static void str_longitud_bytes(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    latC_apilar_double(mv, (double)strlen(latC_checar_cadena(mv, a)));
}

static void str_longitud_unicode(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    const char *s = latC_checar_cadena(mv, a);
    latC_apilar_double(mv, (double)cadena_utf8_contar(mv, s, strlen(s)));
}

static void str_invertir_unicode(lat_mv *mv) {
    lat_objeto *a = latC_desapilar(mv);
    const char *s = latC_checar_cadena(mv, a);
    size_t len = strlen(s);
    (void)cadena_utf8_contar(mv, s, len); /* Validar antes de reservar. */
    char *ret = malloc(len + 1);
    if (!ret) { latC_error(mv, "Memoria virtual agotada"); return; }
    for (size_t i = 0; i < len;) {
        size_t ancho = cadena_utf8_paso((const unsigned char *)s + i, len - i);
        memcpy(ret + len - i - ancho, s + i, ancho);
        i += ancho;
    }
    ret[len] = '\0';
    cadena_publicar(mv, ret);
}

static void str_subcadena_unicode(lat_mv *mv) {
    lat_objeto *c = latC_desapilar(mv), *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    const char *s = latC_checar_cadena(mv, a);
    size_t inicio = cadena_indice_unicode(mv, b), n = cadena_indice_unicode(mv, c);
    size_t len = strlen(s), puntos = cadena_utf8_contar(mv, s, len);
    if (inicio > puntos) { latC_error(mv, "Indice de cadena fuera de rango"); return; }
    if (n > puntos - inicio) n = puntos - inicio;
    size_t primero = 0, ultimo;
    for (size_t k = 0; k < inicio; ++k)
        primero += cadena_utf8_paso((const unsigned char *)s + primero, len - primero);
    ultimo = primero;
    for (size_t k = 0; k < n; ++k)
        ultimo += cadena_utf8_paso((const unsigned char *)s + ultimo, len - ultimo);
    char *ret = malloc(ultimo - primero + 1);
    if (!ret) { latC_error(mv, "Memoria virtual agotada"); return; }
    memcpy(ret, s + primero, ultimo - primero);
    ret[ultimo - primero] = '\0';
    cadena_publicar(mv, ret);
}

static void str_indice_unicode(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    const char *s = latC_checar_cadena(mv, a), *p = latC_checar_cadena(mv, b);
    (void)cadena_utf8_contar(mv, s, strlen(s));
    (void)cadena_utf8_contar(mv, p, strlen(p));
    const char *hit = strstr(s, p);
    double posicion = hit ? (double)cadena_utf8_contar(mv, s, (size_t)(hit - s)) : -1;
    latC_apilar_double(mv, posicion);
}

static void str_separar_texto(lat_mv *mv) {
    lat_objeto *b = latC_desapilar(mv), *a = latC_desapilar(mv);
    const char *s = latC_checar_cadena(mv, a), *separador = latC_checar_cadena(mv, b);
    size_t ancho = strlen(separador);
    if (!ancho) { latC_error(mv, "Separador de texto vacio"); return; }
    lista *partes = latL_crear(mv);
    for (;;) {
        const char *hit = strstr(s, separador);
        size_t n = hit ? (size_t)(hit - s) : strlen(s);
        char *parte = malloc(n + 1);
        if (!parte) { latC_error(mv, "Memoria virtual agotada"); return; }
        memcpy(parte, s, n); parte[n] = '\0';
        lat_objeto *texto = latC_crear_cadena(mv, parte);
        free(parte);
        latL_agregar(mv, partes, texto);
        if (!hit) break;
        s = hit + ancho;
    }
    lat_objeto *ret = latC_crear_lista(mv, partes);
    latC_apilar(mv, ret);
#if !HABILITAR_GC
    latM_liberar(mv, ret);
#endif
}

static void cadena_caja_ascii(lat_mv *mv, bool mayuscula) {
    lat_objeto *a = latC_desapilar(mv);
    const char *s = latC_checar_cadena(mv, a);
    size_t n = strlen(s);
    char *ret = malloc(n + 1);
    if (!ret) { latC_error(mv, "Memoria virtual agotada"); return; }
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)s[i];
        if (mayuscula && c >= 'a' && c <= 'z') c = (unsigned char)(c - 'a' + 'A');
        else if (!mayuscula && c >= 'A' && c <= 'Z') c = (unsigned char)(c - 'A' + 'a');
        ret[i] = (char)c;
    }
    ret[n] = '\0';
    cadena_publicar(mv, ret);
}

static void str_mayusculas_ascii(lat_mv *mv) { cadena_caja_ascii(mv, true); }
static void str_minusculas_ascii(lat_mv *mv) { cadena_caja_ascii(mv, false); }
