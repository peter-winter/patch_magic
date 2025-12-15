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

void synth::play()
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
    
    ma_device_start(&device_);
}

void synth::data_callback(ma_device* device, void* p_output, const void*, ma_uint32 frame_count)
{
    float* out = static_cast<float*>(p_output);
    auto* self = static_cast<synth*>(device->pUserData);
    auto& rd = self->runtime_;
    
    for (ma_uint32 f = 0; f < frame_count; ++f)
    {
        rd.sample(self->channel_outputs_.data(), rd.channel_count());
        for (size_t ch = 0; ch < rd.channel_count(); ++ch)
            *out++ = self->channel_outputs_[ch];
    }
}

void synth::debug_samples(size_t sample_count)
{
    for (size_t i = 0; i < sample_count; ++i)
    {
        float data[2]; 
        runtime_.sample(data, 2);
        std::cout << data[0] << data[1] << "\n";
    }
}

}
