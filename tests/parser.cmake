file(GLOB PARSER_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Sources/Mini-NeoCSE/ANTLR/*.cpp")
add_executable(infogen_parser_tests tests/parser_regressions.cc
    InfoGen_Data/Sources/Mini-NeoCSE/ISCStream.cc ${PARSER_TEST_SOURCES})
target_include_directories(infogen_parser_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/InfoGen_Data/Sources" ${INFOGEN_DEP_INCLUDE_DIRS})
target_link_libraries(infogen_parser_tests PRIVATE ${INFOGEN_DEP_TARGETS})
foreach(_case missing invalid-token trailing real-overflow int-overflow signed-min valid-values repeated-blocks)
    add_test(NAME parser_${_case} COMMAND infogen_parser_tests ${_case})
    set_tests_properties(parser_${_case} PROPERTIES TIMEOUT 10)
endforeach()
foreach(_fixture "Kepler-1512.sc" "Gaia DR3 3422042582096699520.sc" "Gaia DR3 249928171344223232.sc")
    add_test(NAME "parser_${_fixture}" COMMAND infogen_parser_tests real
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/spaceengine/se-0.991-real/${_fixture}")
endforeach()
