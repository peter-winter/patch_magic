#pragma once

#include "sequences.hpp"
#include "runtime_processor_wrappers.hpp"

#include <array>
#include <string>
#include <vector>

namespace patch_magic
{

constexpr size_t max_source_in_arg_count = 10;

struct const_f_source { float v_; };
struct reg_f_source { size_t idx_; };

struct const_i_source { int32_t v_; };
struct reg_i_source { size_t idx_; };

using in_arg_source = std::variant<const_i_source, reg_i_source, reg_f_source, const_f_source>;

using in_args_source_array_t = std::array<in_arg_source, max_source_in_arg_count>;

struct op_source
{
    template<typename... Args>
    constexpr op_source(runtime_processor_wrapper_variant_t op, size_t out_idx, Args... ins) noexcept: 
        op_(op), out_idx_{out_idx}, ins_{ins...}
    {}
        
    runtime_processor_wrapper_variant_t op_;
    size_t out_idx_;
    in_args_source_array_t ins_;
};

struct instrument_source
{
    std::string name_;
    std::string patch_name_;
    std::string sequence_name_;
    float on_duration_;
};

struct patch_source
{
    std::string name_;
    std::vector<op_source> ops_;
};

using sequence_descr_t = std::variant<sequences::seq_item_note, sequences::seq_item_tick>;

struct sequence_source
{
    std::string name_;
    sequence_descr_t sequence_descr_;
};

struct source
{
    std::vector<patch_source> patches_;
    std::vector<instrument_source> instruments_;
    std::vector<sequence_source> sequences_;
    float top_sequence_duration_;
};

}
