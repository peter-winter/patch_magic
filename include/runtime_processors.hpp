#pragma once

#include <cstdint>

struct sample_rate_wrapper
{
    uint32_t value_;
};

#include "runtime_processors/generators.hpp"
#include "runtime_processors/various.hpp"
#include "runtime_processors/envelope.hpp"

namespace patch_magic
{

struct stateless
{
    inline void reset() {}
};

}
