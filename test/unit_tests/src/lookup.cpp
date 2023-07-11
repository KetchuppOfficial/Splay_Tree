#include <gtest/gtest.h>
#include <iterator>

#include "splay_tree.hpp"

TEST (Lookup, Find)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5, 6};

    auto key = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++key)
        EXPECT_EQ (it, tree.find (key));

    yLab::Splay_Tree<int> empty_tree;
    EXPECT_EQ (empty_tree.find(0), empty_tree.end());
}

TEST (Lookup, Contains)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5, 6};

    EXPECT_FALSE (tree.contains (0));
    EXPECT_FALSE (tree.contains (7));
    EXPECT_TRUE (std::all_of (tree.begin(), tree.end(),
                              [&tree](auto &&key){ return tree.contains(key); }));

    yLab::Splay_Tree<int> empty_tree;
    EXPECT_FALSE (empty_tree.contains(0));
}

TEST (Lookup, Lower_Bound)
{
    yLab::Splay_Tree tree = {1, 3};

    EXPECT_EQ (tree.lower_bound (0), tree.find (1));
    EXPECT_EQ (tree.lower_bound (1), tree.find (1));
    EXPECT_EQ (tree.lower_bound (2), tree.find (3));
    EXPECT_EQ (tree.lower_bound (3), tree.find (3));
    EXPECT_EQ (tree.lower_bound (4), tree.end());

    yLab::Splay_Tree<int> empty_tree;
    EXPECT_EQ (empty_tree.lower_bound (0), empty_tree.end());
}

TEST (Lookup, Upper_Bound)
{
    yLab::Splay_Tree tree = {1, 3};

    EXPECT_EQ (tree.upper_bound (0), tree.find (1));
    EXPECT_EQ (tree.upper_bound (1), tree.find (3));
    EXPECT_EQ (tree.upper_bound (2), tree.find (3));
    EXPECT_EQ (tree.upper_bound (3), tree.end());
    EXPECT_EQ (tree.upper_bound (4), tree.end());

    yLab::Splay_Tree<int> empty_tree;
    EXPECT_EQ (empty_tree.upper_bound (0), empty_tree.end());
}
