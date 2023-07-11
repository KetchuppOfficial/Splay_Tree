#include <gtest/gtest.h>

#include "nodes.hpp"

TEST (End_Node, Move_Constructor)
{
    int x{};
    yLab::End_Node end_node_1{&x};
    auto end_node_2 = std::move (end_node_1);

    EXPECT_EQ (end_node_1.get_left(), nullptr);
    EXPECT_EQ (end_node_2.get_left(), &x);
}

TEST (End_Node, Move_Assignment)
{
    int x{};
    yLab::End_Node end_node_1{&x};

    int y{};
    yLab::End_Node end_node_2{&y};

    end_node_2 = std::move (end_node_1);
    EXPECT_EQ (end_node_1.get_left(), &y);
    EXPECT_EQ (end_node_2.get_left(), &x);
}

TEST (Node, Move_Constructor)
{
    using key_type = int;

    yLab::Node node_1{1};
    yLab::Node left{2};
    yLab::Node right{3};
    yLab::Node parent{4};
    node_1.set_left (&left);
    node_1.set_right (&right);
    node_1.set_parent (&parent);

    auto node_2 = std::move (node_1);

    EXPECT_EQ (node_1.get_left(), nullptr);
    EXPECT_EQ (node_1.get_right(), nullptr);
    EXPECT_EQ (node_1.get_parent(), nullptr);
    EXPECT_EQ (node_1.key(), 1);

    EXPECT_EQ (node_2.get_left(), &left);
    EXPECT_EQ (node_2.get_right(), &right);
    EXPECT_EQ (node_2.get_parent(), &parent);
    EXPECT_EQ (node_2.key(), 1);
}

TEST (Node, Move_Assignment)
{
    using key_type = int;

    yLab::Node node_1{1};
    yLab::Node left_1{2};
    yLab::Node right_1{3};
    yLab::Node parent_1{4};
    node_1.set_left (&left_1);
    node_1.set_right (&right_1);
    node_1.set_parent (&parent_1);

    yLab::Node node_2{5};
    yLab::Node left_2{6};
    yLab::Node right_2{7};
    yLab::Node parent_2{8};
    node_2.set_left (&left_2);
    node_2.set_right (&right_2);
    node_2.set_parent (&parent_2);

    node_2 = std::move (node_1);

    EXPECT_EQ (node_1.get_left(), &left_2);
    EXPECT_EQ (node_1.get_right(), &right_2);
    EXPECT_EQ (node_1.get_parent(), &parent_2);
    EXPECT_EQ (node_1.key(), 5);

    EXPECT_EQ (node_2.get_left(), &left_1);
    EXPECT_EQ (node_2.get_right(), &right_1);
    EXPECT_EQ (node_2.get_parent(), &parent_1);
    EXPECT_EQ (node_2.key(), 1);
}
