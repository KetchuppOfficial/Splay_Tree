add_executable(driver ./src/driver.cpp)
add_executable(augmented_driver ./src/driver.cpp)
add_executable(ans_generator ./src/driver.cpp)
add_executable(generator ./src/generator.cpp)

target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR})

target_compile_definitions(driver
                           PRIVATE SPLAY_TREE)

target_link_libraries(driver PRIVATE fmt::fmt)

target_include_directories(augmented_driver
                           PRIVATE ${INCLUDE_DIR})

target_compile_definitions(augmented_driver
                           PRIVATE AUGMENTED_SPLAY_TREE)

target_link_libraries(augmented_driver PRIVATE fmt::fmt)

target_include_directories(ans_generator
                           PRIVATE ${INCLUDE_DIR})

target_link_libraries(ans_generator PRIVATE fmt::fmt)

target_link_libraries(generator PRIVATE fmt::fmt Boost::program_options)
