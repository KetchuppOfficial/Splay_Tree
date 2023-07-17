#include <gtest/gtest.h>
#include <vector>

#include "splay_node.hpp"

TEST (Splay_Node, Constructors)
{
    std::vector vec{1, 2, 3, 4, 5};
    auto vec_copy = vec;

    yLab::Splay_Node node_1{vec};
    EXPECT_EQ (node_1.get_key(), vec);

    yLab::Splay_Node node_2{std::move (vec)};
    EXPECT_TRUE (vec.empty());
    EXPECT_EQ (node_2.get_key(), vec_copy);
}

TEST (Splay_Node, Size)
{
    yLab::Splay_Node left{1}, right{3};
    yLab::Splay_Node node{2, &left, &right};

    EXPECT_EQ (decltype(node)::size (&node), 3);
    EXPECT_EQ (decltype(node)::size (nullptr), 0);
}

// Look at the picture in node_base.hpp
TEST (Splay_Node, Left_Rotate)
{
    yLab::Splay_Node a{1}, b{2}, c{3};
    yLab::Splay_Node y{4, &b, &c};
    yLab::Splay_Node x{5, &a, &y};
    yLab::Splay_Node parent{6, &x};
    a.set_parent (&x);
    b.set_parent (&y);
    c.set_parent (&y);
    y.set_parent (&x);
    x.set_parent (&parent);

    x.left_rotate();

    EXPECT_EQ (parent.get_parent(), nullptr);
    EXPECT_EQ (parent.get_left(), &y);
    EXPECT_EQ (parent.get_right(), nullptr);
    EXPECT_EQ (parent.size_, 6);

    EXPECT_EQ (y.get_parent(), &parent);
    EXPECT_EQ (y.get_left(), &x);
    EXPECT_EQ (y.get_right(), &c);
    EXPECT_EQ (y.size_, 5);

    EXPECT_EQ (x.get_parent(), &y);
    EXPECT_EQ (x.get_left(), &a);
    EXPECT_EQ (x.get_right(), &b);
    EXPECT_EQ (x.size_, 3);

    EXPECT_EQ (a.get_parent(), &x);
    EXPECT_EQ (a.get_left(), nullptr);
    EXPECT_EQ (a.get_right(), nullptr);
    EXPECT_EQ (a.size_, 1);

    EXPECT_EQ (b.get_parent(), &x);
    EXPECT_EQ (b.get_left(), nullptr);
    EXPECT_EQ (b.get_right(), nullptr);
    EXPECT_EQ (b.size_, 1);

    EXPECT_EQ (c.get_parent(), &y);
    EXPECT_EQ (c.get_left(), nullptr);
    EXPECT_EQ (c.get_right(), nullptr);
    EXPECT_EQ (c.size_, 1);
}

// Look at the picture in node_base.hpp
TEST (Splay_Node, Right_Rotate)
{
    yLab::Splay_Node a{1}, b{2}, c{3};
    yLab::Splay_Node y{4, &a, &b};
    yLab::Splay_Node x{5, &y, &c};
    yLab::Splay_Node parent{6, &x};
    a.set_parent (&y);
    b.set_parent (&y);
    c.set_parent (&x);
    y.set_parent (&x);
    x.set_parent (&parent);

    x.right_rotate();

    EXPECT_EQ (parent.get_parent(), nullptr);
    EXPECT_EQ (parent.get_left(), &y);
    EXPECT_EQ (parent.get_right(), nullptr);
    EXPECT_EQ (parent.size_, 6);

    EXPECT_EQ (y.get_parent(), &parent);
    EXPECT_EQ (y.get_left(), &a);
    EXPECT_EQ (y.get_right(), &x);
    EXPECT_EQ (y.size_, 5);

    EXPECT_EQ (x.get_parent(), &y);
    EXPECT_EQ (x.get_left(), &b);
    EXPECT_EQ (x.get_right(), &c);
    EXPECT_EQ (x.size_, 3);

    EXPECT_EQ (a.get_parent(), &y);
    EXPECT_EQ (a.get_left(), nullptr);
    EXPECT_EQ (a.get_right(), nullptr);
    EXPECT_EQ (a.size_, 1);

    EXPECT_EQ (b.get_parent(), &x);
    EXPECT_EQ (b.get_left(), nullptr);
    EXPECT_EQ (b.get_right(), nullptr);
    EXPECT_EQ (b.size_, 1);

    EXPECT_EQ (c.get_parent(), &x);
    EXPECT_EQ (c.get_left(), nullptr);
    EXPECT_EQ (c.get_right(), nullptr);
    EXPECT_EQ (c.size_, 1);
}
