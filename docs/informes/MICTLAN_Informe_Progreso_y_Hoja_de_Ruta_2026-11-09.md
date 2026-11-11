# Mictlán: informe de progreso y hoja de ruta hacia una alfa funcional

**Fecha de corte:** 9 de noviembre de 2026  
**Horizonte:** 10 al 28 de noviembre de 2026 (19 días de trabajo calendario)  
**Proyectos:** Mictlan-Core · Mictlan-SDK · Mictlan-Lab  
**Rama de investigación local:** `investigacion/propiedad-memoria`  
**Último commit local comunicado:** `b25065a`  
**Estado del informe:** diagnóstico y planificación. No equivale a una certificación de estabilidad ni a la ejecución de nuevas pruebas.

---

## 1. Resumen ejecutivo

Mictlán **ya es un intérprete experimental capaz de ejecutar programas**, construido como derivación del lenguaje Latino y complementado por un SDK de interfaz, gráficos y FFI. No estamos creando desde cero un compilador o una máquina virtual. Contamos con una base funcional sobre Debian GNU/Linux, ejemplos ejecutados, componentes gráficos y una aplicación de laboratorio.

El principal riesgo de ingeniería es la **propiedad de memoria**: algunas operaciones comparten punteros, mientras que ciertos destructores asumen propiedad exclusiva. Los ensayos con AddressSanitizer reprodujeron accesos a memoria liberada en listas anidadas y objetos repetidos. Ya corregimos una fuga localizada de 120 bytes en la inserción de listas, pero el problema general no está resuelto.

En los últimos trabajos se incorporó `latrecurso.c` y `latrecurso.h`, un gestor interno inicial de recursos compartidos con funciones `latR_crear`, `latR_adquirir`, `latR_dato`, `latR_propietarios` y `latR_soltar`. El binario `liblatino.so` compiló y una prueba automatizada ASan/LSan pasó. **Ese módulo aún no controla automáticamente las listas existentes, los contextos ni la pila.**

**Conclusión:** una alfa de alcance acotado es plausible en menos de 20 días si dejamos de añadir grandes capacidades, estabilizamos las operaciones esenciales y preparamos una instalación y documentación reproducibles. No es razonable prometer un lenguaje generalista, un IDE completo, un GC universal, motores 3D y compatibilidad multiplataforma dentro de ese mismo plazo.

## 2. Alcance y evidencia del análisis

Este documento combina tres tipos de información, que deben mantenerse separados:

1. **Registros locales aportados durante el desarrollo:** commits, compilaciones y diagnósticos de memoria en la rama `investigacion/propiedad-memoria`.
2. **Repositorio remoto consultado mediante la conexión con GitHub:** estructura y código de `TuxMatic99/Mictlan-Core` y `TuxMatic99/Mictlan-SDK` en `desarrollo/mictlan-alpha`, y `TuxMatic99/Mictlan-Lab` en `main`.
3. **Criterios propuestos de ingeniería:** prioridades, pruebas de aceptación y calendario. Son decisiones para el futuro, no características ya terminadas.

**Límite de verificación:** la rama de investigación reciente está local en la computadora del desarrollador. No se inspeccionó directamente su contenido completo desde GitHub; para ella se utilizaron las salidas de terminal aportadas. La cifra de 32/32 del SDK corresponde a la última ejecución comunicada, no a una prueba repetida después del nuevo módulo de recursos.

## 3. Estado de cada proyecto

| Proyecto | Capacidades acreditadas | Limitación actual |
|---|---|---|
| **Mictlán Core** | Ejecutable `mictlan`; intérprete y VM de origen Latino; variables, cálculos y programas de ejemplo; bibliotecas de listas/diccionarios; regresiones de referencia; biblioteca `liblatino.so`; primeras herramientas ASan/LSan | Modelo de propiedad de objetos compartidos inconsistente, nuevas pruebas de memoria fallan, distribución incompleta, documentación pública heredada |
| **Mictlán SDK** | Componentes de GUI, eventos, callbacks, dibujo 2D/Raylib, pruebas FFI de gráficos/ciencia; última ejecución comunicada: 32/32 CTest | Integración y compatibilidad con el Core nuevo sin repetir; API/instalación aún no estabilizadas; README todavía identifica LatinoVox |
| **Mictlán Lab** | Aplicación de referencia que utiliza `host_ui`, etiquetas, botones y eventos; ejemplos `.mict` de ciencia y otros experimentos | Prototipo de demostración, no aplicación pública empaquetada; falta prueba reproducible en instalación limpia |

