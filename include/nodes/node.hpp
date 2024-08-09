#ifndef INCLUDE_NODES_NODE_HPP
#define INCLUDE_NODES_NODE_HPP

#include <utility>
#include <ostream>
#include <cassert>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "node_base.hpp"

namespace yLab
{

template<typename Key_T>
class Node : public Node_Base
{
    using base_node = Node_Base;
    using base_node_ptr = base_node *;
    using node_ptr = Node *;
    using const_node_ptr = const Node *;

public:

    using key_type = Key_T;

    Node(const key_type &key, node_ptr left = nullptr, node_ptr right = nullptr,
         base_node_ptr parent = nullptr)
        : Node_Base{left, right, parent}, key_{key} {}

    Node(key_type &&key, node_ptr left = nullptr, node_ptr right = nullptr,
         base_node_ptr parent = nullptr)
        : Node_Base{left, right, parent}, key_{std::move(key)} {}

    ~Node() override = default;

    const key_type &get_key() const { return key_; }

protected:

    Key_T key_;
};

template<typename Key_T>
void dot_dump(std::ostream &os, const Node<Key_T> *node)
{
    assert(node);
    fmt::print(os, "    node_{} [shape = record, color = blue, style = filled, "
                   "fillcolor = chartreuse, fontcolor = black, label = \"{}\"];\n",
               fmt::ptr(node), node->get_key());
}

} // namespace yLab

#endif // INCLUDE_NODES_NODE_HPP
