#include <gtest/gtest.h>
#include <vector>
#include <set>
#include <numeric>
#include <algorithm>

#include "nodes/node.hpp"
#include "trees/search_tree.hpp"

using key_type = int;
using node_type = yLab::Node<key_type>;
using tree_type = yLab::Search_Tree<node_type>;

// Constructors and operator=

TEST(Search_Tree, Constructors)
{
    tree_type empty_tree;
    EXPECT_EQ(empty_tree.size(), 0);
    EXPECT_TRUE(empty_tree.empty());
    EXPECT_EQ(empty_tree.begin(), empty_tree.end());
}

TEST(Search_Tree, Comparator_Constructor)
{
    using custom_comp = std::greater<key_type>;

    yLab::Search_Tree<node_type, custom_comp> empty_tree{custom_comp{}};

    EXPECT_EQ(empty_tree.size(), 0);
    EXPECT_TRUE(empty_tree.empty());
    EXPECT_EQ(empty_tree.begin(), empty_tree.end());
}

TEST(Search_Tree, Initializer_List_Constructor)
{
    tree_type tree{1, 2, 3, 2, 4, 1, 5};

    EXPECT_EQ(tree.size(), 5);
    EXPECT_FALSE(tree.empty());

    std::vector<key_type> vec;
    vec.resize(tree.size());
    std::iota(vec.begin(), vec.end(), 1);

    EXPECT_TRUE(std::equal(tree.begin(), tree.end(), vec.begin()));
}

TEST(Search_Tree, Iterator_Constructor)
{
    std::vector from{1, 2, 3, 2, 4, 1, 5};
    std::vector vec{1, 2, 3, 4, 5};

    tree_type tree(from.begin(), from.end());

    EXPECT_EQ(tree.size(), 5);
    EXPECT_FALSE(tree.empty());

    EXPECT_TRUE(std::equal(tree.begin(), tree.end(), vec.begin()));
}

TEST(Search_Tree, Copy_Constructor)
{
    tree_type tree{5, 2, 4, 1, 3, 7, 11};

    auto copy{tree};

    EXPECT_EQ(tree, copy);
}

TEST(Search_Tree, Move_Constructor)
{
    tree_type moved_from{3, 2, 1, 4, 5};
    auto moved_from_copy{moved_from};

    auto moved_to = std::move(moved_from);

    EXPECT_EQ(moved_to, moved_from_copy);
    EXPECT_EQ(moved_from.size(), 0);
    EXPECT_EQ(moved_from.begin(), moved_from.end());

    tree_type empty;
    auto moved_to_2 = std::move(empty);
    EXPECT_EQ(empty, moved_to_2);
}

TEST(Search_Tree, Move_Assignment)
{
    tree_type moved_from{1, 2, 3, 4, 5}, moved_to{6, 7, 8};
    auto moved_from_copy{moved_from};

    moved_to = std::move(moved_from);

    EXPECT_EQ(moved_to, moved_from_copy);
}

TEST(Search_Tree, Copy_Assignment)
{
    tree_type tree{5, 2, 4, 1, 3, 7, 11};
    tree_type copy;

    copy = tree;

    EXPECT_EQ(copy, tree);
}

// Lookup

TEST(Search_Tree, Find)
{
    tree_type tree{1, 2, 3, 4, 5, 6};

    auto key = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++key)
        EXPECT_EQ(it, tree.find(key));

    tree_type empty_tree;
    EXPECT_EQ(empty_tree.find(0), empty_tree.end());
}

TEST(Search_Tree, Contains)
{
    tree_type tree = {1, 2, 3, 4, 5, 6};

    EXPECT_FALSE(tree.contains(0));
    EXPECT_FALSE(tree.contains(7));
    EXPECT_TRUE(std::all_of(tree.begin(), tree.end(),
                            [&tree](auto &&key){ return tree.contains(key); }));

    tree_type empty_tree;
    EXPECT_FALSE(empty_tree.contains(0));
}

TEST(Search_Tree, Lower_Bound)
{
    tree_type tree = {1, 3};

    EXPECT_EQ(tree.lower_bound(0), tree.find(1));
    EXPECT_EQ(tree.lower_bound(1), tree.find(1));
    EXPECT_EQ(tree.lower_bound(2), tree.find(3));
    EXPECT_EQ(tree.lower_bound(3), tree.find(3));
    EXPECT_EQ(tree.lower_bound(4), tree.end());

    tree_type empty_tree;
    EXPECT_EQ(empty_tree.lower_bound(0), empty_tree.end());
}

TEST(Search_Tree, Upper_Bound)
{
    tree_type tree = {1, 3};

    EXPECT_EQ(tree.upper_bound(0), tree.find(1));
    EXPECT_EQ(tree.upper_bound(1), tree.find(3));
    EXPECT_EQ(tree.upper_bound(2), tree.find(3));
    EXPECT_EQ(tree.upper_bound(3), tree.end());
    EXPECT_EQ(tree.upper_bound(4), tree.end());

    tree_type empty_tree;
    EXPECT_EQ(empty_tree.upper_bound(0), empty_tree.end());
}

