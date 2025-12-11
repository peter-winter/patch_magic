#pragma once

#include <cmath>
#include <cstddef>

namespace patch_magic
{
    
struct runtime_data;
struct runtime_op;

inline void process_sine(runtime& rt, const runtime_op& op);
inline void process_vol(runtime& rt, const runtime_op& op);

}
