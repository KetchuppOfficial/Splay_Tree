#ifndef INCLUDE_TREES_SEARCH_TREE_HPP
#define INCLUDE_TREES_SEARCH_TREE_HPP

#include <utility>
#include <cassert>
#include <functional>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include <ostream>
#include <algorithm>
#include <compare>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "nodes/node_base.hpp"
#include "nodes/node_concepts.hpp"
#include "tree_iterator.hpp"

namespace yLab
{

template<typename Node_T, typename Compare = std::less<typename Node_T::key_type>>
requires std::derived_from<Node_T, Node_Base>
class Search_Tree
{
protected:

    using base_node_type = Node_Base;
    using base_node_ptr = base_node_type *;
    using const_base_node_ptr = const base_node_type *;
    using node_ptr = Node_T *;
    using const_node_ptr = const Node_T *;

public:

    using node_type = Node_T;
    using key_type = typename node_type::key_type;
    using key_compare = Compare;
    using value_type = key_type;
    using value_compare = Compare;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = tree_iterator<node_type>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;

    Search_Tree() : Search_Tree(key_compare()) {}

    explicit Search_Tree(const key_compare &comp) : comp_(comp) {}

    template<std::input_iterator It>
    Search_Tree(It first, It last, const key_compare &comp = key_compare()) : Search_Tree(comp)
    {
        insert(first, last);
    }

    Search_Tree(std::initializer_list<value_type> ilist, const key_compare &comp = key_compare())
               : Search_Tree(ilist.begin(), ilist.end(), comp) {}

    Search_Tree(const Search_Tree &rhs) : Search_Tree(rhs.begin(), rhs.end(), rhs.comp_) {}

    Search_Tree &operator=(const Search_Tree &rhs)
    {
        auto tmp_tree{rhs};
        swap(tmp_tree);

        return *this;
    }

    Search_Tree(Search_Tree &&rhs) noexcept (std::is_nothrow_move_constructible_v<Compare>)
        : size_{std::exchange(rhs.size_, 0)}, comp_(std::move(rhs.comp_))
    {
        if (rhs.get_root())
            take_ownership_of_tree_of(rhs);
    }

    Search_Tree &operator=(Search_Tree &&rhs) noexcept (noexcept(swap(rhs)))
    {
        swap(rhs);
        return *this;
    }

    virtual ~Search_Tree() { clean_up(); }

    // observers

    key_compare key_comp() const { return comp_; }
    value_compare value_comp() const { return key_comp(); }

    // capacity

    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size() == 0; }

    // iterators

