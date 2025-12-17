#pragma once

#include "utils.hpp"
#include "runtime_processors.hpp"
#include "note_data.hpp"

#include <tuple>
#include <type_traits>
#include <vector>
#include <utility>
#include <variant>

namespace patch_magic
{
        
template<typename RuntimeProcessor>
struct runtime_processor_wrapper_impl
{};
    
template<typename R, typename... Args>
struct runtime_processor_wrapper_impl<R(*)(Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    using state_type = stateless;
};

template<typename R, typename State, typename... Args>
struct runtime_processor_wrapper_impl<R(*)(State&, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    using state_type = State;
};

template<typename R, typename... Args>
struct runtime_processor_wrapper_impl<R(*)(sample_rate_wrapper, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    using state_type = stateless;
};

template<typename R, typename State, typename... Args>
struct runtime_processor_wrapper_impl<R(*)(State&, sample_rate_wrapper, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    using state_type = State;
};

template<typename R, typename... Args>
struct runtime_processor_wrapper_impl<R(*)(const note_data&, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    using state_type = stateless;
};

template<typename R, typename State, typename... Args>
struct runtime_processor_wrapper_impl<R(*)(State&, const note_data&, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    using state_type = State;
};

template<typename R, typename... Args>
struct runtime_processor_wrapper_impl<R(*)(sample_rate_wrapper, const note_data&, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    using state_type = stateless;
};

template<typename R, typename State, typename... Args>
struct runtime_processor_wrapper_impl<R(*)(State&, sample_rate_wrapper, const note_data&, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    using state_type = State;
};

template<auto RuntimeProcessor>
struct runtime_processor_wrapper
{
    using impl_t = runtime_processor_wrapper_impl<std::decay_t<decltype(RuntimeProcessor)>>;
    using state_type = typename impl_t::state_type;
    static constexpr size_t in_arg_count = impl_t::in_arg_count;
    
    static constexpr auto proc = RuntimeProcessor;
};

constexpr runtime_processor_wrapper<calc_sine> sine;
constexpr runtime_processor_wrapper<calc_vol> vol;
constexpr runtime_processor_wrapper<calc_env_ar> env_ar;


template<auto... RuntimeProcessors>
struct runtime_processor_wrapper_variant
{
    using type = std::variant<std::decay_t<decltype(RuntimeProcessors)>...>;
};

template<typename T>
using runtime_processor_state_t = typename T::state_type;

template<typename T>
struct runtime_processor_states
{};

template<typename... T>
struct runtime_processor_states<std::variant<T...>>
{
    using type = unique_tuple_t<std::vector<typename T::state_type>...>;
};


using runtime_processor_wrapper_variant_t = typename runtime_processor_wrapper_variant<
    sine, 
    vol,
    env_ar
>::type;

using runtime_processor_states_t = typename runtime_processor_states<runtime_processor_wrapper_variant_t>::type;


}

