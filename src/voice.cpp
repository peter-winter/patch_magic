#include "voice.hpp"

namespace patch_magic
{
    
voice_slot::voice_slot(size_t reg_count, const patch& p):
    reg_count_(reg_count), p_(p), active_(false)
{
    reset();
}

void voice_slot::reset()
{
    rd_.regs_f_.resize(reg_count_, 0.0f);
    rd_.regs_i_.resize(reg_count_, 0);
    
    rd_.states_ = p_.state_prototypes();
    
    std::apply(
        [&](auto&... states)
        {
            auto reset_states = [](auto& state_vector){ for (auto& s : state_vector){ s.reset(); } };
            (reset_states(states), ...);
        }, 
        rd_.states_
    );
}

float voice_slot::sample()
{
    for (const auto& op : p_.ops())
        op.process_(rd_, p_.get_runtime_data(), op);
    
    return rd_.regs_f_[0];
}

}
