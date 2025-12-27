#include "main.h"

#include <sequences.hpp>

using namespace patch_magic;

void arpeggio()
{
    std::cout << "=== Arpeggio in D-Minor, overlapping notes ===\n";
    
    synth s;
    
    std::vector<patch_source> patches;
    patches.emplace_back(
        "sine",
        ops_source
        {
            {base_freq, 0},
            {sine, 0, reg_f_source{0}, const_f_source{0.1f}},
            {env_ar, 1, const_f_source{0.1f}, const_f_source{0.1f}},
            {vol, 0, reg_f_source{1}, reg_f_source{0}}
        }
    );
    
    auto a = _(0, 0, 1, 0, 2, 1, x, _(3, 3), 2, x, 3, 3, 4, 3);
    auto b = a + _(2, 3);
    auto c = b + _(1, 2);
    auto d = c + _(0, 1);
    
    std::vector<flow_source> flows;
    flows.emplace_back(
        "f", 
        flow(0.25f)
            .rep(2)
                .alt()
                    .base(chords::a_minor[3])
                    .base(chords::d_minor[4])
                .tla()
                (a, b)
            .per()
    );
    
    std::vector<instrument_source> instruments{{"i1", "sine", "f"}};
    
    source src{ patches, instruments, flows };
    
    s.load(src);
    
    std::cout << "press Enter to play\n";
    std::cin.get();
    
    s.set_debug_callback(print_debug);
    s.play();
    poll_until_done(s);
}

REGISTER_EXAMPLE(arpeggio);

