#pragma once

#include <variant>
#include <vector>
#include <cstddef>

#include "runtime_op.hpp"
#include "states.hpp"

namespace patch_magic
{

using runtime_states = std::vector<std::variant<sine_state, stateless>>;
using runtime_ops = std::vector<runtime_op>;
using float_consts = std::vector<float>;
using rt_regs = std::vector<float>;
using rt_outs = std::vector<float>;

struct runtime_data
{
    runtime_data(float sample_rate, size_t channel_count, size_t reg_count, size_t const_count);
    
    void clear();
    
    float sample_rate_;
    size_t channel_count_;
    size_t reg_count_;
    size_t const_count_;
    float_consts constants_;
    rt_regs regs_;
    rt_outs outs_;
    runtime_states states_;
    runtime_ops ops_;
};
   
}
