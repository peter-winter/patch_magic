#pragma once

#include "runtime_processor_wrappers.hpp"
#include "source.hpp"
#include "runtime.hpp"
#include "invoker.hpp"

#include <stdexcept>
#include <map>
#include <string>

namespace patch_magic
{
    
class loader
{
public:
    loader(runtime& r):
        r_(r)
    {}
    
    void load(const source& src)
    {
        for (const auto& p : src.patches_)
            load_patch(p);
        for (const auto& i : src.instruments_)
            load_instrument(i);
    }
            
private:
    void load_instrument(const instrument_source& src_i)
    {
        auto it = patch_map_.find(src_i.patch_name_);
        if (it == patch_map_.end())
            throw std::invalid_argument("Unknown patch");
        r_.add_instrument(r_.get_patch(it->second));
    }
    
    void load_patch(const patch_source& src_p)
    {
        size_t patch_idx = r_.add_patch();
        patch_map_[src_p.name_] = patch_idx;
        
        load_patch_ops(src_p.ops_, r_.get_patch(patch_idx));
    }
    
    void load_patch_ops(const std::vector<op_source>& src_ops, patch& p)
    {
        size_t idx = 0;
        for (const auto& src_op : src_ops)
        {
            p.add_op(create_runtime_op(idx, src_op, p));
            
            std::visit(
                [&]<typename Wrapper>(const Wrapper&)
                {
                    using state_t = runtime_processor_state_t<Wrapper>;
                    p.add_state_prototype<state_t>();
                },
                src_op.op_
            );
            
            ++idx;
        }
    }
    
    runtime_op create_runtime_op(size_t op_idx, const op_source& src_op, patch& p)
    {
        return std::visit(
            [&]<typename Wrapper>(const Wrapper&){ return create_runtime_op<Wrapper>(op_idx, src_op.out_idx_, src_op.ins_, p); },
            src_op.op_
        );
    }

    template<typename Wrapper>
    runtime_op create_runtime_op(size_t op_idx, size_t out_idx, const in_args_source_array_t& ins, patch& p)
    {
        runtime_op op;
        op.process_ = invoker<Wrapper::proc>::invoke;
        op.state_idx_ = op_idx;
        op.store_idx_ = out_idx;
        bind_ins(ins, op, p, std::make_index_sequence<Wrapper::in_arg_count>{});
        return op;
    }
    
    size_t get_arg_idx(const in_arg_source& i, patch& p)
    {
        if (std::holds_alternative<const_f_source>(i))
        {
            return p.add_const_f(std::get<const_f_source>(i).v_);
        }
        else if (std::holds_alternative<const_i_source>(i))
        {
            return p.add_const_i(std::get<const_i_source>(i).v_);
        }
        else if (std::holds_alternative<reg_f_source>(i))
            return std::get<reg_f_source>(i).idx_;
        else
            return std::get<reg_i_source>(i).idx_;
    }
    
    template<size_t I>
    void bind_loader(const in_arg_source& i, runtime_op& op)
    {
        if (std::holds_alternative<const_f_source>(i))
            op.loader_idx_[I] = arg_loader_type::loader_const;
        else if (std::holds_alternative<const_i_source>(i))
            op.loader_idx_[I] = arg_loader_type::loader_const;
        else if (std::holds_alternative<reg_f_source>(i))
            op.loader_idx_[I] = arg_loader_type::loader_reg;
        else
            op.loader_idx_[I] = arg_loader_type::loader_reg;
    }

    template<size_t... I>
    void bind_ins(const in_args_source_array_t& ins, runtime_op& rt_op, patch& p, std::index_sequence<I...>)
    {
        ((rt_op.arg_idx_[I] = get_arg_idx(ins[I], p)), ...);
        (bind_loader<I>(ins[I], rt_op), ...);
    }
    
    runtime& r_;
    std::map<std::string, size_t> patch_map_;
};

}
