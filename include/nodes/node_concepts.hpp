#ifndef INCLUDE_NODES_SUBTREE_SIZES_HPP
#define INCLUDE_NODES_SUBTREE_SIZES_HPP

#include <concepts>

namespace yLab
{

/*
 * There is a semantic rule additionally to the following concept.
 * Method size shall return the number of nodes in the subtree
 * rooted at the node pointed to by node_ptr
 */
template<typename T>
concept contains_subtree_size = requires(const T *node_ptr)
{
    typename T::size_type;
    { T::size(node_ptr) } -> std::convertible_to<typename T::size_type>;
};

template<typename T>
concept has_parent = requires(T *node_ptr, const T *const_node_ptr)
{
    { node_ptr->get_parent() } -> std::convertible_to<T *>;
    { const_node_ptr->get_parent() } -> std::convertible_to<const T*>;
};

} // namespace yLab

#endif // INCLUDE_NODES_SUBTREE_SIZES_HPP
