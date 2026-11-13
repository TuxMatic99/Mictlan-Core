string(REPEAT "x" 2048 texto)
execute_process(COMMAND "${MICTLAN_EJECUTABLE}" -e "poner(\"${texto}\")"
  RESULT_VARIABLE codigo OUTPUT_VARIABLE salida ERROR_VARIABLE diagnostico)
if(NOT codigo STREQUAL "0" OR NOT diagnostico STREQUAL "" OR NOT salida STREQUAL "${texto}\n")
  message(FATAL_ERROR "La expresion larga termino con ${codigo}\n${diagnostico}\n${salida}")
endif()
