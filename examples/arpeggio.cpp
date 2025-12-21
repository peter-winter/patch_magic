#include "main.h"

#include <notes.hpp>

#include <sequences.hpp>

using namespace patch_magic;
using namespace patch_magic::sequences;

int arpeggio()
{
    try
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
                    {env_ar, 1, const_f_source{0.2f}, const_f_source{0.8f}},  // Fast attack, longer release
                    {vol, 0, reg_f_source{1}, reg_f_source{0}}
                }
            }
        };
        
        std::vector<sequence_source> seqs
        {
            {
                "seq1",
                _(
                    // 40-note arpeggio in D minor
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
                ) | scales::d_minor[3]
            }
        };
        
        std::vector<instrument_source> instruments{{"i1", "sine", "seq1", 0.8f}};
        
        source src{ patches, instruments, seqs, 4.0f };
        
        s.load(src);
        
        std::cout << "press Enter to play\n";
        std::cin.get();
        
        s.set_debug_callback(print_debug);
        
        s.play();
        
        while (s.is_running())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

REGISTER_EXAMPLE(arpeggio);

