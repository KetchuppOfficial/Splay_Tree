/*
 * Tests for ordinary splay tree are not provided as they are just copies of the following tests
 * without verifying sizes of subtrees
 */

#include <gtest/gtest.h>
#include <vector>
#include <set>
#include <numeric>
#include <algorithm>

#include "trees.hpp"

using key_type = int;
using tree_type = yLab::Augmented_Splay_Tree<key_type>;

// Constructors and operator=

TEST(Augmented_Splay_Tree, Constructors)
{
    tree_type empty_tree;
    EXPECT_EQ(empty_tree.size(), 0);
    EXPECT_TRUE(empty_tree.empty());
    EXPECT_EQ(empty_tree.begin(), empty_tree.end());
}

TEST(Augmented_Splay_Tree, Comparator_Constructor)
{
    using custom_comparator = std::greater<key_type>;

    yLab::Augmented_Splay_Tree<key_type, custom_comparator> empty_tree{custom_comparator{}};

    EXPECT_EQ(empty_tree.size(), 0);
    EXPECT_TRUE(empty_tree.empty());
    EXPECT_EQ(empty_tree.begin(), empty_tree.end());
}

TEST(Augmented_Splay_Tree, Initializer_List_Constructor)
{
    tree_type tree{1, 2, 3, 2, 4, 1, 5};

    EXPECT_EQ(tree.size(), 5);
    EXPECT_FALSE(tree.empty());

    std::vector<key_type> vec;
    vec.resize(tree.size());
    std::iota(vec.begin(), vec.end(), 1);

    EXPECT_TRUE(std::ranges::equal(tree, vec));
    EXPECT_TRUE(tree.subtree_sizes_verifier());
}

TEST(Augmented_Splay_Tree, Iterator_Constructor)
{
    std::vector from{1, 2, 3, 2, 4, 1, 5};
    std::vector vec{1, 2, 3, 4, 5};

    tree_type tree(from.begin(), from.end());

    EXPECT_EQ(tree.size(), 5);
    EXPECT_FALSE(tree.empty());

    EXPECT_TRUE(std::ranges::equal(tree, vec));
    EXPECT_TRUE(tree.subtree_sizes_verifier());
}

TEST(Augmented_Splay_Tree, Copy_Constructor)
{
    tree_type tree{5, 2, 4, 1, 3, 7, 11};

    auto copy{tree};

    EXPECT_EQ(tree, copy);
    EXPECT_TRUE(copy.subtree_sizes_verifier());
}

TEST(Augmented_Splay_Tree, Move_Constructor)
{
    tree_type moved_from{3, 2, 1, 4, 5};
    auto moved_from_copy{moved_from};

    auto moved_to = std::move(moved_from);

    EXPECT_EQ(moved_to, moved_from_copy);
    EXPECT_TRUE(moved_to.subtree_sizes_verifier());
    EXPECT_EQ(moved_from.size(), 0);
    EXPECT_EQ(moved_from.begin(), moved_from.end());
}

TEST(Augmented_Splay_Tree, Move_Assignment)
{
    tree_type moved_from{1, 2, 3, 4, 5}, moved_to{6, 7, 8};
    auto moved_from_copy{moved_from};

    moved_to = std::move(moved_from);

    EXPECT_EQ(moved_to, moved_from_copy);
    EXPECT_TRUE(moved_to.subtree_sizes_verifier());
}

TEST(Augmented_Splay_Tree, Copy_Assignment)
{
    tree_type tree{5, 2, 4, 1, 3, 7, 11};
    tree_type copy;

    copy = tree;

    EXPECT_EQ(copy, tree);
    EXPECT_TRUE(copy.subtree_sizes_verifier());
}

// Lookup

TEST(Augmented_Splay_Tree, Find)
{
    tree_type tree{1, 2, 3, 4, 5, 6};

    auto key = 1;
    for (auto it = tree.begin(), ite = tree.end(); it != ite; ++it, ++key)
    {
        EXPECT_EQ(it, tree.find(key));
        EXPECT_TRUE(tree.subtree_sizes_verifier());
    }

    tree_type empty_tree;
    EXPECT_EQ(empty_tree.find(0), empty_tree.end());
}

