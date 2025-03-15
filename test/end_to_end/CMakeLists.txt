add_executable(driver ./src/driver.cpp)
target_include_directories(driver PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(generator ./src/generator.cpp)

set(TARGETS driver generator)
foreach(target ${TARGETS})
    target_compile_features(${target} PRIVATE cxx_std_23)
    target_link_libraries(${target}
    PRIVATE
        fmt::fmt
        CLI11::CLI11
    )
endforeach()

include(GNUInstallDirs)
install(TARGETS ${TARGETS})
