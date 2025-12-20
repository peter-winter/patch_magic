#pragma once

#include <vector>
#include <string>

#include "patch.hpp"
#include "sequences.hpp"
#include "voice.hpp"
#include "event_generators.hpp"

namespace patch_magic
{

using namespace sequences;

using voice_slots_t = std::vector<voice_slot>;
using idx_vector_t = std::vector<size_t>;

class instrument
{
public:
    instrument(std::string name, size_t max_voice_count, size_t reg_count, const patch& p, const sequence& seq);
        
    void reset();
    
    float sample();
    
    const std::string& get_display_line();
    
private:
    size_t allocate_voice();
    void process_events();
    void do_on(uint32_t id, float freq);
    void do_off(uint32_t id);
    
    std::string name_;
    
    size_t max_voice_count_;
    size_t reg_count_;
    const patch& p_;
    const sequence& seq_;
    voice_slots_t voice_slots_;
    
    idx_vector_t order_[2];
    size_t read_order_idx_;
    size_t write_order_idx_;
    size_t audible_count_;
    size_t free_count_;
    idx_vector_t free_;
    size_t unordered_samples_total_;
    seq_event_generator gen_;
    
    std::string display_line_;
};

}
