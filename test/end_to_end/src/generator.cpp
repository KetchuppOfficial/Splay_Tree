#include <utility>
#include <cstddef>
#include <vector>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <cmath>
#include <random>
#include <exception>

#include <fmt/core.h>
#include <boost/program_options.hpp>

namespace yLab
{

template<typename Key_T, typename Distr_T, typename Engine>
std::pair<Key_T, Key_T> generate_keys(std::size_t n_keys, Distr_T distr, Engine gen)
{
    std::vector<Key_T> keys;
    keys.reserve(n_keys);

    for (auto _ : std::views::iota(0uz, n_keys))
        keys.emplace_back(distr(gen));

    auto min = *std::ranges::min_element(keys);
    auto max = *std::ranges::max_element(keys);

    std::cout << n_keys << '\n';
    std::ranges::copy(keys, std::ostream_iterator<Key_T>{std::cout, " "});
    std::cout << std::endl;

    return std::pair{std::midpoint(min, max), (max - min) / 2};
}

template<typename Key_T, typename Distr_T, typename Engine>
void generate_queries(std::size_t n_queries, Distr_T distr, Engine gen)
{
    std::cout << n_queries << std::endl;
    for (auto _ : std::views::iota(0uz, n_queries))
    {
        Key_T lower_bound = std::round(distr(gen));
        Key_T upper_bound = std::round(distr(gen));
        if (lower_bound > upper_bound)
            std::swap(lower_bound, upper_bound);

        std::cout << lower_bound << ' ' << upper_bound << ' ';
    }
    std::cout << std::endl;
}

} // namespace yLab

int main(int argc, char *argv[]) try
{
    using key_type = int;

    namespace po = boost::program_options;

    po::options_description desc{"Allowed options"};

    std::size_t n_keys, n_queries;
    desc.add_options()
        ("help", "Produce help message")
        ("n-keys", po::value<std::size_t>(&n_keys)->required(),
         "Set the number of keys to generate")
        ("n-queries", po::value<std::size_t>(&n_queries)->required(),
         "Set the number of queries to generate");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }

    po::notify(vm);

    std::random_device rd;
    std::mt19937_64 gen{rd()};

    std::uniform_int_distribution<key_type> key{};
    auto [mean, stddef] = yLab::generate_keys<key_type>(n_keys, key, gen);

    std::normal_distribution query{static_cast<double>(mean), static_cast<double>(stddef)};
    yLab::generate_queries<key_type>(n_queries, query, gen);

    return 0;
}
catch(const std::exception &e)
{
    fmt::print("Error: {}. Abort\n", e.what());
    return 1;
}
catch (...)
{
    fmt::print("Unknown exception caught. Abort\n");
    return 1;
}
