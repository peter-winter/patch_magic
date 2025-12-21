#pragma once

#include <variant>
#include <vector>
#include <cstdint>

#include "sequences.hpp"

namespace patch_magic
{

using namespace sequences;
    
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

class base_event_generator
{
public:
    base_event_generator(uint32_t sample_rate, float duration, event_id_generator& eidg);
    
    bool inc();
    bool done() const { return done_; }
    
protected:
    time_tracker tt_;
    bool done_;
    event_id_generator& eidg_;
};

class simple_event_generator : public base_event_generator
{
public:
    simple_event_generator(uint32_t sample_rate, float duration, const simple_note& s, event_id_generator& eidg);
    
    void inc();
    void generate_events(events& es, float on_duration);
    
    size_t get_max_simultaneous_events_count() const { return 1; }
    
private:
    float freq_;
    uint32_t on_event_fired_;
    bool off_event_fired_;
};

class parallel_event_generator : public base_event_generator
{
public:
    parallel_event_generator(uint32_t sample_rate, float duration, const parallel_note& p, event_id_generator& eidg);
    
    void inc();
    void generate_events(events& es, float on_duration);
    
    size_t get_max_simultaneous_events_count() const { return ns_.size(); }
    
private:
    std::vector<float> freqs_;
    std::vector<uint32_t> on_events_fired_;
    bool off_events_fired_;
};

struct item_event_generator_wrapper;

using item_event_generators = std::vector<item_event_generator_wrapper>;

class seq_event_generator
{
public:
    seq_event_generator(uint32_t sample_rate, float duration, const note_sequence& seq event_id_generator& eidg);
    
    void generate_events(events& es, float on_duration);
    void inc();
    bool done() const;
    size_t get_max_simultaneous_events_count() const;

private:
    item_event_generators create_item_generators(uint32_t sample_rate, float duration, const note_sequence& seq, event_id_generator& eidg);    
    item_event_generator_wrapper create_item_generator(uint32_t sample_rate, float duration, const item& it, event_id_generator& eidg);
    
    time_tracker item_tt_;
    size_t current_item_idx_;
    item_event_generators gens_;
};

using item_event_generator = std::variant<simple_event_generator, parallel_event_generator, seq_event_generator>;

struct item_event_generator_wrapper
{
    item_event_generator gen_;
};

template<typename T>
struct event_generator
{};

template<>
struct event_generator<simple> { using type = simple_event_generator; };

template<>
struct event_generator<parallel> { using type = parallel_event_generator; };

template<>
struct event_generator<sequence> { using type = seq_event_generator; };

template<typename T>
using event_generator_t = typename event_generator<T>::type;

}


