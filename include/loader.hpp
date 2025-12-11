#pragma once

#include "op_descriptions.hpp"
#include "runtime_op.hpp"
#include "program.hpp"

namespace patch_magic
{
    
class loader
{
public:
    loader(runtime_data& rd):
        rd_(rd)
    {}
    
    template<typename Op>
    runtime_op load_op(const Op& src, size_t op_idx)
    {
        runtime_op op{};
        const auto& descr = std::get<op_descr<Op>>(op_descriptions);
        
        op.process_ = descr.process_fn_;
        op.state_idx_ = op_idx;
        bind(src, op);
        
        return op;
    }
    
private:
    void bind(const sine& src, loaded_op& op);
    {
        bind_in(op.load_arg0_, op.arg0_idx_, src.freq_);
        bind_out(op.store_, op.store_idx_, src.out_);
    }
    
    void bind(const vol& src, loaded_op& op)
    {
        bind_in(op.load_arg0_, op.arg0_idx_, src.gain_);
        bind_in(op.load_arg1_, op.arg1_idx_, src.in_);
        bind_out(op.store_, op.store_idx_, src.out_);
    }
    
    void bind_in(auto& load_fn_ref, auto& load_index, const in_arg& arg)
    {
        if (std::holds_alternative<cnst_f>(arg))
        {
            load_fn_ref = load_const;
            load_index = rt.constants_.size();
            rd_.constants_.push_back(std::get<cnst_f>(arg).val_);
        }
        else
        {
            load_fn_ref = load_reg;
            load_index = std::get<reg_f>(arg).idx_;
        }
    }

    void bind_out(auto& store_fn_ref, auto& store_idx, const out_arg& arg)
    {
        if (std::holds_alternative<out>(arg))
        {
            store_fn_ref = store_out;
            store_idx = std::get<out>(arg).idx_;
        }
        else
        {
            store_fn_ref = store_reg;
            store_idx = std::get<reg_f>(arg).idx_;
        }
    }
};

}
