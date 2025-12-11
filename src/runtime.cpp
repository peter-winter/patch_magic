#include "runtime.hpp"
#include "loader.hpp"

namespace patch_magic
{

runtime_data::runtime_data(float sample_rate, size_t channels, size_t reg_count, size_t const_count):
    sample_rate_(sample_rate),
    channels_(channels),
    reg_count_(reg_count),
    const_count_(const_count)
{
    clear();
}

void runtime_data::clear()
{
    constants_.resize(const_count_, 0.0f);
    regs_.resize(reg_count_, 0.0f);
    outs_.resize(channels_, 0.0f);
    runtime_ops_.clear();
}
    
    
runtime::runtime(float sample_rate, size_t channels, size_t reg_count, size_t const_count):
    data_(sample_rate, channels, reg_count, const_count)
{}


void runtime::process_ops()
{
    for (const auto& op : loaded_ops_)
        op.process_(data_, op);
}

void runtime::load(const program& prog)
{
    loader ld;
    
    size_t idx = 0;
    for (const auto& src_op : prog.ops_)
    {
        runtime_ops_.push_back(
            std::visit(
                [&]<typename Op>(const Op& src)
                {
                    data_.states_.emplace_back(typename op_descr<Op>::rt_state{});
                    return ld.load_op(src, idx);
                },
                src_op
            )
        );
        ++idx;
    }
}
    
}
