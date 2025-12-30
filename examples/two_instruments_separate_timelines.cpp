#include "main.h"

using namespace patch_magic;

void two_instruments_separate_timelines()
{
    std::cout << "=== Two instruments on separate timelines ===\n";
    std::cout << "Bass (220 Hz): every 2 seconds, medium-sharp envelope\n";
    std::cout << "Treble (440 Hz): every 1 second, sharper envelope\n";
    
    synth s;

    std::vector<patch_source> patches
    {
        // Bass patch: 220 Hz, medium-sharp AR envelope
        {
            "bass_patch",
            {
                {sine, 0, const_f_source{220.0f}, const_f_source{0.5f}},
                {env_ar, 1, const_f_source{0.05f}, const_f_source{0.3f}},  // att 50 ms, rel 300 ms
                {vol, 0, reg_f_source{1}, reg_f_source{0}}
            }
        },
        // Treble patch: 440 Hz, sharper AR envelope
        {
            "treble_patch",
            {
                {sine, 0, const_f_source{440.0f}, const_f_source{0.5f}},
                {env_ar, 1, const_f_source{0.01f}, const_f_source{0.15f}}, // att 10 ms, rel 150 ms
                {vol, 0, reg_f_source{1}, reg_f_source{0}}
            }
        }
    };

    std::vector<flow_source> flows;
    flows.emplace_back("bass_tl", flow{ note_len(2.0f) , _(1, x) });
    flows.emplace_back("treble_tl", flow{ _(1, x, 1, x) });

    std::vector<instrument_source> instruments
    {
        {"bass_instr", "bass_patch", "bass_tl"},
        {"treble_instr","treble_patch", "treble_tl"}
    };

    source src{ patches, instruments, flows };
    s.load(src);
    
    std::cout << "Press Enter to play\n";
    std::cin.get();
    
    s.set_debug_callback(print_debug);
    s.loop();
    poll_until_done(s);
}

REGISTER_EXAMPLE(two_instruments_separate_timelines);

