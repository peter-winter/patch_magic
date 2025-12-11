#pragma once

#include <variant>
#include <vector>

namespace patch_magic
{
    
struct cnst_f { float val_; };
struct reg_f  { std::size_t idx_; };
struct out    { std::size_t idx_; };

using in_arg  = std::variant<cnst_f, reg_f>;
using out_arg = std::variant<out, reg_f>;

struct sine { in_arg freq_; out_arg out_; };
struct vol  { in_arg gain_; in_arg in_; out_arg out_; };

using operation = std::variant<sine, vol>;

struct program
{
    std::vector<operation> ops_;
    float sample_rate_ = 48000.0f;
    std::size_t channels_ = 2;
};

}
