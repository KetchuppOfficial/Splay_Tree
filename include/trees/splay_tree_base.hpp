#ifndef INCLUDE_TREES_SPLAY_TREE_BASE_HPP
#define INCLUDE_TREES_SPLAY_TREE_BASE_HPP

#include <functional>
#include <iterator>
#include <initializer_list>
#include <utility>
#include <cassert>

#include "nodes/node_concepts.hpp"
#include "trees/search_tree.hpp"

namespace yLab
{

template<typename Node_T, typename Compare = std::less<typename Node_T::key_type>>
class Splay_Tree_Base final : public Search_Tree<Node_T, Compare>
{
    using base_tree = Search_Tree<Node_T, Compare>;

    using typename base_tree::base_node_ptr;
    using typename base_tree::const_base_node_ptr;
    using typename base_tree::node_ptr;
    using typename base_tree::const_node_ptr;

public:

    using typename base_tree::node_type;
    using typename base_tree::key_type;
    using typename base_tree::key_compare;
    using typename base_tree::value_type;
    using typename base_tree::size_type;
    using typename base_tree::iterator;
    using typename base_tree::const_iterator;

    Splay_Tree_Base() : Splay_Tree_Base{key_compare()} {}

    explicit Splay_Tree_Base(const key_compare &comp) : base_tree(comp) {}

    template<std::input_iterator it>
    Splay_Tree_Base(it first, it second, const key_compare &comp = key_compare()) : base_tree(comp)
    {
        this->insert(first, second);
    }

    Splay_Tree_Base(std::initializer_list<value_type> ilist,
                    const key_compare &comp = key_compare())
        : Splay_Tree_Base(ilist.begin(), ilist.end(), comp) {}

    Splay_Tree_Base(const Splay_Tree_Base &rhs)
        : Splay_Tree_Base(rhs.begin(), rhs.end(), rhs.comp_) {}

    Splay_Tree_Base &operator=(const Splay_Tree_Base &rhs)
    {
        auto tmp_tree{rhs};
        this->swap(tmp_tree);

        return *this;
    }

    Splay_Tree_Base(Splay_Tree_Base &&rhs) = default;
    Splay_Tree_Base &operator=(Splay_Tree_Base &&rhs) = default;

    ~Splay_Tree_Base() override = default;

    bool join(Splay_Tree_Base &&rhs)
    {
        if (this->empty())
            this->swap(rhs); // leads to change of the comparator if one of rhs differs from ours
        else if (!rhs.empty())
        {
            auto lhs_rightmost = static_cast<node_ptr>(this->get_rightmost());
            auto rhs_leftmost = static_cast<node_ptr>(rhs.get_leftmost());

            if (!this->comp_(lhs_rightmost->get_key(), rhs_leftmost->get_key()))
                return false;

            splay(lhs_rightmost);

            base_node_ptr root = this->get_root();
            base_node_ptr rhs_root = rhs.get_root();
            root->set_right(rhs_root);
            rhs_root->set_parent(root);

            rhs_leftmost->set_left_thread(root);
            base_node_ptr rhs_rightmost = rhs.get_rightmost();
            rhs_rightmost->set_right_thread(&this->end_);
            this->set_rightmost(rhs_rightmost);

            rhs.reset();

            this->size_ += std::exchange(rhs.size_, 0);
        }

        return true;
    }

    Splay_Tree_Base split(const key_type &key)
    requires contains_subtree_size<node_type>
    {
        const_base_node_ptr node = do_find(key);
        base_node_ptr end_node = &this->end_;

        if (node == end_node)
            return {};

        // key is found => the tree is not empty
        base_node_ptr left_root = this->get_root();
        base_node_ptr right_root = left_root->get_right();
        if (right_root)
            left_root->set_right_thread(end_node);
        else
            return {};

        Splay_Tree_Base right_tree;

        right_root->set_parent(&right_tree.end_);
        right_tree.set_root(right_root);
        right_tree.set_leftmost(right_root->minimum());
        right_tree.set_rightmost(this->get_rightmost());
        this->set_rightmost(const_cast<base_node_ptr>(node));

        const auto right_size = node_type::size(static_cast<node_ptr>(right_root));
        right_tree.size_ = right_size;
        this->size_ -= right_size;

        return right_tree;
    }

