#ifndef INCLUDE_AUGMENTED_SPLAY_NODE_HPP
#define INCLUDE_AUGMENTED_SPLAY_NODE_HPP

#include <utility>

#include "node_base.hpp"

namespace yLab
{

template<typename Key_T>
class Augmented_Splay_Node : public Node_Base
{
    using base_node_ptr = Node_Base *;
    using node_ptr = Augmented_Splay_Node *;
    using const_node_ptr = const Augmented_Splay_Node *;

protected:

    Key_T key_;
    std::size_t size_;

public:

    using key_type = Key_T;
    using size_type = std::size_t;

    Augmented_Splay_Node (const key_type &key, node_ptr left = nullptr, node_ptr right = nullptr,
                                                                  base_node_ptr parent = nullptr)
                         : Node_Base{left, right, parent},
                           key_{key},
                           size_{1 + size (left) + size (right)} {}

    Augmented_Splay_Node (key_type &&key, node_ptr left = nullptr, node_ptr right = nullptr,
                                                             base_node_ptr parent = nullptr)
                         : Node_Base{left, right, parent},
                           key_{std::move (key)},
                           size_{1 + size (left) + size (right)} {}

    ~Augmented_Splay_Node () override = default;

    node_ptr get_left () { return static_cast<node_ptr>(this->left_); }
    const_node_ptr get_left () const { return static_cast<const_node_ptr>(this->left_); }
    void set_left (base_node_ptr left) override
    {
        this->left_ = left;
        size_ = 1 + size (get_left()) + size (get_right());
    }

    node_ptr get_right () { return static_cast<node_ptr>(this->right_); }
    const_node_ptr get_right () const { return static_cast<const_node_ptr>(this->right_); }
    void set_right (base_node_ptr right) override
    {
        this->right_ = right;
        size_ = 1 + size (get_left()) + size (get_right());
    }

    node_ptr get_parent () { return static_cast<node_ptr>(this->parent_); }
    const_node_ptr get_parent () const { return static_cast<const_node_ptr>(this->parent_); }

    static size_type size (const_node_ptr node) noexcept { return node ? node->size_ : 0; }
    const key_type &get_key () const { return key_; }
};

} // namespace yLab

#endif // INCLUDE_AUGMENTED_SPLAY_NODE_HPP