#include <utility>
#include <cstddef>
#include <vector>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <exception>

#include <fmt/base.h>
#include <fmt/ranges.h>

#include <CLI/CLI.hpp>

namespace
{

template<typename Key_T, typename Distr_T, typename Engine>
std::pair<Key_T, Key_T> generate_keys(std::size_t n_keys, Distr_T distr, Engine gen)
{
    std::vector<Key_T> keys;
    keys.reserve(n_keys);

    for (auto _ : std::views::iota(0uz, n_keys))
        keys.emplace_back(distr(gen));

    auto [min_it, max_it] = std::ranges::minmax_element(keys);

    fmt::println("k {}", fmt::join(keys, " k "));

    return std::pair{std::midpoint(*min_it, *max_it), (*max_it - *min_it) / 2};
}

struct Query final
{
    int first;
    int second;
};

} // unnamed namespace

namespace fmt
{

template<>
struct formatter<Query> final
{
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

    format_context::iterator format(Query q, format_context &ctx) const
    {
        return format_to(ctx.out(), "q {} {}", q.first, q.second);
    }
};

} // namespace fmt

namespace {

template<typename Key_T, typename Distr_T, typename Engine>
void generate_queries(std::size_t n_queries, Distr_T distr, Engine gen)
{
    std::vector<Query> queries;
    queries.reserve(n_queries);

    for (auto _ : std::views::iota(0uz, n_queries))
    {
        auto first = std::round(distr(gen));
        auto second = std::round(distr(gen));
        if (first > second)
            std::swap(first, second);
        queries.emplace_back(first, second);
    }

    fmt::println("{}", fmt::join(queries, " "));
}

} // unnamed namespace

int main(int argc, char *argv[]) try
{
    using key_type = int;

    CLI::App app{"Splay tree end-to-end tests generator"};

    std::size_t n_keys;
    app.add_option("--n-keys", n_keys, "Set the number of keys to generate")
        ->required();

    std::size_t n_queries;
    app.add_option("--n-queries", n_queries, "Set the number of queries to generate")
        ->required();

    CLI11_PARSE(app, argc, argv);

    std::random_device rd;
    std::mt19937_64 gen{rd()};

    std::uniform_int_distribution<key_type> key;
    auto [mean, stddef] = generate_keys<key_type>(n_keys, key, gen);

    std::normal_distribution query{static_cast<double>(mean), static_cast<double>(stddef)};
    generate_queries<key_type>(n_queries, query, gen);

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
