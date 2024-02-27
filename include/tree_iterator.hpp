#ifndef INCLUDE_TREE_ITERATOR
#define INCLUDE_TREE_ITERATOR

#include <iterator>
#include <concepts>
#include <memory>

namespace yLab
{

namespace detail
{

template<typename Tree_T>
class iterator_attorney;

}

template <typename Node_T, typename Base_Node_T>
requires std::derived_from<Node_T, Base_Node_T>
class tree_iterator final
{
    using const_node_ptr = const Node_T *;
    using const_base_node_ptr = const Base_Node_T *;

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

    template<typename Tree_T>
    friend class detail::iterator_attorney;

private:

    reference get_key() const { return static_cast<const_node_ptr>(node_)->get_key(); }
};

namespace detail
{

template<typename Tree_T>
class iterator_attorney
{
    using node_type = typename Tree_T::node_type;

    template<typename Base_Node_T>
    static auto const_base_ptr(tree_iterator<node_type, Base_Node_T> it)
    {
        return it.node_;
    }

    template<typename Base_Node_T>
    static auto base_ptr(tree_iterator<node_type, Base_Node_T> it)
    {
        return const_cast<Base_Node_T *>(it.node_);
    }

    template<typename Base_Node_T>
    static auto ptr(tree_iterator<node_type, Base_Node_T> it)
    {
        return static_cast<node_type *>(base_ptr(it));
    }

    template<typename Base_Node_T>
    static auto const_ptr(tree_iterator<node_type, Base_Node_T> it)
    {
        return static_cast<const node_type *>(const_base_ptr(it));
    }

    friend Tree_T;
};

} // namespace detail

} // namespace yLab

#endif // INCLUDE_TREE_ITERATOR
