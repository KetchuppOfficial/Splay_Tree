cmake_minimum_required(VERSION 3.20)

project(HWT_LVL_2 LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_subdirectory(test)
