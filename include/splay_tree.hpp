#ifndef INCLUDE_SPLAY_TREE_HPP
#define INCLUDE_SPLAY_TREE_HPP

#include <cassert>
#include <functional>
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <memory>
#include <tuple>
#include <compare>

#include "nodes.hpp"
#include "tree_iterator.hpp"

#ifdef DEBUG
#include <iostream>
#include "graphic_dump.hpp"
#endif // DEBUG

namespace yLab
{

template<typename Key_T, typename Compare = std::less<Key_T>>
class Splay_Tree
{
public:

    using key_type = Key_T;
    using key_compare = Compare;
    using value_type = key_type;
    using value_compare = Compare;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using node_type = Node<key_type>;
    using const_iterator = tree_iterator<node_type>;
    using iterator = const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:

    using node_ptr = node_type *;
    using const_node_ptr = const node_type *;
    using end_node_type = End_Node<node_type>;
    using end_node_ptr = end_node_type *;
    using const_end_node_ptr = const end_node_type *;

    struct Root_Wrapper
    {
        end_node_type end_node_{};

        Root_Wrapper () = default;

        Root_Wrapper (const Root_Wrapper &rhs) = delete;
        Root_Wrapper &operator= (const Root_Wrapper &rhs) = delete;

        Root_Wrapper (Root_Wrapper &&rhs) noexcept (std::is_nothrow_move_constructible_v<end_node_type>)
                     : end_node_{std::move (rhs.end_node_)} {}

        Root_Wrapper &operator= (Root_Wrapper &&rhs) noexcept (std::is_nothrow_swappable_v<end_node_type>)
        {
            std::swap (end_node_, rhs.end_node_);

            return *this;
        }

        void clean_up ()
        {
            for (node_ptr node = end_node_.get_left(), save{}; node != nullptr; node = save)
            {
                if (node->get_left() == nullptr)
                {
                    save = node->get_right();
                    delete node;
                }
                else
                {
                    save = node->get_left();
                    node->set_left (save->get_right());
                    save->set_right (node);
                }
            }
        }

        ~Root_Wrapper () { clean_up(); }
    };

    Root_Wrapper root_{};
    const_end_node_ptr leftmost_ = std::addressof (end_node());
    key_compare comp_;
    size_type size_{};

public:

    Splay_Tree () : Splay_Tree{key_compare{}} {}

    explicit Splay_Tree (const key_compare &comp) : comp_{comp} {}

    template<std::input_iterator it>
    Splay_Tree (it first, it last, const key_compare &comp = key_compare{}) : comp_{comp}
    {
        insert (first, last);
    }

    Splay_Tree (std::initializer_list<value_type> ilist, const key_compare &comp = key_compare{}) 
            : comp_{comp}
    {
        insert (ilist);
    }

    Splay_Tree (const Splay_Tree &rhs) : comp_{rhs.comp_}
    {
        for (auto &&key : rhs)
            insert_unique (key);
    }

    Splay_Tree &operator= (const Splay_Tree &rhs)
    {
        auto tmp_tree{rhs};
        std::swap (*this, tmp_tree);

        return *this;
    }

    Splay_Tree (Splay_Tree &&rhs)
               : root_{std::move (rhs.root_)},
                 leftmost_{std::exchange (rhs.leftmost_, std::addressof (rhs.end_node()))},
                 comp_{std::move (rhs.comp_)},
                 size_{std::move (rhs.size_)} {}

    Splay_Tree &operator= (Splay_Tree &&rhs) noexcept (std::is_nothrow_swappable_v<key_compare> &&
                                                       std::is_nothrow_swappable_v<end_node_type>)
    {
        swap (rhs);

        return *this;
    }

    ~Splay_Tree () = default;

    // Observers

    const key_compare &key_comp () const { return comp_; }

    const value_compare &value_comp () const { return key_comp(); }

    // Capacity

    size_type size () const noexcept { return size_; }
    bool empty () const noexcept { return size() == 0; }

    // Iterators

    iterator begin () noexcept { return iterator{leftmost_}; }
    const_iterator begin () const noexcept { return const_iterator{leftmost_}; }
    iterator end () noexcept { return iterator{std::addressof (end_node())}; }
    const_iterator end () const noexcept { return const_iterator{std::addressof (end_node())}; }

    reverse_iterator rbegin () noexcept { return reverse_iterator{end()}; }
    const_reverse_iterator rbegin () const noexcept { return const_reverse_iterator{end()}; }
    reverse_iterator rend () noexcept { return reverse_iterator{begin()}; }
    const_reverse_iterator rend () const noexcept { return const_reverse_iterator{begin()}; }

