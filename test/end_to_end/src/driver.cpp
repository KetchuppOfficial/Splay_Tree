#include <vector>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <iterator>
#include <chrono>
#include <set>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <CLI/CLI.hpp>

#include "trees/trees.hpp"
#include "nodes/node_concepts.hpp"

namespace
{

using key_type = int;

key_type get_key()
{
    key_type key;
    std::cin >> key;
    if (std::cin.fail())
        throw std::runtime_error{"Error while reading a key"};
    return key;
}

std::pair<key_type, key_type> get_range()
{
    auto key_1 = get_key();
    auto key_2 = get_key();
    return std::pair{key_1, key_2};
}

template<typename Tree_T>
std::size_t answer_query(const Tree_T &tree, std::pair<key_type, key_type> range)
{
    if (range.first > range.second)
        return 0;

    if constexpr (yLab::contains_subtree_size<typename Tree_T::node_type>)
        return tree.n_less_than(range.second) - tree.n_less_than(range.first);
    else
        return std::distance(tree.lower_bound(range.first), tree.lower_bound(range.second));
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
            break;

        switch (query)
        {
            case 'k':
                tree.insert(get_key());
                break;

            case 'q':
                answers.push_back(answer_query(tree, get_range()));
                break;

            default:
                throw std::runtime_error{fmt::format("Unknow query \'{}\'", query)};
        }
    }

    auto finish = std::chrono::high_resolution_clock::now();
    return std::pair{std::move(answers), finish - start};
}

} // unnamed namespace

int main(int argc, char **argv) try
{
    CLI::App app{"Program that runs a benchmark from stdin on a search tree"};

    auto *time_flag = app.add_flag("-t,--time", "Measure execution time of the benchmark");
    auto *ans_flag = app.add_flag("-a,--answers", "Print answers to the given range queries");

    std::string tree_type;
    app.add_option("--tree", tree_type, "The type of search tree to run benchmark on")
        ->check(CLI::IsMember({"std::set", "splay", "splay+"}))
        ->required();

    CLI11_PARSE(app, argc, argv);

    auto [answers, time] = [&]
    {
        if (tree_type == "std::set")
            return run_test<std::set<key_type>>();
        else if (tree_type == "splay")
            return run_test<yLab::Splay_Tree<key_type>>();
        else if (tree_type == "splay+")
            return run_test<yLab::Augmented_Splay_Tree<key_type>>();
        std::unreachable();
    }();

    if (*time_flag)
        fmt::print("{} ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(time).count());

    if (*ans_flag)
        fmt::println("{}", fmt::join(answers, " "));

    return 0;
}
catch (const std::exception &e)
{
    fmt::println(stderr, "Caught an instance of {}.\n what(): {}", typeid(e).name(), e.what());
    return 1;
}
catch (...)
{
    fmt::println(stderr, "Caught an unknown exception");
    return 1;
}
