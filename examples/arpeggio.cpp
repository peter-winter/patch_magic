#include "main.h"

#include <sequences.hpp>

using namespace patch_magic;
using namespace patch_magic::sequences;

void arpeggio()
{
    std::cout << "=== Arpeggio in D-Minor, overlapping notes ===\n";
    
    synth s;
    
    std::vector<patch_source> patches
    {
        {
            "sine",
            {
                {base_freq, 0},
                {sine, 0, reg_f_source{0}, const_f_source{0.3f}},
                {env_ar, 1, const_f_source{0.2f}, const_f_source{0.8f}},
                {vol, 0, reg_f_source{1}, reg_f_source{0}}
            }
        }
    };
    
    std::vector<sequence_source> seqs
    {
        {
            "seq1", // 40-note arpeggio in D minor
            _(
                _(_1, _2, _3, _4),
                _(_5, _6, _7, _5),
                _(_3, _4, _5, _6),
                _(_7, _8, _6, _4),
                _(_2, _3, _4, _5),
                _(_6, _7, _8, _9),
                _(_7, _5, _6, _4),
                _(_5, _6, _7, _8),
                _(_9, _7, _5, _6),
                _(_4, _2, _3, _4)
            ) | chords::d_minor[3]
        }
    };
    
    std::vector<instrument_source> instruments{{"i1", "sine", "seq1", 0.95f}};
    
    source src{ patches, instruments, seqs, 40.0f };
    
    s.load(src);
    
    std::cout << "press Enter to play\n";
    std::cin.get();
    
    s.set_debug_callback(print_debug);
    s.play();
    poll_until_done(s);
}

REGISTER_EXAMPLE(arpeggio);

