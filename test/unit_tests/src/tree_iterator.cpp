#include <gtest/gtest.h>

#include "splay_tree.hpp"

TEST (Iterator, Check_Iterator_Concept)
{
    using node_type = yLab::Splay_Node<int>;
    static_assert (std::bidirectional_iterator<yLab::tree_iterator<node_type, yLab::Node_Base>>);
}

TEST (Iterator, Preincrement)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_3 = ++it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterator, Postincrement)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_1_copy = it_1;
    auto it_3 = it_1++;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterator, Predecrement)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_3 = --it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterator, Postdecrement)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_1_copy = it_1;
    auto it_3 = it_1--;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterator, Dereference)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
        EXPECT_EQ (*it, elem);
}

TEST (Iterator, Arrow)
{
    yLab::Splay_Tree<std::pair<int, int>> tree = {{1, -1}, {2, -2}, {3, -3}};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
    {
        EXPECT_EQ (it->first, elem);
        EXPECT_EQ (it->second, -elem);
    }  
}
