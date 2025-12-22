#include "runtime.hpp"
#include "polyphony.hpp"
#include "event_id_generator.hpp"

namespace patch_magic
{

runtime::runtime(size_t max_voice_count_per_instrument, uint32_t sample_rate, size_t channel_count, size_t reg_count_per_voice):
    top_sequence_duration_(default_top_sequence_duration),
    max_voice_count_per_instrument_(max_voice_count_per_instrument),
    sample_rate_(sample_rate),
    channel_count_(channel_count),
    reg_count_per_voice_(reg_count_per_voice),
    debug_callback_(nullptr)
{
    reset();
    debug_str_.reserve(1 << 16);
}

void runtime::reset()
{
    for (auto& instr : instruments_)
        instr.reset();
    for (auto& gen : event_generators_)
        std::visit([](auto& g) { g.reset(); }, gen);
}

bool runtime::done() const
{
    for (const auto& gen : event_generators_)
    {
        if (std::visit([](auto& g) { return g.done(); }, gen))
            return false;
    }
    return true;
}

void runtime::sample(float* data, size_t channel_count)
{
    float sum = 0.0f;
        
    for (auto& instr : instruments_)
        sum += instr.sample();
        
    float gain = polyphony_gain(instruments_.size(), polyphony_scale::equal_amplitude);
    float sample = sum * gain;
     
    for (size_t i = 0; i < channel_count; ++i)
        data[i] = sample;
        
    for (auto& gen : event_generators_)
        std::visit([](auto& g) { g.inc(); }, gen);
        
    debug();
}

size_t runtime::add_patch()
{
    patches_.emplace_back(sample_rate_); 
    return patches_.size() - 1;
}

const patch& runtime::get_patch(size_t idx) const
{
    return patches_[idx];
}

patch& runtime::get_patch(size_t idx)
{
    return patches_[idx];
}

size_t runtime::add_event_generator(const sequences::seq_item_note& note_seq_descr)
{
    event_generators_.emplace_back(std::in_place_type<note_event_generator>, sample_rate_, top_sequence_duration_, note_seq_descr, eidg_);
    return event_generators_.size() - 1;
}

size_t runtime::add_event_generator(const sequences::seq_item_tick& sound_seq_descr)
{
    event_generators_.emplace_back(std::in_place_type<sound_event_generator>, sample_rate_, top_sequence_duration_, sound_seq_descr, eidg_);
    return event_generators_.size() - 1;
}

const event_generator& runtime::get_event_generator(size_t idx) const
{
    return event_generators_[idx];
}

event_generator& runtime::get_event_generator(size_t idx)
{
    return event_generators_[idx];
}

size_t runtime::add_instrument(std::string name, const patch& p,    event_generator& gen, float on_duration)
{
    instruments_.emplace_back(name, max_voice_count_per_instrument_, reg_count_per_voice_, p, gen, on_duration); 
    return instruments_.size() - 1;
}

void runtime::debug()
{
    if (debug_callback_)
    {        
        debug_str_.clear();
        
        for (auto& instr : instruments_)
        {
            debug_str_.append(instr.get_display_line());
            debug_str_.append("\n");
        }
        
        debug_str_.append("\033[").append(std::to_string(instruments_.size())).append("A");
        
        debug_callback_(debug_str_.data());
    }
}

}
