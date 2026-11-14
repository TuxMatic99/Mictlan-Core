foreach(caso IN ITEMS numeros precedencia logica comparaciones funciones recursion
                      ambitos asignaciones condicional local_inaccesible argumentos_exceso)
  set(fuente "${CMAKE_CURRENT_LIST_DIR}/${caso}.mict")
  if(caso STREQUAL "logica")
    set(fuente "${CMAKE_CURRENT_LIST_DIR}/${caso}.lat")
  endif()
  set(codigo 0)
  set(diagnostico "")
  if(caso STREQUAL "local_inaccesible")
    set(codigo 1)
    set(diagnostico "Variable 'local_privada' indefinida")
  elseif(caso STREQUAL "argumentos_exceso")
    set(codigo 1)
    set(diagnostico "Numero invalido de argumentos en funcion 'identidad'")
  endif()
  foreach(extension IN ITEMS lat mict)
    add_test(NAME mictlan_p12a_${caso}_${extension} COMMAND "${CMAKE_COMMAND}"
      "-DMICTLAN_EJECUTABLE=$<TARGET_FILE:latino>" "-DFUENTE=${fuente}"
      "-DESPERADO=${CMAKE_CURRENT_LIST_DIR}/${caso}.out"
      "-DTRABAJO=${CMAKE_CURRENT_BINARY_DIR}/conformidad/${caso}-${extension}"
      "-DEXTENSION=${extension}" "-DCODIGO=${codigo}" "-DDIAGNOSTICO=${diagnostico}"
      -P "${CMAKE_CURRENT_LIST_DIR}/verificar.cmake")
    set_tests_properties(mictlan_p12a_${caso}_${extension} PROPERTIES
      TIMEOUT 25 LABELS "p12a;conformidad;compatibilidad")
  endforeach()
endforeach()

add_executable(mictlan_test_p12a_locale "${CMAKE_CURRENT_LIST_DIR}/locale_decimal.c")
target_include_directories(mictlan_test_p12a_locale PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(mictlan_test_p12a_locale PRIVATE liblatino)
foreach(locale IN ITEMS C es_ES.UTF-8 fr_FR.UTF-8)
  string(REPLACE "." "_" nombre "${locale}")
  add_test(NAME mictlan_p12a_locale_${nombre} COMMAND mictlan_test_p12a_locale "${locale}")
  set_tests_properties(mictlan_p12a_locale_${nombre} PROPERTIES
    TIMEOUT 20 LABELS "p12a;conformidad;locale" SKIP_RETURN_CODE 77
    FAIL_REGULAR_EXPRESSION "AddressSanitizer;UndefinedBehaviorSanitizer;LeakSanitizer;runtime error:")
endforeach()
