#include <gtest/gtest.h>
#include <vector>

#include "augmented_node.hpp"
#include "node_concepts.hpp"

TEST(Augmented_Node, Constructors)
{
    std::vector vec{1, 2, 3, 4, 5};
    auto vec_copy = vec;

    yLab::Augmented_Node node_1{vec};
    EXPECT_EQ(node_1.get_key(), vec);

    yLab::Augmented_Node node_2{std::move(vec)};
    EXPECT_TRUE(vec.empty());
    EXPECT_EQ(node_2.get_key(), vec_copy);
}

TEST(Augmented_Node, Size)
{
    using node_type = yLab::Augmented_Node<int>;

    static_assert(yLab::contains_subtree_size<node_type>);

    node_type left{1}, right{3};
    node_type node{2, &left, &right};

    EXPECT_EQ(node_type::size(&left), 1);
    EXPECT_EQ(node_type::size(&right), 1);
    EXPECT_EQ(node_type::size(&node), 3);
    EXPECT_EQ(node_type::size(nullptr), 0);
}

// Look at the picture in node_base.hpp
TEST(Augmented_Node, Left_Rotate)
{
    using node_type = yLab::Augmented_Node<int>;

    node_type a{1}, b{2}, c{3};
    node_type y{4, &b, &c};
    node_type x{5, &a, &y};
    node_type end_node{6, &x};
    a.set_parent(&x);
    b.set_parent(&y);
    c.set_parent(&y);
    y.set_parent(&x);
    x.set_parent(&end_node);

    a.set_left_thread(&end_node);
    a.set_right_thread(&x);
    b.set_left_thread(&x);
    b.set_right_thread(&y);
    c.set_left_thread(&y);
    c.set_right_thread(&end_node);

    x.left_rotate();

    EXPECT_EQ(node_type::size(&end_node), 6);
    EXPECT_EQ(node_type::size(&y), 5);
    EXPECT_EQ(node_type::size(&x), 3);
    EXPECT_EQ(node_type::size(&a), 1);
    EXPECT_EQ(node_type::size(&b), 1);
    EXPECT_EQ(node_type::size(&c), 1);
}

// Look at the picture in node_base.hpp
TEST(Augmented_Node, Right_Rotate)
{
    using node_type = yLab::Augmented_Node<int>;

    node_type a{1}, b{2}, c{3};
    node_type y{4, &a, &b};
    node_type x{5, &y, &c};
    node_type end_node{6, &x};
    a.set_parent(&y);
    b.set_parent(&y);
    c.set_parent(&x);
    y.set_parent(&x);
    x.set_parent(&end_node);

    a.set_left_thread(&end_node);
    a.set_right_thread(&y);
    b.set_left_thread(&y);
    b.set_right_thread(&x);
    c.set_left_thread(&x);
    c.set_right_thread(&end_node);

    x.right_rotate();

    EXPECT_EQ(node_type::size(&end_node), 6);
    EXPECT_EQ(node_type::size(&y), 5);
    EXPECT_EQ(node_type::size(&x), 3);
    EXPECT_EQ(node_type::size(&a), 1);
    EXPECT_EQ(node_type::size(&b), 1);
    EXPECT_EQ(node_type::size(&c), 1);
}
