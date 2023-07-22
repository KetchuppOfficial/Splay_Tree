#include <iostream>
#include <stdexcept>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <fstream>

#ifdef SPLAY_TREE
#include "splay_tree.hpp"
#else
#include <set>
#endif

namespace yLab
{

template<typename Key_T>
std::vector<Key_T> get_keys ()
{
    std::size_t n_keys;
    std::cin >> n_keys;
    if (!std::cin.good())
        throw std::runtime_error{"Error while reading the number of keys"};

    std::vector<Key_T> keys;
    keys.reserve (n_keys);

    for (auto key_i = 0; key_i != n_keys; ++key_i)
    {
        Key_T key;
        std::cin >> key;
        if (!std::cin.good())
            throw std::runtime_error{"Error while reading keys"};

        keys.emplace_back (key);
    }

    return keys;
}

template<typename Key_T>
std::vector<std::pair<Key_T, Key_T>> get_queries ()
{
    std::size_t n_queries;
    std::cin >> n_queries;
    if (!std::cin.good())
        throw std::runtime_error{"Error while reading the number of queries"};

    std::vector<std::pair<Key_T, Key_T>> queries;
    queries.reserve (n_queries);

    for (auto query_i = 0; query_i != n_queries; ++query_i)
    {
        Key_T lower_bound;
        Key_T upper_bound;

        std::cin >> lower_bound >> upper_bound;
        if (std::cin.fail())
            throw std::runtime_error{"Error while reading queries"};

        queries.emplace_back (lower_bound, upper_bound);
    }

    return queries;
}

} // namespace yLab

int main ()
{
    using key_type = int;
    
    auto keys = yLab::get_keys<key_type>();
    auto queries = yLab::get_queries<key_type>();

    #ifdef SPLAY_TREE
    yLab::Splay_Tree<key_type> tree (keys.begin(), keys.end());
    #else
    std::set<key_type> tree (keys.begin(), keys.end());
    #endif

    auto start = std::chrono::high_resolution_clock::now();

    std::for_each (queries.begin(), queries.end(),
                   [&tree](auto &&pair){ std::cout << std::distance (tree.lower_bound (pair.first),
                                                                     tree.upper_bound (pair.second)) << " "; });

    std::cout << std::endl;

    auto finish = std::chrono::high_resolution_clock::now();

    #ifdef SPLAY_TREE
    std::ofstream file{"splay_tree.info"};
    #else
    std::ofstream file{"std_set.info"};
    #endif

    file << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << std::endl;

    return 0;
}
