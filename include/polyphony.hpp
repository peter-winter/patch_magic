#pragma once

namespace patch_magic
{
    
enum class polyphony_scale
{
    equal_amplitude,   // divide-by-N
    equal_power        // divide-by-sqrt(N), unison mode
};

constexpr float polyphony_gain(size_t voice_count, polyphony_scale mode)
{
    if (voice_count <= 1) return 1.0f;
    switch (mode)
    {
        case polyphony_scale::equal_power:
            return 1.0f / std::sqrt(static_cast<float>(voice_count));
        case polyphony_scale::equal_amplitude:
            return 1.0f / static_cast<float>(voice_count);
    }
    return 1.0f;
}

}
