add_executable(driver ./src/driver.cpp)
target_include_directories(driver PRIVATE ${INCLUDE_DIR})
target_compile_definitions(driver PRIVATE SPLAY_TREE ANSWERS)
target_link_libraries(driver PRIVATE fmt::fmt)

add_executable(stopwatch ./src/driver.cpp)
target_include_directories(stopwatch PRIVATE ${INCLUDE_DIR})
target_compile_definitions(stopwatch PRIVATE SPLAY_TREE TIME)
target_link_libraries(stopwatch PRIVATE fmt::fmt)

add_executable(augmented_driver ./src/driver.cpp)
target_include_directories(augmented_driver PRIVATE ${INCLUDE_DIR})
target_compile_definitions(augmented_driver PRIVATE AUGMENTED_SPLAY_TREE ANSWERS)
target_link_libraries(augmented_driver PRIVATE fmt::fmt)

add_executable(augmented_stopwatch ./src/driver.cpp)
target_include_directories(augmented_stopwatch PRIVATE ${INCLUDE_DIR})
target_compile_definitions(augmented_stopwatch PRIVATE AUGMENTED_SPLAY_TREE TIME)
target_link_libraries(augmented_stopwatch PRIVATE fmt::fmt)

add_executable(ans_generator ./src/driver.cpp)
target_include_directories(ans_generator PRIVATE ${INCLUDE_DIR})
target_compile_definitions(ans_generator PRIVATE ANSWERS)
target_link_libraries(ans_generator PRIVATE fmt::fmt)

add_executable(std_stopwatch ./src/driver.cpp)
target_include_directories(std_stopwatch PRIVATE ${INCLUDE_DIR})
target_compile_definitions(std_stopwatch PRIVATE TIME)
target_link_libraries(std_stopwatch PRIVATE fmt::fmt)

add_executable(generator ./src/generator.cpp)
target_link_libraries(generator PRIVATE fmt::fmt Boost::program_options)

install(TARGETS driver
                augmented_driver
                stopwatch
                augmented_stopwatch
                std_stopwatch
                ans_generator
                generator
        DESTINATION ${INSTALL_DIR})
