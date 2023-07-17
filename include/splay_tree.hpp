#ifndef INCLUDE_SPLAY_TREE_HPP
#define INCLUDE_SPLAY_TREE_HPP

#include <functional>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <cassert>

#include "search_tree.hpp"
#include "splay_node.hpp"

namespace yLab
{

template<typename Key_T, typename Compare = std::less<Key_T>>
class Splay_Tree : public Search_Tree<Splay_Node<Key_T>, Compare>
{
    using base_tree = Search_Tree<Splay_Node<Key_T>, Compare>;
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

    Splay_Tree (const Splay_Tree &rhs) : base_tree{rhs}
    {
        for (auto &&key : rhs)
            this->insert_unique (key);
    }

    Splay_Tree &operator= (const Splay_Tree &rhs)
    {
        auto tmp_tree{rhs};
        std::swap (*this, tmp_tree);

        return *this;
    }

    Splay_Tree (Splay_Tree &&rhs) : base_tree{std::move (rhs)} {}

    Splay_Tree &operator= (Splay_Tree &&rhs)
    {
        this->swap (rhs);
        return *this;
    }

    // Lookup

    const_iterator find (const key_type &key) const override
    {
        auto node = this->find_impl (key);
        
        if (node)
        {
            splay (const_cast<node_ptr>(node));
            return const_iterator{node};
        }
        else
            return this->end();
    }

    const_iterator lower_bound (const key_type &key) const override
    {
        auto node = this->lower_bound_impl (key);

        if (node)
        {
            splay (const_cast<node_ptr>(node));
            return const_iterator{node};
        }
        else
            return this->end();
    }

    const_iterator upper_bound (const key_type &key) const override
    {
        auto node = this->upper_bound_impl (key);

        if (node)
        {
            splay (const_cast<node_ptr>(node));
            return const_iterator{node};
        }
        else
            return this->end();
    }

protected:

    node_ptr insert_impl (const key_type &key, base_node_ptr parent) override
    {
        auto new_node = this->bst_insert (key, parent);
        splay (new_node);

        return new_node;
    }

    void erase_impl (node_ptr node) override
    {
        splay (node);
        this->bst_erase (node);
    }

private:

    void splay (node_ptr node) const
    {
        assert (node);
        
        while (node != this->control_node_.get_root())
        {
            auto parent = node->get_parent();
            auto node_is_left_child = node->is_left_child ();

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
                auto parent_is_left_child = parent->is_left_child ();

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
