#pragma once

#include <miniaudio.h>

#include "program.hpp"
#include "runtime.hpp"  

namespace patch_magic
{

class synth
{
public:
    synth(float sample_rate, size_t channels, size_t reg_count = 256, size_t const_count = 256);
    ~synth();

    void load(const program& prog);
    void play();

private:    
    static inline void data_callback(ma_device* device, void* p_output, const void*, ma_uint32 frame_count);

    float sample_rate_;
    size_t channels_;
    
    runtime_data runtime_data_;
    
    ma_device device_{};
    bool device_initialized_ = false;
};

}
