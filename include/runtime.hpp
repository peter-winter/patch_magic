#pragma once

#include <vector>

#include "instrument.hpp"
#include "patch.hpp"
#include "timeline.hpp"

namespace patch_magic
{

using instruments_t = std::vector<instrument>;
using patches_t = std::vector<patch>;
using timelines_t = std::vector<timeline>;

using debug_callback = void(*)(const char*);

class runtime
{
public:
    runtime(size_t max_voice_count_per_instrument, uint32_t sample_rate, size_t channel_count, size_t reg_count_per_voice);

    void reset();

    bool active() const;
    
    void set_debug_callback(debug_callback cb) { debug_callback_ = cb; }
    
    uint32_t sample_rate() const { return sample_rate_; }
    size_t channel_count() const { return channel_count_; }
    size_t reg_count_per_voice() const { return reg_count_per_voice_; }
    
    size_t add_patch() { patches_.emplace_back(sample_rate_); return patches_.size() - 1; }
    const patch& get_patch(size_t idx) const { return patches_[idx]; }
    patch& get_patch(size_t idx) { return patches_[idx]; }
    
    size_t add_instrument(std::string name, const patch& p, timeline& t) { instruments_.emplace_back(name, max_voice_count_per_instrument_, reg_count_per_voice_, p, t); return instruments_.size() - 1; }
    const instruments_t& get_instruments() const { return instruments_; }
        
    size_t add_timeline(sample_index duration_samples, bool looping) { timelines_.emplace_back(duration_samples, looping); return timelines_.size() - 1; }
    const timeline& get_timeline(size_t idx) const { return timelines_[idx]; }
    timeline& get_timeline(size_t idx) { return timelines_[idx]; }
    
    void sample(float* data, size_t channel_count);
    
private:
    void debug();
    
    size_t max_voice_count_per_instrument_;
    uint32_t sample_rate_;
    size_t channel_count_;
    size_t reg_count_per_voice_;
    
    instruments_t instruments_;
    patches_t patches_;
    timelines_t timelines_;
    
    debug_callback debug_callback_;
    std::string debug_str_;
};
   
}
