#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <vector>
#include <cstddef>
#include <iostream>

#include "synth.hpp"

namespace patch_magic
{

synth::synth(float sample_rate, size_t channels, size_t reg_count, size_t const_count):
    sample_rate_(sample_rate),
    channels_(channels),
    rt_(sample_rate, channels, reg_count, const_count)
{}

synth::~synth()
{
    if (device_initialized_)
        ma_device_uninit(&device_);
}

void synth::load(const program& prog)
{
    rt_.load(prog);
}

void synth::play()
{
    instance_ = this;

    ma_device_config cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format = ma_format_f32;
    cfg.playback.channels = static_cast<ma_uint32>(rt_.channels_);
    cfg.sampleRate = static_cast<ma_uint32>(rt_.sample_rate_);
    cfg.dataCallback = data_callback;
    cfg.pUserData = this;

    if (device_initialized_) ma_device_uninit(&device_);

    ma_result r = ma_device_init(nullptr, &cfg, &device_);
    if (r != MA_SUCCESS) throw std::runtime_error("Failed to init audio");

    device_initialized_ = true;
    ma_device_start(&device_);

    std::cout << "Playing – press Enter to stop...\n";
    std::cin.get();
}

void synth::data_callback(ma_device*, void* p_output, const void*, ma_uint32 frame_count)
{
    float* out = static_cast<float*>(p_output);
    auto* self = static_cast<synth*>(device->pUserData);
    runtime& rt = self->rt_;
    
    for (ma_uint32 f = 0; f < frame_count; ++f)
    {
        rt.process_ops();
        
        for (std::size_t ch = 0; ch < rt.channels(); ++ch)
            *out++ = rt.out(ch);
    }
}

}
