#ifndef INCLUDE_SEARCH_TREE_HPP
#define INCLUDE_SEARCH_TREE_HPP

#include <functional>
#include <concepts>
#include <iterator>
#include <initializer_list>
#include <memory>
#include <utility>
#include <cassert>
#include <type_traits>
#include <algorithm>
#include <compare>
#include <ostream>
#include <cstddef>

#include "tree_iterator.hpp"
#include "node_concepts.hpp"

namespace yLab::detail
{

template<typename T>
class Control_Node final
{
    using base_node_type = T;
    using base_node_ptr = T *;
    using const_base_node_ptr = const T *;

public:

    Control_Node() noexcept (std::is_nothrow_constructible_v<base_node_type>)
        : head_{nullptr, &head_, &head_} {}

    Control_Node(const Control_Node &rhs) = delete;
    Control_Node &operator=(const Control_Node &rhs) = delete;

    Control_Node(Control_Node &&rhs) noexcept
    {
        if (rhs.get_root())
            take_ownership_of_tree_of(rhs);
    }

    Control_Node &operator=(Control_Node &&rhs)
        noexcept (std::is_nothrow_swappable_v<base_node_type>)
    {
        swap(rhs);
        return *this;
    }

    ~Control_Node() { clean_up(); }

    base_node_ptr get_end_node() noexcept { return std::addressof(head_); }
    const_base_node_ptr get_end_node() const noexcept { return std::addressof(head_); }

    base_node_ptr get_root() noexcept { return head_.get_left(); }
    const_base_node_ptr get_root() const noexcept { return head_.get_left(); }
    void set_root(base_node_ptr root) noexcept { head_.set_left(root); }

    base_node_ptr get_leftmost() noexcept { return head_.get_right(); }
    const_base_node_ptr get_leftmost() const noexcept { return head_.get_right(); }
    void set_leftmost(base_node_ptr leftmost) noexcept { head_.set_right(leftmost); }

    base_node_ptr get_rightmost() noexcept { return head_.get_parent(); }
    const_base_node_ptr get_rightmost() const noexcept { return head_.get_parent(); }
    void set_rightmost(base_node_ptr rightmost) noexcept { head_.set_parent(rightmost); }

    void clean_up()
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

    void swap(Control_Node &rhs) noexcept (std::is_nothrow_swappable_v<base_node_type>)
    {
        if (get_root())
        {
            if (rhs.get_root())
            {
                adjust_tree_to_end_node_of(rhs);
                rhs.adjust_tree_to_end_node_of(*this);
                std::swap(head_, rhs.head_);
            }
            else
                rhs.take_ownership_of_tree_of(*this);
        }
        else if (rhs.get_root())
            take_ownership_of_tree_of(rhs);
    }

    void reset() noexcept
    {
        set_root(nullptr);

        base_node_ptr end = get_end_node();
        set_leftmost(end);
        set_rightmost(end);
    }

private:

    void take_ownership_of_tree_of(Control_Node &rhs) noexcept
    {
        assert(get_root() == nullptr);

        rhs.adjust_tree_to_end_node_of(*this);

        set_root(rhs.get_root());
        set_leftmost(rhs.get_leftmost());
        set_rightmost(rhs.get_rightmost());
        rhs.reset();
    }

    void adjust_tree_to_end_node_of(Control_Node &rhs) noexcept
    {
        assert(get_root());

        base_node_ptr end = rhs.get_end_node();
        get_root()->set_parent(end);
        get_leftmost()->set_left_thread(end);
        get_rightmost()->set_right_thread(end);
    }

    // left child of head_ is the root of the tree
    // right child of head_ is the leftmost element of the tree
    // parent of head_ is the rightmost element of the tree
    base_node_type head_;
};

} // namespace yLab::detail

namespace std
{

template<typename T>
void swap(yLab::detail::Control_Node<T> &lhs, yLab::detail::Control_Node<T> &rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

} // namespace std

namespace yLab
{

template<typename Node_T, typename Base_Node_T,
         typename Compare = std::less<typename Node_T::key_type>>
requires std::derived_from<Node_T, Base_Node_T>
class Search_Tree
{
protected:

    using base_node_type = Base_Node_T;
    using base_node_ptr = base_node_type *;
    using const_base_node_ptr = const base_node_type *;
    using node_ptr = Node_T *;
    using const_node_ptr = const Node_T *;

    detail::Control_Node<base_node_type> control_node_;
    Compare comp_;
    std::size_t size_ = 0;

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
    using iterator = tree_iterator<node_type, base_node_type>;
    using const_iterator = iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = reverse_iterator;

    Search_Tree() : Search_Tree{key_compare{}} {}

    explicit Search_Tree(const key_compare &comp) : comp_{comp} {}

    template<std::input_iterator It>
    Search_Tree(It first, It last, const key_compare &comp = key_compare{}) : comp_{comp}
    {
        insert(first, last);
    }

    Search_Tree(std::initializer_list<value_type> ilist, const key_compare &comp = key_compare{})
               : Search_Tree(ilist.begin(), ilist.end(), comp) {}

    Search_Tree(const Search_Tree &rhs) : Search_Tree(rhs.begin(), rhs.end(), rhs.comp_) {}

    Search_Tree &operator=(const Search_Tree &rhs)
    {
        auto tmp_tree{rhs};
        swap(tmp_tree);

        return *this;
    }

