#pragma once

#include <tuple>

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

}