**Observación importante:** el Core publicado conserva el README heredado de Latino y el SDK el de LatinoVox. Esto puede confundir a usuarios con nombres, rutas, versiones e instrucciones no vigentes. La documentación necesita alinearse con la versión alfa real.

### ¿Es ya un lenguaje de programación?

Sí: un intérprete que reconoce sintaxis, evalúa programas y ejecuta instrucciones constituye un lenguaje ejecutable, no solamente pseudocódigo. Lo pendiente es que resulte **suficientemente confiable, instalable y documentado** para uso cotidiano limitado.

## 4. Cronología de hitos

### Fundación del proyecto

- Bifurcación y adaptación del intérprete de Latino hacia Mictlán, conservando atribución y licencia MIT.
- Compilación inicial de `mictlan`, `liblatino.so` y bibliotecas.
- Ejecución del ejemplo `hola_mictlan.lat` con salida esperada, incluida la operación `6 * 7 = 42`.
- Registro inicial de 11 suites internas del núcleo aprobadas en una etapa anterior. **No sustituye una ejecución completa de la suite actual.**
- Separación en tres repositorios: Core (lenguaje), SDK (capacidades), Lab (aplicación demostrativa).

### SDK y laboratorio

- Desarrollo de contenedores, jerarquía visual, layouts, eventos, callbacks y despachadores.
- Motor de dibujo 2D y su integración progresiva con la aplicación gráfica.
- Pruebas de interfaz y FFI. Último registro local anterior a esta fase: **32 de 32 pruebas de SDK aprobadas**.
- Operaciones científicas vectoriales mediante FFI, incluyendo `ciencia.sumar2(...)`, con pruebas de valores, tipos, errores y alias.
- Aplicación de laboratorio con ventana, etiquetas, botones, estado y eventos.

### Estabilización de memoria

- Reproducción de una fuga en operaciones vectoriales; los experimentos previos registraron **600 bytes en 15 reservas** para tres resultados vectoriales, tras eliminar una reserva temporal. Es un dato de un escenario concreto, no un total global de fugas.
- Prueba de referencias compartidas en programas del lenguaje, preservando el comportamiento de alias y reasignación.
- Prueba de lista numérica clonada: mitigación experimental validada en su caso.
- Reproducción de `heap-use-after-free` para listas anidadas y elementos repetidos.
- Inserción de lista: **120 bytes en cinco reservas** antes de la corrección; cero fugas detectadas en los diagnósticos de inserción después de la corrección.
- Dos diagnósticos de inserción automatizados en CTest, aprobados con ASan/LSan.
- Dos prototipos de retención de recursos compartidos aprobados y guardados.
- Primer módulo `latrecurso` integrado en Core y verificado mediante CTest.

### Commits locales de investigación documentados

| Commit | Hito |
|---|---|
| `4cefcd1` | Diagnósticos iniciales de propiedad y mitigación numérica |
| `a9be33b` | Corrección de espacios de diagnóstico |
| `d2ab15f` | Primer contrato de arquitectura de memoria |
| `a67e57e` | Prototipo básico de recuento de referencias |
| `115ea70` | Prototipo de envoltorios y recurso compartido |
| `69bd309` | Corrección de fuga en inserción de listas y casos límite |
| `6c04600` | Automatización de las pruebas de inserción |
| `ad5fd75` | Registro de la regresión de elemento repetido |
| `ed0ae81` | Ampliación del contrato de propiedad |
| `b25065a` | Gestor inicial de recursos compartidos y prueba ASan automatizada |

**Estado al corte:** `git status --short --branch` mostró `## investigacion/propiedad-memoria` sin cambios adicionales después de `b25065a`. Son commits **locales**; no se asume que estén publicados en GitHub.

## 5. Lecciones técnicas: qué hemos aprendido

