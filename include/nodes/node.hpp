#ifndef INCLUDE_NODES_NODE_HPP
#define INCLUDE_NODES_NODE_HPP

#include <utility>
#include <ostream>

#include "node_base.hpp"

namespace yLab
{

template<typename Key_T>
class Node : public Node_Base
{
    using base_node_ptr = Node_Base *;
    using node_ptr = Node *;
    using const_node_ptr = const Node *;

protected:

    using Node_Base::left_;
    using Node_Base::right_;
    using Node_Base::parent_;

    Key_T key_;

public:

    using key_type = Key_T;

    Node(const key_type &key, node_ptr left = nullptr, node_ptr right = nullptr,
                                                       base_node_ptr parent = nullptr)
        : Node_Base{left, right, parent}, key_{key} {}

    Node(key_type &&key, node_ptr left = nullptr, node_ptr right = nullptr,
                                                  base_node_ptr parent = nullptr)
        : Node_Base{left, right, parent}, key_{std::move(key)} {}

    ~Node() override = default;

    node_ptr get_left() { return static_cast<node_ptr>(left_); }
    const_node_ptr get_left() const { return static_cast<const_node_ptr>(left_); }

    node_ptr get_right() { return static_cast<node_ptr>(right_); }
    const_node_ptr get_right() const { return static_cast<const_node_ptr>(right_); }

    node_ptr get_parent() { return static_cast<node_ptr>(parent_); }
    const_node_ptr get_parent() const { return static_cast<const_node_ptr>(parent_); }

    const key_type &get_key() const { return key_; }
};

template<typename Key_T>
void dot_dump(std::ostream &os, const Node<Key_T> *node)
{
    static const char *nil_properties = " [shape = record, color = blue, style = filled,"
                                        " fillcolor = black, fontcolor = white,"
                                        " label = \"nil\"];\n";
    os << "    node_" << node << " [shape = record, color = blue, style = filled,"
                                    " fillcolor = chartreuse, fontcolor = black, label = \""
        << node->get_key() << "\"];\n";

    if (node->get_left() == nullptr)
        os << "    left_nil_node_" << node << nil_properties;

    if (node->get_right() == nullptr)
        os << "    right_nil_node_" << node << nil_properties;
}

template<typename Key_T>
class Threaded_Node : public Threaded_Node_Base
{
    using base_node_ptr = Threaded_Node_Base *;
    using node_ptr = Threaded_Node *;
    using const_node_ptr = const Threaded_Node *;

protected:

    using Threaded_Node_Base::left_;
    using Threaded_Node_Base::right_;

    Key_T key_;

public:

    using key_type = Key_T;

    Threaded_Node(const key_type &key, node_ptr left = nullptr, node_ptr right = nullptr)
                 : Threaded_Node_Base{left, right}, key_{key} {}

    Threaded_Node(key_type &&key, node_ptr left = nullptr, node_ptr right = nullptr)
                 : Threaded_Node_Base{left, right}, key_{std::move(key)} {}

    ~Threaded_Node() override = default;

    node_ptr get_left() noexcept
    {
        return left_.is_thread_ ? nullptr : static_cast<node_ptr>(left_.ptr_);
    }

    const_node_ptr get_left() const noexcept
    {
        return left_.is_thread_ ? nullptr : static_cast<const_node_ptr>(left_.ptr_);
    }

    node_ptr get_right() noexcept
    {
        return right_.is_thread_ ? nullptr : static_cast<node_ptr>(right_.ptr_);
    }

    const_node_ptr get_right() const noexcept
    {
        return right_.is_thread_ ? nullptr : static_cast<const_node_ptr>(right_.ptr_);
    }

    const key_type &get_key() const { return key_; }
};

template<typename Key_T>
void dot_dump(std::ostream &os, const Threaded_Node<Key_T> *node)
{
    static const char *nil_properties = " [shape = record, color = blue, style = filled,"
                                        " fillcolor = black, fontcolor = white,"
                                        " label = \"nil\"];\n";
    os << "    node_" << node << " [shape = record, color = blue, style = filled,"
                                    " fillcolor = chartreuse, fontcolor = black, label = \""
        << node->get_key() << "\"];\n";
}

} // namespace yLab

#endif // INCLUDE_NODES_NODE_HPP
