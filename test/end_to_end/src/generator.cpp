#include <random>
#include <cmath>
#include <iostream>
#include <utility>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <stdexcept>
#include <cstdlib>

namespace yLab
{

std::pair<std::size_t, std::size_t> get_cmd_line_args(int argc, char *argv[])
{
    if (argc != 3)
        throw std::runtime_error{"Program requires 2 arguments"};

    auto n_keys = std::atoi(argv[1]);
    if (n_keys < 0)
        throw std::runtime_error{"The number of keys has to be a positive integer"};

    auto n_queries = std::atoi(argv[2]);
    if (n_queries < 0)
        throw std::runtime_error{"The number of queries has to be a positive integer"};

    return std::pair{n_keys, n_queries};
}

template<typename Key_T, typename Distr_T, typename Engine>
std::pair<Key_T, Key_T> generate_keys(std::size_t n_keys, Distr_T distr, Engine gen)
{
    std::vector<Key_T> keys;
    keys.reserve(n_keys);

    for (auto key_i = 0; key_i != n_keys; ++key_i)
        keys.emplace_back(distr(gen));

    auto min = *std::ranges::min_element(keys);
    auto max = *std::ranges::max_element(keys);

    std::cout << n_keys << std::endl;
    std::ranges::copy(keys, std::ostream_iterator<Key_T>{std::cout, " "});
    std::cout << std::endl;

    return std::pair{std::midpoint(min, max), (max - min) / 2};
}

template<typename Key_T, typename Distr_T, typename Engine>
void generate_queries(std::size_t n_queries, Distr_T distr, Engine gen)
{
    std::cout << n_queries << std::endl;
    for (auto query_i = 0; query_i != n_queries; ++query_i)
    {
        Key_T lower_bound = std::round(distr(gen));
        Key_T upper_bound = std::round(distr(gen));
        if (lower_bound > upper_bound)
            std::swap(lower_bound, upper_bound);

        std::cout << lower_bound << " " << upper_bound << " ";
    }
    std::cout << std::endl;
}

} // namespace yLab

int main(int argc, char *argv[])
{
    using key_type = int;

    auto [n_keys, n_queries] = yLab::get_cmd_line_args(argc, argv);

    std::random_device rd;
    std::mt19937_64 gen{rd()};

    std::uniform_int_distribution<key_type> key{};
    auto [mean, stddef] = yLab::generate_keys<key_type>(n_keys, key, gen);

    std::normal_distribution query{static_cast<double>(mean), static_cast<double>(stddef)};
    yLab::generate_queries<key_type>(n_queries, query, gen);

    return 0;
}
