#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "math.hpp"
#include "states.hpp"

namespace patch_magic
{

struct sample_rate_wrapper
{
    uint32_t value_;
};

inline float calc_sine(sine_state& s, sample_rate_wrapper sample_rate, float f) noexcept
{
    double inc = two_pi * f / sample_rate.value_;
    s.phase_ += inc;
    return std::sin(s.phase_);
}

inline float calc_vol(float g, float x) noexcept
{
    return x * g;
}

}
