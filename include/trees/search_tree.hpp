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
#include <iostream>

#include "../tree_iterator.hpp"
#include "../nodes/node_concepts.hpp"

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

    class Control_Node final
    {
        // left child of head_ is the root of the tree
        // right child of head_ is the leftmost element of the tree
        base_node_type head_;

    public:

        Control_Node() { set_leftmost(get_end_node()); }

        Control_Node(const Control_Node &rhs) = delete;
        Control_Node &operator=(const Control_Node &rhs) = delete;

        Control_Node(Control_Node &&rhs) : head_{std::move(rhs.head_)}
        {
            reset_leftmost_or_parent_of_root();
            rhs.reset();
        }

        Control_Node &operator=(Control_Node &&rhs)
        {
            head_ = std::move(rhs.head_);

            reset_leftmost_or_parent_of_root();
            rhs.reset();

            return *this;
        }

        ~Control_Node() { clean_up(); }

        base_node_ptr get_end_node() noexcept { return std::addressof(head_); }
        const_base_node_ptr get_end_node() const noexcept { return std::addressof(head_); }

        node_ptr get_root() noexcept { return static_cast<node_ptr>(head_.get_left()); }
        const_node_ptr get_root() const noexcept
        {
            return static_cast<const_node_ptr>(head_.get_left());
        }
        void set_root(base_node_ptr root) noexcept { head_.set_left(root); }

        base_node_ptr get_leftmost() noexcept { return head_.get_right(); }
        const_base_node_ptr get_leftmost() const noexcept { return head_.get_right(); }
        void set_leftmost(base_node_ptr leftmost) noexcept { head_.set_right(leftmost); }

        // Use _unsafe getters only if you are sure that dynamic type of *head_.parent_ is Node_T.
        // Using _unsafe getters in other cases leads to UB
        node_ptr get_leftmost_unsafe() noexcept { return static_cast<node_ptr>(get_leftmost()); }
        const_node_ptr get_leftmost_unsafe() const noexcept
        {
            return static_cast<const_node_ptr>(get_leftmost());
        }

        void clean_up()
        {
            for (base_node_ptr node = get_root(), save; node != nullptr; node = save)
            {
                if (node->get_left() == nullptr)
                {
                    save = node->get_right();
                    delete node;
                }
                else
                {
                    save = node->get_left();
                    node->set_left(save->get_right());
                    save->set_right(node);
                }
            }
        }

        void reset_leftmost_or_parent_of_root()
        {
            if (get_root())
                get_root()->set_parent(get_end_node());
            else
                set_leftmost(get_end_node());
        }

        void reset()
        {
            set_root(nullptr);
            set_leftmost(get_end_node());
        }
    };

    Control_Node control_node_;
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
               : Search_Tree{ilist.begin(), ilist.end(), comp} {}

    Search_Tree(const Search_Tree &rhs) : Search_Tree{rhs.begin(), rhs.end(), rhs.comp_} {}

    Search_Tree &operator=(const Search_Tree &rhs)
    {
        auto tmp_tree{rhs};
        swap(tmp_tree);

        return *this;
    }

    Search_Tree(Search_Tree &&rhs) : control_node_{std::move(rhs.control_node_)},
                                     comp_{std::move(rhs.comp_)},
                                     size_{std::exchange(rhs.size_, 0)} {}

    Search_Tree &operator=(Search_Tree &&rhs) noexcept (std::is_nothrow_swappable_v<key_compare> &&
                                                        std::is_nothrow_swappable_v<Control_Node>)
    {
        swap(rhs);
        return *this;
    }

    virtual ~Search_Tree() = default;

    // Observers

    key_compare key_comp() const { return comp_; }
    value_compare value_comp() const { return key_comp(); }

    // Capacity

    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size() == 0; }

    // Iterators

    const_iterator begin() const noexcept { return const_iterator{control_node_.get_leftmost()}; }
    const_iterator end() const noexcept { return const_iterator{control_node_.get_end_node()}; }

    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }

    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

    // Lookup

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

    void swap(Search_Tree &other) noexcept (std::is_nothrow_swappable_v<key_compare> &&
                                            std::is_nothrow_swappable_v<Control_Node>)
    {
        std::swap(control_node_, other.control_node_);
        std::swap(comp_, other.comp_);
        std::swap(size_, other.size_);
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

            if (size_ == 0 || (size_ > 0 && comp_(key, *begin())))
                control_node_.set_leftmost(const_cast<node_ptr>(node));

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
            insert_unique(*first);
    }

    void insert(std::initializer_list<value_type> ilist) { insert(ilist.begin(), ilist.end()); }

    iterator erase(iterator pos)
    {
        auto node = base_ptr(pos);
        ++pos;

        erase_impl(static_cast<node_ptr>(node));

        if (node == control_node_.get_leftmost())
            control_node_.set_leftmost(base_ptr(pos));
        size_--;

        delete node;

        return pos;
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

    void graphic_dump(std::ostream &os = std::cout) const
    {
        if (empty())
            return;

        os << "digraph Tree\n"
              "{\n"
              "    rankdir = TB;\n"
              "    node [shape = record];\n\n";

        auto begin_node = const_base_ptr(begin());
        auto end_node = const_base_ptr(end());

        dot_dump(os, end_node);

        for (auto node = begin_node; node != end_node; node = node->successor())
            dot_dump(os, static_cast<const_node_ptr>(node));

        os << std::endl;

        for (auto node = begin_node; node != end_node; node = node->successor())
            Search_Tree::arrow_dump(os, node);

        os << "    node_" << end_node << " -> node_"
           << end_node->get_left() << " [color = \"blue\"];\n}\n";
    }

protected:

    static const_base_node_ptr const_base_ptr(iterator it) { return it.node_; }

    static base_node_ptr base_ptr(iterator it)
    {
        return const_cast<base_node_ptr>(const_base_ptr(it));
    }

    static const_node_ptr const_ptr(iterator it)
    {
        return static_cast<const_node_ptr>(const_base_ptr(it));
    }

    static node_ptr ptr(iterator it) { return const_cast<node_ptr>(const_ptr(it)); }

    virtual const_base_node_ptr find_impl(const key_type &key) const
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

    std::pair<const_node_ptr, const_base_node_ptr> find_with_parent(const key_type &key) const
    {
        auto node = control_node_.get_root();
        auto parent = control_node_.get_end_node();

        while (node)
        {
            if (comp_(key, node->get_key())) // key < node->get_key()
                parent = std::exchange(node, node->get_left());
            else if (comp_(node->get_key(), key)) // key > node->get_key()
                parent = std::exchange(node, node->get_right());
            else
                break;
        }

        return std::pair{node, parent};
    }

    virtual const_base_node_ptr lower_bound_impl(const key_type &key) const
    {
        auto node = control_node_.get_root();
        auto lower_bound = control_node_.get_end_node();

        while (node)
        {
            if (!comp_(node->get_key(), key)) // key <= node->get_key()
                lower_bound = std::exchange(node, node->get_left());
            else
                node = node->get_right();
        }

        return lower_bound;
    }

    virtual const_base_node_ptr upper_bound_impl(const key_type &key) const
    {
        auto node = control_node_.get_root();
        auto upper_bound = control_node_.get_end_node();

        while (node)
        {
            if (comp_(key, node->get_key())) // key < node->get_key()
                upper_bound = std::exchange(node, node->get_left());
            else
                node = node->get_right();
        }

        return upper_bound;
    }

    virtual node_ptr insert_impl(const key_type &key, base_node_ptr parent)
    {
        return bst_insert(key, parent);
    }

    node_ptr bst_insert(const key_type &key, base_node_ptr parent)
    requires(has_parent<node_type>)
    {
        node_ptr new_node = new node_type{key, nullptr, nullptr, parent};

        if (parent == control_node_.get_end_node() ||
            comp_(key, static_cast<node_ptr>(parent)->get_key()))
        {
            parent->set_left(new_node);
        }
        else
            parent->set_right(new_node);

        return new_node;
    }

    node_ptr bst_insert(const key_type &key, base_node_ptr parent)
    requires(!has_parent<node_type>)
    {
        node_ptr new_node = new node_type{key};

        if (auto end_node = control_node_.get_end_node(); parent == end_node)
        {
            new_node->set_left_thread(end_node);
            new_node->set_right_thread(end_node);

            parent->set_left(new_node);
        }
        else if (comp_(key, static_cast<node_ptr>(parent)->get_key()))
        {
            // O(1) because parent has no left child
            base_node_ptr predecessor = parent->predecessor();
            if (predecessor != end_node)
                predecessor->set_right_thread(new_node);

            new_node->set_left_thread(predecessor);
            new_node->set_right_thread(parent);

            parent->set_left(new_node);
        }
        else
        {
            // O(1) because parent has no right child
            base_node_ptr successor = parent->successor();
            if (successor != end_node)
                successor->set_left_thread(new_node);

            new_node->set_right_thread(successor);
            new_node->set_left_thread(parent);

            parent->set_right(new_node);
        }

        return new_node;
    }

    void insert_unique(const key_type &key)
    {
        auto [node, parent] = find_with_parent(key);

        if (node == nullptr)
        {
            node = insert_impl(key, const_cast<base_node_ptr>(parent));

            if (size_ == 0 || (size_ > 0 && comp_(key, *begin())))
                control_node_.set_leftmost(const_cast<node_ptr>(node));

            size_++;
        }
    }

    virtual void erase_impl(node_ptr node) { bst_erase(node); }

    void bst_erase(node_ptr node)
    requires(has_parent<node_type>)
    {
        assert(node);

        if (node->get_left() == nullptr)
            transplant(node, node->get_right());
        else if (node->get_right() == nullptr)
            transplant(node, node->get_left());
        else
        {
            auto successor = node->get_right()->minimum();
            assert(successor->get_left() == nullptr);

            if (successor != node->get_right())
            {
                transplant(successor, successor->get_right());
                successor->set_right(node->get_right());
                successor->get_right()->set_parent(successor);
            }

            transplant(node, successor);
            successor->set_left(node->get_left());
            successor->get_left()->set_parent(successor);
        }
    }

    // replaces the subtree rooted at node u with the subtree rooted at node v
    void transplant(base_node_ptr u, base_node_ptr v)
    requires(has_parent<node_type>)
    {
        assert(u);

        if (u == control_node_.get_root())
            control_node_.set_root(v);
        else if (u->is_left_child())
            u->get_parent()->set_left(v);
        else
            u->get_parent()->set_right(v);

        if (v)
            v->set_parent(u->get_parent());
    }

    void bst_erase(node_ptr node)
    requires(!has_parent<node_type>)
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

    static void bst_erase_no_children_case(node_ptr node)
    requires(!has_parent<node_type>)
    {
        // O(1) because node has both threads
        auto successor = node->successor();
        auto predecessor = node->predecessor();

        if (node == successor->get_left()) // node is a left child of successor
            successor->set_left_thread(predecessor);
        else
            predecessor->set_right_thread(successor);
    }

    static void bst_erase_only_left_child_case(node_ptr node)
    requires(!has_parent<node_type>)
    {
        auto child = node->get_left();
        auto successor = node->successor(); // O(1) because node has right thread
        auto predecessor = child->maximum(); // O(log n)

        predecessor->set_right_thread(successor);

        if (node == successor->get_left())
            successor->set_left(child);
        else
        {
            auto min = child->minimum(); // O(log n)
            auto parent = min->predecessor(); // O(1) because min has at least left thread
            parent->set_right(child);
        }
    }

    static void bst_erase_only_right_child_case(node_ptr node)
    requires(!has_parent<node_type>)
    {
        auto child = node->get_right();
        auto successor = child->minimum(); // O(log n)
        auto predecessor = node->predecessor(); // O(1) because node has left thread

        successor->set_left_thread(predecessor);

        if (node == predecessor->get_right())
            predecessor->set_right(child);
        else
        {
            auto max = child->maximum(); // O(log n)
            auto parent = max->successor(); // O(1) because max has at least right thread
            parent->set_left(child);
        }
    }

    // TODO: maybe this method has to be static
    void bst_erase_both_children_case(node_ptr node)
    requires(!has_parent<node_type>)
    {
        // TBD
    }

    static void arrow_dump(std::ostream &os, const_base_node_ptr node)
    requires(has_parent<node_type>)
    {
        assert(node);

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

    static void arrow_dump(std::ostream &os, const_base_node_ptr node)
    requires(!has_parent<node_type>)
    {
        assert(node);

        os << "    node_" << node;
        if (node->has_left_thread())
            os << ":w -> node_" << node->predecessor() << " [style=dotted, ";
        else
            os << " -> node_" << node->get_left() << " [";
        os << "color = \"blue\"];\n";

        os << "    node_" << node;
        if (node->has_right_thread())
            os << ":e -> node_" << node->successor() << " [style=dotted, ";
        else
            os << " -> node_" << node->get_right() << " [";
        os << "color = \"red\"];\n";
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
