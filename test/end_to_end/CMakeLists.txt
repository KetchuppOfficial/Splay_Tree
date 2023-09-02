add_executable(driver ./src/driver.cpp)
add_executable(generator ./src/generator.cpp)
add_executable(ans_generator ./src/driver.cpp)

target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR})

target_compile_options(driver
                       PRIVATE -DSPLAY_TREE)

target_include_directories(generator
                           PRIVATE ${INCLUDE_DIR})

target_include_directories(ans_generator
                           PRIVATE ${INCLUDE_DIR})

install(TARGETS driver generator ans_generator
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
