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

#ifdef DEBUG
#include <iostream>
#endif // DEBUG

#include "tree_iterator.hpp"

namespace yLab
{

template<typename T>
concept BT_Node = requires (const T &cnode, T &node)
{
    { node->get_left() } -> std::same_as<T *>;
    { node->get_right() } -> std::same_as<T *>;
    { node->get_parent() } -> std::same_as<T *>;

    { cnode->get_left() } -> std::same_as<const T *>;
    { cnode->get_right() } -> std::same_as<const T *>;
    { cnode->get_parent() } -> std::same_as<const T *>;
};

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

    class Control_Node final
    {
        // left child of head_ is the root of the tree
        // parent of head_ is the leftmost element of the tree
        base_node_type head_{};

    public:

        Control_Node () { set_leftmost (get_end_node()); }

        Control_Node (const Control_Node &rhs) = delete;
        Control_Node &operator= (const Control_Node &rhs) = delete;

        Control_Node (Control_Node &&rhs) : head_{std::move (rhs.head_)}
        {
            reset_leftmost_or_parent_of_root();
            rhs.reset();
        }

        Control_Node &operator= (Control_Node &&rhs)
        {
            head_ = std::move (rhs.head_);

            reset_leftmost_or_parent_of_root();
            rhs.reset();

            return *this;
        }

        ~Control_Node () { clean_up(); }

        base_node_ptr get_end_node () noexcept { return std::addressof (head_); }
        const_base_node_ptr get_end_node () const noexcept { return std::addressof (head_); }

        node_ptr get_root () noexcept { return static_cast<node_ptr>(head_.get_left()); }
        const_node_ptr get_root () const noexcept
        {
            return static_cast<const_node_ptr>(head_.get_left());
        }
        void set_root (base_node_ptr root) noexcept { head_.set_left (root); }

        base_node_ptr get_leftmost () noexcept { return head_.get_parent(); }
        const_base_node_ptr get_leftmost () const noexcept { return head_.get_parent(); }
        void set_leftmost (base_node_ptr leftmost) noexcept { head_.set_parent (leftmost); }

        // Use _unsafe getters only if you are sure that dynamic type of *head_.parent_ is Node_T.
        // Using _unsafe getters in other cases leads to UB
        node_ptr get_leftmost_unsafe () noexcept { return static_cast<node_ptr>(get_leftmost()); }
        const_node_ptr get_leftmost_unsafe () const noexcept
        {
            return static_cast<const_node_ptr>(get_leftmost());
        }

        void clean_up ()
        {
            for (base_node_ptr node = get_root(), save{}; node != nullptr; node = save)
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

        void reset_leftmost_or_parent_of_root ()
        {
            if (get_root())
                get_root()->set_parent (get_end_node());
            else
                set_leftmost (get_end_node());
        }

        void reset ()
        {
            set_root (nullptr);
            set_leftmost (get_end_node());
        }
    };

    Control_Node control_node_{};
    Compare comp_;
    std::size_t size_{};

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

    Search_Tree () : Search_Tree{key_compare{}} {}

    explicit Search_Tree (const key_compare &comp) : comp_{comp} {}

    template<std::input_iterator it>
    Search_Tree (it first, it last, const key_compare &comp = key_compare{}) : comp_{comp}
    {
        insert (first, last);
    }

    Search_Tree (std::initializer_list<value_type> ilist, const key_compare &comp = key_compare{})
                : comp_{comp}
    {
        insert (ilist);
    }

    Search_Tree (const Search_Tree &rhs) : comp_{rhs.comp_}
    {
        for (auto &&key : rhs)
            insert_unique (key);
    }

    Search_Tree &operator= (const Search_Tree &rhs)
    {
        auto tmp_tree{rhs};
        std::swap (*this, tmp_tree);

        return *this;
    }

    Search_Tree (Search_Tree &&rhs) : control_node_{std::move (rhs.control_node_)},
                                      comp_{std::move (rhs.comp_)},
                                      size_{std::exchange (rhs.size_, 0)} {}

    Search_Tree &operator= (Search_Tree &&rhs) noexcept (std::is_nothrow_swappable_v<key_compare> &&
                                                         std::is_nothrow_swappable_v<Control_Node>)
    {
        swap (rhs);

        return *this;
    }

    virtual ~Search_Tree () = default;

    // Observers

    const key_compare &key_comp () const { return comp_; }
    const value_compare &value_comp () const { return key_comp(); }

    // Capacity

    size_type size () const noexcept { return size_; }
    bool empty () const noexcept { return size() == 0; }

