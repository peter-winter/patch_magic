#pragma once

#include "runtime.hpp"
#include "primitives.hpp"

#include <tuple>
#include <utility>

namespace patch_magic
{

using float_arg_loader = float (*)(const voice_runtime_data&, const patch_runtime_data&, size_t);
using int32_arg_loader = int32_t (*)(const voice_runtime_data&, const patch_runtime_data&, size_t);

constexpr float_arg_loader load_fns_f[2] = {load_const_f, load_reg_f};
constexpr int32_arg_loader load_fns_i[2] = {load_const_i, load_reg_i};
    
template<typename... T>
struct type_sequence
{};

template<typename T>
struct arg_loader
{};

template<>
struct arg_loader<float>
{
    template<size_t I>
    static float load(const voice_runtime_data& vrd, const patch_runtime_data& prd, const runtime_op& op)
    {
        return load_fns_f[std::to_underlying(op.loader_idx_[I])](vrd, prd, op.arg_idx_[I]);
    }
};

template<>
struct arg_loader<int32_t>
{
    template<size_t I>
    static float load(const voice_runtime_data& vrd, const patch_runtime_data& prd, const runtime_op& op)
    {
        return load_fns_i[std::to_underlying(op.loader_idx_[I])](vrd, prd, op.arg_idx_[I]);
    }
};

template<size_t... I, typename... T>
inline constexpr auto load_args(const voice_runtime_data& vrd, const patch_runtime_data& prd, const runtime_op& op, std::index_sequence<I...>, type_sequence<T...>)
{
    std::tuple<T...> result;
    ((std::get<I>(result) = arg_loader<T>::template load<I>(vrd, prd, op)), ...);
    return result;
};

template<typename T>
struct storer
{};

template<>
struct storer<float>
{
    static void store(voice_runtime_data& vrd, size_t idx, float data)
    {
        store_reg_f(vrd, idx, data);
    }
};

template<>
struct storer<int32_t>
{
    static void store(voice_runtime_data& vrd, size_t idx, int32_t data)
    {
        store_reg_i(vrd, idx, data);
    }
};

template<typename RuntimeProcessor>
struct invoker_impl
{};
    
template<typename R, typename... Args>
struct invoker_impl<R(*)(Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    static constexpr auto seq = std::make_index_sequence<in_arg_count>{};
    static constexpr auto type_seq = type_sequence<std::decay_t<Args>...>{};
    using arg_tuple = std::tuple<std::decay_t<Args>...>;
    
    template<typename Proc, size_t... I>
    static R call(Proc proc, const arg_tuple& args, std::index_sequence<I...>)
    {
        return proc(std::get<I>(args)...);
    }
    
    template<typename Proc>
    static void invoke(Proc proc, voice_runtime_data& vrd, const patch_runtime_data& prd, const runtime_op& op)
    {
        auto args = load_args(vrd, prd, op, seq, type_seq);
        
        auto res = call(proc, args, seq);
        storer<R>::store(vrd, op.store_idx_, res);
    }
};

template<typename R, typename State, typename... Args>
struct invoker_impl<R(*)(State&, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    static constexpr auto seq = std::make_index_sequence<in_arg_count>{};
    static constexpr auto type_seq = type_sequence<std::decay_t<Args>...>{};
    using arg_tuple = std::tuple<std::decay_t<Args>...>;
    
    template<typename Proc, size_t... I>
    static auto call(State& s, Proc proc, const arg_tuple& args, std::index_sequence<I...>)
    {
        return proc(s, std::get<I>(args)...);
    }
    
    template<typename Proc>
    static void invoke(Proc proc, voice_runtime_data& vrd, const patch_runtime_data& prd, const runtime_op& op)
    {
        auto args = load_args(vrd, prd, op, seq, type_seq);
        
        auto& s = vrd.get_state<State>(op.state_idx_).inner_;
        auto res = call(s, proc, args, op.state_idx_, seq);
        storer<R>::store(vrd, op.store_idx_, res);
    }
};

template<typename R, typename... Args>
struct invoker_impl<R(*)(sample_rate_wrapper, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    static constexpr auto seq = std::make_index_sequence<in_arg_count>{};
    static constexpr auto type_seq = type_sequence<std::decay_t<Args>...>{};
    using arg_tuple = std::tuple<std::decay_t<Args>...>;
    
    template<typename Proc, size_t... I>
    static auto call(Proc proc, uint32_t sample_rate, const arg_tuple& args, std::index_sequence<I...>)
    {
        return proc(sample_rate_wrapper{sample_rate}, std::get<I>(args)...);
    }
    
    template<typename Proc>
    static void invoke(Proc proc, voice_runtime_data& vrd, const patch_runtime_data& prd, const runtime_op& op)
    {
        auto args = load_args(vrd, prd, op, seq, type_seq);
                
        auto res = call(proc, prd.sample_rate_, args, seq);
        storer<R>::store(vrd, op.store_idx_, res);
    }
};

template<typename R, typename State, typename... Args>
struct invoker_impl<R(*)(State&, sample_rate_wrapper, Args...) noexcept>
{
    static constexpr size_t in_arg_count = sizeof...(Args);
    static constexpr auto seq = std::make_index_sequence<in_arg_count>{};
    static constexpr auto type_seq = type_sequence<std::decay_t<Args>...>{};
    using arg_tuple = std::tuple<std::decay_t<Args>...>;
    
    template<typename Proc, size_t... I>
    static auto call(State&s, uint32_t sample_rate, Proc proc, const arg_tuple& args, std::index_sequence<I...>)
    {
        return proc(s, sample_rate_wrapper{sample_rate}, std::get<I>(args)...);
    }
    
    template<typename Proc>
    static void invoke(Proc proc, voice_runtime_data& vrd, const patch_runtime_data& prd, const runtime_op& op)
    {
        auto args = load_args(vrd, prd, op, seq, type_seq);
        
        auto& s = vrd.get_state<State>(op.state_idx_);
        auto res = call(s, prd.sample_rate_, proc, args, seq);
        storer<R>::store(vrd, op.store_idx_, res);
    }
};

template<auto RuntimeProcessor>
struct invoker
{
    using impl_t = invoker_impl<std::decay_t<decltype(RuntimeProcessor)>>;
    
    static void invoke(voice_runtime_data& vrd, const patch_runtime_data& prd, const runtime_op& op)
    {
        impl_t::invoke(proc, vrd, prd, op);
    }
    
    static constexpr auto proc = RuntimeProcessor;
};


}

