#pragma once

#include <vector>

#include "patch.hpp"
#include "voice.hpp"

namespace patch_magic
{

using voice_slots_t = std::vector<voice_slot>;

class instrument
{
public:
    instrument(size_t max_voice_count, size_t reg_count, const patch& p);
        
    void reset();
    
    float sample();
    
private:
    size_t max_voice_count_;
    size_t reg_count_;
    const patch& p_;
    voice_slots_t voice_slots_;
};

}
