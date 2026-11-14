#[[
/***********************************************************************************
 * Copyright (c) 2015-2021 Lenguaje-Latino, programación con sintaxis al Español.  *
 *                                                                                 *
 *                            CMake_Root de Latino                                 *
 ***********************************************************************************/
]]

#
# CMAKE configuraciones
cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

include(MictlanVersion)
project(latino VERSION "${MICTLAN_VERSION_NUMERICA}")

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build [Debug]" FORCE) # Default to a Debug build
endif()
# set(CMAKE_BUILD_TYPE "Release")

# if(CMAKE_COMPILER_IS_GNUCXX)
#     target_link_libraries(${PROJECT_TEST_NAME} gcov)
# endif()

set(program_files "$ENV{ProgramFiles}")

#
# Mensajes de construcion
message(STATUS "Mictlán ${MICTLAN_VERSION} · derivado de Latino")
message(STATUS "Generador .............. ${CMAKE_GENERATOR}"    )
message(STATUS "Tipo de Build .......... ${CMAKE_BUILD_TYPE}"   )

include(BuildLatOptions)


#
# Codigo Fuente
add_subdirectory(src)


#find_path(GLIB_INCLUDE_DIR NAMES glib.h PATH_SUFFIXES glib-2.0)

#IF(UNIX)
#  execute_process(COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/tools/install_deps.sh)
#  execute_process(COMMAND bash ${CMAKE_CURRENT_LIST_DIR}/tools/key.sh)
#ENDIF(UNIX)