    const_iterator cbegin () const noexcept { return begin(); }
    const_iterator cend () const noexcept { return end(); }
    const_reverse_iterator crbegin () const noexcept { return rbegin(); }
    const_reverse_iterator crend () const noexcept { return rend(); }

    // Modifiers

    void swap (Splay_Tree &other) noexcept (std::is_nothrow_swappable_v<key_compare> &&
                                          std::is_nothrow_swappable_v<end_node_type>)
    {
        std::swap (root_, other.root_);
        std::swap (leftmost_, other.leftmost_);
        std::swap (comp_, other.comp_);
    }

    void clear ()
    {
        root_.clean_up ();

        leftmost_ = std::addressof (end_node());
        set_root (nullptr);

        size_ = 0;
    }

    std::pair<iterator, bool> insert (const key_type &key)
    {
        auto [node, parent, side] = find_v2 (get_root(), key);
    
        if (node == nullptr) // No node with such key in the tree
        {
            auto actual_parent = parent ? parent : std::addressof (end_node());
            auto new_node = insert_hint_unique (key, actual_parent, side);

            return std::pair{iterator{new_node}, true};
        }
        else
            return std::pair{iterator{node}, false};
    }

    template<std::input_iterator it>
    void insert (it first, it last)
    {
        for (; first != last; ++first)
            insert_unique (*first);
    }

    void insert (std::initializer_list<value_type> ilist)
    {
        for (auto &&key : ilist)
            insert_unique (key);
    }

    iterator erase (iterator pos)
    {
        auto node = const_cast<end_node_ptr>(pos.node_);
        ++pos;

        if (node == leftmost_)
            leftmost_ = pos.node_;

        erase (static_cast<node_ptr>(node));

        delete node;
        size_--;

        return pos;
    }

    size_type erase (const key_type &key)
    {
        auto it = find (key);
        if (it == end())
            return 0;
        else
        {
            erase (it);
            return 1;
        }
    }

    // Lookup

    const_iterator find (const key_type &key) const
    {
        auto node = find (get_root(), key);

        if (node)
        {
            splay (const_cast<node_ptr>(node));
            return const_iterator{node};
        }
        else
            return end();
    }

    iterator find (const key_type &key)
    {
        return static_cast<const Splay_Tree &>(*this).find (key);
    }

    const_iterator lower_bound (const key_type &key) const
    {
        auto node = lower_bound (get_root(), key);

        if (node)
        {
            splay (const_cast<node_ptr>(node));
            return const_iterator{node};
        }
        else
            return end();
    }

    iterator lower_bound (const key_type &key)
    {
        return static_cast<const Splay_Tree &>(*this).lower_bound (key);
    }

    const_iterator upper_bound (const key_type &key) const
    {
        auto node = upper_bound (get_root(), key);

        if (node)
        {
            splay (const_cast<node_ptr>(node));
            return const_iterator{node};
        }
        else
            return end();
    }

    iterator upper_bound (const key_type &key)
    {
        return static_cast<const Splay_Tree &>(*this).upper_bound (key);
    }

    bool contains (const key_type &key) const { return find (key) != end(); }

    #ifdef DEBUG
    
    // I see how this violates SRP but I don't know any better implementation
    void graphic_dump (std::ostream &os = std::cout) const
    {
        if (empty())
            return;

        detail::graphic_dump (os, static_cast<const_node_ptr>(leftmost_), std::addressof (end_node()));
    }

    #endif // DEBUG

private:

    end_node_type &end_node () { return root_.end_node_; }
    const end_node_type &end_node () const { return root_.end_node_; }

    void set_root (node_ptr new_root) noexcept { return end_node().set_left (new_root); }
    const_node_ptr get_root () const noexcept { return end_node().get_left(); }
    node_ptr get_root () noexcept { return end_node().get_left(); }

    const_node_ptr find (const_node_ptr node, const key_type &key) const
    {
        while (node)
        {
            if (comp_(key, node->key()))
                node = node->get_left();
            else if (comp_(node->key(), key))
                node = node->get_right();
            else
                return node;
        }

        return nullptr;
    }

    enum class Side { left, right };

    // No need for const overload as find_v2 is used only in insert
    std::tuple<node_ptr, end_node_ptr, Side> find_v2 (node_ptr node, const key_type &key)
    {
        // (parent == nullptr) ==> (key == get_root().key())
        // (node != nullptr) ==> (parent != nullptr)

        end_node_ptr parent = nullptr;
        Side side = Side::left; // root is a left child of end_node

        while (node)
        {
            auto is_less = comp_(key, node->key());
            if (!is_less && !comp_(node->key(), key))
                break;
            
            parent = node;
            if (is_less)
            {
                node = node->get_left();
                side = Side::left;
            }
            else
            {
                node = node->get_right();
                side = Side::right;
            }
        }

        return std::make_tuple (node, parent, side);
    }

