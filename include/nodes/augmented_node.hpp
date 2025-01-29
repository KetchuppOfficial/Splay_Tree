#ifndef INCLUDE_NODES_AUGMENTED_NODE_HPP
#define INCLUDE_NODES_AUGMENTED_NODE_HPP

#include <cstddef>
#include <utility>
#include <ostream>
#include <cassert>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "nodes/node.hpp"

namespace yLab
{

template<typename Key_T>
class Augmented_Node final : public Node<Key_T>
{
    using base_node = Node_Base;
    using base_node_ptr = base_node *;
    using node_ptr = Augmented_Node *;
    using const_node_ptr = const Augmented_Node *;

public:

    using typename Node<Key_T>::key_type;
    using size_type = std::size_t;

    Augmented_Node(const Key_T &key, node_ptr left = nullptr, node_ptr right = nullptr,
                   base_node_ptr parent = nullptr)
        : Node<Key_T>{key, left, right, parent}, size_{1 + size(left) + size(right)} {}

    Augmented_Node(Key_T &&key, node_ptr left = nullptr, node_ptr right = nullptr,
                   base_node_ptr parent = nullptr)
        : Node<Key_T>{std::move(key), left, right, parent}, size_{1 + size(left) + size(right)} {}

    ~Augmented_Node() override = default;

    static size_type size(const_node_ptr node) noexcept { return node ? node->size_ : 0; }

private:

    void do_set_left(base_node_ptr left) noexcept override
    {
        size_ = 1 + size(static_cast<node_ptr>(left));
        if (!this->has_right_thread())
            size_ += size(static_cast<node_ptr>(this->right_));
    }

    void do_set_left_thread(base_node_ptr left) noexcept override
    {
        if (!this->has_left_thread())
            size_ -= size(static_cast<node_ptr>(this->left_));
    }

    void do_set_right(base_node_ptr right) noexcept override
    {
        size_ = 1 + size(static_cast<node_ptr>(right));
        if (!this->has_left_thread())
            size_ += size(static_cast<node_ptr>(this->left_));
    }

    void do_set_right_thread(base_node_ptr right) noexcept override
    {
        if (!this->has_right_thread())
            size_ -= size(static_cast<node_ptr>(this->right_));
    }

    size_type size_;
};

template<typename Key_T>
void dot_dump(std::ostream &os, const Augmented_Node<Key_T> &node)
{
    fmt::print(os, "    node_{} [shape = record, color = blue, style = filled, "
                   "fillcolor = chartreuse, fontcolor = black, label = \"key: {} | size: {}\"];\n",
               fmt::ptr(&node), node.get_key(), Augmented_Node<Key_T>::size(&node));
}

} // namespace yLab

#endif // INCLUDE_NODES_AUGMENTED_NODE_HPP
