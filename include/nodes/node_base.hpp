#ifndef INCLUDE_NODES_NODE_BASE_HPP
#define INCLUDE_NODES_NODE_BASE_HPP

#include <cassert>
#include <ostream>

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

    Node_Base(node_ptr left = nullptr, node_ptr right = nullptr, node_ptr parent = nullptr)
             : left_{left}, right_{right}, parent_{parent} {}

    Node_Base(const Node_Base &rhs) = delete;
    Node_Base &operator=(const Node_Base &rhs) = delete;

    Node_Base(Node_Base &&rhs) = default;
    Node_Base &operator=(Node_Base &&rhs) = default;

    virtual ~Node_Base() = default;

    // Getters and setters

    node_ptr get_left() { return left_; }
    const_node_ptr get_left() const { return left_; }
    virtual void set_left(node_ptr left) { left_ = left; }

    node_ptr get_right() { return right_; }
    const_node_ptr get_right() const { return right_; }
    virtual void set_right(node_ptr right) { right_ = right; }

    node_ptr get_parent() { return parent_; }
    const_node_ptr get_parent() const { return parent_; }
    void set_parent(node_ptr parent) { parent_ = parent; }

    // Basic interface of a node in a search tree

    bool is_left_child() const noexcept { return parent_ && this == parent_->left_; }
    bool is_right_child() const noexcept { return parent_ && this == parent_->right_; }

    const_node_ptr maximum() const noexcept
    {
        auto node = this;
        while (node->right_)
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
        while (node->left_)
            node = node->left_;

        return node;
    }

    node_ptr minimum() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->minimum());
    }

    const_node_ptr successor() const noexcept
    {
        if (right_)
            return right_->minimum();

        auto node = this;
        while (node->is_right_child())
            node = node->parent_;

        return node->parent_;
    }

    node_ptr successor() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->successor());
    }

    const_node_ptr predecessor() const noexcept
    {
        if (left_)
            return left_->maximum();

        auto node = this;
        while (node->is_left_child())
            node = node->parent_;

        return node->parent_;
    }

    node_ptr predecessor() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->predecessor());
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
        assert(right_);

        auto y = right_;
        auto b = y->left_;

        set_right(b);
        if (b)
            b->parent_ = this;

        y->set_left(this);

        y->parent_ = parent_;
        if (is_left_child())
            parent_->set_left(y);
        else
            parent_->set_right(y);

        parent_ = y;
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
        assert(left_);

        auto y = left_;
        auto b = y->right_;

        set_left(b);
        if (b)
            b->parent_ = this;

        y->set_right(this);

        y->parent_ = parent_;
        if (is_left_child())
            parent_->set_left(y);
        else
            parent_->set_right(y);

        parent_ = y;
    }
};

inline void dot_dump(std::ostream &os, const Node_Base *node)
{
    os << "    node_" << node << " [color = black, style = filled, fillcolor = olivedrab,"
          " label = \"end node\"];\n";
}

class Threaded_Node_Base
{
    using node_ptr = Threaded_Node_Base *;
    using const_node_ptr = const Threaded_Node_Base *;

protected:

    struct link final
    {
        node_ptr ptr_;
        bool is_thread_;
    };

    link left_;
    link right_;

public:

    Threaded_Node_Base(node_ptr left = nullptr, node_ptr right = nullptr)
                      : left_{left, false}, right_{right, false} {}

    Threaded_Node_Base(const Threaded_Node_Base &rhs) = delete;
    Threaded_Node_Base &operator=(const Threaded_Node_Base &rhs) = delete;

    Threaded_Node_Base(Threaded_Node_Base &&rhs) = default;
    Threaded_Node_Base &operator=(Threaded_Node_Base &&rhs) = default;

    virtual ~Threaded_Node_Base() = default;

    // Getters and setters

    node_ptr get_left() noexcept { return left_.is_thread_ ? nullptr : left_.ptr_; }
    const_node_ptr get_left() const noexcept { return left_.is_thread_ ? nullptr : left_.ptr_; }

    virtual void set_left(node_ptr left)
    {
        left_.ptr_ = left;
        left_.is_thread_ = false;
    }

    bool has_left_thread() const noexcept { return left_.is_thread_; }

    void set_left_thread(node_ptr left_thread)
    {
        left_.ptr_ = left_thread;
        left_.is_thread_ = true;
    }

    node_ptr get_right() noexcept { return right_.is_thread_ ? nullptr : right_.ptr_; }
    const_node_ptr get_right() const noexcept { return right_.is_thread_ ? nullptr : right_.ptr_; }

    virtual void set_right(node_ptr right)
    {
        right_.ptr_ = right;
        right_.is_thread_ = false;
    }

    bool has_right_thread() const noexcept { return right_.is_thread_; }

    void set_right_thread(node_ptr right_thread)
    {
        right_.ptr_ = right_thread;
        right_.is_thread_ = true;
    }

    // Basic interface of a node in a search tree

    const_node_ptr maximum() const noexcept
    {
        auto node = this;
        while (node->get_right())
            node = node->right_.ptr_;

        return node;
    }

    node_ptr maximum() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->maximum());
    }

    const_node_ptr minimum() const noexcept
    {
        auto node = this;
        while (node->get_left())
            node = node->left_.ptr_;

        return node;
    }

    node_ptr minimum() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->minimum());
    }

    const_node_ptr successor() const noexcept
    {
        if (right_.is_thread_)
            return right_.ptr_;
        else
            return right_.ptr_->minimum();
    }

    node_ptr successor() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->successor());
    }

    const_node_ptr predecessor() const noexcept
    {
        if (left_.is_thread_)
            return left_.ptr_;
        else
            return left_.ptr_->maximum();
    }

    node_ptr predecessor() noexcept
    {
        return const_cast<node_ptr>(static_cast<const_node_ptr>(this)->predecessor());
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
        assert(!right_.is_thread_);

        node_ptr y = right_.ptr_;

        if (y->left_.is_thread_)
        {
            assert(y->left_.ptr_ == this); // if (b == nullptr), then y is the successor of x
            set_right_thread(y);
        }
        else
            set_right(y->left_.ptr_);

        y->set_left(this);
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
        assert(!left_.is_thread_);

        node_ptr y = left_.ptr_;

        if (y->right_.is_thread_)
        {
            assert(y->right_.ptr_ == this); // if (b == nullptr), then y is the predecessor of x
            set_left_thread(y);
        }
        else
            set_left(y->right_.ptr_);

        y->set_right(this);
    }
};

inline void dot_dump(std::ostream &os, const Threaded_Node_Base *node)
{
    os << "    node_" << node << " [color = black, style = filled, fillcolor = olivedrab,"
          " label = \"end node\"];\n";
}

} // namespace yLab

#endif // INCLUDE_NODES_NODE_BASE_HPP
