#ifndef INCLUDE_SPLAY_TREES_HPP
#define INCLUDE_SPLAY_TREES_HPP

#include <functional>

#include "../nodes/node_base.hpp"
#include "../nodes/splay_node.hpp"
#include "../nodes/augmented_splay_node.hpp"
#include "splay_tree_base.hpp"

namespace yLab
{

template<typename Key_T, typename Compare = std::less<Key_T>>
using Splay_Tree = Splay_Tree_Base<Splay_Node<Key_T>, Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Augmented_Splay_Tree = Splay_Tree_Base<Augmented_Splay_Node<Key_T>, Node_Base, Compare>;

} // namespace yLab

#endif // INCLUDE_SPLAY_TREES_HPP
