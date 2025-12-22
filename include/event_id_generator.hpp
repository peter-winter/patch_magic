#pragma once

namespace patch_magic
{
    
struct event_id_generator
{
    uint32_t generate() { return ++current_id_; }
    uint32_t current_id_{0};
};

}

