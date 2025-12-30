#include "main.h"

#include <notes.hpp>
#include <sequences.hpp>

using namespace patch_magic;
using namespace patch_magic::notes;

void envelope()
{
    std::cout << "=== Envelopes test, 0.5s attack, 5s release. Overlapping sounds ===\n";
    
    synth s;
    
    std::vector<patch_source> patches
    {
        {
            "sine440",
            {
                
                {base_freq, 0},
                {sine, 0, reg_f_source{0}, const_f_source{0.5f}},
                {env_ar, 1, const_f_source{0.5f}, const_f_source{5.0f}},
                {vol, 0, reg_f_source{1}, reg_f_source{0}}
            }
        }
    };
    
    auto seq = _(c(3), x, g(3), x, e(3), x);
    
    std::vector<flow_source> flows;
    flows.emplace_back("f", flow { seq });
    
    std::vector<instrument_source> instruments{{"i1", "sine440", "f"}};
    
    source src{ patches, instruments, flows };
    
    s.load(src);
    
    std::cout << "press Enter to play\n";
    std::cin.get();
    
    s.set_debug_callback(print_debug);
    s.play();
    poll_until_done(s);
}

REGISTER_EXAMPLE(envelope);
