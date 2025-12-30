#pragma once

#include <variant>
#include <vector>
#include <cstdint>
#include <ranges>

#include "sequences.hpp"
#include "event_id_generator.hpp"

namespace patch_magic
{

enum class event_type { note_on, note_off };

struct event
{
    event(event_type type, uint32_t id, float freq = inaudible_freq)
        : type_(type), id_(id), freq_(freq)
    {}
    
    static event off(uint32_t id) { return event(event_type::note_off, id); }
    static event on(uint32_t id, float freq) { return event(event_type::note_on, id, freq); }
    
    event_type type_;
    uint32_t id_;
    float freq_;
};

struct timed_event
{
    uint64_t time_point_;
    event e_;
};

using timed_events = std::vector<timed_event>;

class event_generator
{
public:
    event_generator(uint32_t sample_rate, const flow& f, event_id_generator& eidg);
    
    using event_view = std::ranges::subrange<std::vector<timed_event>::const_iterator>;
    
    void inc();
    bool done() const;
    const event_view get_current_events() const;
    void reset();
    void prepare_events();
    void rewind();
    
private:    
    void generate_timed_events(const flow& f);
    void generate_timed_events(const sequence& s, uint64_t progress, uint64_t sequence_length_in_samples, note_from_number base);
    void generate_timed_events(const note_placeholder& p, uint64_t progress, uint64_t note_length_in_samples, note_from_number base);
    void generate_timed_events(const note& n, uint64_t progress, uint64_t note_length_in_samples);
        
    uint32_t sample_rate_;
    uint64_t current_sample_;
    event_id_generator& eidg_;
    timed_events timed_events_;
    std::vector<timed_event>::const_iterator next_event_it_;
    event_view current_event_view_;
    uint64_t total_samples_;
};


}


