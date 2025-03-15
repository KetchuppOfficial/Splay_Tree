add_executable(unit_tests
    src/base_node.cpp
    src/node.cpp
    src/augmented_node.cpp
    src/tree_iterator.cpp
    src/search_tree.cpp
    src/augmented_splay_tree.cpp
)

target_compile_features(unit_tests PRIVATE cxx_std_23)

target_link_libraries(unit_tests
PRIVATE
    GTest::GTest
    GTest::gtest_main
    Threads::Threads
    fmt::fmt
)

target_include_directories(unit_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(unit_tests)
