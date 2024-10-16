#ifndef INCLUDE_TREE_ITERATOR_HPP
#define INCLUDE_TREE_ITERATOR_HPP

#include <concepts>
#include <iterator>
#include <cstddef>
#include <memory>

#include "nodes/node_base.hpp"

namespace yLab
{

template <typename Node_T>
requires std::derived_from<Node_T, Node_Base>
class tree_iterator final
{
    using const_node_ptr = const Node_T *;
    using const_base_node_ptr = const Node_Base *;

    const_base_node_ptr node_;

public:

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = typename Node_T::key_type;
    using reference = const value_type &;
    using pointer = const value_type *;

    tree_iterator() = default;
    explicit tree_iterator(const_base_node_ptr node) noexcept : node_{node} {}

    reference operator*() const { return get_key(); }
    pointer operator->() const { return std::addressof(get_key()); }

    tree_iterator &operator++() noexcept
    {
        node_ = node_->successor();
        return *this;
    }

    tree_iterator operator++(int) noexcept
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    tree_iterator &operator--() noexcept
    {
        node_ = node_->predecessor();
        return *this;
    }

    tree_iterator operator--(int) noexcept
    {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    bool operator==(const tree_iterator &rhs) const noexcept { return node_ == rhs.node_; }

    template<typename node_t, typename Compare>
    requires std::derived_from<node_t, Node_Base>
    friend class Search_Tree;

private:

    reference get_key() const { return static_cast<const_node_ptr>(node_)->get_key(); }
};

} // namespace yLab

#endif // INCLUDE_TREE_ITERATOR_HPP