    Search_Tree(Search_Tree &&rhs) : control_node_{std::move(rhs.control_node_)},
                                     comp_{std::move(rhs.comp_)},
                                     size_{std::exchange(rhs.size_, 0)} {}

    Search_Tree &operator=(Search_Tree &&rhs) noexcept(noexcept(swap(rhs)))
    {
        swap(rhs);
        return *this;
    }

    virtual ~Search_Tree() = default;

    // observers

    key_compare key_comp() const { return comp_; }
    value_compare value_comp() const { return key_comp(); }

    // capacity

    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size() == 0; }

    // iterators

    const_iterator begin() const noexcept { return const_iterator{control_node_.get_leftmost()}; }
    const_iterator end() const noexcept { return const_iterator{control_node_.get_end_node()}; }

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

    void swap(Search_Tree &rhs) noexcept (std::is_nothrow_swappable_v<decltype(control_node_)> &&
                                          std::is_nothrow_swappable_v<key_compare>)
    {
        std::swap(control_node_, rhs.control_node_);
        std::swap(comp_, rhs.comp_);
        std::swap(size_, rhs.size_);
    }

    void clear()
    {
        control_node_.clean_up();
        control_node_.reset();

        size_ = 0;
    }

    std::pair<iterator, bool> insert(const key_type &key)
    {
        auto [node, parent] = find_with_parent(key);

        if (node == nullptr)
        {
            node = insert_impl(key, const_cast<base_node_ptr>(parent));

            if (size_ == 0)
            {
                control_node_.set_leftmost(const_cast<base_node_ptr>(node));
                control_node_.set_rightmost(const_cast<base_node_ptr>(node));
            }
            else if (comp_(key, *begin()))
                control_node_.set_leftmost(const_cast<base_node_ptr>(node));
            else if (comp_(static_cast<node_ptr>(control_node_.get_rightmost())->get_key(), key))
                control_node_.set_rightmost(const_cast<base_node_ptr>(node));

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

        if (node == control_node_.get_leftmost())
            control_node_.set_leftmost(base_ptr(res));

        if (node == control_node_.get_rightmost())
        {
            if (size_ == 1)
                control_node_.set_rightmost(control_node_.get_end_node());
            else
                control_node_.set_rightmost(base_ptr(std::prev(pos)));
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

        dot_dump(os, end_node);
        dump_subtree(os, static_cast<const_node_ptr>(control_node_.get_root()));

        os << '\n';

        for (auto node = const_base_ptr(begin()); node != end_node; node = node->successor())
            arrow_dump(os, node);

        if (!empty())
            std::println(os, "    node_{} -> node_{} [color = \"blue\"];",
                         reinterpret_cast<const void *>(end_node),
                         reinterpret_cast<const void *>(end_node->get_left_unsafe()));

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

    // implementation of operations on tree

    virtual const_base_node_ptr find_impl(const key_type &key) const
    {
        const_base_node_ptr node = control_node_.get_root();

        while (node)
        {
            if (comp_(key, static_cast<const_node_ptr>(node)->get_key()))
                node = node->get_left();
            else if (comp_(static_cast<const_node_ptr>(node)->get_key(), key))
                node = node->get_right();
            else
                return node;
        }

        return control_node_.get_end_node();
    }

    std::pair<const_base_node_ptr, const_base_node_ptr> find_with_parent(const key_type &key) const
    {
        const_base_node_ptr node = control_node_.get_root();
        const_base_node_ptr parent = control_node_.get_end_node();

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
        const_base_node_ptr node = control_node_.get_root();
        const_base_node_ptr lower_bound = control_node_.get_end_node();

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
        const_base_node_ptr node = control_node_.get_root();
        const_base_node_ptr upper_bound = control_node_.get_end_node();

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
        base_node_ptr end_node = control_node_.get_end_node();

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
                if (node == control_node_.get_root())
                    control_node_.set_root(nullptr);
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

        // predecessor cannot be end-node because node has the left child
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

        // successor cannot be end-node because node has the right child
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
        dot_dump(os, node);

        if (auto left = node->get_left(); left)
            dump_subtree(os, static_cast<const_node_ptr>(left));

        if (auto right = node->get_right(); right)
            dump_subtree(os, static_cast<const_node_ptr>(right));
    }

    static void arrow_dump(std::ostream &os, const_base_node_ptr node)
    {
        assert(node);

        auto self = reinterpret_cast<const void *>(node);

        if (node->has_left_thread())
            std::println(os, "    node_{}:w -> node_{} [style = dotted, color = \"blue\"];",
                         self, reinterpret_cast<const void *>(node->get_left_unsafe()));
        else
            std::println(os, "    node_{} -> node_{} [color = \"blue\"];",
                         self, reinterpret_cast<const void *>(node->get_left_unsafe()));

        if (node->has_right_thread())
            std::println(os, "    node_{}:e -> node_{} [style = dotted, color = \"red\"];",
                         self, reinterpret_cast<const void *>(node->get_right_unsafe()));
        else
            std::println(os, "    node_{} -> node_{} [color = \"red\"];",
                         self, reinterpret_cast<const void *>(node->get_right_unsafe()));
    }
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

#endif // INCLUDE_SEARCH_TREE_HPP
