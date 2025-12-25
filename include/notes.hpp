#pragma once

#include <cstdint>
#include <utility>
#include <array>

#include "math.hpp"
#include "utils.hpp"

namespace patch_magic
{

struct note
{
    float freq_;
};

using note_from_number = note (*)(int64_t);
    
namespace notes
{
    constexpr std::array<float, 120> note_frequencies =
    {
        16.351598f, 17.323915f, 18.354048f, 19.445436f, 20.601722f, 21.826765f,
        23.124651f, 24.499714f, 25.956544f, 27.500000f, 29.135235f, 30.867707f,
        32.703197f, 34.647829f, 36.708096f, 38.890873f, 41.203445f, 43.653530f,
        46.249303f, 48.999428f, 51.913087f, 55.000000f, 58.270470f, 61.735413f,
        65.406395f, 69.295658f, 73.416192f, 77.781746f, 82.406889f, 87.307061f,
        92.498606f, 97.998856f, 103.826174f, 110.000000f, 116.540940f, 123.470825f,
        130.812790f, 138.591315f, 146.832384f, 155.563492f, 164.813778f, 174.614121f,
        184.997211f, 195.997711f, 207.652349f, 220.000000f, 233.081881f, 246.941651f,
        261.625565f, 277.182631f, 293.664768f, 311.126984f, 329.627557f, 349.228241f,
        369.994423f, 391.995422f, 415.304698f, 440.000000f, 466.163761f, 493.883301f,
        523.251131f, 554.365262f, 587.329536f, 622.253968f, 659.255114f, 698.456482f,
        739.988845f, 783.990875f, 830.609395f, 880.000000f, 932.327523f, 987.766603f,
        1046.502262f, 1108.730524f, 1174.659072f, 1244.507935f, 1318.510228f, 1396.912963f,
        1479.977691f, 1567.981749f, 1661.218790f, 1760.000000f, 1864.655046f, 1975.533205f,
        2093.004525f, 2217.461048f, 2349.318143f, 2489.015870f, 2637.020455f, 2793.825927f,
        2959.955382f, 3135.963499f, 3322.437581f, 3520.000000f, 3729.310092f, 3951.066410f,
        4186.009049f, 4434.922097f, 4698.636287f, 4978.031740f, 5274.040911f, 5587.651854f,
        5919.910763f, 6271.926997f, 6644.875161f, 7040.000000f, 7458.620184f, 7902.132820f,
        8366.018099f, 8869.844193f, 9397.272574f, 9956.063481f, 10548.081821f, 11175.303708f,
        11839.821527f, 12543.853995f, 13289.750323f, 14080.000000f, 14917.240368f, 15804.265640f
    };

    constexpr note c(uint32_t octave)  { return {note_frequencies[octave * 12 + 0]}; }
    constexpr note cs(uint32_t octave) { return {note_frequencies[octave * 12 + 1]}; }
    constexpr note d(uint32_t octave)  { return {note_frequencies[octave * 12 + 2]}; }
    constexpr note ds(uint32_t octave) { return {note_frequencies[octave * 12 + 3]}; }
    constexpr note e(uint32_t octave)  { return {note_frequencies[octave * 12 + 4]}; }
    constexpr note f(uint32_t octave)  { return {note_frequencies[octave * 12 + 5]}; }
    constexpr note fs(uint32_t octave) { return {note_frequencies[octave * 12 + 6]}; }
    constexpr note g(uint32_t octave)  { return {note_frequencies[octave * 12 + 7]}; }
    constexpr note gs(uint32_t octave) { return {note_frequencies[octave * 12 + 8]}; }
    constexpr note a(uint32_t octave)  { return {note_frequencies[octave * 12 + 9]}; }
    constexpr note as(uint32_t octave) { return {note_frequencies[octave * 12 + 10]}; }
    constexpr note b(uint32_t octave)  { return {note_frequencies[octave * 12 + 11]}; }

