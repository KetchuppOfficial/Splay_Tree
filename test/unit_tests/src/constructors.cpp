#include <gtest/gtest.h>
#include <algorithm>

#include "splay_tree.hpp"

TEST (Constructors, Default_Constructor)
{
    const yLab::Splay_Tree<int> tree;
    EXPECT_EQ (tree.size(), 0);
    EXPECT_TRUE (tree.empty());
    EXPECT_EQ (tree.begin(), tree.end());
    EXPECT_EQ (tree.lower_bound(1), tree.end());
    EXPECT_EQ (tree.upper_bound(1), tree.end());
}

TEST (Constructors, Copy_Constructor)
{
    yLab::Splay_Tree<int> tree {1, 2, 3, 4, 5};

    auto tree_2 {tree};
    EXPECT_EQ (tree, tree_2);
}

TEST (Constructors, Move_Constructor)
{
    yLab::Splay_Tree tree = {3, 2, 1, 4, 5};
    std::vector vec = {1, 2, 3, 4, 5};

    auto tree_2 = std::move (tree);
    EXPECT_EQ (tree_2.size(), 5);
    EXPECT_TRUE (std::equal (tree.begin(), tree.end(), vec.begin()));
}
