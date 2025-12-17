#pragma once

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

struct sound_on_source { uint32_t id_; };
struct sound_off_source { uint32_t id_; };
struct note_on_source { uint32_t id_; float freq_; };
struct note_off_source { uint32_t id_; };

using timed_event_arg = std::variant<sound_on_source, sound_off_source>;

struct timed_event_source
{
    float time_point_;
    timed_event_arg ev_;
};

struct instrument_source
{
    std::string name_;
    std::string patch_name_;
    std::string timeline_name_;
};

struct patch_source
{
    std::string name_;
    std::vector<op_source> ops_;
};

struct timeline_source
{
    std::string name_;
    std::vector<timed_event_source> events_;
};

struct source
{
    std::vector<patch_source> patches_;
    std::vector<instrument_source> instruments_;
    std::vector<timeline_source> timelines_;
};

}
