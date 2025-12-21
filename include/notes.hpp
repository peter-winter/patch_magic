#pragma once

#include <cstdint>
#include <utility>
#include <array>

#include "math.hpp"

namespace patch_magic
{

namespace detail
{
    constexpr float compute(uint32_t i)
    {
        // Scientific pitch notation: C4 = middle C, A4 = 440 Hz (12-TET)
        constexpr float a4 = 440.0f;
        // c4 == 48, so 48 - 57 == -9 => c4 is 9 semitones down from a4
        uint32_t semitones_from_a4 = i - 57;
        float exponent = static_cast<float>(semitones_from_a4) / 12.0f;
        return a4 * std::pow(2.0f, exponent);
    }
            
    template<auto Intervals, uint32_t Octave, uint32_t Root>
    constexpr float any_scale(uint32_t nr)
    {
        if (nr < 1)
            return inaudible_freq;
    
        constexpr size_t s = std::size(Intervals);
        
        uint32_t group = (nr - 1) % s;
        uint32_t oct_add = (nr - 1) / s;
        uint32_t final_i = (Octave + oct_add) * 12 + (Root + Intervals[group]);  // (0=C, ..., 11=B)
        return compute(final_i);
    }
        
    template<auto Intervals, uint32_t Root>
    struct scale_filler
    {
        constexpr static auto fill(std::index_sequence<Octave...>)
        {
            return std::array{ any_scale<Intervals, Octave, Root>... };
        }
    };
    
    template<uint32_t Root>
    struct note_filler
    {
        constexpr static auto fill(std::index_sequence<Octave...>)
        {
            return std::array{ compute(Octave * 12 + Root)... };
        }
    };
}

namespace notes
{
    using namespace detail;
    
    constexpr auto is = std::make_index_sequence<10>{};
    
    constexpr auto c = note_filler<0>::fill(is);
    constexpr auto cs = note_filler<1>::fill(is);
    constexpr auto d = note_filler<2>::fill(is);
    constexpr auto ds = note_filler<3>::fill(is);
    constexpr auto e = note_filler<4>::fill(is);
    constexpr auto f = note_filler<5>::fill(is);
    constexpr auto fs = note_filler<6>::fill(is);
    constexpr auto g = note_filler<7>::fill(is);
    constexpr auto gs = note_filler<8>::fill(is);
    constexpr auto a = note_filler<9>::fill(is);
    constexpr auto as = note_filler<10>::fill(is);
    constexpr auto b = note_filler<11>::fill(is);
              
    constexpr auto db = cs;
    constexpr auto eb = ds;
    constexpr auto gb = fs;
    constexpr auto ab = gs;
    constexpr auto bb = as;
};


namespace scales
{
    namespace chords
    {
        using namespace detail;
        
        constexpr std::array<size_t, 3> major_intervals = { 0, 4, 7 };
        constexpr std::array<size_t, 3> minor_intervals = { 0, 3, 7 };
        
        constexpr auto is = std::make_index_sequence<10>{};
    
        constexpr auto c_major = scale_filler<major_intervals, 0>::fill(is);
        constexpr auto cs_major = scale_filler<major_intervals, 1>::fill(is);
        constexpr auto d_major = scale_filler<major_intervals, 2>::fill(is);
        constexpr auto ds_major = scale_filler<major_intervals, 3>::fill(is);
        constexpr auto e_major = scale_filler<major_intervals, 4>::fill(is);
        constexpr auto f_major = scale_filler<major_intervals, 5>::fill(is);
        constexpr auto fs_major = scale_filler<major_intervals, 6>::fill(is);
        constexpr auto g_major = scale_filler<major_intervals, 7>::fill(is);
        constexpr auto gs_major = scale_filler<major_intervals, 8>::fill(is);
        constexpr auto a_major = scale_filler<major_intervals, 9>::fill(is);
        constexpr auto as_major = scale_filler<major_intervals, 10>::fill(is);
        constexpr auto b_major = scale_filler<major_intervals, 11>::fill(is);
                  
        constexpr auto df_major = cs_major;
        constexpr auto ef_major = ds_major;
        constexpr auto gf_major = fs_major;
        constexpr auto af_major = gs_major;
        constexpr auto bf_major = as_major;
                  
        constexpr auto c_minor = scale_filler<minor_intervals, 0>::fill(is);
        constexpr auto cs_minor = scale_filler<minor_intervals, 1>::fill(is);
        constexpr auto d_minor = scale_filler<minor_intervals, 2>::fill(is);
        constexpr auto ds_minor = scale_filler<minor_intervals, 3>::fill(is);
        constexpr auto e_minor = scale_filler<minor_intervals, 4>::fill(is);
        constexpr auto f_minor = scale_filler<minor_intervals, 5>::fill(is);
        constexpr auto fs_minor = scale_filler<minor_intervals, 6>::fill(is);
        constexpr auto g_minor = scale_filler<minor_intervals, 7>::fill(is);
        constexpr auto gs_minor = scale_filler<minor_intervals, 8>::fill(is);
        constexpr auto a_minor = scale_filler<minor_intervals, 9>::fill(is);
        constexpr auto as_minor = scale_filler<minor_intervals, 10>::fill(is);
        constexpr auto b_minor = scale_filler<minor_intervals, 11>::fill(is);
                  
        constexpr auto df_minor = cs_minor;
        constexpr auto ef_minor = ds_minor;
        constexpr auto gf_minor = fs_minor;
        constexpr auto af_minor = gs_minor;
        constexpr auto bf_minor = as_minor;
    }
};

}



