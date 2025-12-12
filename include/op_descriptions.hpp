#pragma once

#include "program.hpp"
#include "runtime_processors.hpp"
#include "states.hpp"

namespace patch_magic
{

template<typename Op>
struct op_descr
{};

template<>
struct op_descr<sine>
{
    using rt_state = sine_state;
    runtime_op::process_fn process_fn_{process_sine};
    
    auto in_descr() { return std::make_tuple(&sine::freq_); }
    auto out_descr() { return &sine::out_; }
};

template<>
struct op_descr<vol>
{
    using rt_state = stateless;
    runtime_op::process_fn process_fn_{process_vol};
    
    auto in_descr() { return std::make_tuple(&vol::gain_, &vol::in_); }
    auto out_descr() { return &vol::out_; }
};


}
