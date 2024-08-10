aux_source_directory(./src SRC_LIST)

add_executable(unit_tests ${SRC_LIST})

target_link_libraries(unit_tests
                      PRIVATE ${GTEST_BOTH_LIBRARIES}
                      PRIVATE ${CMAKE_THREAD_LIBS_INIT}
                      PRIVATE fmt::fmt)

target_include_directories(unit_tests
                           PRIVATE ${INCLUDE_DIR})

gtest_discover_tests(unit_tests)
