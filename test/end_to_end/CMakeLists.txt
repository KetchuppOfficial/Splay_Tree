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

add_executable(std_driver ./src/driver.cpp)
target_include_directories(std_driver PRIVATE ${INCLUDE_DIR})
target_compile_definitions(std_driver PRIVATE ANSWERS)
target_link_libraries(std_driver PRIVATE fmt::fmt)

add_executable(std_stopwatch ./src/driver.cpp)
target_include_directories(std_stopwatch PRIVATE ${INCLUDE_DIR})
target_compile_definitions(std_stopwatch PRIVATE TIME)
target_link_libraries(std_stopwatch PRIVATE fmt::fmt)

add_executable(generator ./src/generator.cpp)
target_link_libraries(generator PRIVATE fmt::fmt CLI11::CLI11)

install(TARGETS driver stopwatch
                augmented_driver augmented_stopwatch
                std_driver std_stopwatch
                generator
        DESTINATION ${INSTALL_DIR})
