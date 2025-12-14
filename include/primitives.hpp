#pragma once

#include "runtime.hpp"

namespace patch_magic
{
    
inline float load_const_f(const runtime_data& rd, size_t i) { return rd.constants_f_[i]; }
inline int32_t load_const_i(const runtime_data& rd, size_t i) { return rd.constants_i_[i]; }
inline float load_reg_f(const runtime_data& rd, size_t i) { return rd.regs_f_[i]; }
inline int32_t load_reg_i(const runtime_data& rd, size_t i) { return rd.regs_i_[i]; }
inline void store_reg_f(runtime_data& rd, size_t i, float v) { rd.regs_f_[i] = v; }
inline void store_reg_i(runtime_data& rd, size_t i, float v) { rd.regs_i_[i] = v; }

}
