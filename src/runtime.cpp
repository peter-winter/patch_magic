#include "runtime.hpp"
#include "polyphony.hpp"

namespace patch_magic
{

runtime::runtime(size_t max_voice_count_per_instrument, uint32_t sample_rate, size_t channel_count, size_t reg_count_per_voice):
    max_voice_count_per_instrument_(max_voice_count_per_instrument),
    sample_rate_(sample_rate),
    channel_count_(channel_count),
    reg_count_per_voice_(reg_count_per_voice),
    debug_callback_(nullptr)
{
    reset();
    debug_str_.reserve(1 << 16);
}

void runtime::reset()
{
    for (auto& instr : instruments_)
        instr.reset();
    for (auto& t : timelines_)
        t.reset();
}

bool runtime::active() const
{
    for (const auto& t : timelines_)
    {
        if (t.active())
            return true;
    }
    return false;
}

void runtime::sample(float* data, size_t channel_count)
{
    float sum = 0.0f;
    
    for (auto& t : timelines_)
        t.prepare_events();
    
    for (auto& instr : instruments_)
    {
        sum += instr.sample();
    }
        
    float gain = polyphony_gain(instruments_.size(), polyphony_scale::equal_amplitude);
    float sample = sum * gain;
     
    for (size_t i = 0; i < channel_count; ++i)
        data[i] = sample;
        
    for (auto& t : timelines_)
        t.inc();
        
    debug();
}

void runtime::debug()
{
    if (debug_callback_)
    {        
        debug_str_.clear();
        
        for (auto& instr : instruments_)
        {
            debug_str_.append(instr.get_display_line());
            debug_str_.append("\n");
        }
        
        debug_str_.append("\033[").append(std::to_string(instruments_.size())).append("A");
        
        debug_callback_(debug_str_.data());
    }
}

}
