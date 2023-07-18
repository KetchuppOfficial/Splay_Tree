#include <gtest/gtest.h>

#include "search_tree.hpp"
#include "splay_node.hpp"

using node_type = yLab::Splay_Node<int>;

template<typename Key_T>
using tree_type = yLab::Search_Tree<yLab::Splay_Node<Key_T>>;

TEST (Iterator, Check_Iterator_Concept)
{
    using node_type = yLab::Splay_Node<int>;
    static_assert (std::bidirectional_iterator<yLab::tree_iterator<node_type, yLab::Node_Base>>);
}

TEST (Iterator, Preincrement)
{
    tree_type<int> tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_3 = ++it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterator, Postincrement)
{
    tree_type<int> tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (3);
    auto it_1_copy = it_1;
    auto it_3 = it_1++;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterator, Predecrement)
{
    tree_type<int> tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_3 = --it_1;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_2);
}

TEST (Iterator, Postdecrement)
{
    tree_type<int> tree = {1, 2, 3, 4, 5};
    auto it_1 = tree.find (2);
    auto it_2 = tree.find (1);
    auto it_1_copy = it_1;
    auto it_3 = it_1--;

    EXPECT_EQ (it_1, it_2);
    EXPECT_EQ (it_3, it_1_copy);
}

TEST (Iterator, Dereference)
{
    tree_type<int> tree = {1, 2, 3, 4, 5};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
        EXPECT_EQ (*it, elem);
}

TEST (Iterator, Arrow)
{
    tree_type<std::pair<int, int>> tree = {{1, -1}, {2, -2}, {3, -3}};

    auto elem = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++elem)
    {
        EXPECT_EQ (it->first, elem);
        EXPECT_EQ (it->second, -elem);
    }  
}