1. **Compilar no basta.** Una biblioteca puede generar `liblatino.so` correctamente y contener errores de memoria. Por eso necesitamos pruebas de ejecución e instrumentación.
2. **Un puntero no informa quién posee el recurso.** `latL_agregar()` almacena `void *`, usado tanto para objetos del lenguaje como para estructuras internas (`hash_val`). Incrementar referencias allí indiscriminadamente sería peligroso.
3. **Copiar un envoltorio no copia necesariamente el recurso.** `latC_apilar()` y `latH_asignar()` pueden conservar punteros a listas o diccionarios compartidos.
4. **Clonar una lista y compartir sus hijos exige reglas coherentes.** `latO_destruir()` destruye recursivamente; la clonación parcial y otras operaciones pueden compartir elementos.
5. **El recuento de referencias requiere disciplina completa.** Cada adquisición debe tener una liberación y debe distinguirse propiedad, préstamo y transferencia. Los ciclos requieren un mecanismo complementario.
6. **Una modificación pequeña con regresión es más valiosa que un parche general sin pruebas.** La corrección de inserción mejoró funcionamiento y memoria con un alcance acotado.

El nuevo `lat_recurso` conserva el recurso únicamente cuando los participantes utilizan sus funciones de forma correcta. Todavía necesita pruebas de errores, comprobación de desbordamiento/subdesbordamiento del contador, diseño frente a posibles usos concurrentes y, principalmente, integración con los contenedores del intérprete. No cambiar aún `HABILITAR_GC` (actualmente `0`).

## 6. Qué significará «medio funcional» el 28 de noviembre

**Definición de MVP alfa 0.1:** una persona puede descargar o recibir una carpeta preparada para **Debian GNU/Linux x86_64**, ejecutar el intérprete `mictlan`, escribir y lanzar programas pequeños, consultar ejemplos y obtener errores comprensibles; opcionalmente puede abrir el laboratorio gráfico en el mismo entorno.

### Funciones mínimas exigidas

| Área | Criterio verificable |
|---|---|
| Ejecutable | `mictlan` compila y arranca sin depender de una ruta secreta del desarrollador |
| Programas | Ejecuta ejemplo de saludo/cálculo, variables, operaciones y cadenas |
| Control | Un programa de ejemplo utiliza condiciones, bucles y funciones |
| Colecciones | Listas y diccionarios soportan operaciones documentadas de lectura, escritura y alias sin UAF en casos admitidos |
| Errores | Entradas incorrectas producen errores controlados, no caída de proceso por memoria |
| SDK mínimo | Demo de interfaz o FFI funciona con la misma versión de Core |
| Pruebas | Regresiones de Core y 32 pruebas del SDK recompiladas y ejecutadas, resultados registrados |
| Uso real | README corto, guía de instalación y al menos tres ejemplos verificados de principio a fin |

El soporte de `.mict` como extensión nativa de la CLI se debe **verificar**, no asumir. La extensión `.lat` sí aparece en el código y ejemplos actuales; Lab utiliza ejemplos `.mict`.

### Fuera del alcance obligatorio

- Lenguaje totalmente nuevo e independiente de Latino a nivel interno.
- IDE gráfico integral, depurador visual o gestor avanzado de paquetes.
- Motor 3D, Vulkan, DirectX, física de videojuegos, multimedia y red completas.
- Compatibilidad verificada simultáneamente en Windows, macOS y múltiples Linux.
- Recolector de basura universal con ciclos completamente resueltos.
- Promesa de ausencia absoluta de fugas en cualquier extensión.

Esas capacidades permanecen como objetivos a mediano plazo, no criterios de cierre de esta alfa.

## 7. Riesgos y prioridades

