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

set(latCore TRUE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/")

project(latino)

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
include(LatLogo)
message(STATUS ".............................................." )
message(STATUS ">>>>>>   Latino ")
message(STATUS ".............................................." )
message(STATUS "Generador .............. ${CMAKE_GENERATOR}"    )
message(STATUS "Tipo de Build .......... ${CMAKE_BUILD_TYPE}"   )

include(GetLatVersion)
message(STATUS "Latino Version ......... ${LATINO_VERSION}"     )

# Los recursos .rc pertenecen al ejecutable de Windows.
# En Linux no necesitamos actualizar ese archivo.
if(WIN32)
    include(ResourceUpdate)
endif()

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
#se comentan los test que requieren entrada de datos leer()
add_test(NAME coverage COMMAND latino ejemplos/coverage.lat)

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
            "ASAN_OPTIONS=detect_leaks=0:halt_on_error=1"
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
                "ASAN_OPTIONS=detect_leaks=0:halt_on_error=1"
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