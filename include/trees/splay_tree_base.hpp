#ifndef INCLUDE_SPLAY_TREE_HPP
#define INCLUDE_SPLAY_TREE_HPP

#include <functional>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <cassert>
#include <concepts>

#include "search_tree.hpp"
#include "../subtree_sizes.hpp"

namespace yLab
{

template<typename Node_T, typename Base_Node_T,
         typename Compare = std::less<typename Node_T::key_type>>
class Splay_Tree_Base : public Search_Tree<Node_T, Base_Node_T, Compare>
{
    using base_tree = Search_Tree<Node_T, Base_Node_T, Compare>;
    using base_tree::control_node_;
    using base_tree::comp_;
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

    using base_tree::end;
    using base_tree::size;
    using base_tree::empty;
    using base_tree::insert;
    using base_tree::lower_bound;
    using base_tree::upper_bound;

    Splay_Tree_Base () : Splay_Tree_Base{key_compare{}} {}

    explicit Splay_Tree_Base (const key_compare &comp) : base_tree{comp} {}

    template<std::input_iterator it>
    Splay_Tree_Base (it first, it second, const key_compare &comp = key_compare{}) : base_tree{comp}
    {
        insert (first, second);
    }

    Splay_Tree_Base (std::initializer_list<value_type> ilist,
                     const key_compare &comp = key_compare{}) : base_tree{comp}
    {
        insert (ilist);
    }

    Splay_Tree_Base (const Splay_Tree_Base &rhs) : base_tree{rhs.comp_}
    {
        insert (rhs.begin(), rhs.end());
    }

    Splay_Tree_Base &operator= (const Splay_Tree_Base &rhs)
    {
        auto tmp_tree{rhs};
        std::swap (*this, tmp_tree);

        return *this;
    }

    Splay_Tree_Base (Splay_Tree_Base &&rhs) = default;
    Splay_Tree_Base &operator= (Splay_Tree_Base &&rhs) = default;

    ~Splay_Tree_Base () override = default;

    size_type n_less_than (const key_type &key) const
    requires contains_subtree_size<node_type>
    {
        if (empty())
            return 0;
        else
            return n_less_than_node (lower_bound (key));
    }

    size_type n_less_or_equal_to (const key_type &key) const
    requires contains_subtree_size<node_type>
    {
        if (empty())
            return 0;
        else
            return n_less_than_node (upper_bound (key));
    }

protected:

    using base_tree::find_with_parent;
    using base_tree::transplant;
    using base_tree::bst_insert;

    // Lookup

    const_base_node_ptr find_impl (const key_type &key) const override
    {
        auto [node, parent] = find_with_parent (key);
        auto result = lookup_splay (node, parent);

        return result;
    }

    const_base_node_ptr lower_bound_impl (const key_type &key) const override
    {
        auto parent = control_node_.get_end_node();
        const_base_node_ptr lower_bound = nullptr;

        auto node = control_node_.get_root();
        while (node)
        {
            parent = node;

            if (!comp_(node->get_key(), key)) // key <= node->get_key()
                lower_bound = std::exchange (node, node->get_left());
            else
                node = node->get_right();
        }

        auto result = lookup_splay (lower_bound, parent);

        return result;
    }

    const_base_node_ptr upper_bound_impl (const key_type &key) const override
    {
        auto parent = control_node_.get_end_node();
        const_base_node_ptr upper_bound = nullptr;

        auto node = control_node_.get_root();
        while (node)
        {
            parent = node;

            if (comp_(key, node->get_key())) // key < node->get_key()
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
            auto end_node = control_node_.get_end_node();
            if (parent != end_node)
                splay (static_cast<node_ptr>(const_cast<base_node_ptr>(parent)));

            return end_node;
        }
    }

    size_type n_less_than_node (const_iterator it) const
    requires contains_subtree_size<node_type>
    {
        if (it == end())
            return size();
        else
            return node_type::size (static_cast<const_node_ptr>(it.base())->get_left());
    }

    // Modifiers

    node_ptr insert_impl (const key_type &key, base_node_ptr parent) override
    {
        auto new_node = bst_insert (key, parent);
        splay (new_node);

        return new_node;
    }

    void erase_impl (node_ptr node) override
    {
        assert (node);

        splay (node);

        if (node->get_left() == nullptr)
            transplant (node, node->get_right());
        else if (node->get_right() == nullptr)
            transplant (node, node->get_left());
        else
            join (node->get_left(), node->get_right());
    }

    void join (node_ptr left_subtree, node_ptr right_subtree)
    {
        assert (control_node_.get_root());
        assert (left_subtree == control_node_.get_root()->get_left());
        assert (right_subtree == control_node_.get_root()->get_right());

        control_node_.set_root (left_subtree);
        left_subtree->set_parent (control_node_.get_end_node());

        auto left_max = static_cast<node_ptr>(left_subtree->maximum());

        splay (left_max);
        left_max->set_right (right_subtree);
        right_subtree->set_parent (left_max);
    }

    void splay (node_ptr node) const
    {
        assert (node);

        while (node != control_node_.get_root())
        {
            auto parent = node->get_parent();
            auto node_is_left_child = node->is_left_child();

            if (parent == control_node_.get_root())
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

} // namespace yLab

#endif // INCLUDE_SPLAY_TREE_HPP
