#pragma once

#include "runtime.hpp"

namespace patch_magic
{
    
inline float load_const_f(const voice_runtime_data&, const patch_runtime_data& prd, size_t i) { return prd.get_const_f(i); }
inline int32_t load_const_i(const voice_runtime_data&, const patch_runtime_data& prd, size_t i) { return prd.get_const_f(i); }
inline float load_reg_f(const voice_runtime_data& vrd, const patch_runtime_data&, size_t i) { return vrd.get_reg_f(i); }
inline int32_t load_reg_i(const voice_runtime_data& vrd, const patch_runtime_data&, size_t i) { return vrd.get_reg_i(i); }
inline void store_reg_f(voice_runtime_data& vrd, size_t i, float v) { vrd.set_reg_f(i, v); }
inline void store_reg_i(voice_runtime_data& vrd, size_t i, float v) { vrd.set_reg_i(i, v); }

}
