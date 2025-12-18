#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "note_data.hpp"

namespace patch_magic
{
    
inline float calc_base_freq(const note_data& nd) noexcept
{
    return nd.base_freq_;
}

}
