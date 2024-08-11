#include <vector>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <ranges>
#include <utility>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <chrono>

#include <fmt/core.h>

#if defined(SPLAY_TREE) || defined(AUGMENTED_SPLAY_TREE)
#include "trees/trees.hpp"
#else
#include <set>
#endif

#include "nodes/node_concepts.hpp"

namespace
{

template<typename Tree_T>
std::size_t answer_query(const Tree_T &tree, int key_1, int key_2)
{
    if (key_1 <= key_2)
    {
        if constexpr (yLab::contains_subtree_size<typename Tree_T::node_type>)
            return tree.n_less_or_equal_to(key_2) - tree.n_less_than(key_1);
        else
            return std::distance(tree.lower_bound(key_1), tree.upper_bound(key_2));
    }
    else
        return 0;
}

template<typename Tree_T>
auto run_test()
{
    std::vector<std::size_t> answers;
    Tree_T tree;

    auto start = std::chrono::high_resolution_clock::now();

    for (;;)
    {
        char query;
        std::cin >> query;

        if (!std::cin.good())
        {
            if (std::cin.eof())
                break;
            throw std::runtime_error{"Error while reading a query"};
        }

        switch (query)
        {
            case 'k':
            {
                int key;
                std::cin >> key;
                if (!std::cin.good())
                {
                    if (std::cin.eof())
                        break;
                    throw std::runtime_error{"Error while reading a key"};
                }
                tree.insert(key);
                continue;
            }

            case 'q':
            {
                int key_1;
                std::cin >> key_1;
                if (!std::cin.good())
                    throw std::runtime_error{"Error while reading the first key in a pair"};

                int key_2;
                std::cin >> key_2;
                if (!std::cin.good())
                {
                    if (std::cin.eof())
                    {
                        answers.emplace_back(answer_query(tree, key_1, key_2));
                        break;
                    }

                    throw std::runtime_error{"Error while reading the second key"};
                }

                answers.emplace_back(answer_query(tree, key_1, key_2));
                continue;
            }

            default:
                throw std::runtime_error{"Unknow query"};
        }

        break;
    }

    auto finish = std::chrono::high_resolution_clock::now();
    return std::pair{answers, finish - start};
}

} // unnamed namespace

int main() try
{
    using key_type = int;

#if defined(AUGMENTED_SPLAY_TREE)
    using tree_type = yLab::Augmented_Splay_Tree<key_type>;
    std::ofstream file{"augmented_splay_tree.info"};
#elif defined(SPLAY_TREE)
    using tree_type = yLab::Splay_Tree<key_type>;
    std::ofstream file{"splay_tree.info"};
#else
    using tree_type = std::set<key_type>;
    std::ofstream file{"std_set.info"};
#endif

    auto [answers, time] = run_test<tree_type>();

    std::ranges::copy(answers, std::ostream_iterator<std::size_t>{std::cout, " "});
    std::cout << std::endl;

    file << std::chrono::duration_cast<std::chrono::milliseconds>(time).count()
         << std::endl;

    return 0;
}
catch (const std::exception &e)
{
    fmt::print("Error: {}. Abort\n", e.what());
    return 1;
}
catch (...)
{
    fmt::print("Unknown exception caught. Abort\n");
    return 1;
}
