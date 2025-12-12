#pragma once

#include "op_descriptions.hpp"
#include "runtime_op.hpp"
#include "program.hpp"
#include "runtime.hpp"
#include "primitives.hpp"

#include <stdexcept>

namespace patch_magic
{
    
class loader
{
public:
    loader(runtime_data& rd):
        rd_(rd)
    {}
    
    void load(const prog_ops& ops)
    {
        visit_all(ops, [&]<typename Op>(const Op& op, auto idx){ rd_.ops_.emplace_back(load_op(op, idx)); });
    }
    
    void create_states(const prog_ops& ops)
    {
        visit_all(ops, [&]<typename Op>(const Op&, auto){ rd_.states_.emplace_back(typename op_descr<Op>::rt_state{}); });
    }
        
private:
    template<typename F>
    void visit_all(const prog_ops& ops, F f)
    {
        size_t idx = 0;
        for (const auto& prog_op : ops)
        {
            std::visit(
                [&]<typename Op>(const Op& op)
                {
                    f(op, idx);
                },
                prog_op
            );
            ++idx;
        }
    }
    
    template<typename Op>
    runtime_op load_op(const Op& src, size_t op_idx)
    {
        runtime_op op{};
        op_descr<Op> descr;
        
        op.process_ = descr.process_fn_;
        op.state_idx_ = op_idx;
        bind_ins(src, descr.in_descr(), op);
        bind_out(src, descr.out_descr(), op);
        return op;
    }
    
    template<typename Op, typename InsDescr>
    void bind_ins(const Op& op, const InsDescr& ins, runtime_op& rt_op)
    {
        const auto op_in0_descr = op.*(std::get<0>(ins));
        rt_op.arg0_idx_ = load_in_idx(op_in0_descr);
        rt_op.load_arg0_ = load_in_fn(op_in0_descr);
        if constexpr (std::tuple_size_v<InsDescr> > 1)
        {
            const auto op_in1_descr = op.*(std::get<1>(ins));
            rt_op.arg1_idx_ = load_in_idx(op_in1_descr);
            rt_op.load_arg1_ = load_in_fn(op_in1_descr);
        }
    }
    
    template<typename Op, typename OutDescr>
    void bind_out(const Op& op, const OutDescr& os, runtime_op& rt_op)
    {
        const auto op_out_descr = op.*os;
        rt_op.store_idx_ = load_out_idx(op_out_descr);
        rt_op.store_ = load_out_fn(op_out_descr);
    }
    
    template<typename In>
    size_t load_in_idx(const In& i)
    {
        if (std::holds_alternative<cnst_f>(i))
        {
            rd_.constants_.push_back(std::get<cnst_f>(i).val_);
            return rd_.constants_.size() - 1;
        }
        else
        {
            return get_reg_idx(std::get<reg_f>(i));
        }
    }
    
    template<typename Out>
    size_t load_out_idx(const Out& o)
    {
        if (std::holds_alternative<out>(o))
        {
            return get_out_idx(std::get<out>(o));
        }
        else
        {
            return get_reg_idx(std::get<reg_f>(o));
        }
    }
    
    size_t get_reg_idx(const reg_f& r)
    {
        size_t idx = r.idx_;
        if (idx >= rd_.reg_count_)
            throw std::out_of_range("Out of range register idx");
        return idx;
    }
    
    size_t get_out_idx(const out& o)
    {
        size_t idx = o.idx_;
        if (idx >= rd_.channel_count_)
            throw std::out_of_range("Out of range output idx");
        return idx;
    }
    
    template<typename In>
    runtime_op::load_fn load_in_fn(const In& i)
    {
        if (std::holds_alternative<cnst_f>(i))
            return load_const;
        else
            return load_reg;
    }
    
    template<typename Out>
    runtime_op::store_fn load_out_fn(const Out& o)
    {
        if (std::holds_alternative<out>(o))
            return store_out;
        else
            return store_reg;
    }
    
    runtime_data& rd_;
};

}
