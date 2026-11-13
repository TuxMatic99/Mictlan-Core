foreach(caso IN ITEMS identidad extensiones latino_valores latino_control latino_colecciones
                     diagnosticos repl modulos)
  add_test(NAME mictlan_alpha_${caso} COMMAND "${CMAKE_COMMAND}"
    "-DMICTLAN_EJECUTABLE=$<TARGET_FILE:latino>" "-DCASO=${caso}"
    "-DTRABAJO=${CMAKE_CURRENT_BINARY_DIR}/alpha-tests/${caso}"
    "-DVERSION=${MICTLAN_VERSION}" -P "${CMAKE_CURRENT_LIST_DIR}/verificar.cmake")
  set_tests_properties(mictlan_alpha_${caso} PROPERTIES TIMEOUT 45 LABELS "alpha;compatibilidad")
endforeach()
