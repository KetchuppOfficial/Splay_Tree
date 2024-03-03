#include <iostream>
#include <stdexcept>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iterator>

#if defined(SPLAY_TREE) || defined(AUGMENTED_SPLAY_TREE)
#include "trees.hpp"
#else
#include <set>
#endif

#include "node_concepts.hpp"

namespace
{

template<typename Key_T>
std::vector<Key_T> get_keys()
{
    std::size_t n_keys;
    std::cin >> n_keys;
    if (!std::cin.good())
        throw std::runtime_error{"Error while reading the number of keys"};

    std::vector<Key_T> keys;
    keys.reserve(n_keys);

    for (auto key_i = 0; key_i != n_keys; ++key_i)
    {
        Key_T key;
        std::cin >> key;
        if (!std::cin.good())
            throw std::runtime_error{"Error while reading keys"};

        keys.push_back(key);
    }

    return keys;
}

template<typename Key_T>
std::vector<std::pair<Key_T, Key_T>> get_queries()
{
    std::size_t n_queries;
    std::cin >> n_queries;
    if (!std::cin.good())
        throw std::runtime_error{"Error while reading the number of queries"};

    std::vector<std::pair<Key_T, Key_T>> queries;
    queries.reserve(n_queries);

    for (auto query_i = 0; query_i != n_queries; ++query_i)
    {
        Key_T lower_bound;
        Key_T upper_bound;

        std::cin >> lower_bound >> upper_bound;
        if (std::cin.fail())
            throw std::runtime_error{"Error while reading queries"};

        queries.emplace_back(lower_bound, upper_bound);
    }

    return queries;
}

template<typename Tree_T, std::forward_iterator It>
std::vector<std::size_t> run_test(const Tree_T &tree, It from, It to)
{
    std::vector<std::size_t> answers;
    answers.reserve(std::distance(from, to));

    std::for_each(from, to, [&tree, &answers](auto &&pair)
    {
        if (pair.first <= pair.second)
        {
            std::size_t n_elems;

            if constexpr (yLab::contains_subtree_size<typename Tree_T::node_type>)
                n_elems = tree.n_less_or_equal_to(pair.second) - tree.n_less_than(pair.first);
            else
                n_elems = std::distance(tree.lower_bound(pair.first),
                                        tree.upper_bound(pair.second));

            answers.push_back(n_elems);
        }
        else
            answers.push_back(0);
    });

    return answers;
}

} // unnamed namespace

int main()
{
    using key_type = int;

    auto keys = get_keys<key_type>();
    auto queries = get_queries<key_type>();

#if defined(AUGMENTED_SPLAY_TREE)
    yLab::Augmented_Splay_Tree<key_type> tree{keys.begin(), keys.end()};
    std::ofstream file{"augmented_splay_tree.info"};
#elif defined(SPLAY_TREE)
    yLab::Splay_Tree<key_type> tree{keys.begin(), keys.end()};
    std::ofstream file{"splay_tree.info"};
#else
    std::set<key_type> tree{keys.begin(), keys.end()};
    std::ofstream file{"std_set.info"};
#endif

    auto start = std::chrono::high_resolution_clock::now();
    auto answers = run_test(tree, queries.begin(), queries.end());
    auto finish = std::chrono::high_resolution_clock::now();

    std::ranges::copy(answers, std::ostream_iterator<std::size_t>{std::cout, " "});
    std::cout << std::endl;

    file << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count()
         << std::endl;

    return 0;
}
