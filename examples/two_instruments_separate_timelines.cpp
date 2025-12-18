#include <synth.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#include "main.h"

using namespace patch_magic;

int two_instruments_separate_timelines()
{
    try
    {
        std::cout << "=== Two instruments on separate timelines ===\n";
        std::cout << "Bass (220 Hz): every 2 seconds, medium-sharp envelope\n";
        std::cout << "Treble (440 Hz): every 1 second, sharper envelope\n";
        std::cout << "Total duration: 10 seconds (Ctrl+C to stop early)\n\n";

        synth s;

        std::vector<patch_source> patches
        {
            // Bass patch: 220 Hz, medium-sharp AR envelope
            {
                "bass_patch",
                {
                    {sine, 0, const_f_source{220.0f}, const_f_source{0.5f}},
                    {env_ar, 1, const_f_source{0.05f}, const_f_source{0.3f}},  // att 50 ms, rel 300 ms
                    {vol, 0, reg_f_source{1}, reg_f_source{0}}             // envelope → volume
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

        std::vector<timeline_source> timelines
        {
            // Bass timeline: note on every 2 seconds
            {
                "bass_tl",
                {
                    {0.0f, sound_on_source{1}},
                    {1.0f, sound_off_source{1}}
                },
                2.0f,   // duration
                true    // looping
            },
            
            // Treble timeline: note on every 1 second (twice as often)
            {
                "treble_tl",
                {
                    {0.0f, sound_on_source{1}},
                    {0.5f, sound_off_source{1}}
                },
                1.0f,
                true
            }
        };

        std::vector<instrument_source> instruments
        {
            {"bass_instr",  "bass_patch",   "bass_tl"},
            {"treble_instr","treble_patch", "treble_tl"}
        };

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

REGISTER_EXAMPLE(two_instruments_separate_timelines);

