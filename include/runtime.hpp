#pragma once

#include <vector>

#include "instrument.hpp"
#include "patch.hpp"

namespace patch_magic
{

using instruments_t = std::vector<instrument>;
using patches_t = std::vector<patch>;

class runtime
{
public:
    runtime(size_t max_voice_count_per_instrument, uint32_t sample_rate, size_t channel_count, size_t reg_count_per_voice);

    void reset();

    uint32_t sample_rate() const { return sample_rate_; }
    size_t channel_count() const { return channel_count_; }
    
    size_t add_patch() { patches_.emplace_back(sample_rate_); return patches_.size() - 1; }
    const patch& get_patch(size_t idx) const { return patches_[idx]; }
    patch& get_patch(size_t idx) { return patches_[idx]; }
    
    size_t add_instrument(const patch& p) { instruments_.emplace_back(max_voice_count_per_instrument_, reg_count_per_voice_, p); return instruments_.size() - 1; }
    const instruments_t& get_instruments() const { return instruments_; }
        
    void sample(float* data, size_t channel_count);
    
private:
    size_t max_voice_count_per_instrument_;
    uint32_t sample_rate_;
    size_t channel_count_;
    size_t reg_count_per_voice_;
        
    instruments_t instruments_;
    patches_t patches_;
};
   
}
