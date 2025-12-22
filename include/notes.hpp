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
        template<size_t... Octave>
        constexpr static auto fill(std::index_sequence<Octave...>)
        {
            return std::array{ any_scale<Intervals, Octave, Root>... };
        }
    };
}

namespace notes
{
    using namespace detail;
    
    struct note
    {
        float freq_;
    };

    template<uint32_t Root>
    struct note_filler
    {
        template<size_t... Octave>
        constexpr static auto fill(std::index_sequence<Octave...>)
        {
            return std::array{ note{compute(Octave * 12 + Root)}... };
        }
    };
    
    constexpr auto is = std::make_index_sequence<10>{};
    
    inline const auto c{note_filler<0>::fill(is)};
    inline const auto cs{note_filler<1>::fill(is)};
    inline const auto d{note_filler<2>::fill(is)};
    inline const auto ds{note_filler<3>::fill(is)};
    inline const auto e{note_filler<4>::fill(is)};
    inline const auto f{note_filler<5>::fill(is)};
    inline const auto fs{note_filler<6>::fill(is)};
    inline const auto g{note_filler<7>::fill(is)};
    inline const auto gs{note_filler<8>::fill(is)};
    inline const auto a{note_filler<9>::fill(is)};
    inline const auto as{note_filler<10>::fill(is)};
    inline const auto b{note_filler<11>::fill(is)};
              
    inline const auto db = cs;
    inline const auto eb = ds;
    inline const auto gb = fs;
    inline const auto ab = gs;
    inline const auto bb = as;
};

namespace chords
{
    using namespace detail;
    
    constexpr std::array<size_t, 3> major_intervals = { 0, 4, 7 };
    constexpr std::array<size_t, 3> minor_intervals = { 0, 3, 7 };
    
    inline const auto is = std::make_index_sequence<10>{};

    inline const auto c_major = scale_filler<major_intervals, 0>::fill(is);
    inline const auto cs_major = scale_filler<major_intervals, 1>::fill(is);
    inline const auto d_major = scale_filler<major_intervals, 2>::fill(is);
    inline const auto ds_major = scale_filler<major_intervals, 3>::fill(is);
    inline const auto e_major = scale_filler<major_intervals, 4>::fill(is);
    inline const auto f_major = scale_filler<major_intervals, 5>::fill(is);
    inline const auto fs_major = scale_filler<major_intervals, 6>::fill(is);
    inline const auto g_major = scale_filler<major_intervals, 7>::fill(is);
    inline const auto gs_major = scale_filler<major_intervals, 8>::fill(is);
    inline const auto a_major = scale_filler<major_intervals, 9>::fill(is);
    inline const auto as_major = scale_filler<major_intervals, 10>::fill(is);
    inline const auto b_major = scale_filler<major_intervals, 11>::fill(is);
              
    inline const auto df_major = cs_major;
    inline const auto ef_major = ds_major;
    inline const auto gf_major = fs_major;
    inline const auto af_major = gs_major;
    inline const auto bf_major = as_major;
              
    inline const auto c_minor = scale_filler<minor_intervals, 0>::fill(is);
    inline const auto cs_minor = scale_filler<minor_intervals, 1>::fill(is);
    inline const auto d_minor = scale_filler<minor_intervals, 2>::fill(is);
    inline const auto ds_minor = scale_filler<minor_intervals, 3>::fill(is);
    inline const auto e_minor = scale_filler<minor_intervals, 4>::fill(is);
    inline const auto f_minor = scale_filler<minor_intervals, 5>::fill(is);
    inline const auto fs_minor = scale_filler<minor_intervals, 6>::fill(is);
    inline const auto g_minor = scale_filler<minor_intervals, 7>::fill(is);
    inline const auto gs_minor = scale_filler<minor_intervals, 8>::fill(is);
    inline const auto a_minor = scale_filler<minor_intervals, 9>::fill(is);
    inline const auto as_minor = scale_filler<minor_intervals, 10>::fill(is);
    inline const auto b_minor = scale_filler<minor_intervals, 11>::fill(is);
              
    inline const auto df_minor = cs_minor;
    inline const auto ef_minor = ds_minor;
    inline const auto gf_minor = fs_minor;
    inline const auto af_minor = gs_minor;
    inline const auto bf_minor = as_minor;
}

namespace scales
{
    
};

}



