#include "sequences.hpp"
#include "event_generators.hpp"

namespace patch_magic
{
    
using namespace sequences;

bool time_tracker::inc()
{
    time_ += (1.0 / sample_rate_);
    
    if (time_ >= duration_)
    {
        time_ -= duration_;
        return true;
    }
    return false;
}

note_event_generator::note_event_generator(uint32_t sample_rate, float duration, const seq_item_note& descr, event_id_generator& eidg):
    item_tt_(sample_rate, duration)
{
    reset();
}

void note_event_generator::generate_events(events& es, float on_duration)
{
    
}

void note_event_generator::inc()
{
    //bool inc_res = item_tt_.inc();
}

bool note_event_generator::done() const
{
    return false;
}

size_t note_event_generator::get_max_simultaneous_events_count() const
{
    return 0;
}

void note_event_generator::reset()
{
}

sound_event_generator::sound_event_generator(uint32_t sample_rate, float duration, const seq_item_tick& descr, event_id_generator& eidg):
    item_tt_(sample_rate, duration)
{
    reset();
}

void sound_event_generator::generate_events(events& es, float on_duration)
{
    
}

void sound_event_generator::inc()
{
    //bool inc_res = item_tt_.inc();
}

bool sound_event_generator::done() const
{
    return false;
}

size_t sound_event_generator::get_max_simultaneous_events_count() const
{
    return 0;
}

void sound_event_generator::reset()
{
}

}