    size_type n_less_than(const key_type &key) const
    requires contains_subtree_size<node_type>
    {
        return this->empty() ? 0 : n_less_than_node(this->lower_bound(key));
    }

private:

    // Lookup

    const_base_node_ptr do_find(const key_type &key) const override
    {
        auto [node, parent] = this->find_with_parent(key);
        auto result = lookup_splay(node, parent);

        return result;
    }

    const_base_node_ptr do_lower_bound(const key_type &key) const override
    {
        const_base_node_ptr lower_bound = nullptr;
        const_base_node_ptr parent = &this->end_;
        const_base_node_ptr node = this->get_root();

        while (node)
        {
            parent = node;

            if (!this->comp_(static_cast<const_node_ptr>(node)->get_key(), key))
                lower_bound = std::exchange(node, node->get_left());
            else
                node = node->get_right();
        }

        auto result = lookup_splay(lower_bound, parent);

        return result;
    }

    const_base_node_ptr do_upper_bound(const key_type &key) const override
    {
        const_base_node_ptr upper_bound = nullptr;
        const_base_node_ptr parent = &this->end_;
        const_base_node_ptr node = this->get_root();

        while (node)
        {
            parent = node;

            if (this->comp_(key, static_cast<const_node_ptr>(node)->get_key()))
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
            const_base_node_ptr end_node = &this->end_;
            if (parent != end_node)
                splay(const_cast<base_node_ptr>(parent));

            return end_node;
        }
    }

    size_type n_less_than_node(const_iterator it) const
    requires contains_subtree_size<node_type>
    {
        if (it == this->end())
            return this->size();
        else
        {
            const_base_node_ptr node = base_tree::const_base_ptr(it);
            return node_type::size(static_cast<const_node_ptr>(node->get_left()));
        }
    }

    // Modifiers

    base_node_ptr do_insert(const key_type &key, base_node_ptr parent) override
    {
        assert(parent);
        assert(!parent->get_left() || !parent->get_right());

        base_node_ptr new_node = new node_type{key, nullptr, nullptr, parent};
        base_node_ptr end_node = &this->end_;

        if (parent == end_node)
        {
            new_node->set_left_thread(end_node);
            new_node->set_right_thread(end_node);

            parent->set_left(new_node);
        }
        else if (this->comp_(key, static_cast<node_ptr>(parent)->get_key()))
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

        return new_node;
    }

    void unlink_node(base_node_ptr node) override
    {
        assert(node);

        splay(node);

        base_node_ptr left = node->get_left();
        base_node_ptr right = node->get_right();

        if (left)
        {
            if (right)
            {
                this->set_root(left);
                left->set_parent(&this->end_);

                base_node_ptr predecessor = left->maximum();
                splay(predecessor);
                predecessor->set_right(right);
                right->set_parent(predecessor);

                base_node_ptr successor = right->minimum();
                successor->set_left_thread(predecessor);
            }
            else
            {
                this->transplant(node, left);
                base_node_ptr predecessor = left->maximum();
                predecessor->set_right_thread(node->get_right_unsafe());
            }
        }
        else if (right)
        {
            this->transplant(node, right);
            base_node_ptr successor = right->minimum();
            successor->set_left_thread(node->get_left_unsafe());
        }

        if (this->size() == 1)
            this->set_root(nullptr);
    }

    void splay(base_node_ptr node) const
    {
        assert(node);

        while (node != this->get_root())
        {
            if (base_node_ptr parent = node->get_parent(); parent == this->get_root())
            {
                if (node->is_left_child())
                    parent->right_rotate();
                else
                    parent->left_rotate();
            }
            else
            {
                base_node_ptr grandparent = parent->get_parent();

                if (node->is_left_child())
                {
                    if (parent->is_left_child())
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
                    if (parent->is_left_child())
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

#endif // INCLUDE_TREES_SPLAY_TREE_BASE_HPP
