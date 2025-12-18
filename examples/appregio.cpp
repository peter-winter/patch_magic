#include "main.h"

#include <notes.hpp>

using namespace patch_magic;

int appregio()
{
    try
    {
        std::cout << "=== Appregio, lots of simultaneous sounds ===\n";
        
        synth s;
        
        std::vector<patch_source> patches
        {
            {
                "sine",
                {
                    {base_freq, 0},
                    {sine, 0, reg_f_source{0}, const_f_source{0.5f}},
                    {env_ar, 1, const_f_source{0.3f}, const_f_source{0.3f}},
                    {vol, 0, reg_f_source{1}, reg_f_source{0}}
                }
            }
        };
        
        std::vector<timeline_source> timelines
        {
            {
                "t1",
                {
                    {1.0f, note_on_source{1, notes::C(4)}},
                    {2.0f, note_off_source{1}},
                    {2.5f, note_on_source{2, notes::D(4)}},
                    {3.5f, note_off_source{2}},
                    {4.0f, note_on_source{3, notes::E(4)}},
                    {5.0f, note_off_source{3}}
                },
                7.0f,
                false
            }
        };
        
        std::vector<instrument_source> instruments{{"i1", "sine", "t1"}};
        
        source src{ patches, instruments, timelines };
        
        s.load(src);
        
        std::cout << "press Enter to play\n";
        std::cin.get();
        
        s.set_debug_callback(print_debug);
        
        s.play();
        
        while (s.is_running())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

REGISTER_EXAMPLE(appregio);
