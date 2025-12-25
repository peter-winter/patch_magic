#pragma once

#include <miniaudio.h>

#include "source.hpp"
#include "runtime.hpp"  

#include <array>

namespace patch_magic
{

constexpr size_t max_channels = 8;

class synth
{
public:
    synth(size_t max_voice_count_per_instrument = 32, uint32_t sample_rate = 48000, size_t channels = 2, size_t reg_count_per_voice = 256);
    ~synth();

    void load(const source& src);
    void play();
    void loop();
    bool done() const;
    
    void set_debug_callback(debug_callback cb);

private:    
    static inline void data_callback(ma_device* device, void* p_output, const void*, ma_uint32 frame_count);

    void prepare_device();
    
    runtime runtime_;
    
    ma_device device_{};
    bool device_initialized_ = false;
    std::array<float, max_channels> channel_outputs_;
};

}
