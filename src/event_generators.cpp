#include "sequences.hpp"
#include "event_generators.hpp"

#include <algorithm>

namespace patch_magic
{

event_generator::event_generator(uint32_t sample_rate, const flow& f, event_id_generator& eidg):
    sample_rate_(sample_rate), 
    duration_in_samples_(f.total_duration() * sample_rate_),
    current_sample_(0),
    eidg_(eidg)
{
    generate_timed_events(f);
    std::ranges::sort(timed_events_, {}, &timed_event::time_point_);
    
    reset();
}

void event_generator::inc()
{
    current_sample_++;
}

bool event_generator::done() const
{
    return current_sample_ >= duration_in_samples_;
}

void event_generator::reset()
{
    rewind();
}

void event_generator::generate_timed_events(const flow& f)
{
    uint64_t progress = 0;
    for (const auto& seq : f.get_sequences())
        progress += generate_timed_events(seq, progress);
}

uint64_t event_generator::generate_timed_events(const sequence& s, uint64_t progress)
{
    uint64_t note_length_in_samples = s.note_length() * sample_rate_;
    
    for (const auto& wrapper : s.get_items())
    {
        progress += std::visit(
            overloaded{
                [this, progress, note_length_in_samples](const auto& item) { return generate_timed_events(item, progress, note_length_in_samples); },
                [note_length_in_samples](const pause&) { return note_length_in_samples; }
            },
            wrapper.item_
        );
    }
    return note_length_in_samples * s.get_items().size();
}

uint64_t event_generator::generate_timed_events(const subsequence& sub, uint64_t progress, uint64_t note_length_in_samples)
{
    size_t i = 0;
    size_t n = sub.get_items().size();
    for (const auto& wrapper : sub.get_items())
    {
        uint64_t this_note_length_in_samples = note_length_in_samples / n;
        if (i < note_length_in_samples % n)
            this_note_length_in_samples++;
            
        progress += std::visit(
            overloaded{
                [this, progress, this_note_length_in_samples](const auto& item) { return generate_timed_events(item, progress, this_note_length_in_samples); },
                [this_note_length_in_samples](const pause&) { return this_note_length_in_samples; },
                [this_note_length_in_samples](const note_placeholder&) { return this_note_length_in_samples; }
            },
            wrapper.item_
        );
        i++;
    }
    return note_length_in_samples;
}

uint64_t event_generator::generate_timed_events(const note& n, uint64_t progress, uint64_t note_length_in_samples)
{
    auto id = eidg_.generate();
    timed_events_.emplace_back(progress, event::on(id, n.freq_));
    timed_events_.emplace_back(progress + note_length_in_samples - 1, event::off(id));
    return note_length_in_samples;
}

void event_generator::prepare_events()
{
    if (current_sample_ < next_event_it_->time_point_ || next_event_it_ == timed_events_.end())
    {
        current_event_view_ = event_view{timed_events_.end(), timed_events_.end()};
        return;
    }
            
    auto upper = std::ranges::upper_bound(next_event_it_, timed_events_.end(), current_sample_, {}, &timed_event::time_point_);

    current_event_view_ = event_view{next_event_it_, upper};
    next_event_it_ = upper;
}

const event_generator::event_view event_generator::get_current_events() const
{
    return current_event_view_;
}

void event_generator::rewind()
{
    current_sample_ = 0;
    next_event_it_ = timed_events_.begin();
}

}

