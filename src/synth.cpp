#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <vector>
#include <cstddef>
#include <iostream>

#include "synth.hpp"
#include "loader.hpp"

namespace patch_magic
{

synth::synth(size_t max_voice_count_per_instrument, uint32_t sample_rate, size_t channels, size_t reg_count_per_voice):
    runtime_(max_voice_count_per_instrument, sample_rate, channels, reg_count_per_voice)
{}

void synth::set_debug_callback(debug_callback cb)
{
    runtime_.set_debug_callback(cb);
}

synth::~synth()
{
    if (device_initialized_)
        ma_device_uninit(&device_);
}

void synth::load(const source& src)
{
    loader ld(runtime_);
    
    ld.load(src);
}

bool synth::done() const
{
    return runtime_.done();
}

void synth::play()
{
    runtime_.reset();
    prepare_device();
    ma_device_start(&device_);
}

void synth::loop()
{
    runtime_.set_looping();
    play();
}

void synth::prepare_device()
{
    ma_device_config cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format = ma_format_f32;
    cfg.playback.channels = static_cast<ma_uint32>(runtime_.channel_count());
    cfg.sampleRate = static_cast<ma_uint32>(runtime_.sample_rate());
    cfg.dataCallback = data_callback;
    cfg.pUserData = this;

    if (device_initialized_)
        ma_device_uninit(&device_);

    ma_result r = ma_device_init(nullptr, &cfg, &device_);
    if (r != MA_SUCCESS)
        throw std::runtime_error("Failed to init audio");

    device_initialized_ = true;
}

void synth::data_callback(ma_device* device, void* p_output, const void*, ma_uint32 frame_count)
{
    float* out = static_cast<float*>(p_output);
    auto* self = static_cast<synth*>(device->pUserData);
    auto& r = self->runtime_;
    
    for (ma_uint32 f = 0; f < frame_count; ++f)
    {
        r.sample(self->channel_outputs_.data(), r.channel_count());
        for (size_t ch = 0; ch < r.channel_count(); ++ch)
            *out++ = self->channel_outputs_[ch];
    }
}


}
