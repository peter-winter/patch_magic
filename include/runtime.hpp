#pragma once

#include <variant>
#include <vector>
#include <cstddef>

#include "runtime_op.hpp"

namespace patch_magic
{

struct runtime_data
{
    runtime_data(float sample_rate, size_t channels, size_t reg_count, size_t const_count);
    
    void clear();
    
    float sample_rate_;
    size_t channels_;
    size_t reg_count_;
    size_t const_count_;
    std::vector<float> constants_;
    std::vector<float> regs_;
    std::vector<float> outs_;
    std::vector<std::variant<sine_state, stateless>> states_;
};
    
class runtime
{
public:
    runtime(float sample_rate, size_t channels, size_t reg_count, size_t const_count);
        
    float out(size_t idx) const { return data_.outs_[idx]; }
    
    void process_ops();
    
    void load(const program& prog);
    
private:
    runtime_data data_;    
    std::vector<runtime_op> runtime_ops_;
};

}