    // Iterators

    const_iterator begin () const noexcept { return const_iterator{control_node_.get_leftmost()}; }
    const_iterator end () const noexcept { return const_iterator{control_node_.get_end_node()}; }

    const_reverse_iterator rbegin () const noexcept { return const_reverse_iterator{end()}; }
    const_reverse_iterator rend () const noexcept { return const_reverse_iterator{begin()}; }

    const_iterator cbegin () const noexcept { return begin(); }
    const_iterator cend () const noexcept { return end(); }

    const_reverse_iterator crbegin () const noexcept { return rbegin(); }
    const_reverse_iterator crend () const noexcept { return rend(); }

    // Lookup

    const_iterator find (const key_type &key) const
    {
        auto node = find_impl (key);
        return const_iterator{node};
    }

    bool contains (const key_type &key) const { return find (key) != end(); }

    // Finds first element that is not less than key
    const_iterator lower_bound (const key_type &key) const
    {
        auto node = lower_bound_impl (key);
        return const_iterator{node};
    }

    // Finds first element that is greater than key
    const_iterator upper_bound (const key_type &key) const
    {
        auto node = upper_bound_impl (key);
        return const_iterator{node};
    }

    // Modifiers

    void swap (Search_Tree &other) noexcept (std::is_nothrow_swappable_v<key_compare> &&
                                             std::is_nothrow_swappable_v<base_node_type>)
    {
        std::swap (control_node_, other.control_node_);
        std::swap (comp_, other.comp_);
        std::swap (size_, other.size_);
    }

    void clear ()
    {
        control_node_.clean_up ();
        control_node_.set_leftmost (control_node_.get_end_node());
        control_node_.set_root (nullptr);

        size_ = 0;
    }

