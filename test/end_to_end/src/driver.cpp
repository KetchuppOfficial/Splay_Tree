#include <iostream>
#include <stdexcept>
#include <vector>
#include <utility>
#include <algorithm>

#include "splay_tree.hpp"

namespace yLab
{

using key_type = int;
using tree_type = yLab::Splay_Tree<key_type>;
using query_type = std::pair<key_type, key_type>;

std::pair<tree_type, std::vector<query_type>> get_tree_and_queries ()
{
    std::size_t n_elems;
    std::cin >> n_elems;
    if (!std::cin.good())
        throw std::runtime_error{"Error while reading the number of elements"};

    tree_type tree;
    for (auto elem_i = 0; elem_i != n_elems; ++elem_i)
    {
        key_type elem;
        std::cin >> elem;
        if (!std::cin.good())
            throw std::runtime_error{"Error while reading elements"};

        tree.insert (elem);
    }

    std::size_t n_queries;
    std::cin >> n_queries;
    if (!std::cin.good())
        throw std::runtime_error{"Error while reading the number of queries"};

    std::vector<query_type> queries;
    queries.reserve (n_queries);

    for (auto query_i = 0; query_i != n_queries; ++query_i)
    {
        key_type lower_bound;
        key_type upper_bound;

        std::cin >> lower_bound >> upper_bound;
        if (std::cin.fail())
            throw std::runtime_error{"Error while reading queries"};

        queries.emplace_back (lower_bound, upper_bound);
    }

    return std::pair{tree, queries};
}

} // namespace yLab

int main ()
{
    auto [tree, queries] = yLab::get_tree_and_queries();

    std::for_each (queries.begin(), queries.end(),
                   [&tree](auto &&pair){ std::cout << std::distance (tree.lower_bound (pair.first),
                                                                     tree.upper_bound (pair.second)) << " "; });
    std::cout << std::endl;

    return 0;
}
