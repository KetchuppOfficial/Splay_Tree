#include <gtest/gtest.h>
#include <utility>

#include "splay_tree.hpp"

TEST (Iterators, Check_Iterator_Concept)
{
    using node_type = yLab::Node<int>;
    static_assert (std::bidirectional_iterator<yLab::tree_iterator<node_type>>);
}

TEST (Iterators, Preincrement)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_3 = ++it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterators, Postincrement)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_1_copy = it_1;
    auto it_3 = it_1++;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterators, Predecrement)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_3 = --it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterators, Postdecrement)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_1_copy = it_1;
    auto it_3 = it_1--;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterators, Dereference)
{
    yLab::Splay_Tree tree = {1, 2, 3, 4, 5};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
        EXPECT_EQ (*it, elem);
}

TEST (Iterators, Arrow)
{
    yLab::Splay_Tree<std::pair<int, int>> tree = {{1, -1}, {2, -2}, {3, -3}};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
    {
        EXPECT_EQ (it->first, elem);
        EXPECT_EQ (it->second, -elem);
    }  
}
