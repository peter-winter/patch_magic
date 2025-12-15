#include <synth.hpp>
#include <iostream>

#include "main.h"

using namespace patch_magic;

int sine_440_220_mixed()
{
    try
    {
        std::cout << "=== 440 and 220 Hz sine waves mixed ===\n";
        
        synth s;
        
        std::vector<patch_source> patches
        {
            {
                "sine440",
                {
                    {sine, 0, const_f_source{440.0f}},
                    {vol, 0, const_f_source{0.6f}, reg_f_source{0}}
                }
            },
            {
                "sine220",
                {
                    {sine, 0, const_f_source{220.0f}},
                    {vol, 0, const_f_source{0.6f}, reg_f_source{0}}
                }
            }
        };
        
        std::vector<instrument_source> instruments{{"i1", "sine440"}, {"i2", "sine220"}};
        
        source src{ patches, instruments };
        
        s.load(src);
        
        std::cout << "press Enter to play...\n";
        std::cin.get();
        
        s.play();
        
        std::cout << "press Enter to stop...\n";
        std::cin.get();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

REGISTER_EXAMPLE(sine_440_220_mixed);
