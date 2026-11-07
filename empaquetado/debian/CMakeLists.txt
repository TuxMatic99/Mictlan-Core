
cmake_minimum_required(VERSION 3.20)

# Proyecto independiente para empaquetar Mictlán.
project(MictlanPaquete VERSION 0.1.0 LANGUAGES NONE)

# Localizar Core y el directorio general del proyecto.
get_filename_component(
    MICTLAN_CORE
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    ABSOLUTE
)

get_filename_component(
    MICTLAN_PROYECTO
    "${MICTLAN_CORE}/.."
    ABSOLUTE
)

# Utilizar el intérprete Release ya compilado.
set(
    MICTLAN_EJECUTABLE
    "${MICTLAN_PROYECTO}/build-core-release/mictlan"
)

if(NOT EXISTS "${MICTLAN_EJECUTABLE}")
    message(FATAL_ERROR
        "Falta el ejecutable Release de Mictlán."
    )
endif()

# Archivos que contendrá el paquete Debian.
install(
    PROGRAMS "${MICTLAN_EJECUTABLE}"
    DESTINATION bin
)


# Retirar simbolos de la copia que se empaqueta.
# El ejecutable original de Release no se modifica.

install(CODE [[
    set(_binario
        "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/mictlan"
    )

    if(NOT EXISTS "${_binario}")
        message(FATAL_ERROR
            "No existe el ejecutable instalado: ${_binario}"
        )
    endif()

    find_program(_strip NAMES strip REQUIRED)

    execute_process(
        COMMAND "${_strip}" --strip-unneeded "${_binario}"
        RESULT_VARIABLE _resultado
    )

    if(NOT _resultado EQUAL 0)
        message(FATAL_ERROR
            "No se pudieron retirar los simbolos."
        )
    endif()
]])


install(
    FILES "${MICTLAN_CORE}/ejemplos/hola_mictlan.lat"
    DESTINATION share/mictlan/ejemplos
)

install(
    FILES "${MICTLAN_CORE}/LICENSE"
    DESTINATION share/doc/mictlan
    RENAME copyright
)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
    DESTINATION share/doc/mictlan
    RENAME README.Debian
)

# Configuración específica de nuestro paquete.
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "mictlan")
set(CPACK_PACKAGE_VERSION "0.1.0")

# Revisión preliminar anterior al lanzamiento estable.
set(CPACK_DEBIAN_PACKAGE_RELEASE "0~alpha1")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Interprete de programacion Mictlan en espanol"
)

set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "Interprete experimental de proposito general basado en Latino."
)

# Contacto provisional SOLO para pruebas locales.
set(CPACK_DEBIAN_PACKAGE_MAINTAINER
    "Tuxmatic <videosexperimentalesmarioia@gmail.com>"
)

set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# Instalar dentro de /usr cuando el paquete sea instalado.
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

# Detectar las bibliotecas compartidas necesarias.
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# Generar nombres de archivo compatibles con Debian.
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")



# Permisos seguros para directorios del paquete Debian.
# Equivalen a 755: propietario rwx, grupo r-x, otros r-x.

set(CPACK_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE
)


# Retirar simbolos innecesarios de los binarios
# incluidos en el paquete de distribucion.



# Generar una pagina de manual comprimida.

find_program(MICTLAN_GZIP NAMES gzip REQUIRED)

set(
    MICTLAN_MANUAL_COMPRIMIDO
    "${CMAKE_CURRENT_BINARY_DIR}/mictlan.1.gz"
)

execute_process(
    COMMAND
        "${MICTLAN_GZIP}"
        -9 -n -c
        "${CMAKE_CURRENT_SOURCE_DIR}/mictlan.1"

    OUTPUT_FILE "${MICTLAN_MANUAL_COMPRIMIDO}"
    RESULT_VARIABLE MICTLAN_GZIP_RESULT
)

if(NOT MICTLAN_GZIP_RESULT EQUAL 0)
    message(FATAL_ERROR
        "No se pudo comprimir el manual de Mictlan."
    )
endif()

install(
    FILES "${MICTLAN_MANUAL_COMPRIMIDO}"
    DESTINATION share/man/man1
)




# ============================================
# HISTORIAL DE VERSIONES DE MICTLAN
# ============================================

set(
    MICTLAN_CHANGELOG_GZ
    "${CMAKE_CURRENT_BINARY_DIR}/changelog.Debian.gz"
)

execute_process(
    COMMAND
        "${MICTLAN_GZIP}"
        -9 -n -c
        "${CMAKE_CURRENT_SOURCE_DIR}/changelog"

    OUTPUT_FILE "${MICTLAN_CHANGELOG_GZ}"
    RESULT_VARIABLE MICTLAN_CHANGELOG_RESULT
)

if(NOT MICTLAN_CHANGELOG_RESULT EQUAL 0)
    message(FATAL_ERROR
        "No se pudo comprimir el historial Debian."
    )
endif()

install(
    FILES "${MICTLAN_CHANGELOG_GZ}"
    DESTINATION share/doc/mictlan
)



include(CPack)
