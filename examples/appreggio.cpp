#include "main.h"

#include <notes.hpp>

#include <sequences.hpp>

using namespace patch_magic;

int appreggio()
{
    try
    {
        std::cout << "=== Appregio in D-Minor, overlapping notes ===\n";
        
        synth s;
        
        std::vector<patch_source> patches
        {
            {
                "sine",
                {
                    {base_freq, 0},
                    {sine, 0, reg_f_source{0}, const_f_source{0.3f}},
                    {env_ar, 1, const_f_source{0.5f}, const_f_source{0.8f}},  // Fast attack, longer release for natural overlap
                    {vol, 0, reg_f_source{1}, reg_f_source{0}}
                }
            }
        };
        
        std::vector<timeline_source> timelines
        {
            {
                "t1",
                {
                    // 40-note arpeggio in D minor
                    // Each note is turned ON, then explicitly turned OFF exactly 0.4 seconds later
                    // New note every 0.1 seconds → steady state of exactly 4 simultaneous notes
                    // Pattern uses notes from the D minor triad across octaves
                    
                    {0.0f,  note_on_source{1,  notes::D(3)}}, {0.4f,  note_off_source{1}},
                    {0.1f,  note_on_source{2,  notes::F(3)}}, {0.5f,  note_off_source{2}},
                    {0.2f,  note_on_source{3,  notes::A(3)}}, {0.6f,  note_off_source{3}},
                    {0.3f,  note_on_source{4,  notes::D(4)}}, {0.7f,  note_off_source{4}},
                    
                    {0.4f,  note_on_source{5,  notes::F(4)}}, {0.8f,  note_off_source{5}},
                    {0.5f,  note_on_source{6,  notes::A(4)}}, {0.9f,  note_off_source{6}},
                    {0.6f,  note_on_source{7,  notes::D(5)}}, {1.0f,  note_off_source{7}},
                    {0.7f,  note_on_source{8,  notes::F(4)}}, {1.1f,  note_off_source{8}},
                    
                    {0.8f,  note_on_source{9,  notes::A(3)}}, {1.2f,  note_off_source{9}},
                    {0.9f,  note_on_source{10, notes::D(4)}}, {1.3f,  note_off_source{10}},
                    {1.0f,  note_on_source{11, notes::F(4)}}, {1.4f,  note_off_source{11}},
                    {1.1f,  note_on_source{12, notes::A(4)}}, {1.5f,  note_off_source{12}},
                    
                    {1.2f,  note_on_source{13, notes::D(5)}}, {1.6f,  note_off_source{13}},
                    {1.3f,  note_on_source{14, notes::F(5)}}, {1.7f,  note_off_source{14}},
                    {1.4f,  note_on_source{15, notes::A(4)}}, {1.8f,  note_off_source{15}},
                    {1.5f,  note_on_source{16, notes::D(4)}}, {1.9f,  note_off_source{16}},
                    
                    {1.6f,  note_on_source{17, notes::F(3)}}, {2.0f,  note_off_source{17}},
                    {1.7f,  note_on_source{18, notes::A(3)}}, {2.1f,  note_off_source{18}},
                    {1.8f,  note_on_source{19, notes::D(4)}}, {2.2f,  note_off_source{19}},
                    {1.9f,  note_on_source{20, notes::F(4)}}, {2.3f,  note_off_source{20}},
                    
                    {2.0f,  note_on_source{21, notes::A(4)}}, {2.4f,  note_off_source{21}},
                    {2.1f,  note_on_source{22, notes::D(5)}}, {2.5f,  note_off_source{22}},
                    {2.2f,  note_on_source{23, notes::F(5)}}, {2.6f,  note_off_source{23}},
                    {2.3f,  note_on_source{24, notes::A(5)}}, {2.7f,  note_off_source{24}},
                    
                    {2.4f,  note_on_source{25, notes::D(5)}}, {2.8f,  note_off_source{25}},
                    {2.5f,  note_on_source{26, notes::F(4)}}, {2.9f,  note_off_source{26}},
                    {2.6f,  note_on_source{27, notes::A(4)}}, {3.0f,  note_off_source{27}},
                    {2.7f,  note_on_source{28, notes::D(4)}}, {3.1f,  note_off_source{28}},
                    
                    {2.8f,  note_on_source{29, notes::F(4)}}, {3.2f,  note_off_source{29}},
                    {2.9f,  note_on_source{30, notes::A(4)}}, {3.3f,  note_off_source{30}},
                    {3.0f,  note_on_source{31, notes::D(5)}}, {3.4f,  note_off_source{31}},
                    {3.1f,  note_on_source{32, notes::F(5)}}, {3.5f,  note_off_source{32}},
                    
                    {3.2f,  note_on_source{33, notes::A(5)}}, {3.6f,  note_off_source{33}},
                    {3.3f,  note_on_source{34, notes::D(5)}}, {3.7f,  note_off_source{34}},
                    {3.4f,  note_on_source{35, notes::F(4)}}, {3.8f,  note_off_source{35}},
                    {3.5f,  note_on_source{36, notes::A(4)}}, {3.9f,  note_off_source{36}},
                    
                    {3.6f,  note_on_source{37, notes::D(4)}}, {4.0f,  note_off_source{37}},
                    {3.7f,  note_on_source{38, notes::F(3)}}, {4.1f,  note_off_source{38}},
                    {3.8f,  note_on_source{39, notes::A(3)}}, {4.2f,  note_off_source{39}},
                    {3.9f,  note_on_source{40, notes::D(4)}}, {4.3f,  note_off_source{40}}
                },
                7.0f,   // Sufficient duration to cover all events and releases
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

    using namespace sequences;
    
    auto descr01 = _(_1);
    auto descr02 = _(_1, _2);
    auto descr03 = _(_1, _2, _(_3, _4));
    
    auto descr04 = _(_1 & _2);
    auto descr05 = _(_1 & _2 & _3);
    auto descr06 = _(_1 & (_2 & _3));
    auto descr07 = _((_1 & _2) & _3);
    auto descr08 = _(_1, _2) & _3;
    auto descr09 = _3 & _(_1, _2);
    auto descr10 = _(_1, _2) & (_3 & _4);
    auto descr11 = (_3 & _4) & _(_1, _2);
    auto descr12 = _(_1, _2) & (_3, _4);
    
    auto descr13 = _1 * 3;
    auto descr14 = (_1 & _2) * 3;
    auto descr15 = 3 * _1;
    auto descr16 = 3 * (_1 & _2);
    auto descr17 = _(_1, _2) * 4;
    auto descr18 = 4 * _(_1, _2);
    auto descr19 = _(_1, _2) * _4;
    auto descr20 = _4 * _(_1, _2);
    auto descr21 = _(_1, _2) * (_3 & _4);
    auto descr22 = (_3 & _4) * _(_1, _2);
    auto descr23 = _(_3, _4) * _(_1, _2);
    
    return 0;
}

REGISTER_EXAMPLE(appreggio);

