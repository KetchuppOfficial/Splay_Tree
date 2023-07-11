/*
 * This header contains implementation of iterator of ARB_Tree.
 */

#ifndef INCLUDE_TREE_ITERATOR
#define INCLUDE_TREE_ITERATOR

#include <iterator>

#include "nodes.hpp"

namespace yLab
{

template <typename Node_T>
class tree_iterator final
{
    using const_node_ptr = const Node_T *; 
    using const_end_node_ptr = const End_Node<Node_T> *;

    const_end_node_ptr node_;

public:

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = typename Node_T::key_type;
    using reference = const value_type &;
    using pointer = const value_type *;

    tree_iterator () = default;
    explicit tree_iterator (const_end_node_ptr node) noexcept : node_{node} {}

    reference operator* () const { return static_cast<const_node_ptr>(node_)->key(); }
    pointer operator-> () const { return &static_cast<const_node_ptr>(node_)->key(); }

    tree_iterator &operator++ () noexcept
    {
        node_ = detail::successor (static_cast<const_node_ptr>(node_));
        return *this;
    }
    
    tree_iterator operator++ (int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    tree_iterator &operator-- () noexcept
    {
        node_ = detail::predecessor (node_);
        return *this;
    }

    tree_iterator operator-- (int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator== (const tree_iterator &rhs) const noexcept { return node_ == rhs.node_; }

    template<typename key_t, typename compare> friend class Splay_Tree;
};

} // namespace yLab

#endif // INCLUDE_TREE_ITERATOR
