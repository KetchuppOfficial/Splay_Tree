cmake_minimum_required(VERSION 3.20)

project(HWT_LVL_2
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(INSTALL_DIR ${PROJECT_SOURCE_DIR}/bin)

find_package(fmt REQUIRED)
find_package(CLI11 REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

enable_testing()

add_subdirectory(test)