    // Finds first element that is not less than key
    const_node_ptr lower_bound (const_node_ptr node, const key_type &key) const
    {    
        const_node_ptr result = nullptr;
        while (node)
        {
            if (!comp_(node->key(), key))
            {
                result = node;
                node = node->get_left();
            }
            else
                node = node->get_right();
        }

        return result;
    }

    // Finds first element that is greater than key
    const_node_ptr upper_bound (const_node_ptr node, const key_type &key) const
    {
        const_node_ptr result = nullptr;
        while (node)
        {
            if (comp_(key, node->key()))
            {
                result = node;
                node = node->get_left();
            }
            else
                node = node->get_right();
        }

        return result;
    }

    node_ptr insert_hint_unique (const key_type &key, end_node_ptr parent, Side side)
    {
        auto new_node = new node_type{key};
        new_node->set_parent (parent);

        if (side == Side::left)
            parent->set_left (new_node);
        else
            static_cast<node_ptr>(parent)->set_right (new_node);

        if (new_node == leftmost_->get_left())
            leftmost_ = new_node;

        splay (new_node);
        size_++;

        return new_node;
    }

    void insert_unique (const key_type &key)
    {
        auto [node, parent, side] = find_v2 (get_root(), key);
    
        if (node == nullptr)
        {
            auto actual_parent = parent ? parent : std::addressof (end_node());
            insert_hint_unique (key, actual_parent, side);
        }
    }

    void erase (node_ptr node)
    {
        assert (node);

        splay (node);

        if (node->get_left() == nullptr)
            transplant (node, node->get_right());
        else if (node->get_right() == nullptr)
            transplant (node, node->get_left());
        else
        {
            auto successor = detail::minimum (node->get_right());

            if (successor->parent_unsafe() != node)
            {
                transplant (successor, successor->get_right());
                successor->set_right (node->get_right());
                successor->get_right()->set_parent (successor);
            }

            transplant (node, successor);
            successor->set_left (node->get_left());
            successor->get_left()->set_parent (successor);
        }
    }

    void transplant (node_ptr u, node_ptr v)
    {
        assert (u);
        
        if (u == get_root ())
            set_root (v);
        else if (detail::is_left_child (u))
            u->get_parent()->set_left (v);
        else
            u->parent_unsafe()->set_right (v);

        if (v)
            v->set_parent (u->get_parent());
    }

    void splay (node_ptr node) const
    {
        assert (node);
        
        while (node != get_root())
        {
            auto parent = node->parent_unsafe();
            auto node_is_left_child = detail::is_left_child (node);

            if (parent == get_root())
            {
                if (node_is_left_child)
                    detail::right_rotate (parent);
                else
                    detail::left_rotate (parent);
            }
            else
            {
                auto grandparent = parent->parent_unsafe();
                auto parent_is_left_child = detail::is_left_child (parent);

                if (node_is_left_child && parent_is_left_child)
                {
                    detail::right_rotate (grandparent);
                    detail::right_rotate (parent);
                }
                else if (!node_is_left_child && !parent_is_left_child)
                {
                    detail::left_rotate (grandparent);
                    detail::left_rotate (parent);
                }
                else if (node_is_left_child && !parent_is_left_child)
                {
                    detail::right_rotate (parent);
                    detail::left_rotate (grandparent);
                }
                else
                {
                    detail::left_rotate (parent);
                    detail::right_rotate (grandparent);
                }
            }
        }

        assert (search_verifier());
    }

    bool search_verifier () const
    {
        return std::is_sorted (begin(), end(), comp_);
    }
};

template<typename Key_T, typename Compare>
bool operator== (const Splay_Tree<Key_T, Compare> &lhs, const Splay_Tree<Key_T, Compare> &rhs)
{
    return (lhs.size() == rhs.size()) &&
           (std::equal (lhs.begin(), lhs.end(), rhs.begin()));
}

template<typename Key_T, typename Compare>
auto operator<=> (const Splay_Tree<Key_T, Compare> &lhs, const Splay_Tree<Key_T, Compare> &rhs)
-> decltype (std::compare_three_way{}(*lhs.begin(), *rhs.begin()))
{
    return std::lexicographical_compare_three_way (lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace yLab

#endif // INCLUDE_SPLAY_TREE_HPP
