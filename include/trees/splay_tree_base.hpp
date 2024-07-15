#ifndef INCLUDE_SPLAY_TREE_HPP
#define INCLUDE_SPLAY_TREE_HPP

#include <functional>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <cassert>
#include <concepts>
#include <stdexcept>

#include "search_tree.hpp"
#include "node_concepts.hpp"

namespace yLab
{

template<typename Node_T, typename Base_Node_T,
         typename Compare = std::less<typename Node_T::key_type>>
class Splay_Tree_Base final : public Search_Tree<Node_T, Base_Node_T, Compare>
{
    using base_tree = Search_Tree<Node_T, Base_Node_T, Compare>;
    using base_tree::control_node_;
    using base_tree::comp_;
    using base_tree::size_;
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

    using base_tree::begin;
    using base_tree::end;
    using base_tree::swap;
    using base_tree::size;
    using base_tree::empty;
    using base_tree::insert;
    using base_tree::lower_bound;
    using base_tree::upper_bound;

    Splay_Tree_Base() : Splay_Tree_Base{key_compare{}} {}

    explicit Splay_Tree_Base(const key_compare &comp) : base_tree{comp} {}

    template<std::input_iterator it>
    Splay_Tree_Base(it first, it second, const key_compare &comp = key_compare{}) : base_tree{comp}
    {
        insert(first, second);
    }

    Splay_Tree_Base(std::initializer_list<value_type> ilist,
                    const key_compare &comp = key_compare{})
        : Splay_Tree_Base(ilist.begin(), ilist.end(), comp) {}

    Splay_Tree_Base(const Splay_Tree_Base &rhs)
        : Splay_Tree_Base(rhs.begin(), rhs.end(), rhs.comp_) {}

    Splay_Tree_Base &operator=(const Splay_Tree_Base &rhs)
    {
        auto tmp_tree{rhs};
        swap(tmp_tree);

        return *this;
    }

    Splay_Tree_Base(Splay_Tree_Base &&rhs) = default;
    Splay_Tree_Base &operator=(Splay_Tree_Base &&rhs) = default;

    ~Splay_Tree_Base() override = default;

    void join(Splay_Tree_Base &&rhs)
    {
        if (empty())
            swap(rhs); // leads to change of the comparator if one of rhs differs from ours
        else if (!rhs.empty())
        {
            auto lhs_rightmost = static_cast<node_ptr>(control_node_.get_rightmost());
            auto rhs_leftmost = static_cast<node_ptr>(rhs.control_node_.get_leftmost());

            if (!comp_(lhs_rightmost->get_key(), rhs_leftmost->get_key()))
                throw std::runtime_error{"Trees can't be joined"};

            splay(lhs_rightmost);

            base_node_ptr root = control_node_.get_root();
            base_node_ptr rhs_root = rhs.control_node_.get_root();
            root->set_right(rhs_root);
            rhs_root->set_parent(root);

            rhs_leftmost->set_left_thread(root);
            base_node_ptr rhs_rightmost = rhs.control_node_.get_rightmost();
            rhs_rightmost->set_right_thread(control_node_.get_end_node());
            control_node_.set_rightmost(rhs_rightmost);

            rhs.control_node_.reset();

            size_ += rhs.size_;
            rhs.size_ = 0;
        }
    }

    Splay_Tree_Base split(const key_type &key)
    requires contains_subtree_size<node_type>
    {
        const_base_node_ptr node = find_impl(key);
        if (node == control_node_.get_end_node())
            return {};

        // key is found => the tree is not empty
        base_node_ptr left_root = control_node_.get_root();
        base_node_ptr right_root = left_root->get_left();
        if (right_root)
            left_root->set_right_thread(control_node_.get_end_node());
        else
            return {};

        Splay_Tree_Base right_tree;
        right_root->set_parent(right_tree.control_node_.get_end_node());
        right_tree.control_node_.set_root(right_root);
        right_tree.control_node_.set_leftmost(right_root->minimum());
        right_tree.control_node_.set_rightmost(control_node_.get_rightmost());
        control_node_.set_rightmost(const_cast<base_node_ptr>(node));

        right_tree.size_ = node_type::size(static_cast<node_ptr>(right_root));
        size_ -= right_tree.size_;

        return right_tree;
    }

    size_type n_less_than(const key_type &key) const
    requires contains_subtree_size<node_type>
    {
        return empty() ? 0 : n_less_than_node(lower_bound(key));
    }

    size_type n_less_or_equal_to(const key_type &key) const
    requires contains_subtree_size<node_type>
    {
        return empty() ? 0 : n_less_than_node(upper_bound(key));
    }

private:

    using base_tree::find_with_parent;
    using base_tree::transplant;

    // Lookup

    const_base_node_ptr find_impl(const key_type &key) const override
    {
        auto [node, parent] = find_with_parent(key);
        auto result = lookup_splay(node, parent);

        return result;
    }

