#include "runtime.hpp"

namespace patch_magic
{

runtime_data::runtime_data(uint32_t sample_rate, size_t channel_count, size_t reg_count, size_t const_count):
    sample_rate_(sample_rate),
    channel_count_(channel_count),
    reg_count_(reg_count),
    const_count_(const_count)
{
    clear();
}

void runtime_data::clear()
{
    constants_f_.resize(const_count_, 0.0f);
    constants_i_.resize(const_count_, 0.0f);
    regs_f_.resize(reg_count_, 0.0f);
    regs_i_.resize(reg_count_, 0.0f);
    states_ = runtime_processor_states_t{};
    ops_.clear();
}
    
}
