#include "voice.hpp"

namespace patch_magic
{
    
voice_slot::voice_slot(size_t reg_count, const patch& p):
    reg_count_(reg_count), p_(p)
{
    rd_.regs_f_.resize(reg_count_, 0.0f);
    rd_.regs_i_.resize(reg_count_, 0);
    
    rd_.processor_states_ = p_.state_prototypes();
    
    reset();
}

void voice_slot::reset()
{
    std::apply(
        [&](auto&... states)
        {
            auto reset_states = [](auto& state_vector){ for (auto& s : state_vector){ s.reset(); } };
            (reset_states(states), ...);
        }, 
        rd_.processor_states_
    );
    
    smoothed_power_ = 0.0f;
}

float voice_slot::sample()
{
    for (const auto& op : p_.ops())
        op.process_(rd_, p_.get_runtime_data(), op);
    
    float sample = rd_.regs_f_[0];
    smoothed_power_ = alpha * sample * sample + (1 - alpha) * smoothed_power_;
    
    return sample;
}

void voice_slot::set_state(voice_slot_state state)
{
    if (rd_.state_ != voice_slot_state::active && state == voice_slot_state::active)
        reset();
    rd_.state_ = state;
    
    rd_.nd_.active_ = (rd_.state_ == voice_slot_state::active);
}

void voice_slot::set_base_freq(float freq)
{
    rd_.nd_.base_freq_ = freq;
}

float voice_slot::score() const
{
    return static_cast<float>(rd_.nd_.active_) * 1000.0 + smoothed_power();
}

}
