#pragma once

#include <cstddef>

namespace patch_magic
{

struct runtime_data;

struct runtime_op
{
    using process_fn = void(*)(runtime_data&, const runtime_op&);
    using load_fn = float(*)(const runtime_data&, std::size_t);
    using store_fn = void(*)(runtime_data&, std::size_t, float);
    
    process_fn process_ = nullptr;
    
    load_fn load_arg0_ = nullptr;
    load_fn load_arg1_ = nullptr;
    std::size_t arg0_idx_ = 0;
    std::size_t arg1_idx_ = 0;

    store_fn store_ = nullptr;
    std::size_t store_idx_ = 0;

    std::size_t state_idx_ = 0;
};

}
