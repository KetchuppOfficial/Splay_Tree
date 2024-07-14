#ifndef INCLUDE_TREES_HPP
#define INCLUDE_TREES_HPP

#include <functional>

#include "node_base.hpp"
#include "node.hpp"
#include "augmented_node.hpp"

#include "search_tree.hpp"
#include "splay_tree_base.hpp"

namespace yLab
{

template<typename Key_T, typename Compare = std::less<Key_T>>
using BST = Search_Tree<Node<Key_T>, Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Augmented_BST = Search_Tree<Augmented_Node<Key_T>, Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Splay_Tree = Splay_Tree_Base<Node<Key_T>, Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Augmented_Splay_Tree = Splay_Tree_Base<Augmented_Node<Key_T>, Node_Base, Compare>;

} // namespace yLab

#endif // INCLUDE_TREES_HPP
