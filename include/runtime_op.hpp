#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

namespace patch_magic
{

struct voice_runtime_data;
struct patch_runtime_data;

constexpr size_t max_runtime_processor_in_arg_count = 10;

enum class arg_loader_type : size_t { loader_const = 0, loader_reg = 1};

struct runtime_op
{
    using process_fn = void(*)(voice_runtime_data&, const patch_runtime_data&, const runtime_op&);
    
    process_fn process_;
    
    std::array<arg_loader_type, max_runtime_processor_in_arg_count> loader_idx_;
    std::array<size_t, max_runtime_processor_in_arg_count> arg_idx_;

    size_t store_idx_;

    size_t state_idx_;
};

}
