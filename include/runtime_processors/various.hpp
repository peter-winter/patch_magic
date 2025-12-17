#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "math.hpp"
#include "note_data.hpp"

namespace patch_magic
{
    
inline float calc_vol(float gain, float in) noexcept
{
    // Perceptual curve: v^4
    return pow4(gain) * in;
}

}
