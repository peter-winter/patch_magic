#pragma once

#include <tuple>
#include <cstdint>

namespace patch_magic
{
    
template<typename T>
struct contains_type
{};

template<typename... T>
struct unique_type_list : contains_type<T>...
{
    using tuple_type = std::tuple<T...>;

    template<typename New>
    struct insert
    {
        static unique_type_list<T...> foo(contains_type<New>*);
        static unique_type_list<T..., New> foo(...);

        using type = decltype(foo(std::declval<unique_type_list<T...>*>()));
    };

    template<typename New>
    typename insert<New>::type operator + (New);
};

template<typename... T>
struct unique_types_tuple
{
    using unique_list = decltype((std::declval<unique_type_list<>>() + ... + std::declval<T>()));
    using type = typename unique_list::tuple_type;
};

template<typename...T>
using unique_tuple_t = typename unique_types_tuple<T...>::type;

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template <typename T, std::size_t S>
constexpr std::pair<const T&, std::int64_t> free_array_access(std::int64_t idx, const std::array<T, S>& arr)
{
    constexpr std::int64_t size = static_cast<std::int64_t>(S);

    std::int64_t original = idx;
    std::int64_t wraps = original / size;
    std::int64_t remainder = original % size;

    if (remainder < 0)
    {
        remainder += size;
        --wraps;
    }

    std::size_t normalized = static_cast<std::size_t>(remainder);
    return {arr[normalized], wraps};
}

}
