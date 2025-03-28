#include <gtest/gtest.h>
#include <vector>

#include "nodes/node.hpp"

TEST(Node, Constructors)
{
    std::vector vec{1, 2, 3, 4, 5};
    auto vec_copy = vec;

    yLab::Node node_1{nullptr, vec};
    EXPECT_EQ(node_1.get_key(), vec);

    yLab::Node node_2{nullptr, std::move(vec)};
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(node_2.get_key(), vec_copy);
}
