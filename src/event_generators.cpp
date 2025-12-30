#include "sequences.hpp"
#include "event_generators.hpp"

#include <algorithm>

namespace patch_magic
{

event_generator::event_generator(uint32_t sample_rate, const flow& f, event_id_generator& eidg):
    sample_rate_(sample_rate),
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
    return current_sample_ == total_samples_;
}

void event_generator::reset()
{
    rewind();
}

void event_generator::generate_timed_events(const flow& f)
{
    uint64_t progress = 0;
    float current_note_length = 1.0f;
    note_from_number current_base = nullptr;
        
    for (const auto& item : f)
    {
        uint64_t this_item_length_in_samples = std::visit(
            overloaded{
                [&](const sequence& seq){ return static_cast<uint64_t>(current_note_length * sample_rate_ * seq.get_items().size()); },
                [&](auto){ return uint64_t(0); }
            },
            item
        );
        
        std::visit(
            overloaded{
                [&](const sequence& seq){ generate_timed_events(seq, progress, this_item_length_in_samples, current_base); },
                [&](note_length_change change){ current_note_length = change.val_; },
                [&](base_change change){ current_base = change.base_; }
            },
            item
        );
        
        progress += this_item_length_in_samples;
    }
    
    total_samples_ = progress;
}

void event_generator::generate_timed_events(const sequence& s, uint64_t progress, uint64_t sequence_length_in_samples, note_from_number base)
{
    size_t i = 0;
    size_t n = s.get_items().size();
    
    for (const auto& wrapper : s.get_items())
    {
        uint64_t this_item_length_in_samples = sequence_length_in_samples / n;
        if (i < sequence_length_in_samples % n)
            this_item_length_in_samples++;
          
        std::visit(
            overloaded{
                [&](const sequence& seq) { generate_timed_events(seq, progress, this_item_length_in_samples, base); },
                [&](const note_placeholder& p) { generate_timed_events(p, progress, this_item_length_in_samples, base); },
                [&](const note& n) { generate_timed_events(n, progress, this_item_length_in_samples); },
                [](const pause&) { }
            },
            wrapper.wrapped_
        );
        
        progress += this_item_length_in_samples;
    }
}

void event_generator::generate_timed_events(const note_placeholder& p, uint64_t progress, uint64_t note_length_in_samples, note_from_number base)
{
    generate_timed_events(base ? base(p.degree_) : note{inaudible_frequency}, progress, note_length_in_samples);
}

void event_generator::generate_timed_events(const note& n, uint64_t progress, uint64_t note_length_in_samples)
{
    auto id = eidg_.generate();
    timed_events_.emplace_back(progress, event::on(id, n.freq_));
    timed_events_.emplace_back(progress + note_length_in_samples - 1, event::off(id));
}

void event_generator::prepare_events()
{
    if (next_event_it_ == timed_events_.end() || current_sample_ < next_event_it_->time_point_)
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

