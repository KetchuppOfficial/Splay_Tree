#ifndef INCLUDE_NODES_NODE_CONCEPTS_HPP
#define INCLUDE_NODES_NODE_CONCEPTS_HPP

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

} // namespace yLab

#endif // INCLUDE_NODES_NODE_CONCEPTS_HPP