// Modifiers

TEST(Search_Tree, Swap)
{
    tree_type tree_1{1, 2, 3, 4}, tree_2{5, 6, 7};
    auto copy_1{tree_1}, copy_2{tree_2};

    tree_1.swap(tree_2);

    EXPECT_EQ(tree_1, copy_2);
    EXPECT_EQ(tree_2, copy_1);
}

TEST(Search_Tree, Clear)
{
    tree_type tree{1, 2, 3, 4, 5, 6}, empty_tree;

    tree.clear();

    EXPECT_EQ(tree, empty_tree);
}

TEST(Search_Tree, Insert_By_Key)
{
    tree_type tree;

    auto [it_1, is_inserted_1] = tree.insert(1);

    EXPECT_TRUE(is_inserted_1);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(*it_1, 1);
    EXPECT_EQ(it_1, tree.begin());
    EXPECT_EQ(++it_1, tree.end());

    auto [it_2, is_inserted_2] = tree.insert(1);

    EXPECT_FALSE(is_inserted_2);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(*it_2, 1);
    EXPECT_EQ(it_2, tree.begin());
    EXPECT_EQ(std::next(it_2), tree.end());

    auto [it_3, is_inserted_3] = tree.insert(2);

    EXPECT_TRUE(is_inserted_3);
    EXPECT_EQ(tree.size(), 2);
    EXPECT_EQ(*it_3, 2);
    EXPECT_EQ(it_3, std::next(tree.begin()));
    EXPECT_EQ(std::next(it_3), tree.end());
}

TEST(Search_Tree, Insert_Range)
{
    std::set model{1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35};

    tree_type tree;
    tree.insert(model.begin(), model.end());

    EXPECT_TRUE(std::equal(tree.begin(), tree.end(), model.begin(), model.end()));
}

TEST(Search_Tree, Insert_By_Initializer_List)
{
    auto ilist = {1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35};

    std::set<key_type> model;
    model.insert(ilist);

    tree_type tree;
    tree.insert(ilist);

    EXPECT_TRUE(std::equal(tree.begin(), tree.end(), model.begin(), model.end()));
}

TEST(Search_Tree, Erase_By_Iterator)
{
    std::set<int> model{15, 2, 1, 8, 3, 5, 7, 9, 4, 11};
    tree_type tree{model.begin(), model.end()};

    auto it = tree.find(15);
    tree.erase(it);
    model.erase(15);

    EXPECT_TRUE(std::equal(tree.begin(), tree.end(), model.begin(), model.end()));
}

TEST(Search_Tree, Erase_By_Key)
{
    std::vector<key_type> vec(1000);
    std::iota(vec.begin(), vec.end(), 1);

    for (auto key : vec)
    {
        tree_type tree{vec.begin(), vec.end()};
        std::set<key_type> model{vec.begin(), vec.end()};

        auto is_erased = tree.erase(key);
        EXPECT_EQ(is_erased, 1);

        model.erase(key);
        EXPECT_TRUE(std::equal(tree.begin(), tree.end(), model.begin(), model.end()));

        is_erased = tree.erase(-key);
        EXPECT_EQ(is_erased, 0);
        EXPECT_TRUE(std::equal(tree.begin(), tree.end(), model.begin(), model.end()));
    }

    tree_type tree_2{1}, empty_tree;

    tree_2.erase(1);
    EXPECT_EQ(tree_2, empty_tree);
}

TEST(Search_Tree, Equality)
{
    tree_type tree_1{1, 2, 3, 4, 5, 6};
    tree_type tree_2{1, 2, 3, 4, 5};
    tree_type tree_3{1, 2, 3, 4, 5, 6, 7};
    auto tree_4{tree_1};

    EXPECT_EQ(tree_1, tree_1);
    EXPECT_EQ(tree_1, tree_4);
    EXPECT_EQ(tree_4, tree_1);

    EXPECT_NE(tree_1, tree_2);
    EXPECT_NE(tree_2, tree_1);
    EXPECT_NE(tree_2, tree_3);
    EXPECT_NE(tree_3, tree_2);
    EXPECT_NE(tree_3, tree_1);
    EXPECT_NE(tree_1, tree_3);
}

TEST(Search_Tree, Ordering)
{
    tree_type tree_1{1, 2, 3, 4, 5};
    tree_type tree_2{1, 2, 3, 4, 5, 6};
    tree_type tree_3{1, 2, 3, 4};
    tree_type tree_4{1, 2, 3, 4, 6};
    tree_type tree_5{1, 2, 3, 4, -1};

    EXPECT_LT(tree_1, tree_2);
    EXPECT_GE(tree_2, tree_1);

    EXPECT_GT(tree_1, tree_3);
    EXPECT_LE(tree_3, tree_1);

    EXPECT_LT(tree_1, tree_4);
    EXPECT_GE(tree_4, tree_1);

    EXPECT_GT(tree_1, tree_5);
    EXPECT_LE(tree_5, tree_1);
}
