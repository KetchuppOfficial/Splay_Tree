#ifndef INCLUDE_SPLAY_TREE_HPP
#define INCLUDE_SPLAY_TREE_HPP

#include <functional>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <cassert>

#include "search_tree.hpp"
#include "splay_node.hpp"

namespace yLab
{

template<typename Key_T, typename Compare = std::less<Key_T>>
class Splay_Tree : public Search_Tree<Splay_Node<Key_T>, Node_Base, Compare>
{
    using base_tree = Search_Tree<Splay_Node<Key_T>, Node_Base, Compare>;
    using typename base_tree::base_node_ptr;
    using typename base_tree::const_base_node_ptr;
    using typename base_tree::node_ptr;
    using typename base_tree::const_node_ptr;

public:

    using typename base_tree::node_type;
    using typename base_tree::key_type;
    using typename base_tree::key_compare;
    using typename base_tree::value_type;
    using typename base_tree::value_compare;
    using typename base_tree::pointer;
    using typename base_tree::const_pointer;
    using typename base_tree::reference;
    using typename base_tree::const_reference;
    using typename base_tree::size_type;
    using typename base_tree::difference_type;
    using typename base_tree::iterator;
    using typename base_tree::const_iterator;
    using typename base_tree::reverse_iterator;
    using typename base_tree::const_reverse_iterator;

    Splay_Tree () : Splay_Tree{key_compare{}} {}

    explicit Splay_Tree (const key_compare &comp) : base_tree{comp} {}

    template<std::input_iterator it>
    Splay_Tree (it first, it second, const key_compare &comp = key_compare{}) : base_tree{comp}
    {
        this->insert (first, second);
    }

    Splay_Tree (std::initializer_list<value_type> ilist, const key_compare &comp = key_compare{})
               : base_tree{comp}
    {
        this->insert (ilist);
    }

    Splay_Tree (const Splay_Tree &rhs) : base_tree{rhs.comp_}
    {
        this->insert (rhs.begin(), rhs.end());
    }

    Splay_Tree &operator= (const Splay_Tree &rhs)
    {
        auto tmp_tree{rhs};
        std::swap (*this, tmp_tree);

        return *this;
    }

    Splay_Tree (Splay_Tree &&rhs) = default;
    Splay_Tree &operator= (Splay_Tree &&rhs) = default;

protected:

    // Lookup

    const_base_node_ptr find_impl (const key_type &key) const override
    {
        auto [node, parent] = this->find_with_parent (key);
        auto result = lookup_splay (node, parent);

        return result;
    }

    const_base_node_ptr lower_bound_impl (const key_type &key) const override
    {
        auto parent = this->control_node_.get_end_node();
        const_base_node_ptr lower_bound = nullptr;

        auto node = this->control_node_.get_root();
        while (node)
        {
            parent = node;

            if (!this->comp_(node->get_key(), key)) // key <= node->get_key()
                lower_bound = std::exchange (node, node->get_left());
            else
                node = node->get_right();
        }

        auto result = lookup_splay (lower_bound, parent);

        return result;
    }

    const_base_node_ptr upper_bound_impl (const key_type &key) const override
    {
        auto parent = this->control_node_.get_end_node();
        const_base_node_ptr upper_bound = nullptr;

        auto node = this->control_node_.get_root();
        while (node)
        {
            parent = node;

            if (this->comp_(key, node->get_key())) // key < node->get_key()
                upper_bound = std::exchange (node, node->get_left());
            else
                node = node->get_right();
        }

        auto result = lookup_splay (upper_bound, parent);

        return result;
    }

    const_base_node_ptr lookup_splay (const_base_node_ptr node, const_base_node_ptr parent) const
    {
        if (node)
        {
            splay (static_cast<node_ptr>(const_cast<base_node_ptr>(node)));
            return node;
        }
        else
        {
            auto end_node = this->control_node_.get_end_node();
            if (parent != end_node)
                splay (static_cast<node_ptr>(const_cast<base_node_ptr>(parent)));

            return end_node;
        }
    }

    // Modifiers

    node_ptr insert_impl (const key_type &key, base_node_ptr parent) override
    {
        auto new_node = this->bst_insert (key, parent);
        Splay_Tree::increment_size (parent, this->control_node_.get_end_node());
        splay (new_node);

        return new_node;
    }

    static void increment_size (base_node_ptr from, base_node_ptr to)
    {
        assert (from);

        for (; from != to; from = from->get_parent())
            static_cast<node_ptr>(from)->size_++;
    }

    void erase_impl (node_ptr node) override
    {
        assert (node);

        splay (node);

        if (node->get_left() == nullptr)
            this->transplant (node, node->get_right());
        else if (node->get_right() == nullptr)
            this->transplant (node, node->get_left());
        else
            join (node->get_left(), node->get_right());
    }

    void join (node_ptr left_subtree, node_ptr right_subtree)
    {
        assert (left_subtree);
        assert (right_subtree);
        assert (left_subtree == this->control_node_.get_root()->get_left());
        assert (right_subtree == this->control_node_.get_root()->get_right());

        this->control_node_.set_root (left_subtree);
        left_subtree->set_parent (this->control_node_.get_end_node());

        auto left_max = static_cast<node_ptr>(left_subtree->maximum());

        splay (left_max);
        left_max->set_right (right_subtree);
        right_subtree->set_parent (left_max);
        left_max->size_ += right_subtree->size_;
    }

    void splay (node_ptr node) const
    {
        assert (node);

        while (node != this->control_node_.get_root())
        {
            auto parent = node->get_parent();
            auto node_is_left_child = node->is_left_child();

            if (parent == this->control_node_.get_root())
            {
                if (node_is_left_child)
                    parent->right_rotate();
                else
                    parent->left_rotate();
            }
            else
            {
                auto grandparent = parent->get_parent();
                auto parent_is_left_child = parent->is_left_child();

                if (node_is_left_child && parent_is_left_child)
                {
                    grandparent->right_rotate();
                    parent->right_rotate();
                }
                else if (!node_is_left_child && !parent_is_left_child)
                {
                    grandparent->left_rotate();
                    parent->left_rotate();
                }
                else if (node_is_left_child && !parent_is_left_child)
                {
                    parent->right_rotate();
                    grandparent->left_rotate();
                }
                else
                {
                    parent->left_rotate();
                    grandparent->right_rotate();
                }
            }
        }
    }
};

template<typename It, typename Compare>
Splay_Tree (It begin, It end, const Compare &comp)
-> Splay_Tree<typename std::iterator_traits<It>::value_type, Compare>;

template<typename T, typename... U>
Splay_Tree (T, U...) -> Splay_Tree<std::enable_if_t<(std::is_same_v<T, U> && ...), T>, std::less<T>>;

} // namespace yLab

#endif // INCLUDE_SPLAY_TREE_HPP
