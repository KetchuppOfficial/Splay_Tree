#ifndef INCLUDE_NODE_BASE_HPP
#define INCLUDE_NODE_BASE_HPP

#include <cassert>

namespace yLab
{

class Node_Base
{
    using node_ptr = Node_Base *;
    using const_node_ptr = const Node_Base *;

protected:

    node_ptr left_;
    node_ptr right_;
    node_ptr parent_;

public:

    Node_Base (node_ptr left = nullptr, node_ptr right = nullptr, node_ptr parent = nullptr)
              : left_{left}, right_{right}, parent_{parent} {}

    Node_Base (const Node_Base &rhs) = delete;
    Node_Base &operator= (const Node_Base &rhs) = delete;

    Node_Base (Node_Base &&rhs) = default;
    Node_Base &operator= (Node_Base &&rhs) = default;

    virtual ~Node_Base () = default;

    // Getters and setters

    node_ptr get_left () { return left_; }
    const_node_ptr get_left () const { return left_; }
    void set_left (node_ptr left) { left_ = left; }

    node_ptr get_right () { return right_; }
    const_node_ptr get_right () const { return right_; }
    void set_right (node_ptr right) { right_ = right; }

    node_ptr get_parent () { return parent_; }
    const_node_ptr get_parent () const { return parent_; }
    void set_parent (node_ptr parent) { parent_ = parent; }

    // Basic interface of a node in a search tree

    bool is_left_child () const noexcept { return parent_ && this == parent_->left_; }
    bool is_right_child () const noexcept { return parent_ && this == parent_->right_; }

    const_node_ptr maximum () const noexcept
    {
        auto node = this;
        while (node->right_)
            node = node->right_;

        return node;
    }

    node_ptr maximum () noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->maximum());
    }

    const_node_ptr minimum () const noexcept
    {
        auto node = this;
        while (node->left_)
            node = node->left_;

        return node;
    }

    node_ptr minimum () noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->minimum());
    }

    const_node_ptr successor () const noexcept
    {
        if (right_)
            return right_->minimum();

        auto node = this;
        while (node->is_right_child())
            node = node->parent_;

        return node->parent_;
    }

    node_ptr successor () noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->successor());
    }

    const_node_ptr predecessor () const noexcept
    {
        if (left_)
            return left_->maximum();

        auto node = this;
        while (node->is_left_child())
            node = node->parent_;

        return node->parent_;
    }

    node_ptr predecessor () noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->predecessor());
    }

    virtual void left_rotate () noexcept { left_rotate_impl(); }
    virtual void right_rotate () noexcept { right_rotate_impl(); }

protected:

    /*
     *   |               |
     *   x               y
     *  / \             / \
     * a   y    -->    x   c
     *    / \         / \
     *   b   c       a   b
     */
    void left_rotate_impl () noexcept
    {
        assert (right_);

        auto x = this;
        auto y = right_;
        auto b = y->left_;

        x->right_ = b;
        y->left_ = x;
        if (b)
            b->parent_ = x;

        y->parent_ = x->parent_;
        if (x->is_left_child())
            x->parent_->left_ = y;
        else
            x->parent_->right_ = y;

        x->parent_ = y;
    }

    /*
     *   |               |
     *   y               x
     *  / \             / \
     * a   x    <--    y   c
     *    / \         / \
     *   b   c       a   b
     */
    void right_rotate_impl () noexcept
    {
        assert (left_);
        
        auto x = this;
        auto y = left_;
        auto b = y->right_;

        x->left_ = b;
        y->right_ = x;
        if (b)
            b->parent_ = x;

        y->parent_ = x->parent_;
        if (x->is_left_child())
            x->parent_->left_ = y;
        else
            x->parent_->right_ = y;

        x->parent_ = y;
    }
};

} // namespace yLab

#endif // INCLUDE_NODE_BASE_HPP
