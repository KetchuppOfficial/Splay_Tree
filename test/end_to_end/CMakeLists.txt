add_executable(driver ./src/driver.cpp)
add_executable(augmented_driver ./src/driver.cpp)
add_executable(ans_generator ./src/driver.cpp)
add_executable(generator ./src/generator.cpp)

target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR}/nodes
                           PRIVATE ${INCLUDE_DIR}/trees)

target_compile_definitions(driver
                           PRIVATE SPLAY_TREE)

target_include_directories(augmented_driver
                           PRIVATE ${INCLUDE_DIR}/nodes
                           PRIVATE ${INCLUDE_DIR}/trees)

target_compile_definitions(augmented_driver
                           PRIVATE AUGMENTED_SPLAY_TREE)

target_include_directories(ans_generator
                           PRIVATE ${INCLUDE_DIR}/nodes)

install(TARGETS driver augmented_driver generator ans_generator
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
