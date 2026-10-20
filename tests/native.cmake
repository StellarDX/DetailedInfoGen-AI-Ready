add_executable(infogen_native_tests tests/native_regressions.cc ${PROJECT_SOURCES} ${PROJECT_HEADERS})
target_include_directories(infogen_native_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Headers"
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Sources"
    ${INFOGEN_DEP_INCLUDE_DIRS})
target_link_libraries(infogen_native_tests PRIVATE pybind11::embed ${INFOGEN_DEP_TARGETS})
foreach(_case initialization position temperature synchronous-temperature esi rotation kepler coplanar sequential binary-negative binary-apsis binary-wrap binary-period binary-single binary-frame binary-rounded binary-small-period binary-missing binary-epoch binary-eccentricity binary-retrograde binary-tie binary-empty)
    add_test(NAME native_${_case} COMMAND infogen_native_tests ${_case})
    set_tests_properties(native_${_case} PROPERTIES TIMEOUT 45
        ENVIRONMENT_MODIFICATION "PYTHONPATH=path_list_prepend:${Python3_SITELIB}")
endforeach()
set_tests_properties(native_kepler PROPERTIES TIMEOUT 5)
