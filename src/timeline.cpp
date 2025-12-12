#include "timeline.hpp"

namespace patch_magic
{
    
void timeline::note_on_at(sample_index when, note_id id, float freq)
{
    insert(timed_event{when, note_on{id, freq}});
}

void timeline::note_off_at(sample_index when, note_id id)
{
    insert(timed_event{when, note_off{id}});
}

void timeline::sound_on_at(sample_index when, note_id id)
{
    insert(timed_event{when, sound_on{id}});
}

void timeline::sound_off_at(sample_index when, note_id id)
{
    insert(timed_event{when, sound_off{id}});
}

void timeline::inc()
{
    ++current_sample_;
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
}

void timeline::insert(const timed_event& te)
{
    auto it = std::ranges::upper_bound(events_, te.when, std::less<>{}, &timed_event::when);
    events_.insert(it, te);
}

}
