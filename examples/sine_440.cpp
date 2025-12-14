#include <synth.hpp>
#include <iostream>

#include "main.h"

using namespace patch_magic;

int sine_440()
{
    try
    {
        std::cout << "=== 440 Hz sine wave ===\n";
        
        synth s(480000, 2);
        
        source src
        {
            {
                {sine, 0, source_const_f{440.0f}},
                {vol, 0, source_const_f{0.6f}, source_reg_f{0}}
            }
        };
        
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

REGISTER_EXAMPLE(sine_440);
