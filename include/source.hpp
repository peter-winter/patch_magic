#pragma once

#include "runtime_processor_wrappers.hpp"

namespace patch_magic
{

constexpr size_t max_source_in_arg_count = 10;

struct source_const_f { float v_; };
struct source_reg_f { size_t idx_; };

struct source_const_i { int32_t v_; };
struct source_reg_i { size_t idx_; };

using source_in_arg = std::variant<source_const_f, source_reg_f, source_const_i, source_reg_i>;

using source_in_args_array_t = std::array<source_in_arg, max_source_in_arg_count>;

struct source_op
{
    template<typename... Args>
    constexpr source_op(runtime_processor_wrapper_variant_t op, size_t out_idx, Args... ins) noexcept: 
        op_(op), out_idx_{out_idx}, ins_{ins...}
    {}
        
    runtime_processor_wrapper_variant_t op_;
    size_t out_idx_;
    source_in_args_array_t ins_;
};

struct source
{
    std::vector<source_op> ops_;
};

}
