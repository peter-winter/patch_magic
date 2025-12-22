#include "main.h"

using namespace patch_magic;
using namespace patch_magic::sequences;

void sine_440_220_mixed()
{
    std::cout << "=== 440 and 220 Hz sine waves mixed, 10 seconds playback ===\n";
    
    synth s;
    
    std::vector<patch_source> patches
    {
        {
            "sine440",
            {
                {sine, 0, const_f_source{440.0f}, const_f_source{0.5f}},
                {vol, 0, const_f_source{0.6f}, reg_f_source{0}}
            }
        },
        {
            "sine220",
            {
                {sine, 0, const_f_source{220.0f}, const_f_source{0.5f}},
                {vol, 0, const_f_source{0.6f}, reg_f_source{0}}
            }
        }
    };
    
    std::vector<sequence_source> seqs
    {
        {
            "seq", _s(o)
        }
    };
    
    std::vector<instrument_source> instruments{{"i1", "sine440", "seq", 0.95f}, {"i2", "sine220", "seq", 1.0f}};
    
    source src{ patches, instruments, seqs, 10.0f };
    
    s.load(src);
    
    std::cout << "Press Enter to play\n";
    std::cin.get();
    
    s.set_debug_callback(print_debug);
    s.play();
    poll_until_done(s);
}

REGISTER_EXAMPLE(sine_440_220_mixed);
