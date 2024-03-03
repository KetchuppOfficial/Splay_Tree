#ifndef INCLUDE_SPLAY_TREES_HPP
#define INCLUDE_SPLAY_TREES_HPP

#include <functional>

#include "../nodes/node_base.hpp"
#include "../nodes/node.hpp"
#include "../nodes/augmented_node.hpp"
#include "splay_tree_base.hpp"

namespace yLab
{

template<typename Key_T, typename Compare = std::less<Key_T>>
using BST = Search_Tree<Node<Key_T>, Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Threaded_BST = Search_Tree<Threaded_Node<Key_T>, Threaded_Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Splay_Tree = Splay_Tree_Base<Node<Key_T>, Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Augmented_Splay_Tree = Splay_Tree_Base<Augmented_Node<Key_T>, Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Threaded_Splay_Tree = Splay_Tree_Base<Threaded_Node<Key_T>, Threaded_Node_Base, Compare>;

template<typename Key_T, typename Compare = std::less<Key_T>>
using Threaded_Augmented_Splay_Tree
    = Splay_Tree_Base<Threaded_Augmented_Node<Key_T>,Threaded_Node_Base, Compare>;

} // namespace yLab

#endif // INCLUDE_SPLAY_TREES_HPP
