#ifndef INCLUDE_NODES_NODE_BASE_HPP
#define INCLUDE_NODES_NODE_BASE_HPP

#include <cassert>
#include <ostream>

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace yLab
{

class Node_Base
{
    using node_ptr = Node_Base *;
    using const_node_ptr = const Node_Base *;

public:

    Node_Base(node_ptr left = nullptr, node_ptr right = nullptr,
              node_ptr parent = nullptr) noexcept : left_{left}, right_{right}, parent_{parent} {}

    Node_Base(const Node_Base &rhs) = delete;
    Node_Base &operator=(const Node_Base &rhs) = delete;

    Node_Base(Node_Base &&rhs) = default;
    Node_Base &operator=(Node_Base &&rhs) = default;

    virtual ~Node_Base() = default;

    // getters and setters

    /*
     * _unsafe methods give you access to left_ and right_ pointers regardless of whether they
     * represent a child or a thread; such methods shall be used after calling
     * has_left_thread()/has_right_thread()
     */

    node_ptr get_left() noexcept { return left_thread_ ? nullptr : left_; }
    const_node_ptr get_left() const noexcept { return left_thread_ ? nullptr : left_; }
    node_ptr get_left_unsafe() noexcept { return left_; }
    const_node_ptr get_left_unsafe() const noexcept { return left_; }

    virtual void set_left(node_ptr left) noexcept
    {
        left_ = left;
        left_thread_ = false;
    }

    bool has_left_thread() const noexcept { return left_thread_; }
    virtual void set_left_thread(node_ptr left) noexcept
    {
        left_ = left;
        left_thread_ = true;
    }

    node_ptr get_right() noexcept { return right_thread_ ? nullptr : right_; }
    const_node_ptr get_right() const noexcept { return right_thread_ ? nullptr : right_; }
    node_ptr get_right_unsafe() noexcept { return right_; }
    const_node_ptr get_right_unsafe() const noexcept { return right_; }
    virtual void set_right(node_ptr right) noexcept
    {
        right_ = right;
        right_thread_ = false;
    }

    bool has_right_thread() const noexcept { return right_thread_; }
    virtual void set_right_thread(node_ptr right) noexcept
    {
        right_ = right;
        right_thread_ = true;
    }

    node_ptr get_parent() noexcept { return parent_; }
    const_node_ptr get_parent() const noexcept { return parent_; }
    void set_parent(node_ptr parent) noexcept { parent_ = parent; }

    // Basic interface of a node in a binary search tree

    bool is_left_child() const noexcept { return parent_ && this == parent_->left_; }

    const const_node_ptr maximum() const noexcept
    {
        auto node = this;
        while (!node->has_right_thread())
            node = node->right_;

        return node;
    }

    node_ptr maximum() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->maximum());
    }

    const_node_ptr minimum() const noexcept
    {
        auto node = this;
        while (!node->has_left_thread())
            node = node->left_;

        return node;
    }

    node_ptr minimum() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->minimum());
    }

    node_ptr successor() noexcept { return has_right_thread() ? right_ : right_->minimum(); }
    const_node_ptr successor() const noexcept
    {
        return has_right_thread() ? right_ : right_->minimum();
    }

    node_ptr predecessor() noexcept { return has_left_thread() ? left_ : left_->maximum(); }
    const_node_ptr predecessor() const noexcept
    {
        return has_left_thread() ? left_ : left_->maximum();
    }

    /*
     *   |               |
     *   x = this        y
     *  / \             / \
     * a   y    -->    x   c
     *    / \         / \
     *   b   c       a   b
     */
    void left_rotate() noexcept
    {
        assert(!has_right_thread());

        node_ptr y = right_;

        if (y->has_left_thread())
        {
            // if "b" is a thread, then "y" is the successor of "x"
            assert(y->left_ == this);
            set_right_thread(y);
        }
        else
        {
            node_ptr b = y->left_;
            set_right(b);
            b->parent_ = this;
        }

        y->set_left(this);

        y->parent_ = parent_;
        if (is_left_child())
            parent_->set_left(y);
        else
            parent_->set_right(y);

        set_parent(y);
    }

    /*
     *   |               |
     *   y               x = this
     *  / \             / \
     * a   x    <--    y   c
     *    / \         / \
     *   b   c       a   b
     */
    void right_rotate() noexcept
    {
        assert(!has_left_thread());

        node_ptr y = left_;

        if (y->has_right_thread())
        {
            // if "b" is a thread, then "y" is the predecessor of "x"
            assert(y->right_ == this);
            set_left_thread(y);
        }
        else
        {
            node_ptr b = y->right_;
            set_left(b);
            b->parent_ = this;
        }

        y->set_right(this);

        y->parent_ = parent_;
        if (is_left_child())
            parent_->set_left(y);
        else
            parent_->set_right(y);

        set_parent(y);
    }

protected:

    node_ptr left_;
    node_ptr right_;
    node_ptr parent_;
    bool left_thread_ = false;
    bool right_thread_ = false;
};

inline void dot_dump(std::ostream &os, const Node_Base *node)
{
    assert(node);
    fmt::print(os, "    node_{} [color = black, style = filled, fillcolor = olivedrab, "
                   "label = \"end node\"];\n", fmt::ptr(node));
}

} // namespace yLab

#endif // INCLUDE_NODES_NODE_BASE_HPP