    std::pair<iterator, bool> insert (const key_type &key)
    {
        auto [node, parent] = find_with_parent (key);

        if (node == nullptr)
        {
            auto new_node = insert_impl (key, const_cast<base_node_ptr>(parent));

            if (size_ == 0 ||
                (size_ > 0 && comp_(key, control_node_.get_leftmost_unsafe()->get_key())))
            {
                control_node_.set_leftmost (new_node);
            }
            size_++;

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
        auto node = const_cast<base_node_ptr>(pos.base());
        ++pos;

        erase_impl (static_cast<node_ptr>(node));

        if (node == control_node_.get_leftmost())
            control_node_.set_leftmost (const_cast<base_node_ptr>(pos.base()));
        size_--;

        delete node;

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

    #ifdef DEBUG

    void graphic_dump (std::ostream &os = std::cout) const
    {
        if (empty())
            return;

        os << "digraph Tree\n"
              "{\n"
              "    rankdir = TB;\n"
              "    node [shape = record];\n\n";

        auto begin_node = begin().base();
        auto end_node = end().base();

        os << "    node_" << end_node
           << " [color = black, style = filled, fillcolor = yellow, label = \"end node\"];\n";

        for (auto node = begin_node; node != end_node; node = node->successor())
            Search_Tree::node_dump (os, node);

        os << std::endl;

        for (auto node = begin_node; node != end_node; node = node->successor())
            Search_Tree::arrow_dump (os, node);

        os << "    node_" << end_node << " -> node_"
           << end_node->get_left() << " [color = \"blue\"];\n}\n";
    }

    #endif // DEBUG

protected:

    virtual const_base_node_ptr find_impl (const key_type &key) const
    {
        auto node = control_node_.get_root();

        while (node)
        {
            if (comp_(key, node->get_key())) // key < node->get_key()
                node = node->get_left();
            else if (comp_(node->get_key(), key)) // key > node->get_key()
                node = node->get_right();
            else
                return node;
        }

        return control_node_.get_end_node();
    }

    std::pair<const_node_ptr, const_base_node_ptr> find_with_parent (const key_type &key) const
    {
        auto node = control_node_.get_root();
        auto parent = control_node_.get_end_node();

        while (node)
        {
            if (comp_(key, node->get_key())) // key < node->get_key()
                parent = std::exchange (node, node->get_left());
            else if (comp_(node->get_key(), key)) // key > node->get_key()
                parent = std::exchange (node, node->get_right());
            else
                break;
        }

        return std::pair{node, parent};
    }

    virtual const_base_node_ptr lower_bound_impl (const key_type &key) const
    {
        auto node = control_node_.get_root();
        const_base_node_ptr lower_bound = control_node_.get_end_node();

        while (node)
        {
            if (!comp_(node->get_key(), key)) // key <= node->get_key()
                lower_bound = std::exchange (node, node->get_left());
            else
                node = node->get_right();
        }

        return lower_bound;
    }

    virtual const_base_node_ptr upper_bound_impl (const key_type &key) const
    {
        auto node = control_node_.get_root();
        const_base_node_ptr upper_bound = control_node_.get_end_node();

        while (node)
        {
            if (comp_(key, node->get_key())) // key < node->get_key()
                upper_bound = std::exchange (node, node->get_left());
            else
                node = node->get_right();
        }

        return upper_bound;
    }

    virtual node_ptr insert_impl (const key_type &key, base_node_ptr parent)
    {
        return bst_insert (key, parent);
    }

    node_ptr bst_insert (const key_type &key, base_node_ptr parent)
    {
        auto new_node = new node_type{key};
        new_node->set_parent (parent);

        if (parent == control_node_.get_end_node() ||
            comp_(key, static_cast<node_ptr>(parent)->get_key()))
        {
            parent->set_left (new_node);
        }
        else
            parent->set_right (new_node);

        return new_node;
    }

    void insert_unique (const key_type &key)
    {
        auto [node, parent] = find_with_parent (key);

        if (node == nullptr)
        {
            auto new_node = insert_impl (key, const_cast<base_node_ptr>(parent));

            if (size_ == 0 ||
                (size_ > 0 && comp_(key, control_node_.get_leftmost_unsafe()->get_key())))
            {
                control_node_.set_leftmost (new_node);
            }
            size_++;
        }
    }

    virtual void erase_impl (node_ptr node) { bst_erase (node); }

    void bst_erase (node_ptr node)
    {
        assert (node);

        if (node->get_left() == nullptr)
            transplant (node, node->get_right());
        else if (node->get_right() == nullptr)
            transplant (node, node->get_left());
        else
        {
            auto successor = node->get_right()->minimum();
            // successor->left_ == nullptr because successor follows node

            if (successor->get_parent() != node)
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

    // replaces the subtree rooted at node u with the subtree rooted at node v
    void transplant (base_node_ptr u, base_node_ptr v)
    {
        assert (u);

        if (u == control_node_.get_root())
            control_node_.set_root (v);
        else if (u->is_left_child())
            u->get_parent()->set_left (v);
        else
            u->get_parent()->set_right (v);

        if (v)
            v->set_parent (u->get_parent());
    }

    #ifdef DEBUG

    static void node_dump (std::ostream &os, const_base_node_ptr node)
    {
        assert (node);

        os << "    node_" << node
           << " [shape = record, color = black, style = filled, fillcolor = red, fontcolor = black,"
              " label = \"" << static_cast<const_node_ptr>(node)->get_key() << "\"];\n";

        if (node->get_left() == nullptr)
            os << "    left_nil_node_" << node << " [shape = record, color = red, "
                  "style = filled, fillcolor = black, fontcolor = white, label = \"nil\"];\n";

        if (node->get_right() == nullptr)
            os << "    right_nil_node_" << node << " [shape = record, color = red, "
                  "style = filled, fillcolor = black, fontcolor = white, label = \"nil\"];\n";
    }

    static void arrow_dump (std::ostream &os, const_base_node_ptr node)
    {
        assert (node);

        os << "    node_" << node << " -> ";
        if (node->get_left())
            os << "node_" << node->get_left();
        else
            os << "left_nil_node_" << node;
        os << " [color = \"blue\"];\n";

        os << "    node_" << node << " -> ";
        if (node->get_right())
            os << "node_" << node->get_right();
        else
            os << "right_nil_node_" << node;
        os << " [color = \"gold\"];\n";

        os << "    node_" << node << " -> "
            << "node_" << node->get_parent() << " [color = \"dimgray\"];\n";
    }

    #endif // DEBUG
};

template<typename Node_T, typename Compare>
bool operator== (const Search_Tree<Node_T, Compare> &lhs, const Search_Tree<Node_T, Compare> &rhs)
{
    return (lhs.size() == rhs.size()) &&
           (std::equal (lhs.begin(), lhs.end(), rhs.begin()));
}

template<typename Node_T, typename Compare>
auto operator<=> (const Search_Tree<Node_T, Compare> &lhs, const Search_Tree<Node_T, Compare> &rhs)
-> decltype (std::compare_three_way{}(*lhs.begin(), *rhs.begin()))
{
    return std::lexicographical_compare_three_way (lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

} // namespace yLab

#endif // INCLUDE_SEARCH_TREE_HPP