enable_testing()
include(tests/alpha/registrar.cmake)
include(tests/conformidad/registrar.cmake)
include(tests/cadenas/registrar.cmake)
include(tests/colecciones/registrar.cmake)
add_executable(mictlan_test_recompilacion tests/alpha/recompilacion.c)
target_link_libraries(mictlan_test_recompilacion PRIVATE liblatino)
target_include_directories(mictlan_test_recompilacion PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
add_test(NAME mictlan_alpha_recompilacion COMMAND mictlan_test_recompilacion)
set_tests_properties(mictlan_alpha_recompilacion PROPERTIES TIMEOUT 60 LABELS "alpha;memoria"
  FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")

add_executable(mictlan_test_p11_pila tests/p11_pila.c)
target_include_directories(mictlan_test_p11_pila PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mictlan_test_p11_pila PRIVATE liblatino)
add_test(NAME mictlan_p11_pila COMMAND mictlan_test_p11_pila)
set_tests_properties(mictlan_p11_pila PROPERTIES TIMEOUT 30 LABELS "p11;pila"
  FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")
add_executable(mictlan_test_p11_cadenas tests/p11_gc_cadenas.c)
target_include_directories(mictlan_test_p11_cadenas PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mictlan_test_p11_cadenas PRIVATE liblatino)
add_test(NAME mictlan_p11_gc_cadenas COMMAND mictlan_test_p11_cadenas)
set_tests_properties(mictlan_p11_gc_cadenas PROPERTIES TIMEOUT 30 LABELS "p11;memoria"
  FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")

# La inyección afecta al binario de prueba y su biblioteca estática.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_executable(mictlan_test_agotamiento tests/alpha/agotamiento.c)
  target_link_libraries(mictlan_test_agotamiento PRIVATE liblatino_static)
  target_include_directories(mictlan_test_agotamiento PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
  target_link_options(mictlan_test_agotamiento PRIVATE -Wl,--wrap=calloc -Wl,--wrap=realloc)
  add_test(NAME mictlan_alpha_agotamiento COMMAND mictlan_test_agotamiento)
  set_tests_properties(mictlan_alpha_agotamiento PROPERTIES TIMEOUT 20 LABELS "alpha;memoria"
    FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")
endif()
add_executable(mictlan_test_recoleccion tests/p02g_recoleccion.c)
target_include_directories(mictlan_test_recoleccion PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mictlan_test_recoleccion PRIVATE liblatino)
set_target_properties(mictlan_test_recoleccion PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)
foreach(caso IN ITEMS ciclos raices errores opacos)
  add_test(NAME mictlan_p02g_${caso} COMMAND mictlan_test_recoleccion ${caso})
  set_tests_properties(mictlan_p02g_${caso} PROPERTIES TIMEOUT 20 LABELS "p02g;memoria"
    FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")
endforeach()
add_executable(mictlan_test_formato tests/p02g_formato.c)
target_include_directories(mictlan_test_formato PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mictlan_test_formato PRIVATE liblatino)
add_test(NAME mictlan_formato COMMAND mictlan_test_formato)
set_tests_properties(mictlan_formato PROPERTIES TIMEOUT 20
  FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")
add_test(NAME mictlan_cli_expresion_larga COMMAND "${CMAKE_COMMAND}"
  "-DMICTLAN_EJECUTABLE=$<TARGET_FILE:latino>"
  -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/verificar_expresion_larga.cmake")
set_tests_properties(mictlan_cli_expresion_larga PROPERTIES TIMEOUT 20)
add_executable(mictlan_test_generativo tests/p02g_generativo.c)
target_include_directories(mictlan_test_generativo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mictlan_test_generativo PRIVATE liblatino)
set_target_properties(mictlan_test_generativo PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)
add_test(NAME mictlan_p02g_generativo COMMAND mictlan_test_generativo)
set_tests_properties(mictlan_p02g_generativo PROPERTIES TIMEOUT 60 LABELS "p02g;parser;bytecode;memoria"
  FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")
# Recuperacion real de pila/FFI tras longjmp. Ningun fallo se invierte ni omite.
add_executable(mictlan_test_stack_recovery tests/stack_recovery_regression.c)
target_include_directories(mictlan_test_stack_recovery PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mictlan_test_stack_recovery PRIVATE liblatino)
set_target_properties(mictlan_test_stack_recovery PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES)
foreach(caso IN ITEMS directa anidada prefijo anidada_prefijo prestados reentrada propagar consumido exito)
    add_test(NAME mictlan_pila_${caso}
        COMMAND mictlan_test_stack_recovery ${caso} 200)
    set_tests_properties(mictlan_pila_${caso} PROPERTIES
        TIMEOUT 60 LABELS "p02e;pila;recuperacion"
        FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")
endforeach()

# Programa de cobertura restaurado del historial de Latino. Comprobar valores
# ademas del estado de salida, incluida la recursion de Fibonacci.
foreach(caso IN ITEMS coverage mictlan_marcos_funcion)
  if(caso STREQUAL "coverage")
    set(programa "${CMAKE_CURRENT_SOURCE_DIR}/ejemplos/coverage.lat")
  else()
    set(programa "${CMAKE_CURRENT_SOURCE_DIR}/tests/marcos_funcion.lat")
  endif()
  add_test(NAME ${caso} COMMAND "${CMAKE_COMMAND}"
    "-DMICTLAN_EJECUTABLE=$<TARGET_FILE:latino>"
    "-DMICTLAN_PROGRAMA=${programa}"
    "-DMICTLAN_ESPERADO=${CMAKE_CURRENT_SOURCE_DIR}/tests/${caso}.out"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/verificar_programa.cmake")
  set_tests_properties(${caso} PROPERTIES TIMEOUT 30)
endforeach()

# Regresion: listas compartidas y reasignacion.

add_test(
    NAME mictlan_memoria_listas_referencias
    COMMAND
        "${CMAKE_COMMAND}"
        "-DMICTLAN_EJECUTABLE=$<TARGET_FILE:latino>"
        "-DMICTLAN_PROGRAMA=${CMAKE_CURRENT_SOURCE_DIR}/tests/memoria_listas_referencias.lat"
        -P
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/verificar_memoria_listas_referencias.cmake"
)

set_tests_properties(
    mictlan_memoria_listas_referencias
    PROPERTIES TIMEOUT 15
)
# ==============================================
# MICTLAN CORE: PROPIEDAD DE LISTAS CON ASAN
# ==============================================

# Registrar este diagnostico solamente cuando
# Core este compilado con AddressSanitizer.
if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")

    add_executable(
        mictlan_test_propiedad_lista
        tests/diagnostico_propiedad_lista.c
    )

    target_include_directories(
        mictlan_test_propiedad_lista
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(
        mictlan_test_propiedad_lista
        PRIVATE
        liblatino
    )

    add_test(
        NAME mictlan_memoria_propiedad_numerica_asan
        COMMAND mictlan_test_propiedad_lista
    )

    set_tests_properties(
        mictlan_memoria_propiedad_numerica_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT
                "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )
# Regresion de propiedad de listas anidadas.
# Actualmente falla por heap-use-after-free.
# Debe pasar cuando se corrija la propiedad de memoria.

add_executable(
    mictlan_test_propiedad_anidada
    tests/diagnostico_propiedad_lista_anidada.c
)

target_include_directories(
    mictlan_test_propiedad_anidada
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(
    mictlan_test_propiedad_anidada
    PRIVATE
    liblatino
)

add_test(
    NAME mictlan_memoria_propiedad_anidada_asan
    COMMAND mictlan_test_propiedad_anidada
)

set_tests_properties(
    mictlan_memoria_propiedad_anidada_asan
    PROPERTIES
        TIMEOUT 15
        ENVIRONMENT
            "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
)
endif()
#if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
#    include(CodeCoverage)
#    setup_target_for_coverage(${PROJECT}_coverage ${PROJECT_TEST_NAME} coverage)
#
#    SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage -coverage")
#    SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage -coverage")
#endif() #CMAKE_BUILD_TYPE STREQUAL "Coverage"

set(GCOV "gcov")
if(DEFINED ENV{GCOV})
  set(GCOV $ENV{GCOV})
endif()
add_custom_target(coverage
  COMMAND lcov --gcov-tool ${GCOV} --capture --directory . --output-file coverage.info
  COMMAND lcov --gcov-tool ${GCOV} --remove coverage.info "/usr/*" "*/test/*" --output-file coverage.info
  COMMAND lcov --gcov-tool ${GCOV} --list coverage.info
  COMMAND genhtml coverage.info --output-directory ./coverage
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  VERBATIM)
# ============================================
# MICTLAN: PRUEBAS DE INSERCION CON ASAN
# ============================================

if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")

    # Primera prueba: insercion entre dos elementos.
    add_executable(
        mictlan_test_insercion_lista
        tests/diagnostico_insercion_lista.c
    )

    target_include_directories(
        mictlan_test_insercion_lista
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(
        mictlan_test_insercion_lista
        PRIVATE liblatino
    )

    add_test(
        NAME mictlan_memoria_insercion_basica_asan
        COMMAND mictlan_test_insercion_lista
    )

    # Segunda prueba: inserciones y casos limite.
    add_executable(
        mictlan_test_insercion_limites
        tests/diagnostico_insercion_limites.c
    )

    target_include_directories(
        mictlan_test_insercion_limites
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(
        mictlan_test_insercion_limites
        PRIVATE liblatino
    )

    add_test(
        NAME mictlan_memoria_insercion_limites_asan
        COMMAND mictlan_test_insercion_limites
    )

    # Activar deteccion de fugas para ambas pruebas.
    set_tests_properties(
        mictlan_memoria_insercion_basica_asan
        mictlan_memoria_insercion_limites_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT
                "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )

endif()
# =============================================
# MICTLAN: REFERENCIAS REPETIDAS CON ASAN
# =============================================

if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")

    add_executable(
        mictlan_test_elemento_repetido
        tests/diagnostico_propiedad_elemento_repetido.c
    )

    target_include_directories(
        mictlan_test_elemento_repetido
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(
        mictlan_test_elemento_repetido
        PRIVATE
        liblatino
    )

    add_test(
        NAME mictlan_memoria_elemento_repetido_asan
        COMMAND mictlan_test_elemento_repetido
    )

    set_tests_properties(
        mictlan_memoria_elemento_repetido_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT
                "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )
endif()

# ==============================================
# MICTLAN: RECURSOS COMPARTIDOS CON ASAN
# ==============================================

if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")

    add_executable(
        mictlan_test_recurso_compartido
        tests/test_recurso_compartido.c
    )

    target_include_directories(
        mictlan_test_recurso_compartido
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    target_link_libraries(
        mictlan_test_recurso_compartido
        PRIVATE
        liblatino
    )

    add_test(
        NAME mictlan_memoria_recurso_compartido_asan
        COMMAND mictlan_test_recurso_compartido
    )

    set_tests_properties(
        mictlan_memoria_recurso_compartido_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT
                "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )

endif()
# ============================================
# MICTLAN: RECURSO CON LISTA REAL
# ============================================

if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")

    add_executable(
        mictlan_test_recurso_lista_real
        tests/test_recurso_lista_real.c
    )

    target_include_directories(
        mictlan_test_recurso_lista_real
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    target_link_libraries(
        mictlan_test_recurso_lista_real
        PRIVATE
        liblatino
    )

    add_test(
        NAME mictlan_memoria_recurso_lista_real_asan
        COMMAND mictlan_test_recurso_lista_real
    )

    set_tests_properties(
        mictlan_memoria_recurso_lista_real_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT
                "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )
endif()
# ============================================
# MICTLAN: AUTOEXTENSION DE LISTAS CON ASAN
# ============================================

if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")

    add_executable(
        mictlan_test_autoextension_lista
        tests/diagnostico_extender_misma_lista.c
    )

    target_include_directories(
        mictlan_test_autoextension_lista
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

    target_link_libraries(
        mictlan_test_autoextension_lista
        PRIVATE liblatino
    )

    add_test(
        NAME mictlan_memoria_autoextension_asan
        COMMAND mictlan_test_autoextension_lista
    )

    set_tests_properties(
        mictlan_memoria_autoextension_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT
                "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )
endif()

# ============================================
# MICTLAN: ELEMENTOS COMPARTIDOS CON ASAN
# ============================================

if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")

    add_executable(
        mictlan_test_recurso_elemento_compartido
        tests/test_recurso_elemento_compartido.c
    )

    target_include_directories(
        mictlan_test_recurso_elemento_compartido
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    target_link_libraries(
        mictlan_test_recurso_elemento_compartido
        PRIVATE liblatino
    )

    add_test(
        NAME mictlan_memoria_recurso_elemento_compartido_asan
        COMMAND mictlan_test_recurso_elemento_compartido
    )

    set_tests_properties(
        mictlan_memoria_recurso_elemento_compartido_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT
                "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )

endif()
# ============================================
# MICTLAN: LISTA DE RECURSOS CON ASAN
# ============================================

if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")

    add_executable(
        mictlan_test_lista_recursos
        tests/test_lista_recursos.c
    )

    target_include_directories(
        mictlan_test_lista_recursos
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    target_link_libraries(
        mictlan_test_lista_recursos
        PRIVATE liblatino
    )

    add_test(
        NAME mictlan_memoria_lista_recursos_asan
        COMMAND mictlan_test_lista_recursos
    )

    set_tests_properties(
        mictlan_memoria_lista_recursos_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT
                "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )

endif()
# Mictlan: reemplazo seguro de recursos bajo AddressSanitizer.
if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")
    add_executable(
        mictlan_test_lista_recursos_reemplazo
        tests/test_lista_recursos_reemplazo.c
    )
    target_include_directories(
        mictlan_test_lista_recursos_reemplazo PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
    target_link_libraries(
        mictlan_test_lista_recursos_reemplazo PRIVATE liblatino
    )
    add_test(
        NAME mictlan_memoria_lista_recursos_reemplazo_asan
        COMMAND mictlan_test_lista_recursos_reemplazo
    )
    set_tests_properties(
        mictlan_memoria_lista_recursos_reemplazo_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )
endif()

# P0.2-D: prueba de contrato con dos contenedores y un recurso compartido.
if(CMAKE_C_FLAGS MATCHES "-fsanitize=address")
    add_executable(
        mictlan_test_dos_contenedores_compartidos
        tests/test_dos_contenedores_compartidos.c
    )
    target_include_directories(
        mictlan_test_dos_contenedores_compartidos PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
    target_link_libraries(
        mictlan_test_dos_contenedores_compartidos PRIVATE liblatino
    )
    add_test(
        NAME mictlan_memoria_dos_contenedores_asan
        COMMAND mictlan_test_dos_contenedores_compartidos
    )
    set_tests_properties(
        mictlan_memoria_dos_contenedores_asan
        PROPERTIES
            TIMEOUT 15
            ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
    )
endif()

# P0.2-F: contratos de vida de objetos, tambien en Debug/Release.
add_executable(mictlan_test_p02f_ciclo_vida tests/p02f_ciclo_vida.c)
target_include_directories(mictlan_test_p02f_ciclo_vida PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mictlan_test_p02f_ciclo_vida PRIVATE liblatino)
foreach(caso IN ITEMS compartidos anidados diccionarios ciclo_lista ciclo_mixto
                      extraer reasignar clonar_mixto error ffi clonar_ciclo)
  add_test(NAME mictlan_p02f_${caso}
           COMMAND mictlan_test_p02f_ciclo_vida ${caso})
  set_tests_properties(mictlan_p02f_${caso} PROPERTIES TIMEOUT 20
    LABELS "p02f;memoria;contrato"
    FAIL_REGULAR_EXPRESSION "P02F_ASSERT;AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")
endforeach()

# MICTLAN P0.2-F: regresiones de propiedad (no silenciar fallos de LSan).
# Estos casos se registran solo en builds instrumentados con ASan.
if(CMAKE_C_FLAGS MATCHES "fsanitize=address")
  foreach(caso IN ITEMS control_numero lista_simple sustitucion alias)
    add_test(
      NAME mictlan_p02f_${caso}_asan
      COMMAND $<TARGET_FILE:latino>
              "${CMAKE_CURRENT_SOURCE_DIR}/tests/p02f_${caso}.lat"
    )
    set_tests_properties(mictlan_p02f_${caso}_asan PROPERTIES
      TIMEOUT 15
      LABELS "p02f;memoria;diagnostico"
      ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1:halt_on_error=1"
      FAIL_REGULAR_EXPRESSION "ERROR: AddressSanitizer;ERROR: LeakSanitizer;runtime error:"
    )
  endforeach()
endif()
