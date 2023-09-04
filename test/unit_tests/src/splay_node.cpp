#include <gtest/gtest.h>
#include <vector>

#include "splay_node.hpp"

TEST (Splay_Node, Constructors)
{
    std::vector vec{1, 2, 3, 4, 5};
    auto vec_copy = vec;

    yLab::Splay_Node node_1{vec};
    EXPECT_EQ (node_1.get_key(), vec);

    yLab::Splay_Node node_2{std::move (vec)};
    EXPECT_TRUE (vec.empty());
    EXPECT_EQ (node_2.get_key(), vec_copy);
}