    const_base_node_ptr lower_bound_impl(const key_type &key) const override
    {
        const_base_node_ptr lower_bound = nullptr;
        const_base_node_ptr parent = control_node_.get_end_node();

        const_base_node_ptr node = control_node_.get_root();
        while (node)
        {
            parent = node;

            if (!comp_(static_cast<const_node_ptr>(node)->get_key(), key))
                lower_bound = std::exchange(node, node->get_left());
            else
                node = node->get_right();
        }

        auto result = lookup_splay(lower_bound, parent);

        return result;
    }

    const_base_node_ptr upper_bound_impl(const key_type &key) const override
    {
        const_base_node_ptr upper_bound = nullptr;
        const_base_node_ptr parent = control_node_.get_end_node();

        const_base_node_ptr node = control_node_.get_root();
        while (node)
        {
            parent = node;

            if (comp_(key, static_cast<const_node_ptr>(node)->get_key()))
                upper_bound = std::exchange(node, node->get_left());
            else
                node = node->get_right();
        }

        auto result = lookup_splay(upper_bound, parent);

        return result;
    }

    const_base_node_ptr lookup_splay(const_base_node_ptr node, const_base_node_ptr parent) const
    {
        if (node)
        {
            splay(const_cast<base_node_ptr>(node));
            return node;
        }
        else
        {
            const_base_node_ptr end_node = control_node_.get_end_node();
            if (parent != end_node)
                splay(const_cast<base_node_ptr>(parent));

            return end_node;
        }
    }

    size_type n_less_than_node(const_iterator it) const
    requires contains_subtree_size<node_type>
    {
        if (it == end())
            return size();
        else
        {
            const_base_node_ptr node = base_tree::const_base_ptr(it);
            return node_type::size(static_cast<const_node_ptr>(node->get_left()));
        }
    }

    // Modifiers

    base_node_ptr insert_impl(const key_type &key, base_node_ptr parent) override
    {
        base_node_ptr new_node = new node_type{key, nullptr, nullptr, parent};
        base_node_ptr end_node = control_node_.get_end_node();

        if (parent == end_node)
        {
            new_node->set_left_thread(end_node);
            new_node->set_right_thread(end_node);

            parent->set_left(new_node);
        }
        else
        {
            if (comp_(key, static_cast<node_ptr>(parent)->get_key()))
            {
                base_node_ptr predecessor = parent->get_left_unsafe();

                splay(parent);

                if (predecessor == end_node) // parent is leftmost
                    new_node->set_left_thread(end_node);
                else
                {
                    base_node_ptr left = parent->get_left_unsafe();
                    left->set_parent(new_node);
                    predecessor->set_right_thread(new_node);
                    new_node->set_left(left);
                }

                new_node->set_right_thread(parent);
                parent->set_left(new_node);
            }
            else
            {
                base_node_ptr successor = parent->get_right_unsafe();

                splay(parent);

                if (successor == end_node) // parent is rightmost
                    new_node->set_right_thread(end_node);
                else
                {
                    base_node_ptr right = parent->get_right_unsafe();
                    right->set_parent(new_node);
                    successor->set_left_thread(new_node);
                    new_node->set_right(right);
                }

                new_node->set_left_thread(parent);
                parent->set_right(new_node);
            }
        }

        return new_node;
    }

    void erase_impl(base_node_ptr node) override
    {
        assert(node);

        splay(node);

        base_node_ptr left = node->get_left();
        base_node_ptr right = node->get_right();

        if (left)
        {
            if (right)
            {
                control_node_.set_root(left);
                left->set_parent(control_node_.get_end_node());

                base_node_ptr predecessor = left->maximum();
                splay(predecessor);
                predecessor->set_right(right);
                right->set_parent(predecessor);

                base_node_ptr successor = right->minimum();
                successor->set_left_thread(predecessor);
            }
            else
            {
                transplant(node, left);
                base_node_ptr predecessor = left->maximum();
                predecessor->set_right_thread(node->get_right_unsafe());
            }
        }
        else if (right)
        {
            transplant(node, right);
            base_node_ptr successor = right->minimum();
            successor->set_left_thread(node->get_left_unsafe());
        }

        if (size_ == 1)
            control_node_.set_root(nullptr);
    }

    void splay(base_node_ptr node) const
    {
        assert(node);

        while (node != control_node_.get_root())
        {
            base_node_ptr parent = node->get_parent();
            bool node_is_left_child = node->is_left_child();

            if (parent == control_node_.get_root())
            {
                if (node_is_left_child)
                    parent->right_rotate();
                else
                    parent->left_rotate();
            }
            else
            {
                base_node_ptr grandparent = parent->get_parent();
                bool parent_is_left_child = parent->is_left_child();

                if (node_is_left_child)
                {
                    if (parent_is_left_child)
                    {
                        grandparent->right_rotate();
                        parent->right_rotate();
                    }
                    else
                    {
                        parent->right_rotate();
                        grandparent->left_rotate();
                    }
                }
                else
                {
                    if (parent_is_left_child)
                    {
                        parent->left_rotate();
                        grandparent->right_rotate();
                    }
                    else
                    {
                        grandparent->left_rotate();
                        parent->left_rotate();
                    }
                }
            }
        }
    }
};

} // namespace yLab

#endif // INCLUDE_SPLAY_TREE_HPP
