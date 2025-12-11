#pragma once

namespace patch_magic
{
    
inline float load_const(const runtime& rt, std::size_t i) { return rt.constants_[i]; }
inline float load_reg(const runtime& rt, std::size_t i) { return rt.regs_[i]; }
inline void store_reg(runtime& rt, std::size_t i, float v) { rt.regs_[i] = v; }
inline void store_out(runtime& rt, std::size_t i, float v) { rt.outputs_[i] = v; }

}
