#include <gtest/gtest.h>

#include "node_base.hpp"

TEST (Node_Base, Constructor)
{
    // default constructor
    yLab::Node_Base node_0{};

    EXPECT_EQ (node_0.get_left(), nullptr);
    EXPECT_EQ (node_0.get_right(), nullptr);
    EXPECT_EQ (node_0.get_parent(), nullptr);

    yLab::Node_Base node_1{&node_0};

    EXPECT_EQ (node_1.get_left(), &node_0);
    EXPECT_EQ (node_1.get_right(), nullptr);
    EXPECT_EQ (node_1.get_parent(), nullptr);

    yLab::Node_Base node_2{&node_0, &node_1};

    EXPECT_EQ (node_2.get_left(), &node_0);
    EXPECT_EQ (node_2.get_right(), &node_1);
    EXPECT_EQ (node_2.get_parent(), nullptr);

    yLab::Node_Base node_3{&node_0, &node_1, &node_2};

    EXPECT_EQ (node_3.get_left(), &node_0);
    EXPECT_EQ (node_3.get_right(), &node_1);
    EXPECT_EQ (node_3.get_parent(), &node_2);
}

TEST (Node_Base, Is_Left_Right_Child)
{
    yLab::Node_Base left{}, right{};
    yLab::Node_Base node{&left, &right};
    left.set_parent (&node);
    right.set_parent (&node);

    EXPECT_TRUE (left.is_left_child());
    EXPECT_TRUE (right.is_right_child());

    EXPECT_FALSE (left.is_right_child());
    EXPECT_FALSE (right.is_left_child());
}

/*
 *    +-- 4 --+
 *    |       |
 *    2       6
 *   / \     / \.
 *  1   3   5   7
 */
TEST (Node_Base, Basic_Queries)
{
    yLab::Node_Base node_1{}, node_3{};
    yLab::Node_Base node_2{&node_1, &node_3};
    node_1.set_parent (&node_2);
    node_3.set_parent (&node_2);

    yLab::Node_Base node_5{}, node_7{};
    yLab::Node_Base node_6{&node_5, &node_7};
    node_5.set_parent (&node_6);
    node_7.set_parent (&node_6);

    yLab::Node_Base node_4{&node_2, &node_6};
    node_2.set_parent (&node_4);
    node_6.set_parent (&node_4);

    yLab::Node_Base end_node{&node_4};
    node_4.set_parent (&end_node);

    // Minimum and maximum

    EXPECT_EQ (node_1.minimum(), &node_1);
    EXPECT_EQ (node_1.maximum(), &node_1);

    EXPECT_EQ (node_2.minimum(), &node_1);
    EXPECT_EQ (node_2.maximum(), &node_3);

    EXPECT_EQ (node_3.minimum(), &node_3);
    EXPECT_EQ (node_3.maximum(), &node_3);

    EXPECT_EQ (node_4.minimum(), &node_1);
    EXPECT_EQ (node_4.maximum(), &node_7);

    EXPECT_EQ (node_5.minimum(), &node_5);
    EXPECT_EQ (node_5.maximum(), &node_5);

    EXPECT_EQ (node_6.minimum(), &node_5);
    EXPECT_EQ (node_6.maximum(), &node_7);

    EXPECT_EQ (node_7.minimum(), &node_7);
    EXPECT_EQ (node_7.maximum(), &node_7);

    // Successor and predecessor

    // node_1.predecessor() is undefined
    EXPECT_EQ (node_1.successor(), &node_2);

    EXPECT_EQ (node_2.predecessor(), &node_1);
    EXPECT_EQ (node_2.successor(), &node_3);

    EXPECT_EQ (node_3.predecessor(), &node_2);
    EXPECT_EQ (node_3.successor(), &node_4);

    EXPECT_EQ (node_4.predecessor(), &node_3);
    EXPECT_EQ (node_4.successor(), &node_5);

    EXPECT_EQ (node_5.predecessor(), &node_4);
    EXPECT_EQ (node_5.successor(), &node_6);

    EXPECT_EQ (node_6.predecessor(), &node_5);
    EXPECT_EQ (node_6.successor(), &node_7);

    EXPECT_EQ (node_7.predecessor(), &node_6);
    EXPECT_EQ (node_7.successor(), &end_node);
}

// Look at the picture in node_base.hpp
TEST (Node_Base, Left_Rotate)
{
    yLab::Node_Base a{}, b{}, c{};
    yLab::Node_Base y{&b, &c};
    yLab::Node_Base x{&a, &y};
    yLab::Node_Base parent{&x};
    a.set_parent (&x);
    b.set_parent (&y);
    c.set_parent (&y);
    y.set_parent (&x);
    x.set_parent (&parent);

    x.left_rotate();

    EXPECT_EQ (parent.get_parent(), nullptr);
    EXPECT_EQ (parent.get_left(), &y);
    EXPECT_EQ (parent.get_right(), nullptr);

    EXPECT_EQ (y.get_parent(), &parent);
    EXPECT_EQ (y.get_left(), &x);
    EXPECT_EQ (y.get_right(), &c);

    EXPECT_EQ (x.get_parent(), &y);
    EXPECT_EQ (x.get_left(), &a);
    EXPECT_EQ (x.get_right(), &b);

    EXPECT_EQ (a.get_parent(), &x);
    EXPECT_EQ (a.get_left(), nullptr);
    EXPECT_EQ (a.get_right(), nullptr);

    EXPECT_EQ (b.get_parent(), &x);
    EXPECT_EQ (b.get_left(), nullptr);
    EXPECT_EQ (b.get_right(), nullptr);

    EXPECT_EQ (c.get_parent(), &y);
    EXPECT_EQ (c.get_left(), nullptr);
    EXPECT_EQ (c.get_right(), nullptr);
}

// Look at the picture in node_base.hpp
TEST (Node_Base, Right_Rotate)
{
    yLab::Node_Base a{}, b{}, c{};
    yLab::Node_Base y{&a, &b};
    yLab::Node_Base x{&y, &c};
    yLab::Node_Base parent{&x};
    a.set_parent (&y);
    b.set_parent (&y);
    c.set_parent (&x);
    y.set_parent (&x);
    x.set_parent (&parent);

    x.right_rotate();

    EXPECT_EQ (parent.get_parent(), nullptr);
    EXPECT_EQ (parent.get_left(), &y);
    EXPECT_EQ (parent.get_right(), nullptr);

    EXPECT_EQ (y.get_parent(), &parent);
    EXPECT_EQ (y.get_left(), &a);
    EXPECT_EQ (y.get_right(), &x);

    EXPECT_EQ (x.get_parent(), &y);
    EXPECT_EQ (x.get_left(), &b);
    EXPECT_EQ (x.get_right(), &c);

    EXPECT_EQ (a.get_parent(), &y);
    EXPECT_EQ (a.get_left(), nullptr);
    EXPECT_EQ (a.get_right(), nullptr);

    EXPECT_EQ (b.get_parent(), &x);
    EXPECT_EQ (b.get_left(), nullptr);
    EXPECT_EQ (b.get_right(), nullptr);

    EXPECT_EQ (c.get_parent(), &x);
    EXPECT_EQ (c.get_left(), nullptr);
    EXPECT_EQ (c.get_right(), nullptr);
}
