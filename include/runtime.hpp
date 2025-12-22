#pragma once

#include <vector>

#include "instrument.hpp"
#include "patch.hpp"
#include "sequences.hpp"
#include "event_generators.hpp"

namespace patch_magic
{

using instruments_t = std::vector<instrument>;
using patches_t = std::vector<patch>;
using event_generators_t = std::vector<event_generator>;

using debug_callback = void(*)(const char*);

constexpr float default_top_sequence_duration = 4.0f;

class runtime
{
public:
    runtime(size_t max_voice_count_per_instrument, uint32_t sample_rate, size_t channel_count, size_t reg_count_per_voice);

    void reset();

    bool done() const;
    
    void set_debug_callback(debug_callback cb) { debug_callback_ = cb; }
    
    void set_top_sequence_duration(float duration) { top_sequence_duration_ = duration; }
    uint32_t sample_rate() const { return sample_rate_; }
    size_t channel_count() const { return channel_count_; }
    size_t reg_count_per_voice() const { return reg_count_per_voice_; }
    
    size_t add_patch();
    const patch& get_patch(size_t idx) const;
    patch& get_patch(size_t idx);
    
    size_t add_event_generator(const sequences::seq_item_note& note_seq_descr);
    size_t add_event_generator(const sequences::seq_item_tick& sound_seq_descr);
    
    const event_generator& get_event_generator(size_t idx) const;
    event_generator& get_event_generator(size_t idx);
    
    size_t add_instrument(std::string name, const patch& p, event_generator& gen, float on_duration);
        
    void sample(float* data, size_t channel_count);
    
private:
    void debug();
    
    float top_sequence_duration_;
    size_t max_voice_count_per_instrument_;
    uint32_t sample_rate_;
    size_t channel_count_;
    size_t reg_count_per_voice_;
    
    instruments_t instruments_;
    patches_t patches_;
    event_generators_t event_generators_;
            
    event_id_generator eidg_;
    
    debug_callback debug_callback_;
    std::string debug_str_;
};
   
}
