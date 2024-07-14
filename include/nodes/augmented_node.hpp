#ifndef INCLUDE_NODES_AUGMENTED_NODE
#define INCLUDE_NODES_AUGMENTED_NODE

#include <cstddef>
#include <utility>
#include <ostream>
#include <cassert>

#include "node.hpp"

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

    void set_left(base_node_ptr left) noexcept override
    {
        if (this->has_left_thread())
        {
            this->left_ = left;
            size_ += size(static_cast<node_ptr>(this->left_));
        }
        else
        {
            size_ -= size(static_cast<node_ptr>(this->left_));
            this->left_ = left;
            size_ += size(static_cast<node_ptr>(this->left_));
        }

        this->left_thread_ = false;
    }

    void set_left_thread(base_node_ptr left) noexcept override
    {
        if (!this->has_left_thread())
            size_ -= size(static_cast<node_ptr>(this->left_));

        this->left_ = left;
        this->left_thread_ = true;
    }

    void set_right(base_node_ptr right) noexcept override
    {
        if (this->has_right_thread())
        {
            this->right_ = right;
            size_ += size(static_cast<node_ptr>(this->right_));
        }
        else
        {
            size_ -= size(static_cast<node_ptr>(this->right_));
            this->right_ = right;
            size_ += size(static_cast<node_ptr>(this->right_));
        }

        this->right_thread_ = false;
    }

    void set_right_thread(base_node_ptr right) noexcept override
    {
        if (!this->has_right_thread())
            size_ -= size(static_cast<node_ptr>(this->right_));

        this->right_ = right;
        this->right_thread_ = true;
    }

    static size_type size(const_node_ptr node) noexcept { return node ? node->size_ : 0; }

protected:

    std::size_t size_;
};

template<typename Key_T>
void dot_dump(std::ostream &os, const Augmented_Node<Key_T> *node)
{
    assert(node);

    static const char *nil_properties = " [shape = record, color = blue, style = filled,"
                                        " fillcolor = black, fontcolor = white,"
                                        " label = \"nil\"];\n";

    std::println(os, "    node_{} [shape = record, color = blue, style = filled, "
                     "fillcolor = chartreuse, fontcolor = black, label = \"key: {} | size: {}\"];",
                      reinterpret_cast<const void *>(node), node->get_key(),
                      Augmented_Node<Key_T>::size(node));
}

} // namespace yLab

#endif // INCLUDE_NODES_AUGMENTED_NODE
