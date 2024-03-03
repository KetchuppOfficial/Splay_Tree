#ifndef INCLUDE_NODES_AUGMENTED_NODE_HPP
#define INCLUDE_NODES_AUGMENTED_NODE_HPP

#include <utility>
#include <ostream>

#include "node_base.hpp"

namespace yLab
{

template<typename Key_T>
class Augmented_Node : public Node_Base
{
    using base_node_ptr = Node_Base *;
    using node_ptr = Augmented_Node *;
    using const_node_ptr = const Augmented_Node *;

protected:

    using Node_Base::left_;
    using Node_Base::right_;
    using Node_Base::parent_;

    Key_T key_;
    std::size_t size_;

public:

    using key_type = Key_T;
    using size_type = std::size_t;

    Augmented_Node(const key_type &key, node_ptr left = nullptr, node_ptr right = nullptr,
                                                                 base_node_ptr parent = nullptr)
                  : Node_Base{left, right, parent}, key_{key},
                    size_{1 + size(left) + size(right)} {}

    Augmented_Node(key_type &&key, node_ptr left = nullptr, node_ptr right = nullptr,
                                                            base_node_ptr parent = nullptr)
                  : Node_Base{left, right, parent}, key_{std::move(key)},
                    size_{1 + size(left) + size(right)} {}

    ~Augmented_Node() override = default;

    node_ptr get_left() { return static_cast<node_ptr>(left_); }
    const_node_ptr get_left() const { return static_cast<const_node_ptr>(left_); }
    void set_left(base_node_ptr left) override
    {
        left_ = left;
        size_ = 1 + size(get_left()) + size(get_right());
    }

    node_ptr get_right() { return static_cast<node_ptr>(right_); }
    const_node_ptr get_right() const { return static_cast<const_node_ptr>(right_); }
    void set_right(base_node_ptr right) override
    {
        right_ = right;
        size_ = 1 + size(get_left()) + size(get_right());
    }

    node_ptr get_parent() { return static_cast<node_ptr>(parent_); }
    const_node_ptr get_parent() const { return static_cast<const_node_ptr>(parent_); }

    static size_type size(const_node_ptr node) noexcept { return node ? node->size_ : 0; }
    const key_type &get_key() const { return key_; }
};

template<typename Key_T>
void dot_dump(std::ostream &os, const Augmented_Node<Key_T> *node)
{
    static const char *nil_properties = " [shape = record, color = blue, style = filled,"
                                        " fillcolor = black, fontcolor = white,"
                                        " label = \"nil\"];\n";
    os << "    node_" << node << " [shape = record, color = blue, style = filled,"
                                    " fillcolor = 	chartreuse, fontcolor = black, label = \"key: "
       << node->get_key() << "| size: " << Augmented_Node<Key_T>::size(node) << "\"];\n";

    if (node->get_left() == nullptr)
        os << "    left_nil_node_" << node << nil_properties;

    if (node->get_right() == nullptr)
        os << "    right_nil_node_" << node << nil_properties;
}

template<typename Key_T>
class Threaded_Augmented_Node : public Threaded_Node_Base
{
    using base_node_ptr = Threaded_Node_Base *;
    using node_ptr = Threaded_Augmented_Node *;
    using const_node_ptr = const Threaded_Augmented_Node *;

protected:

    using Threaded_Node_Base::left_;
    using Threaded_Node_Base::right_;

    Key_T key_;
    std::size_t size_;

public:

    using key_type = Key_T;
    using size_type = std::size_t;

    Threaded_Augmented_Node(const key_type &key, node_ptr left = nullptr, node_ptr right = nullptr)
        : Threaded_Node_Base{left, right}, key_{key}, size_{1 + size(left) + size(right)} {}

    Threaded_Augmented_Node(key_type &&key, node_ptr left = nullptr, node_ptr right = nullptr)
        : Threaded_Node_Base{left, right}, key_{std::move(key)},
          size_{1 + size(left) + size(right)} {}

    ~Threaded_Augmented_Node() override = default;

    node_ptr get_left() noexcept
    {
        return left_.is_thread_ ? nullptr : static_cast<node_ptr>(left_.ptr_);
    }

    const_node_ptr get_left() const noexcept
    {
        return left_.is_thread_ ? nullptr : static_cast<const_node_ptr>(left_.ptr_);
    }

    void set_left(base_node_ptr left) override
    {
        left_.ptr_ = left;
        left_.is_thread_ = false;

        size_ = 1 + size(static_cast<node_ptr>(left)) + size(get_right());
    }

    node_ptr get_right() noexcept
    {
        return right_.is_thread_ ? nullptr : static_cast<node_ptr>(right_.ptr_);
    }

    const_node_ptr get_right() const noexcept
    {
        return right_.is_thread_ ? nullptr : static_cast<const_node_ptr>(right_.ptr_);
    }

    void set_right(base_node_ptr right) override
    {
        right_.ptr_ = right;
        right_.is_thread_ = false;

        size_ = 1 + size(get_left()) + size(static_cast<node_ptr>(right));
    }

    static size_type size(const_node_ptr node) noexcept { return node ? node->size_ : 0; }
    const key_type &get_key() const { return key_; }
};

template<typename Key_T>
void dot_dump(std::ostream &os, const Threaded_Augmented_Node<Key_T> *node)
{
    static const char *nil_properties = " [shape = record, color = blue, style = filled,"
                                        " fillcolor = black, fontcolor = white,"
                                        " label = \"nil\"];\n";
    os << "    node_" << node << " [shape = record, color = blue, style = filled,"
                                    " fillcolor = 	chartreuse, fontcolor = black, label = \"key: "
       << node->get_key() << "| size: " << Threaded_Augmented_Node<Key_T>::size(node) << "\"];\n";

    if (node->get_left() == nullptr)
        os << "    left_nil_node_" << node << nil_properties;

    if (node->get_right() == nullptr)
        os << "    right_nil_node_" << node << nil_properties;
}

} // namespace yLab

#endif // INCLUDE_NODES_AUGMENTED_NODE_HPP
