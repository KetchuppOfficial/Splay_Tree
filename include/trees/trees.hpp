#ifndef INCLUDE_TREES_TREES_HPP
#define INCLUDE_TREES_TREES_HPP

#include <functional>

#include "nodes/node.hpp"
#include "nodes/augmented_node.hpp"

#include "trees/search_tree.hpp"
#include "trees/splay_tree_base.hpp"

namespace yLab
{

template<typename Key_T, typename Compare = std::less<Key_T>>
using BST = Search_Tree<Node<Key_T>, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Augmented_BST = Search_Tree<Augmented_Node<Key_T>, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Splay_Tree = Splay_Tree_Base<Node<Key_T>, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Augmented_Splay_Tree = Splay_Tree_Base<Augmented_Node<Key_T>, Compare>;

} // namespace yLab

#endif // INCLUDE_TREES_TREES_HPP