TEST(Augmented_Splay_Tree, Contains)
{
    tree_type tree = {1, 2, 3, 4, 5, 6};

    EXPECT_FALSE(tree.contains(0));
    EXPECT_FALSE(tree.contains(7));
    EXPECT_TRUE(std::all_of(tree.begin(), tree.end(),
                            [&tree](auto &&key){ return tree.contains(key); }));

    tree_type empty_tree;
    EXPECT_FALSE(empty_tree.contains(0));
}

TEST(Augmented_Splay_Tree, Lower_Bound)
{
    tree_type tree = {1, 3};

    EXPECT_EQ(tree.lower_bound(0), tree.find(1));
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    EXPECT_EQ(tree.lower_bound(1), tree.find(1));
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    EXPECT_EQ(tree.lower_bound(2), tree.find(3));
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    EXPECT_EQ(tree.lower_bound(3), tree.find(3));
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    EXPECT_EQ(tree.lower_bound(4), tree.end());
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    tree_type empty_tree;
    EXPECT_EQ(empty_tree.lower_bound(0), empty_tree.end());
}

TEST(Augmented_Splay_Tree, Upper_Bound)
{
    tree_type tree = {1, 3};

    EXPECT_EQ(tree.upper_bound(0), tree.find(1));
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    EXPECT_EQ(tree.upper_bound(1), tree.find(3));
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    EXPECT_EQ(tree.upper_bound(2), tree.find(3));
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    EXPECT_EQ(tree.upper_bound(3), tree.end());
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    EXPECT_EQ(tree.upper_bound(4), tree.end());
    EXPECT_TRUE(tree.subtree_sizes_verifier());

    tree_type empty_tree;
    EXPECT_EQ(empty_tree.upper_bound(0), empty_tree.end());
}

// Modifiers

TEST(Augmented_Splay_Tree, Join)
{
    tree_type tree_1{1, 2, 3, 4, 5};
    tree_type tree_2{6, 7, 8, 9, 10};
    tree_type tree_3{5, 6, 7, 8, 9};
    tree_type empty_tree;
    auto ilist_5 = {1, 2, 3, 4, 5};
    auto ilist_10 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    bool res = tree_1.join(std::move(empty_tree)); // join empty tree: no effect

    EXPECT_TRUE(res);
    EXPECT_TRUE(std::ranges::equal(tree_1, ilist_5));
    EXPECT_EQ(tree_1.size(), 5);
    EXPECT_TRUE(tree_1.subtree_sizes_verifier());
    EXPECT_EQ(empty_tree, tree_type{});
    EXPECT_TRUE(empty_tree.empty());

    res = empty_tree.join(std::move(tree_1)); // join a tree to the empty one

    EXPECT_TRUE(res);
    EXPECT_TRUE(std::ranges::equal(empty_tree, ilist_5));
    EXPECT_EQ(empty_tree.size(), 5);
    EXPECT_TRUE(empty_tree.subtree_sizes_verifier());
    EXPECT_EQ(tree_1, tree_type{});
    EXPECT_TRUE(tree_1.empty());

    tree_1.insert({1, 2, 3, 4, 5});
    res = tree_1.join(std::move(tree_2)); // regular case

    EXPECT_TRUE(res);
    EXPECT_TRUE(std::ranges::equal(tree_1, ilist_10));
    EXPECT_EQ(tree_1.size(), 10);
    EXPECT_TRUE(tree_1.subtree_sizes_verifier());
    EXPECT_EQ(tree_2, tree_type{});
    EXPECT_TRUE(tree_2.empty());

     // trying to join trees which ranges of keys overlap
    res = tree_1.join(std::move(tree_3));
    EXPECT_FALSE(res);
}

