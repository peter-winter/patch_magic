#include "main.h"

using namespace patch_magic;

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
    
    std::vector<flow_source> flows;
    flows.emplace_back("note", flow{ note_len(10.0f), _(1) });
    
    std::vector<instrument_source> instruments{{"i1", "sine440", "note"}, {"i2", "sine220", "note"}};
    
    source src{ patches, instruments, flows };
    
    s.load(src);
    
    std::cout << "Press Enter to play\n";
    std::cin.get();
    
    s.set_debug_callback(print_debug);
    s.play();
    poll_until_done(s);
}

REGISTER_EXAMPLE(sine_440_220_mixed);
