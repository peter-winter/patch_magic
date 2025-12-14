#pragma once

#include <variant>
#include <vector>
#include <tuple>

#include "runtime_op.hpp"
#include "runtime_processor_wrappers.hpp"

namespace patch_magic
{

using runtime_ops_t = std::vector<runtime_op>;
using float_consts_f_t = std::vector<float>;
using float_consts_i_t = std::vector<int32_t>;
using rt_regs_f_t = std::vector<float>;
using rt_regs_i_t = std::vector<int32_t>;
struct runtime_data
{
    runtime_data(uint32_t sample_rate, size_t channel_count, size_t reg_count, size_t const_count);

    template<typename State>
    auto& get_state(size_t idx)
    {
        return std::get<std::vector<State>>(states_)[idx];
    }

    template<typename State>
    void add_state()
    {
        std::get<std::vector<State>>(states_).emplace_back(State{});
    }

    void clear();
    
    uint32_t sample_rate_;
    size_t channel_count_;
    size_t reg_count_;
    size_t const_count_;
    float_consts_f_t constants_f_;
    float_consts_i_t constants_i_;
    rt_regs_f_t regs_f_;
    rt_regs_i_t regs_i_;
    runtime_processor_states_t states_;
    runtime_ops_t ops_;
};
   
}
