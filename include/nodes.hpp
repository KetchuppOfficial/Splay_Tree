#ifndef INCLUDE_NODES_HPP
#define INCLUDE_NODES_HPP

#include <utility>
#include <cassert>

namespace yLab
{

template<typename Node_T>
class End_Node
{
    using node_ptr = Node_T *;
    using const_node_ptr = const Node_T *;

    node_ptr left_{nullptr};

public:

    End_Node () = default;

    End_Node (node_ptr left) : left_{left} {}

    End_Node (const End_Node &rhs) = delete;
    End_Node &operator= (const End_Node &rhs) = delete;

    End_Node (End_Node &&rhs) : left_{std::exchange (rhs.left_, nullptr)} {}

    End_Node &operator= (End_Node &&rhs) noexcept
    {
        std::swap (left_, rhs.left_);
        return *this;
    }

    virtual ~End_Node() = default;

    const_node_ptr get_left () const noexcept { return left_; }
    node_ptr get_left () noexcept { return left_; }
    void set_left (node_ptr left) noexcept { left_ = left; }
};

template<typename Key_T>
class Node : public End_Node<Node<Key_T>>
{
    using node_ptr = Node *;
    using const_node_ptr = const Node *;
    using base_ = End_Node<Node>;
    using end_node_ptr = base_ *;
    using const_end_node_ptr = const base_ *;

    node_ptr right_{nullptr};
    end_node_ptr parent_{nullptr};

    Key_T key_;

public:

    using key_type = Key_T;

    Node (const key_type &key) : key_{key} {}
    Node (key_type &&key) : key_{key} {}

    Node (const Node &rhs) = delete;
    Node &operator= (const Node &rhs) = delete;

    Node (Node &&rhs) : base_{std::move (rhs)},
                        right_{std::exchange (rhs.right_, nullptr)},
                        parent_{std::exchange (rhs.parent_, nullptr)},
                        key_{std::move (rhs.key_)} {}

    Node &operator= (Node &&rhs) noexcept (std::is_nothrow_swappable_v<key_type>)
    {
        std::swap (static_cast<base_ &>(*this), static_cast<base_ &>(rhs));
        std::swap (right_, rhs.right_);
        std::swap (parent_, rhs.parent_);
        std::swap (key_, rhs.key_);
        
        return *this;
    }

    const_node_ptr get_right () const noexcept { return right_; }
    node_ptr get_right () noexcept { return right_; }
    void set_right (node_ptr right) noexcept { right_ = right; }

    const_end_node_ptr get_parent () const noexcept { return parent_; }
    end_node_ptr get_parent () noexcept { return parent_; }
    void set_parent (end_node_ptr parent) noexcept { parent_ = parent; }

    const_node_ptr parent_unsafe () const noexcept { return static_cast<const_node_ptr>(parent_); }
    node_ptr parent_unsafe () noexcept { return static_cast<node_ptr>(parent_); }

    const key_type &key () const { return key_; }
};

namespace detail
{

template<typename Node_Ptr>
bool is_left_child (Node_Ptr node) noexcept
{
    assert (node);
    assert (node->get_parent());
    
    return node == node->get_parent()->get_left();
}

template<typename Node_Ptr>
Node_Ptr minimum (Node_Ptr node) noexcept
{
    assert (node);
    
    while (node->get_left())
        node = node->get_left();

    return node;
}

template<typename Node_Ptr>
Node_Ptr maximum (Node_Ptr node) noexcept
{
    assert (node);
    
    while (node->get_right())
        node = node->get_right();

    return node;
}

template<typename Node_Ptr>
auto successor (Node_Ptr node) noexcept -> decltype (node->get_parent())
{
    assert (node);
    
    if (node->get_right())
        return minimum (node->get_right());

    while (!is_left_child (node))
        node = node->parent_unsafe();
    
    return node->get_parent();
}

template<typename End_Node_Ptr>
auto predecessor (End_Node_Ptr node) noexcept -> decltype (node->get_left())
{
    using node_ptr = decltype (node->get_left());

    assert (node);
    
    if (node->get_left())
        return maximum (node->get_left());

    auto node_ = static_cast<node_ptr>(node);
    while (is_left_child (node_))
        node_ = node_->parent_unsafe();

    return node_->parent_unsafe();
}

/*
 *   |               |
 *   x               y
 *  / \             / \
 * a   y    -->    x   c
 *    / \         / \
 *   b   c       a   b
 */
template<typename Node_Ptr>
void left_rotate (Node_Ptr x) noexcept
{
    assert (x);
    assert (x->get_right());

    auto y = x->get_right();

    auto yl = y->get_left();
    x->set_right (yl);
    if (yl)
        yl->set_parent (x);

    y->set_parent (x->get_parent());
    if (is_left_child (x))
        x->get_parent()->set_left (y);
    else
        x->parent_unsafe()->set_right (y);

    y->set_left (x);
    x->set_parent (y);
}

/*
 *   |               |
 *   y               x
 *  / \             / \
 * a   x    <--    y   c
 *    / \         / \
 *   b   c       a   b
 */
template <typename Node_Ptr>
void right_rotate (Node_Ptr x) noexcept
{
    assert (x);
    assert (x->get_left());

    auto y = x->get_left();

    auto yr = y->get_right();
    x->set_left (yr);
    if (yr)
        yr->set_parent (x);

    y->set_parent (x->get_parent());
    if (is_left_child (x))
        x->get_parent()->set_left (y);
    else
        x->parent_unsafe()->set_right (y);

    y->set_right (x);
    x->set_parent (y);
}

} // namespace detail

} // namespace yLab

#endif // INCLUDE_NODES_HPP
