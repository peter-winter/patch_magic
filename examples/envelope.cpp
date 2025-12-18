#include <synth.hpp>
#include <iostream>

#include "main.h"

using namespace patch_magic;

int envelope()
{
    try
    {
        std::cout << "=== Envelopes test ===\n";
        
        synth s;
        
        std::vector<patch_source> patches
        {
            {
                "sine440",
                {
                    {sine, 0, const_f_source{440.0f}, const_f_source{0.5f}},
                    {env_ar, 1, const_f_source{0.5f}, const_f_source{5.0f}},
                    {vol, 0, reg_f_source{1}, reg_f_source{0}}
                }
            }
        };
        
        std::vector<timeline_source> timelines
        {
            {
                "t1",
                {
                    {1.0f, sound_on_source{1}},
                    {3.0f, sound_off_source{1}},
                    {5.0f, sound_on_source{2}},
                    {7.0f, sound_off_source{2}},
                    {9.0f, sound_on_source{3}},
                    {11.0f, sound_off_source{3}}
                }
            }
        };
        
        std::vector<instrument_source> instruments{{"i1", "sine440", "t1"}};
        
        source src{ patches, instruments, timelines };
        
        s.load(src);
        
        std::cout << "press Enter to play and stop\n";
        std::cin.get();
        
        s.set_debug_callback(print_debug);
        
        s.play();
        std::cin.get();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

REGISTER_EXAMPLE(envelope);