    // Flat aliases (enharmonic equivalents)
    constexpr note db(uint32_t octave) { return cs(octave); }
    constexpr note eb(uint32_t octave) { return ds(octave); }
    constexpr note gb(uint32_t octave) { return fs(octave); }
    constexpr note ab(uint32_t octave) { return gs(octave); }
    constexpr note bb(uint32_t octave) { return as(octave); }
};

namespace chords
{
    constexpr std::array<int32_t, 3> major_intervals = { 0, 4, 7 };
    constexpr std::array<int32_t, 3> minor_intervals = { 0, 3, 7 };
    
    template<auto Intervals, uint32_t Octave, uint32_t Root>
    constexpr note any_scale(int64_t nr)
    {
        auto acc = free_array_access(nr, Intervals);
        auto interval = acc.first;
        auto wraps = acc.second;
        return note{notes::note_frequencies[(Octave + wraps) * 12 + Root + interval]};
    }
    
    template<auto Intervals, uint32_t Root, size_t Octaves = 10>
    struct scale_filler
    {
        template<size_t... Octave>
        constexpr static auto fill_impl(std::index_sequence<Octave...>)
        {
            return std::array{ any_scale<Intervals, Octave, Root>... };
        }
        
        constexpr static auto fill()
        {
            return fill_impl(std::make_index_sequence<Octaves>{});
        }
    };
    
    constexpr auto c_major = scale_filler<major_intervals, 0>::fill();
    constexpr auto cs_major = scale_filler<major_intervals, 1>::fill();
    constexpr auto d_major = scale_filler<major_intervals, 2>::fill();
    constexpr auto ds_major = scale_filler<major_intervals, 3>::fill();
    constexpr auto e_major = scale_filler<major_intervals, 4>::fill();
    constexpr auto f_major = scale_filler<major_intervals, 5>::fill();
    constexpr auto fs_major = scale_filler<major_intervals, 6>::fill();
    constexpr auto g_major = scale_filler<major_intervals, 7>::fill();
    constexpr auto gs_major = scale_filler<major_intervals, 8>::fill();
    constexpr auto a_major = scale_filler<major_intervals, 9>::fill();
    constexpr auto as_major = scale_filler<major_intervals, 10>::fill();
    constexpr auto b_major = scale_filler<major_intervals, 11>::fill();
    
    constexpr auto df_major = cs_major;
    constexpr auto ef_major = ds_major;
    constexpr auto gf_major = fs_major;
    constexpr auto af_major = gs_major;
    constexpr auto bf_major = as_major;
              
    constexpr auto c_minor = scale_filler<minor_intervals, 0>::fill();
    constexpr auto cs_minor = scale_filler<minor_intervals, 1>::fill();
    constexpr auto d_minor = scale_filler<minor_intervals, 2>::fill();
    constexpr auto ds_minor = scale_filler<minor_intervals, 3>::fill();
    constexpr auto e_minor = scale_filler<minor_intervals, 4>::fill();
    constexpr auto f_minor = scale_filler<minor_intervals, 5>::fill();
    constexpr auto fs_minor = scale_filler<minor_intervals, 6>::fill();
    constexpr auto g_minor = scale_filler<minor_intervals, 7>::fill();
    constexpr auto gs_minor = scale_filler<minor_intervals, 8>::fill();
    constexpr auto a_minor = scale_filler<minor_intervals, 9>::fill();
    constexpr auto as_minor = scale_filler<minor_intervals, 10>::fill();
    constexpr auto b_minor = scale_filler<minor_intervals, 11>::fill();
    
    constexpr auto df_minor = cs_minor;
    constexpr auto ef_minor = ds_minor;
    constexpr auto gf_minor = fs_minor;
    constexpr auto af_minor = gs_minor;
    constexpr auto bf_minor = as_minor;
}

namespace scales
{
    
};

}



