#include "runtime.hpp"
#include "polyphony.hpp"

namespace patch_magic
{

runtime::runtime(size_t max_voice_count_per_instrument, uint32_t sample_rate, size_t channel_count, size_t reg_count_per_voice):
    max_voice_count_per_instrument_(max_voice_count_per_instrument),
    sample_rate_(sample_rate),
    channel_count_(channel_count),
    reg_count_per_voice_(reg_count_per_voice)
{
    reset();
}

void runtime::reset()
{
    for (auto& instr : instruments_)
        instr.reset();
    for (auto& t : timelines_)
        t.reset();
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
}

}
