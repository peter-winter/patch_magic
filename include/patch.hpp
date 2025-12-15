#pragma once

#include <vector>

#include "runtime_op.hpp"
#include "runtime_processor_wrappers.hpp"

namespace patch_magic
{

using runtime_ops_t = std::vector<runtime_op>;
using float_consts_f_t = std::vector<float>;
using float_consts_i_t = std::vector<int32_t>;

struct patch_runtime_data
{
    float get_const_f(size_t idx) const { return constants_f_[idx]; }
    int32_t get_const_i(size_t idx) const { return constants_i_[idx]; }
    
    float_consts_f_t constants_f_;
    float_consts_i_t constants_i_;
    uint32_t sample_rate_;
};

class patch
{
public:
    patch() = delete;
    patch(const patch&) = delete;
    patch& operator = (const patch&) = delete;
    patch(patch&&) = default;
    patch& operator=(patch&&) = default;
    
    patch(uint32_t sample_rate)
    {
        rd_.sample_rate_ = sample_rate;
    }
    
    const auto& ops() const { return ops_; }
    void add_op(runtime_op op) { ops_.emplace_back(op); }
    
    template<typename State>
    void add_state_prototype()
    {
        auto& prototypes = std::get<std::vector<State>>(state_prototypes_);
        prototypes.emplace_back(State{});
        prototypes.back().reset();
    }
    
    const auto& state_prototypes() const { return state_prototypes_; }
        
    size_t add_const_f(float v) { rd_.constants_f_.emplace_back(v); return rd_.constants_f_.size() - 1; }
    size_t add_const_i(int32_t v) { rd_.constants_i_.emplace_back(v); return rd_.constants_i_.size() - 1; }
    
    const patch_runtime_data& get_runtime_data() const { return rd_; }
    
private:
    patch_runtime_data rd_;
    runtime_ops_t ops_;
    runtime_processor_states_t state_prototypes_;
};

}