| Riesgo | Gravedad | Plan de mitigación | Condición de parada |
|---|---|---|---|
| `heap-use-after-free` en listas anidadas / compartidas | **Crítica** | Contrato de propiedad, pruebas ASan con casos repetidos, anidados y reasignación | No distribuir una alfa general si rutas admitidas siguen fallando |
| Integrar `lat_recurso` con todas las rutas del VM | **Alta** | Introducirlo por alcance, verificar cada llamada y ciclo de vida | No hacer cambio masivo sin contratos ni cobertura |
| Regresiones SDK/FFI por cambio de ABI o semántica | **Alta** | Congelar disposición pública de `lat_objeto`; recompilar Core/SDK juntos y correr las 32 pruebas | No integrar un cambio si rompe pruebas básicas |
| Fugas restantes de FFI y objetos temporales | Alta | Medición ASan/LSan por escenario, corrección incremental | Documentar cualquier fuga residual, no declarar memoria resuelta |
| Instrucciones de instalación y README obsoletos | Media-alta | Actualizar documentación real y probar en directorio limpio | No ofrecer instrucciones que apunten a proyectos anteriores |
| Falta de CI publicado | Media | GitHub Actions básico en la rama de desarrollo cuando esté lista | No confundir tests locales con tests remotos |
| Trabajo excesivo o cambios de alcance | Alta | Congelar funciones, reservar últimos días para pruebas | Descartar funciones opcionales cuando amenacen plazo |

**Regla de calidad:** no «arreglar» fallos de AddressSanitizer desactivando el detector, declarando `WILL_FAIL`, ignorando excepciones o eliminando las pruebas. Los diagnósticos experimentales deben etiquetarse y separarse de la batería de aceptación estable.

## 8. Hoja de ruta concreta de 19 días

**Punto de partida:** 9 de noviembre.  
**Ventana de ejecución:** 10 al 28 de noviembre, inclusive.  
**Meta:** revisión alfa y decisión de entrega el **28 de noviembre de 2026**. El calendario es una meta de planificación, no una garantía.

| Fechas | Prioridad | Trabajo específico | Entregable / condición para avanzar |
|---|---|---|---|
| **10–11 nov.** (2 días) | P0 | Congelar alcance; inventario de commits, dependencias y tests; reconstruir Core/SDK; distinguir pruebas esperadamente fallidas de regresiones de aceptación | Línea de base reproducible y tablero de fallos |
| **12–16 nov.** (5 días) | P0 | Establecer propiedad para objetos de listas en una ruta limitada; comprobar clones, referencias repetidas, anidadas y alias; revisar destructor y casos nulos | No UAF en las operaciones declaradas compatibles; prueba instrumentada verde o bloqueo documentado |
| **17–20 nov.** (4 días) | P0/P1 | Repetir suites Core y 32 SDK; ensayo CLI, FFI de ciencia y demo Lab; corregir errores prioritarios | Ejemplos representativos ejecutados con Core y SDK de la misma compilación |
| **21–24 nov.** (4 días) | P1 | Alinear nombres y documentación; crear guía de cinco minutos y scripts de ejecución; probar instalación en carpeta nueva; CI mínimo de compilación y CTest | Otra persona puede iniciar la alfa en Debian con instrucciones claras |
| **25–27 nov.** (3 días) | P0/P1 | Congelación de cambios; revisión completa de pruebas, ABI, dependencias, atribución MIT y errores conocidos; corrección de bloqueos | Candidato alfa con registro de pruebas y limitaciones |
| **28 nov.** (1 día) | P0 | Decisión de entrega: alfa limitada o vista previa interna, sin ocultar defectos | Paquete/directorio reproducible y reporte final; no liberar si hay UAF en funciones soportadas |

### Plan de contingencia

Si el 16 de noviembre todavía existen fallos críticos en operaciones de listas **necesarias para los ejemplos**, cambiaremos la meta de «alfa distribuible» a «vista previa técnica privada». Se podrá mostrar Core, FFI y Lab en escenarios controlados, pero no se presentará como un lenguaje seguro para terceros. El trabajo de propiedad de memoria continúa sin sacrificar las pruebas ni el historial Git.

Si la memoria queda estabilizada antes, usaremos los días sobrantes para instalación limpia, errores de usuario y documentación. **No para introducir otro motor gráfico.**

## 9. Criterios de salida: condiciones que no podemos negociar