    const_iterator begin() const noexcept { return const_iterator{get_leftmost()}; }
    const_iterator end() const noexcept { return const_iterator{&end_}; }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }

    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    // lookup

    const_iterator find(const key_type &key) const
    {
        auto node = find_impl(key);
        return const_iterator{node};
    }

    bool contains(const key_type &key) const { return find(key) != end(); }

    // Finds first element that is not less than key
    const_iterator lower_bound(const key_type &key) const
    {
        auto node = lower_bound_impl(key);
        return const_iterator{node};
    }

    // Finds first element that is greater than key
    const_iterator upper_bound(const key_type &key) const
    {
        auto node = upper_bound_impl(key);
        return const_iterator{node};
    }

    // Modifiers

    void swap(Search_Tree &rhs) noexcept (std::is_nothrow_swappable_v<key_compare>)
    {
        if (get_root())
        {
            if (rhs.get_root())
            {
                adjust_tree_to_end_node_of(rhs);
                rhs.adjust_tree_to_end_node_of(*this);
                std::swap(end_, rhs.end_);
            }
            else
                rhs.take_ownership_of_tree_of(*this);
        }
        else if (rhs.get_root())
            take_ownership_of_tree_of(rhs);

        std::swap(size_, rhs.size_);
        std::swap(comp_, rhs.comp_);
    }

    void clear() noexcept
    {
        clean_up();
        reset();
        size_ = 0;
    }

    std::pair<iterator, bool> insert(const key_type &key)
    {
        auto [node, parent] = find_with_parent(key);

        if (node == nullptr)
        {
            node = insert_impl(key, const_cast<base_node_ptr>(parent));

            if (size() == 0)
            {
                set_leftmost(const_cast<base_node_ptr>(node));
                set_rightmost(const_cast<base_node_ptr>(node));
            }
            else if (comp_(key, *begin()))
                set_leftmost(const_cast<base_node_ptr>(node));
            else if (comp_(static_cast<node_ptr>(get_rightmost())->get_key(), key))
                set_rightmost(const_cast<base_node_ptr>(node));

            size_++;

            return std::pair{iterator{node}, true};
        }
        else
            return std::pair{iterator{node}, false};
    }

    template<std::input_iterator It>
    void insert(It first, It last)
    {
        for (; first != last; ++first)
            insert(*first);
    }

    void insert(std::initializer_list<value_type> ilist) { insert(ilist.begin(), ilist.end()); }

    iterator erase(iterator pos)
    {
        base_node_ptr node = base_ptr(pos);
        auto res = std::next(pos);

        if (node == get_leftmost())
            set_leftmost(base_ptr(res));

        if (node == get_rightmost())
        {
            if (size() == 1)
                set_rightmost(&end_);
            else
                set_rightmost(base_ptr(std::prev(pos)));
        }

        erase_impl(node);
        size_--;

        delete node;

        return res;
    }

    size_type erase(const key_type &key)
    {
        if (auto it = find(key); it == end())
            return 0;
        else
        {
            erase(it);
            return 1;
        }
    }

    void graphic_dump(std::ostream &os) const
    {
        os << "digraph Tree\n"
              "{\n"
              "    rankdir = TB;\n"
              "    node [shape = record];\n\n";

        auto end_node = const_base_ptr(end());

        dot_dump(os, *end_node);
        dump_subtree(os, static_cast<const_node_ptr>(get_root()));

        os << '\n';

        for (auto node = const_base_ptr(begin()); node != end_node; node = node->successor())
            arrow_dump(os, node);

        if (!empty())
            fmt::print(os, "    node_{} -> node_{} [color = \"blue\"];\n",
                       fmt::ptr(end_node), fmt::ptr(end_node->get_left_unsafe()));

        os << '}' << std::endl;
    }

    bool subtree_sizes_verifier() const
    requires contains_subtree_size<node_type>
    {
        for (auto it = begin(), ite = end(); it != ite; ++it)
        {
            const_node_ptr node = const_ptr(it);

            auto expected_size = 1 + node_type::size(static_cast<const_node_ptr>(node->get_left()))
                                   + node_type::size(static_cast<const_node_ptr>(node->get_right()));

            if (expected_size != node_type::size(node))
                return false;
        }

        return true;
    }

