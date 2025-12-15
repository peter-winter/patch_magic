#pragma once

#include "patch.hpp"
#include "runtime_processor_wrappers.hpp"
#include "note_data.hpp"

namespace patch_magic
{
    
using rt_regs_f_t = std::vector<float>;
using rt_regs_i_t = std::vector<int32_t>;

struct voice_runtime_data
{
    float get_reg_f(size_t idx) const { return regs_f_[idx]; }
    int32_t get_reg_i(size_t idx) const { return regs_i_[idx]; }
    
    void set_reg_f(size_t idx, float v) { regs_f_[idx] = v; }
    void set_reg_i(size_t idx, int32_t v) { regs_i_[idx] = v; }
    
    template<typename State>
    auto& get_state(size_t idx)
    {
        return std::get<std::vector<State>>(states_)[idx];
    }
    
    rt_regs_f_t regs_f_;
    rt_regs_i_t regs_i_;
    runtime_processor_states_t states_;
    note_data nd_;
};
    
class voice_slot
{
public:
    voice_slot(size_t reg_count, const patch& p);
            
    float sample();
    void reset();
    
private:
    size_t reg_count_;
    const patch& p_;
    voice_runtime_data rd_;
    bool active_;
};

}
