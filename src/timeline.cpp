#include "timeline.hpp"

namespace patch_magic
{
    
timeline::timeline(sample_index duration_samples, bool looping):
    duration_samples_(duration_samples), looping_(looping), active_(true)
{}

void timeline::note_on_at(sample_index when, uint32_t id, float freq)
{
    insert(timed_event{when, event(event_type::note_on, id, freq)});
}

void timeline::note_off_at(sample_index when, uint32_t id)
{
    insert(timed_event{when, event(event_type::note_off, id)});
}

void timeline::sound_on_at(sample_index when, uint32_t id)
{
    insert(timed_event{when, event(event_type::sound_on, id)});
}

void timeline::sound_off_at(sample_index when, uint32_t id)
{
    insert(timed_event{when, event(event_type::sound_off, id)});
}

void timeline::inc()
{
    ++current_sample_;

    if (current_sample_ >= duration_samples_)
    {
        if (looping_)
        {
            current_sample_ = 0;
            next_event_it_ = events_.begin();
        }
        else
        {
            active_ = false;
        }
    }
}

void timeline::prepare_events()
{
    if (next_event_it_ == events_.end() || current_sample_ < next_event_it_->when)
    {
        current_events_ = events_view{events_.end(), events_.end()};
        return;
    }
            
    auto upper = std::ranges::upper_bound(
        next_event_it_, events_.end(), current_sample_,
        std::less<>{}, &timed_event::when);

    current_events_ = events_view{next_event_it_, upper};
    next_event_it_ = upper;
}

timeline::events_view timeline::get_events() const
{
    return current_events_;
}

void timeline::reset()
{
    current_sample_ = 0;
    next_event_it_ = events_.begin();
    active_ = true;
}

void timeline::insert(const timed_event& te)
{
    auto it = std::ranges::upper_bound(events_, te.when, std::less<>{}, &timed_event::when);
    events_.insert(it, te);
}

}
