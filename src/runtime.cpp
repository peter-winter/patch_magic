#include "runtime.hpp"
#include "loader.hpp"

namespace patch_magic
{

runtime_data::runtime_data(float sample_rate, size_t channel_count, size_t reg_count, size_t const_count):
    sample_rate_(sample_rate),
    channel_count_(channel_count),
    reg_count_(reg_count),
    const_count_(const_count)
{
    clear();
}

void runtime_data::clear()
{
    constants_.resize(const_count_, 0.0f);
    regs_.resize(reg_count_, 0.0f);
    outs_.resize(channel_count_, 0.0f);
    states_.clear();
    ops_.clear();
}
    
}
