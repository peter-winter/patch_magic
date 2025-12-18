#include "main.h"

using namespace patch_magic;

int sine_440_220_mixed()
{
    try
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
        
        std::vector<timeline_source> timelines
        {
            {
                "t1",
                {
                    {1.0f, sound_on_source{1}}
                },
                10.0f,  // duration
                false   // looping
            }
        };
        
        std::vector<instrument_source> instruments{{"i1", "sine440", "t1"}, {"i2", "sine220", "t1"}};
        
        source src{ patches, instruments, timelines };
        
        s.load(src);
        
        std::cout << "Press Enter to play\n";
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

REGISTER_EXAMPLE(sine_440_220_mixed);