protected:

    // access to underlying pointer of iterators

    static const_base_node_ptr const_base_ptr(iterator it) noexcept { return it.node_; }

    static base_node_ptr base_ptr(iterator it) noexcept
    {
        return const_cast<base_node_ptr>(const_base_ptr(it));
    }

    static const_node_ptr const_ptr(iterator it) noexcept
    {
        return static_cast<const_node_ptr>(const_base_ptr(it));
    }

    static node_ptr ptr(iterator it) noexcept { return const_cast<node_ptr>(const_ptr(it)); }

    // end-node routines

    base_node_ptr get_root() noexcept { return end_.get_left(); }
    const_base_node_ptr get_root() const noexcept { return end_.get_left(); }
    void set_root(base_node_ptr root) noexcept { end_.set_left(root); }

    base_node_ptr get_leftmost() noexcept { return end_.get_right(); }
    const_base_node_ptr get_leftmost() const noexcept { return end_.get_right(); }
    void set_leftmost(base_node_ptr leftmost) noexcept { end_.set_right(leftmost); }

    base_node_ptr get_rightmost() noexcept { return end_.get_parent(); }
    const_base_node_ptr get_rightmost() const noexcept { return end_.get_parent(); }
    void set_rightmost(base_node_ptr rightmost) noexcept { end_.set_parent(rightmost); }

    void clean_up() noexcept
    {
        for (base_node_ptr node = get_root(), save; node != nullptr; node = save)
        {
            if (base_node_ptr left = node->get_left())
            {
                save = left;
                node->set_left(save->get_right());
                save->set_right(node);
            }
            else
            {
                save = node->get_right();
                delete node;
            }
        }
    }

    void reset() noexcept
    {
        set_root(nullptr);
        set_leftmost(&end_);
        set_rightmost(&end_);
    }

    void take_ownership_of_tree_of(Search_Tree &rhs) noexcept
    {
        assert(get_root() == nullptr);

        rhs.adjust_tree_to_end_node_of(*this);

        set_root(rhs.get_root());
        set_leftmost(rhs.get_leftmost());
        set_rightmost(rhs.get_rightmost());
        rhs.reset();
    }

    void adjust_tree_to_end_node_of(Search_Tree &rhs) noexcept
    {
        assert(get_root());

        base_node_ptr right_end = &rhs.end_;
        get_root()->set_parent(right_end);
        get_leftmost()->set_left_thread(right_end);
        get_rightmost()->set_right_thread(right_end);
    }

    // implementation of operations on tree

    virtual const_base_node_ptr find_impl(const key_type &key) const
    {
        const_base_node_ptr node = get_root();

        while (node)
        {
            if (comp_(key, static_cast<const_node_ptr>(node)->get_key()))
                node = node->get_left();
            else if (comp_(static_cast<const_node_ptr>(node)->get_key(), key))
                node = node->get_right();
            else
                return node;
        }

        return &end_;
    }

    std::pair<const_base_node_ptr, const_base_node_ptr> find_with_parent(const key_type &key) const
    {
        const_base_node_ptr node = get_root();
        const_base_node_ptr parent = &end_;

        while (node)
        {
            if (comp_(key, static_cast<const_node_ptr>(node)->get_key()))
                parent = std::exchange(node, node->get_left());
            else if (comp_(static_cast<const_node_ptr>(node)->get_key(), key))
                parent = std::exchange(node, node->get_right());
            else
                break;
        }

        return std::pair{node, parent};
    }

    virtual const_base_node_ptr lower_bound_impl(const key_type &key) const
    {
        const_base_node_ptr node = get_root();
        const_base_node_ptr lower_bound = &end_;

        while (node)
        {
            if (!comp_(static_cast<const_node_ptr>(node)->get_key(), key))
                lower_bound = std::exchange(node, node->get_left());
            else
                node = node->get_right();
        }

        return lower_bound;
    }

    virtual const_base_node_ptr upper_bound_impl(const key_type &key) const
    {
        const_base_node_ptr node = get_root();
        const_base_node_ptr upper_bound = &end_;

        while (node)
        {
            if (comp_(key, static_cast<const_node_ptr>(node)->get_key()))
                upper_bound = std::exchange(node, node->get_left());
            else
                node = node->get_right();
        }

        return upper_bound;
    }

    virtual base_node_ptr insert_impl(const key_type &key, base_node_ptr parent)
    {
        assert(parent);
        assert(!parent->get_left() || !parent->get_right());

        base_node_ptr new_node = new node_type{key, nullptr, nullptr, parent};
        base_node_ptr end_node = &end_;

        if (parent == end_node)
        {
            new_node->set_left_thread(end_node);
            new_node->set_right_thread(end_node);

            parent->set_left(new_node);
        }
        else if (comp_(key, static_cast<node_ptr>(parent)->get_key()))
        {
            base_node_ptr predecessor = parent->get_left_unsafe();
            if (predecessor != end_node && predecessor->has_right_thread())
                predecessor->set_right_thread(new_node);

            new_node->set_left_thread(predecessor);
            new_node->set_right_thread(parent);

            parent->set_left(new_node);
        }
        else
        {
            base_node_ptr successor = parent->get_right_unsafe();
            if (successor != end_node && successor->has_left_thread())
                successor->set_left_thread(new_node);

            new_node->set_left_thread(parent);
            new_node->set_right_thread(successor);

            parent->set_right(new_node);
        }

        return new_node;
    }

    virtual void erase_impl(base_node_ptr node)
    {
        assert(node);

        if (node->has_left_thread())
        {
            if (node->has_right_thread())
            {
                if (node == get_root())
                    set_root(nullptr);
                else
                    bst_erase_no_children_case(node);
            }
            else
                bst_erase_only_right_child_case(node);
        }
        else
        {
            if (node->has_right_thread())
                bst_erase_only_left_child_case(node);
            else
                bst_erase_both_children_case(node);
        }
    }

    static void bst_erase_no_children_case(base_node_ptr node)
    {
        assert(node->has_left_thread());
        assert(node->has_right_thread());

        base_node_ptr successor = node->get_right_unsafe();
        base_node_ptr predecessor = node->get_left_unsafe();

        if (node == successor->get_left())
            successor->set_left_thread(predecessor);
        else
            predecessor->set_right_thread(successor);
    }

    static void bst_erase_only_left_child_case(base_node_ptr node)
    {
        assert(!node->has_left_thread());
        assert(node->has_right_thread());

        base_node_ptr child = node->get_left_unsafe();
        base_node_ptr successor = node->get_right_unsafe();
        base_node_ptr predecessor = child->maximum(); // O(child's subtree height)

        predecessor->set_right_thread(successor);

        base_node_ptr parent = node->get_parent();
        child->set_parent(parent);

        if (node->is_left_child())
            parent->set_left(child);
        else
            parent->set_right(child);
    }

    static void bst_erase_only_right_child_case(base_node_ptr node)
    {
        assert(node->has_left_thread());
        assert(!node->has_right_thread());

        base_node_ptr child = node->get_right_unsafe();
        base_node_ptr successor = child->minimum(); // O(child's subtree height)
        base_node_ptr predecessor = node->get_left_unsafe();

        successor->set_left_thread(predecessor);

        base_node_ptr parent = node->get_parent();
        child->set_parent(parent);

        if (node->is_left_child())
            parent->set_left(child);
        else
            parent->set_right(child);
    }

    static void bst_erase_both_children_case(base_node_ptr node)
    {
        assert(!node->has_left_thread());
        assert(!node->has_right_thread());

        base_node_ptr left = node->get_left_unsafe();
        base_node_ptr right = node->get_right_unsafe();
        base_node_ptr successor = right->minimum(); // O(log n)

        successor->set_left(left);
        left->set_parent(successor);

        if (left->has_right_thread())
            left->set_right_thread(successor);

        if (successor != right)
        {
            assert(successor->is_left_child());
            base_node_ptr s_parent = successor->get_parent();

            if (successor->has_right_thread())
            {
                assert(successor->get_right_unsafe() == successor->get_parent());
                s_parent->set_left_thread(successor);
            }
            else
            {
                base_node_ptr s_right = successor->get_right_unsafe();
                assert(s_right);
                s_parent->set_left(s_right);
                s_right->set_parent(s_parent);
            }

            successor->set_right(right);
            right->set_parent(successor);
        }

        transplant(node, successor);
    }

    // replaces the subtree rooted at node u with the subtree rooted at node v
    static void transplant(base_node_ptr u, base_node_ptr v)
    {
        assert(u);
        assert(v);

        base_node_ptr parent = u->get_parent();

        if (u->is_left_child())
            parent->set_left(v);
        else
            parent->set_right(v);

        v->set_parent(parent);
    }

    void dump_subtree(std::ostream &os, const_node_ptr node) const
    {
        dot_dump(os, *node);

        if (auto left = node->get_left())
            dump_subtree(os, static_cast<const_node_ptr>(left));

        if (auto right = node->get_right())
            dump_subtree(os, static_cast<const_node_ptr>(right));
    }

    static void arrow_dump(std::ostream &os, const_base_node_ptr node)
    {
        assert(node);

        auto self = fmt::ptr(node);

        if (node->has_left_thread())
            fmt::print(os, "    node_{}:w -> node_{} [style = dotted, color = \"blue\"];\n",
                       self, fmt::ptr(node->get_left_unsafe()));
        else
            fmt::print(os, "    node_{} -> node_{} [color = \"blue\"];\n",
                       self, fmt::ptr(node->get_left_unsafe()));

        if (node->has_right_thread())
            fmt::print(os, "    node_{}:e -> node_{} [style = dotted, color = \"red\"];\n",
                       self, fmt::ptr(node->get_right_unsafe()));
        else
            fmt::print(os, "    node_{} -> node_{} [color = \"red\"];\n",
                       self, fmt::ptr(node->get_right_unsafe()));
    }

    // left child of end_ is the root of the tree
    // right child of end_ is the leftmost element of the tree
    // parent of end_ is the rightmost element of the tree
    base_node_type end_{nullptr, &end_, &end_};
    size_type size_ = 0;
    [[no_unique_address]] key_compare comp_;
};

template<typename Node_T, typename Compare>
bool operator==(const Search_Tree<Node_T, Compare> &lhs, const Search_Tree<Node_T, Compare> &rhs)
{
    return (lhs.size() == rhs.size()) &&
           (std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template<typename Node_T, typename Compare>
auto operator<=>(const Search_Tree<Node_T, Compare> &lhs, const Search_Tree<Node_T, Compare> &rhs)
-> decltype(std::compare_three_way{}(*lhs.begin(), *rhs.begin()))
{
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace yLab

#endif // INCLUDE_TREES_SEARCH_TREE_HPP
