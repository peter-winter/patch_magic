#pragma once

#include <vector>

#include "patch.hpp"
#include "timeline.hpp"
#include "voice.hpp"

namespace patch_magic
{

using voice_slots_t = std::vector<voice_slot>;
using idx_vector_t = std::vector<size_t>;

class instrument
{
public:
    instrument(size_t max_voice_count, size_t reg_count, const patch& p, timeline& t);
        
    void reset();
    
    float sample();
    
private:
    size_t allocate_voice();
    void process_events();
    void do_on(uint32_t id, float freq);
    void do_off(uint32_t id);
    
    size_t max_voice_count_;
    size_t reg_count_;
    const patch& p_;
    timeline& t_;
    voice_slots_t voice_slots_;
    
    idx_vector_t order_[2];
    size_t read_order_idx_;
    size_t write_order_idx_;
    size_t audible_count_;
    size_t free_count_;
    idx_vector_t free_;
};

}
