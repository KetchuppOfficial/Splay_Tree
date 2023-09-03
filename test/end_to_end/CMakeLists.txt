add_executable(driver ./src/driver.cpp)
add_executable(generator ./src/generator.cpp)
add_executable(ans_generator ./src/driver.cpp)

target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR}
                           PRIVATE ${INCLUDE_DIR}/trees)

target_compile_definitions(driver
                           PRIVATE SPLAY_TREE)

target_include_directories(ans_generator
                           PRIVATE ${INCLUDE_DIR})

install(TARGETS driver generator ans_generator
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
