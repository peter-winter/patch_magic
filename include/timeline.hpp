#pragma once

#include <cstdint>
#include <variant>
#include <vector>
#include <ranges>
#include <algorithm>

#include "math.hpp"

namespace patch_magic
{
    
using sample_index = uint64_t;

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

class timeline
{
public:
    timeline(sample_index duration_samples, bool looping);
    
    timeline(timeline&&) = default;
    timeline(const timeline&) = delete;
    timeline& operator = (const timeline&) = delete;
    
    bool active() const { return active_; }
    bool looping() const { return looping_; }
    
    struct timed_event
    {
        sample_index when;
        event        payload;
    };

    void note_on_at(sample_index when, uint32_t id, float freq);
    void note_off_at(sample_index when, uint32_t id);
    void sound_on_at(sample_index when, uint32_t id);
    void sound_off_at(sample_index when, uint32_t id);
    
    using events_view = std::ranges::subrange<std::vector<timed_event>::const_iterator>;
        
    void inc();
    
    void prepare_events();
    events_view get_events() const;

    void reset();
    
    sample_index current_sample() const { return current_sample_; }
    
private:
    void insert(const timed_event& te);
    
    sample_index duration_samples_;
    bool looping_{false};
    bool active_{true};
    
    std::vector<timed_event> events_;
    sample_index current_sample_{0};
    std::vector<timed_event>::const_iterator next_event_it_{events_.begin()};
    events_view current_events_;
};

}
