#pragma once

#include <cmath>
#include <cstddef>

namespace patch_magic
{
    
struct runtime_data;
struct runtime_op;

void process_sine(runtime_data& rt, const runtime_op& op);
void process_vol(runtime_data& rt, const runtime_op& op);

}
