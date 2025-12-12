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
using prog_ops = std::vector<operation>;

struct program
{
    prog_ops ops_;
};

}
