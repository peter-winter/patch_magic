#include "instrument.hpp"
#include "polyphony.hpp"

namespace patch_magic
{

instrument::instrument(size_t max_voice_count, size_t reg_count, const patch& p):
    max_voice_count_(max_voice_count), reg_count_(reg_count), p_(p)
{
    for (size_t i = 0; i < max_voice_count_; ++i)
        voice_slots_.emplace_back(reg_count_, p_);
        
    reset();
}

void instrument::reset()
{        
    for (auto& vs : voice_slots_)
        vs.reset();
}

float instrument::sample()
{
    /*
    float sum = 0.0f;
    for (const auto& vs : voice_slots_)
    {
        float sum += vs.sample();   
    }
    
    return polyphony_gain(max_voice_count_, polyphony_scale::equal_amplitude);
    */
    return voice_slots_[0].sample();
}

}