- [ ] Compilación limpia desde una carpeta nueva de Core en Debian.
- [ ] Ejecución documentada de saludo, funciones, condiciones, bucles, listas y diccionarios.
- [ ] Aliasing/reasignación probados con datos concretos.
- [ ] Diagnósticos de objetos repetidos y listas anidadas resueltos o declarados fuera del conjunto admitido con una restricción verificable; no UAF en funciones que se anuncien como compatibles.
- [ ] Prueba del gestor `lat_recurso` mantenida y casos de error examinados.
- [ ] 32/32 pruebas del SDK ejecutadas de nuevo con la versión definitiva de Core (si el conjunto sigue constando de 32).
- [ ] Prueba de Lab con ventana y eventos realizada en el entorno de entrega.
- [ ] Ejecutable y bibliotecas con rutas de carga reproducibles; sin dependencias ocultas del ordenador de desarrollo.
- [ ] README nuevo, atribución al proyecto Latino, licencia MIT y guía de arranque.
- [ ] Informe de defectos conocidos, alcance y soporte por plataforma.
- [ ] Política Git: separar investigación y versión candidata; no fusionar defectos conocidos inadvertidamente.

## 10. Procedimiento de trabajo recomendado (para una persona que está aprendiendo)

Usar un ciclo pequeño y repetible:

1. Abrir **solo un archivo** en VS Code y comprender qué hace.
2. Cambiar una función o prueba pequeña.
3. Compilar; si falla, detenerse y leer el primer error.
4. Ejecutar un diagnóstico enfocado y luego las regresiones relacionadas.
5. Revisar `git diff --check` y `git status --short --branch`.
6. Hacer un commit local con un mensaje descriptivo únicamente después de verificar.
7. Publicar al remoto `github` solo cuando corresponda y nunca usar el remoto histórico `origin` de Core/SDK para publicaciones.

Para principiantes: **Git guarda versiones; CMake organiza la compilación; CTest repite comprobaciones; ASan/LSan buscan problemas de memoria.** Ninguno reemplaza a los demás.

### Primer paso recomendado después de este informe

No tocar todavía `latO_destruir()` ni cambiar `HABILITAR_GC`. Registrar el estado del trabajo de memoria y preparar una batería de aceptación separada de las pruebas experimentales conocidas por fallar. Inspeccionar exactamente qué rutas deben adquirir y liberar propiedad al almacenar objetos en listas, con énfasis en `latL_extender`, clonación de listas, `dic_valores`, la pila y `latH_asignar`. Elegir **una sola ruta** y documentar su contrato antes de integrar `lat_recurso`.

## 11. Reflexión de progreso

Nuestro mayor avance no es un número de líneas añadidas. Es haber pasado de modificar código sin un criterio reproducible a trabajar con **evidencia experimental, aislamiento de cambios y puntos de recuperación**.

El intérprete heredado aporta mucha funcionalidad y reduce el plazo de construcción, pero también transmite deuda técnica. El nuevo gestor de referencias aporta una dirección arquitectónica prometedora, aunque todavía es infraestructura no utilizada por las rutas antiguas. La alfa será viable si aceptamos la diferencia entre **tener funcionalidades** y **poder garantizar razonablemente su uso dentro de un alcance definido**.

Una alfa modesta con CLI, documentación honesta y tres ejemplos robustos vale más que una colección grande de características que fallan al compartir una lista.

## 12. Fuentes técnicas y repositorios

**Repositorios revisados (versión publicada, que puede estar por detrás de la rama local):**

- [Mictlan-Core](https://github.com/TuxMatic99/Mictlan-Core)
- [Mictlan-SDK](https://github.com/TuxMatic99/Mictlan-SDK)
- [Mictlan-Lab](https://github.com/TuxMatic99/Mictlan-Lab)

**Documentación externa contrastada:**

- [LLVM AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- [LLVM LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html)
- [Python C API: Reference Counting](https://docs.python.org/3/c-api/refcounting.html)
- [CMake: Testing and CTest](https://cmake.org/cmake/help/latest/guide/tutorial/Testing%20and%20CTest.html)
- [CMake: install()](https://cmake.org/cmake/help/latest/command/install.html)
- [GitHub Actions: Triggering workflows](https://docs.github.com/en/actions/how-tos/write-workflows/choose-when-workflows-run/trigger-a-workflow)
- [Git: git-switch](https://git-scm.com/docs/git-switch)

---

**Próxima revisión recomendada:** al cierre del 11 de noviembre, con la matriz de pruebas y el alcance de la versión alfa confirmados.
