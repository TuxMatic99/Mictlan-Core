#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include "latcompat.h"

#ifdef __APPLE__
#include <xlocale.h>
#endif

double latC_strtod_c(const char *nptr, char **endptr) {
  /* Locale propio de esta conversión: no cambia el global ni el del hilo. */
  int previo = errno;
#if defined(_WIN32) || defined(__WIN32__)
  _locale_t loc = _create_locale(LC_NUMERIC, "C");
#else
  locale_t loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0);
#endif
  if (!loc) {
    if (endptr) *endptr = (char *)nptr;
    errno = ENOMEM;
    return 0;
  }
  errno = previo;
#if defined(_WIN32) || defined(__WIN32__)
  double v = _strtod_l(nptr, endptr, loc);
  int estado = errno;
  _free_locale(loc);
#else
  double v = strtod_l(nptr, endptr, loc);
  int estado = errno;
  freelocale(loc);
#endif
  errno = estado;
  return v;
}
