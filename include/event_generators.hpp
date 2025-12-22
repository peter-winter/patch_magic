#pragma once

#include <variant>
#include <vector>
#include <cstdint>

#include "sequences.hpp"
#include "event_id_generator.hpp"

namespace patch_magic
{

enum class event_type { sound_on, sound_off, note_on, note_off };

struct event
{
    event(event_type type, uint32_t id, float freq = inaudible_freq)
        : type_(type), id_(id), freq_(freq)
    {}
    
    event_type type_;
    uint32_t id_;
    float freq_;
};

using events = std::vector<event>;

struct time_tracker
{
    time_tracker(uint32_t sample_rate, float duration):
        sample_rate_(sample_rate), duration_(duration), time_(0.0f)
    {}
    
    bool inc();
    
    uint32_t sample_rate_;
    float duration_;
    float time_;
};

class note_event_generator
{
public:
    note_event_generator(uint32_t sample_rate, float duration, const sequences::seq_item_note& descr, event_id_generator& eidg);
    
    void generate_events(events& es, float on_duration);
    void inc();
    bool done() const;
    size_t get_max_simultaneous_events_count() const;
    void reset();

private:
    
    time_tracker item_tt_;
};

class sound_event_generator
{
public:
    sound_event_generator(uint32_t sample_rate, float duration, const sequences::seq_item_tick& descr, event_id_generator& eidg);
    
    void generate_events(events& es, float on_duration);
    void inc();
    bool done() const;
    size_t get_max_simultaneous_events_count() const;
    void reset();
    
private:
    time_tracker item_tt_;
};


using event_generator = std::variant<note_event_generator, sound_event_generator>;

}


