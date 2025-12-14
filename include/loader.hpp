#pragma once

#include "runtime_processor_wrappers.hpp"
#include "source.hpp"
#include "runtime.hpp"
#include "invoker.hpp"

#include <stdexcept>

namespace patch_magic
{
    
class loader
{
public:
    loader(runtime_data& rd):
        rd_(rd)
    {}
    
    void load(const std::vector<source_op>& src_ops)
    {
        size_t idx = 0;
        for (const auto& src_op : src_ops)
        {
            rd_.ops_.emplace_back(create_runtime_op(idx, src_op));
            
            std::visit(
                [&]<typename Wrapper>(const Wrapper&)
                {
                    using state_t = runtime_processor_state_t<Wrapper>;
                    if constexpr (!std::is_same_v<stateless, state_t>)
                        rd_.add_state<state_t>();
                },
                src_op.op_
            );
            
            ++idx;
        }
    }
        
private:
    runtime_op create_runtime_op(size_t op_idx, const source_op& src_op)
    {
        return std::visit(
            [&]<typename Wrapper>(const Wrapper&){ return create_runtime_op<Wrapper>(op_idx, src_op.out_idx_, src_op.ins_); },
            src_op.op_
        );
    }

    template<typename Wrapper>
    runtime_op create_runtime_op(size_t op_idx, size_t out_idx, const source_in_args_array_t& ins)
    {
        runtime_op op;
        op.process_ = invoker<Wrapper::proc>::invoke;
        op.state_idx_ = op_idx;
        op.store_idx_ = out_idx;
        bind_ins(ins, op, std::make_index_sequence<Wrapper::in_arg_count>{});
        return op;
    }
    
    size_t get_arg_idx(const source_in_arg& i)
    {
        if (std::holds_alternative<source_const_f>(i))
        {
            rd_.constants_f_.push_back(std::get<source_const_f>(i).v_);
            return rd_.constants_f_.size() - 1;
        }
        else if (std::holds_alternative<source_const_i>(i))
        {
            rd_.constants_i_.push_back(std::get<source_const_i>(i).v_);
            return rd_.constants_i_.size() - 1;
        }
        else if (std::holds_alternative<source_reg_f>(i))
            return std::get<source_reg_f>(i).idx_;
        else
            return std::get<source_reg_i>(i).idx_;
    }
    
    template<size_t I>
    void bind_loader(const source_in_arg& i, runtime_op& op)
    {
        if (std::holds_alternative<source_const_f>(i))
            op.loader_idx_[I] = arg_loader_type::loader_const;
        else if (std::holds_alternative<source_const_i>(i))
            op.loader_idx_[I] = arg_loader_type::loader_const;
        else if (std::holds_alternative<source_reg_f>(i))
            op.loader_idx_[I] = arg_loader_type::loader_reg;
        else
            op.loader_idx_[I] = arg_loader_type::loader_reg;
    }

    template<size_t... I>
    void bind_ins(const source_in_args_array_t& ins, runtime_op& rt_op, std::index_sequence<I...>)
    {
        ((rt_op.arg_idx_[I] = get_arg_idx(ins[I])), ...);
        (bind_loader<I>(ins[I], rt_op), ...);
    }
    
    runtime_data& rd_;
};

}
