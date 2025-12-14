#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <vector>
#include <cstddef>
#include <iostream>

#include "synth.hpp"
#include "loader.hpp"

namespace patch_magic
{

synth::synth(uint32_t sample_rate, size_t channels, size_t reg_count, size_t const_count):
    sample_rate_(sample_rate),
    channels_(channels),
    runtime_data_(sample_rate, channels, reg_count, const_count)
{}

synth::~synth()
{
    if (device_initialized_)
        ma_device_uninit(&device_);
}

void synth::load(const source& src)
{
    loader ld(runtime_data_);
    
    ld.load(src.ops_);
}

void synth::play()
{
    ma_device_config cfg = ma_device_config_init(ma_device_type_playback);
    cfg.playback.format = ma_format_f32;
    cfg.playback.channels = static_cast<ma_uint32>(runtime_data_.channel_count_);
    cfg.sampleRate = static_cast<ma_uint32>(runtime_data_.sample_rate_);
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
    auto& rd = self->runtime_data_;
    
    for (ma_uint32 f = 0; f < frame_count; ++f)
    {
        for (const auto& op : rd.ops_)
            op.process_(rd, op);
        
        for (std::size_t ch = 0; ch < rd.channel_count_; ++ch)
            *out++ = rd.regs_f_[0];
    }
}

}
