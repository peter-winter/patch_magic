#pragma once

#include <cstdint>
#include <variant>
#include <vector>
#include <ranges>
#include <algorithm>

namespace patch_magic
{
    
using sample_index = uint64_t;
using note_id      = int32_t;

struct note_on
{
    note_id id_; 
    float f_;
};

struct note_off
{
    note_id id_;
};

struct sound_on
{
    note_id id_;
};

struct sound_off
{
    note_id id_;
};

using event = std::variant<note_on, note_off, sound_on, sound_off>;

class timeline
{
public:
    timeline() = default;
    timeline(timeline&&) = default;
    timeline(const timeline&) = delete;
    timeline& operator = (const timeline&) = delete;
    
    struct timed_event
    {
        sample_index when;
        event        payload;
    };

    void note_on_at(sample_index when, note_id id, float freq);
    void note_off_at(sample_index when, note_id id);
    void sound_on_at(sample_index when, note_id id);
    void sound_off_at(sample_index when, note_id id);
    
    using events_view = std::ranges::subrange<std::vector<timed_event>::const_iterator>;
        
    void inc();
    
    void prepare_events();
    events_view get_events() const;

    void reset();
    
    sample_index current_sample() const { return current_sample_; }
    
private:
    void insert(const timed_event& te);
    
    std::vector<timed_event> events_;
    sample_index current_sample_{0};
    std::vector<timed_event>::const_iterator next_event_it_{events_.begin()};
    events_view current_events_;
};

}
