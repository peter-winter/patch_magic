#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "math.hpp"
#include "note_data.hpp"

namespace patch_magic
{
    
struct sine_state
{
    double phase_ = 0.0f;
    
    inline void reset() { phase_ = 0.0f; }
};

inline float calc_sine(sine_state& s, sample_rate_wrapper sample_rate, float freq, float scaling) noexcept
{
    double inc = two_pi * freq / sample_rate.value_;
    s.phase_ += inc;
    return std::sin(s.phase_) * scaling;
}

}
