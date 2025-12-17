#pragma once

namespace patch_magic
{
    
constexpr double two_pi = 2.0 * 3.14159265358979323846;

inline float pow4(float x){ return x * x * x * x; }
inline float pow3(float x){ return x * x * x; }
inline float pow2(float x){ return x * x; }

constexpr float inaudible_freq = 1.0e-8;

}
