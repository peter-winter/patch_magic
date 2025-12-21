#include "instrument.hpp"
#include "polyphony.hpp"

#include <numeric>
#include <sstream>
#include <iomanip>

namespace patch_magic
{

instrument::instrument(std::string name, max_voice_count, size_t reg_count, const patch& p, const seq_event_generator& gen, float on_duration):
    name_(name),
    max_voice_count_(max_voice_count), reg_count_(reg_count), p_(p),
    read_order_idx_(0), write_order_idx_(1), audible_count_(0), free_count_(max_voice_count),
    unordered_samples_total_(0),
    gen_(gen),
    on_duration_(on_duration)
{
    for (size_t i = 0; i < max_voice_count_; ++i)
        voice_slots_.emplace_back(reg_count_, p_);
    
    order_[0].resize(max_voice_count_);
    order_[1].resize(max_voice_count_);
    free_.resize(max_voice_count_);
    
    events_.reserve(gen_.get_max_simultaneous_events_count());
    
    reset();
    
    display_line_.reserve(1 << 10);
}

void instrument::reset()
{        
    for (auto& vs : voice_slots_)
        vs.reset();
        
    read_order_idx_ = 0;
    write_order_idx_ = 1;
    audible_count_ = 0;
    free_count_ = max_voice_count_;
    
    events_.clear();
    
    std::iota(free_.begin(), free_.end(), 0);
}

void instrument::do_on(uint32_t id, float freq)
{
    size_t idx = allocate_voice();
    auto& slot = voice_slots_[idx];
    
    slot.set_state(voice_slot_state::active);
    slot.set_base_freq(freq);
    slot.set_note_id(id);
}

void instrument::do_off(uint32_t id)
{
    auto& order = order_[read_order_idx_];
        
    for (size_t i = 0; i < audible_count_; ++i)
    {
        size_t idx = order[i];
        auto& slot = voice_slots_[idx];
        if (slot.note_id() == id)
        {
            slot.set_state(voice_slot_state::releasing);
            return;
        }
    }
}
    
void instrument::process_events()
{
    gen_.generate_events(events_, on_duration_);
    
    for (const auto& e : events_)
    {
        switch (e.type_)
        {
            case event_type::note_on:
            case event_type::sound_on:
                do_on(e.id_, ev.freq_);
                break;
            case event_type::note_off:
            case event_type::sound_off:
                do_off(e.id_);
                break;
        };
    }
    events_.clear();
}

size_t instrument::allocate_voice()
{
    size_t result;
    auto& order = order_[read_order_idx_];
    if (free_count_ > 0)
    {
        result = free_[--free_count_];
        order[audible_count_++] = result;
    }
    else
        result = order[max_voice_count_ - 1];
    return result;
}

float instrument::sample()
{
    process_events();
        
    float sum = 0.0f;
    
    const auto& read_order = order_[read_order_idx_];
    auto& write_order = order_[write_order_idx_];
    size_t write_idx = 0;
    
    for (size_t i = 0; i < audible_count_; ++i)
    {
        size_t slot_idx = read_order[i];
        voice_slot& slot = voice_slots_[slot_idx];
        
        sum += slot.sample();
        
        if (slot.state() == voice_slot_state::active || slot.smoothed_power() > inaudible_amplitude)
        {
            if (write_idx != 0)
            {
                size_t prev_slot_idx = write_order[write_idx - 1];
                float prev_score = voice_slots_[prev_slot_idx].score();
            
                if (prev_score < slot.score())
                {
                    write_order[write_idx - 1] = slot_idx;
                    write_order[write_idx] = prev_slot_idx;
                    unordered_samples_total_++;
                }
                else
                    write_order[write_idx] = slot_idx;
            }
            else
                write_order[write_idx] = slot_idx;
            write_idx++;
        }
        else
        {
            free_[free_count_++] = slot_idx;
            slot.set_state(voice_slot_state::free);
        }
    }

    audible_count_ = write_idx;
    std::swap(read_order_idx_, write_order_idx_);
    
    return sum;
}

const std::string& instrument::get_display_line()
{
    display_line_.clear();

    std::ostringstream oss;
    oss << "Instrument: " << name_ << " |";

    for (const auto& s : voice_slots_)
    {
        switch (s.state())
        {
            case voice_slot_state::active:
                oss << "■";
                break;
            case voice_slot_state::free:
                oss << " ";
                break;
            case voice_slot_state::releasing:
                oss << "●";
                break;
        }
    }

    oss << "| "
        << std::setw(4) << std::setfill('0') << audible_count_
        << "/"
        << std::setw(4) << std::setfill('0') << max_voice_count_
        << " aud, "
        << std::setw(4) << std::setfill('0') << free_count_
        << " free |"
        << " unordered: "
        << std::setw(8) << std::setfill('0') << unordered_samples_total_
        << " |";

    display_line_ = oss.str();

    return display_line_;
}

}