#if 0
TEST(Augmented_Splay_Tree, Split)
{
    tree_type tree_1{1, 2, 3, 4, 5};
    auto ilist_1 = {1, 2, 3};
    auto ilist_2 = {4, 5};

    tree_type right_part = tree_1.split(3); // regular case

    EXPECT_TRUE(std::ranges::equal(tree_1, ilist_1));
    EXPECT_EQ(tree_1.size(), 3);

    EXPECT_TRUE(std::ranges::equal(right_part, ilist_2));
    EXPECT_EQ(right_part.size(), 2);

    // tree_1 == {1, 2, 3}

    tree_type empty_tree = tree_1.split(0); // key is less than any in the tree

    EXPECT_TRUE(std::ranges::equal(tree_1, ilist_1));
    EXPECT_EQ(tree_1.size(), 3);

    EXPECT_EQ(empty_tree, tree_type{});
    EXPECT_TRUE(empty_tree.empty());

    empty_tree = tree_1.split(4); // key is greater than any in the tree

    EXPECT_TRUE(std::ranges::equal(tree_1, ilist_1));
    EXPECT_EQ(tree_1.size(), 3);

    EXPECT_EQ(empty_tree, tree_type{});
    EXPECT_TRUE(empty_tree.empty());

    empty_tree = tree_1.split(3); // key is the greatest one in the tree

    EXPECT_TRUE(std::ranges::equal(tree_1, ilist_1));
    EXPECT_EQ(tree_1.size(), 3);

    EXPECT_EQ(empty_tree, tree_type{});
    EXPECT_TRUE(empty_tree.empty());
}
#endif

TEST(Augmented_Splay_Tree, Swap)
{
    tree_type tree_1{1, 2, 3, 4}, tree_2{5, 6, 7};
    auto copy_1{tree_1}, copy_2{tree_2};

    tree_1.swap(tree_2);

    EXPECT_EQ(tree_1, copy_2);
    EXPECT_EQ(tree_2, copy_1);
}

TEST(Augmented_Splay_Tree, Clear)
{
    tree_type tree{1, 2, 3, 4, 5, 6}, empty_tree;

    tree.clear();

    EXPECT_EQ(tree, empty_tree);
}

TEST(Augmented_Splay_Tree, Insert_By_Key)
{
    tree_type tree;

    auto [it_1, is_inserted_1] = tree.insert(1);

    EXPECT_TRUE(is_inserted_1);
    EXPECT_EQ(tree.size(), 1);
    EXPECT_EQ(*it_1, 1);
    EXPECT_EQ(it_1, tree.begin());
    EXPECT_EQ(++it_1, tree.end());
    EXPECT_TRUE(tree.subtree_sizes_verifier());

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
    EXPECT_TRUE(tree.subtree_sizes_verifier());
}

TEST(Augmented_Splay_Tree, Insert_Range)
{
    std::set model{1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35};

    tree_type tree;
    tree.insert(model.begin(), model.end());

    EXPECT_TRUE(std::ranges::equal(tree, model));
    EXPECT_TRUE(tree.subtree_sizes_verifier());
}

TEST(Augmented_Splay_Tree, Insert_By_Initializer_List)
{
    auto ilist = {1, 6, 3, 7, 1, 8, 5, 3, 8, 35162, -46, 35};

    std::set<key_type> model;
    model.insert(ilist);

    tree_type tree;
    tree.insert(ilist);

    EXPECT_TRUE(std::ranges::equal(tree, model));
    EXPECT_TRUE(tree.subtree_sizes_verifier());
}

TEST(Augmented_Splay_Tree, Erase_By_Iterator)
{
    std::set model{15, 2, 1, 8, 3, 5, 7, 9, 4, 11};
    tree_type tree(model.begin(), model.end());

    auto it = tree.find(15);
    tree.erase(it);
    model.erase(15);

    EXPECT_TRUE(std::ranges::equal(tree, model));
    EXPECT_TRUE(tree.subtree_sizes_verifier());
}

TEST(Augmented_Splay_Tree, Erase_By_Key)
{
    std::vector<key_type> vec(1000);
    std::iota(vec.begin(), vec.end(), 1);

    for (auto key : vec)
    {
        tree_type tree(vec.begin(), vec.end());
        std::set model(vec.begin(), vec.end());

        auto is_erased = tree.erase(key);
        EXPECT_EQ(is_erased, 1);

        model.erase(key);
        EXPECT_TRUE(std::ranges::equal(tree, model));
        EXPECT_TRUE(tree.subtree_sizes_verifier());

        is_erased = tree.erase(-key);
        EXPECT_EQ(is_erased, 0);
        EXPECT_TRUE(std::ranges::equal(tree, model));
    }

    tree_type tree_2{1}, empty_tree;

    tree_2.erase(1);
    EXPECT_EQ(tree_2, empty_tree);
}

// Comparison

TEST(Augmented_Splay_Tree, Equality)
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

TEST(Augmented_Splay_Tree, Ordering)
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
