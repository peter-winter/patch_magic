#include <synth.hpp>
#include <iostream>

#include "main.h"

using namespace patch_magic;

int generators()
{
    try
    {
        synth s;

        std::cout << "=== 440 Hz sine wave ===\n";
        {
            program p;
            p.ops_ = {
                sine{ cnst_f{440.0f}, reg_f{0} },
                vol { cnst_f{0.7f}, reg_f{0}, out{0} },
                vol { cnst_f{0.7f}, reg_f{0}, out{1} }
            };
            s.load(p);
            s.play();  // blocks until Enter is pressed
        }

        std::cout << "\n===  Detuned stereo (440 Hz + 442 Hz) ===\n";
        {
            program p;
            p.ops_ = {
                sine{ cnst_f{440.0f}, reg_f{0} },
                sine{ cnst_f{442.0f}, reg_f{1} },
                vol { cnst_f{0.4f}, reg_f{0}, out{0} },
                vol { cnst_f{0.4f}, reg_f{1}, out{1} }
            };
            s.load(p);
            s.play();
        }

        std::cout << "\nAll examples finished.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

REGISTER_EXAMPLE(generators);
