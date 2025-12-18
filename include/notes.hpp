#pragma once

namespace patch_magic
{
    
// Scientific pitch notation: C4 = middle C, A4 = 440 Hz (12-TET)
struct notes
{
    static constexpr float A4 = 440.0f;

    inline static constexpr float compute(int i)
    {
        int semitones_from_A4 = i - 57;
        float exponent = static_cast<float>(semitones_from_A4) / 12.0f;
        return A4 * std::pow(2.0f, exponent);
    }

    static constexpr float C (int octave) { return compute(octave * 12 +  0); }
    static constexpr float Cs(int octave) { return compute(octave * 12 +  1); } // C♯/D♭
    static constexpr float D (int octave) { return compute(octave * 12 +  2); }
    static constexpr float Ds(int octave) { return compute(octave * 12 +  3); } // D♯/E♭
    static constexpr float E (int octave) { return compute(octave * 12 +  4); }
    static constexpr float F (int octave) { return compute(octave * 12 +  5); }
    static constexpr float Fs(int octave) { return compute(octave * 12 +  6); } // F♯/G♭
    static constexpr float G (int octave) { return compute(octave * 12 +  7); }
    static constexpr float Gs(int octave) { return compute(octave * 12 +  8); } // G♯/A♭
    static constexpr float A (int octave) { return compute(octave * 12 +  9); }
    static constexpr float As(int octave) { return compute(octave * 12 + 10); } // A♯/B♭
    static constexpr float B (int octave) { return compute(octave * 12 + 11); }

    // Flat aliases (optional but nice)
    static constexpr float Db(int octave) { return Cs(octave); }
    static constexpr float Eb(int octave) { return Ds(octave); }
    static constexpr float Gb(int octave) { return Fs(octave); }
    static constexpr float Ab(int octave) { return Gs(octave); }
    static constexpr float Bb(int octave) { return As(octave); }
};

// Compile-time sanity checks
static_assert(notes::C(4)  > 261.62f && notes::C(4) < 261.64f, "C4 frequency wrong");
static_assert(notes::A(4) == 440.0f, "A4 must be exactly 440 Hz");

}



