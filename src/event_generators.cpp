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

base_event_generator::base_event_generator(uint32_t sample_rate, float duration, float on_duration, freq_generator fg, event_id_generator& eidg):
        tt_(sample_rate, duration), done_(false), on_duration_(on_duration), fg_(fg), eidg_(eidg)
{}

bool base_event_generator::inc()
{
    if (tt_.inc())
        done_ = true;
    return done_;
}
        
simple_event_generator::simple_event_generator(uint32_t sample_rate, float duration, float on_duration, const simple& s, freq_generator fg, event_id_generator& eidg):
    base_event_generator(sample_rate, duration, on_duration, fg, eidg), s_(s), on_event_fired_(0), off_event_fired_(false)
{}

void simple_event_generator::inc()
{
    if (base_event_generator::inc())
    {
        on_event_fired_ = 0;
        off_event_fired_ = false;
    }
}

void simple_event_generator::generate_events(events& evs)
{
    if (on_event_fired_ == 0)
    {
        uint32_t id = eidg_.generate();
        evs.push_back(event(event_type::note_on, id, fg_(s_.n_)));
        on_event_fired_ = id;
    }
    else if (!off_event_fired_ && (tt_.time_ / tt_.duration_ > on_duration_) && (tt_.time_ < tt_.duration_))
    {
        evs.push_back(event(event_type::note_off, on_event_fired_));
        off_event_fired_ = true;
    }
}
    
parallel_event_generator::parallel_event_generator(uint32_t sample_rate, float duration, float on_duration, const parallel& p, freq_generator fg, event_id_generator& eidg):
    base_event_generator(sample_rate, duration, on_duration, fg, eidg), p_(p), off_events_fired_(false)
{
    on_events_fired_.reserve(p.items_.size());
}

void parallel_event_generator::inc()
{
    if (base_event_generator::inc())
    {
        on_events_fired_.clear();
        off_events_fired_ = false;
    }
}

void parallel_event_generator::generate_events(events& evs)
{
    if (on_events_fired_.empty())
    {
        for (const auto& s : p_.items_)
        {
            uint32_t id = eidg_.generate();
            evs.push_back(event(event_type::note_on, id, fg_(s.n_)));
            on_events_fired_.push_back(id);
        }
    }
    else if (!off_events_fired_ && (tt_.time_ / tt_.duration_ > on_duration_) && (tt_.time_ < tt_.duration_))
    {
        for (uint32_t on : on_events_fired_)
        {
            evs.push_back(event(event_type::note_off, on));
            off_events_fired_ = true;
        }
    }
}
    
seq_event_generator::seq_event_generator(uint32_t sample_rate, float duration, float on_duration, const sequence& seq, freq_generator fg, event_id_generator& eidg):
    item_tt_(sample_rate, duration / seq.size()), current_item_idx_(0), gens_(create_item_generators(sample_rate, duration, on_duration, seq, fg, eidg))
{}

void seq_event_generator::generate_events(events& es)
{
    std::visit(
        [&](auto& gen){ gen.generate_events(es); },
        gens_[current_item_idx_].gen_
    );
}

void seq_event_generator::inc()
{
    bool inc_res = item_tt_.inc();
    
    std::visit(
        [](auto& gen){ return gen.inc(); },
        gens_[current_item_idx_].gen_
    );
    
    if (inc_res)
    {
        current_item_idx_++;
        
        if (current_item_idx_ == gens_.size())
            current_item_idx_ = 0;
    }
}

bool seq_event_generator::done() const
{
    return std::visit(
        [](const auto& gen){ return gen.done(); },
        gens_.back().gen_
    );
}

item_event_generators seq_event_generator::create_item_generators(uint32_t sample_rate, float duration, float on_duration, const sequence& seq, freq_generator fg, event_id_generator& eidg)
{
    item_event_generators ret;
    for (const auto& it : seq)
        ret.emplace_back(create_item_generator(sample_rate, duration / seq.size(), on_duration, it.i_, fg, eidg));
    return ret;
}

item_event_generator_wrapper seq_event_generator::create_item_generator(uint32_t sample_rate, float item_duration, float on_duration, const item& it, freq_generator fg, event_id_generator& eidg)
{
    return item_event_generator_wrapper{
        std::visit(
            [&]<typename T>(const T& i) { return item_event_generator(std::in_place_type<event_generator_t<T>>, sample_rate, item_duration, on_duration, i, fg, eidg); },
            it
        )
    };
}

}

