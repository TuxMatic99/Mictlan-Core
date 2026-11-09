
# Mictlan Core: verificar referencias y reasignacion.

if(NOT DEFINED MICTLAN_EJECUTABLE OR
   NOT DEFINED MICTLAN_PROGRAMA)
    message(FATAL_ERROR "Faltan parametros de prueba")
endif()

execute_process(
    COMMAND
        "${MICTLAN_EJECUTABLE}"
        "${MICTLAN_PROGRAMA}"
    RESULT_VARIABLE estado
    OUTPUT_VARIABLE salida
    ERROR_VARIABLE errores
    TIMEOUT 10
)

if(NOT "${estado}" STREQUAL "0")
    message(FATAL_ERROR
        "El programa fallo: ${estado}\n${errores}")
endif()

string(REPLACE "\r\n" "\n" salida "${salida}")
string(STRIP "${salida}" salida)

string(JOIN "\n" esperado
    "=== LISTAS COMPARTIDAS ==="
    "Original despues de modificar alias:"
    "99"
    "=== REASIGNACION ==="
    "Alias despues de reasignar original:"
    "99"
    "Nueva lista original:"
    "7"
    "Segundo elemento del alias:"
    "77"
    "=== FIN ==="
)

if(NOT "${salida}" STREQUAL "${esperado}")
    message(FATAL_ERROR
        "Salida incorrecta.\nEsperada:\n${esperado}\nObtenida:\n${salida}")
endif()

message(STATUS "MICTLAN_LISTAS_REFERENCIAS_OK")
