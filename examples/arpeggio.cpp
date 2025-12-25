#include "main.h"

#include <sequences.hpp>

using namespace patch_magic;

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
                {sine, 0, reg_f_source{0}, const_f_source{0.1f}},
                {env_ar, 1, const_f_source{0.1f}, const_f_source{0.1f}},
                {vol, 0, reg_f_source{1}, reg_f_source{0}}
            }
        }
    };
    
    std::vector<flow_source> flows
    {
        { "f", flow(chords::d_minor[3], 0.25f)(0, 0, 1, 0, 2, 1, x, _(3, 3), 2, x, 3, 3, 4, 3)(0, 0, 1, 0, 2, 1, x, _(3, 3), 2, x, 3, 3, 4, 3, 2, 3) }
    };
    
    std::vector<instrument_source> instruments{{"i1", "sine", "f"}};
    
    source src{ patches, instruments, flows };
    
    s.load(src);
    
    std::cout << "press Enter to play\n";
    std::cin.get();
    
    s.set_debug_callback(print_debug);
    s.loop();
    poll_until_done(s);
}

REGISTER_EXAMPLE(arpeggio);

