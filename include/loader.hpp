#pragma once

#include "runtime_processor_wrappers.hpp"
#include "source.hpp"
#include "runtime.hpp"
#include "invoker.hpp"
#include "utils.hpp"
#include "event_generators.hpp"

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
        r_.set_top_sequence_duration(src.top_sequence_duration_);
        for (const auto& p : src.patches_)
            load_patch(p);
        for (const auto& t : src.sequences_)
            load_sequence(t);
        for (const auto& i : src.instruments_)
            load_instrument(i);
    }
            
private:
    void load_instrument(const instrument_source& src_i)
    {
        auto patch_it = patch_map_.find(src_i.patch_name_);
        if (patch_it == patch_map_.end())
            throw std::invalid_argument("Unknown patch");
            
        auto ev_g_it = event_generator_map_.find(src_i.sequence_name_);
        if (ev_g_it == event_generator_map_.end())
            throw std::invalid_argument("Unknown sequence");
        r_.add_instrument(src_i.name_, r_.get_patch(patch_it->second), r_.get_event_generator(ev_g_it->second), src_i.on_duration_);
    }
    
    void load_patch(const patch_source& src_p)
    {
        size_t patch_idx = r_.add_patch();
        patch_map_[src_p.name_] = patch_idx;
        
        load_patch_ops(src_p.ops_, r_.get_patch(patch_idx));
    }
    
    void load_sequence(const sequence_source& src_s)
    {
        size_t event_generator_idx = r_.add_event_generator(src_s.sequence_);
        event_generator_map_[src_s.name_] = event_generator_idx;
    }
    
    void load_patch_ops(const std::vector<op_source>& src_ops, patch& p)
    {
        for (const auto& src_op : src_ops)
        {
            size_t state_idx = std::visit(
                [&]<typename Wrapper>(const Wrapper&) -> size_t
                {
                    using state_t = runtime_processor_state_t<Wrapper>;
                    return p.add_state_prototype<state_t>();
                },
                src_op.op_
            );
            
            p.add_op(create_runtime_op(state_idx, src_op, p));
        }
    }
    
    runtime_op create_runtime_op(size_t state_idx, const op_source& src_op, patch& p)
    {
        return std::visit(
            [&]<typename Wrapper>(const Wrapper&){ return create_runtime_op<Wrapper>(state_idx, src_op.out_idx_, src_op.ins_, p); },
            src_op.op_
        );
    }

    template<typename Wrapper>
    runtime_op create_runtime_op(size_t state_idx, size_t out_idx, const in_args_source_array_t& ins, patch& p)
    {
        runtime_op op;
        op.process_ = invoker<Wrapper::proc>::invoke;
        op.state_idx_ = state_idx;
        if (out_idx >= r_.reg_count_per_voice())
            throw std::invalid_argument("Invalid output register index");
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
        {
            size_t idx = std::get<reg_f_source>(i).idx_;
            if (idx >= r_.reg_count_per_voice())
                throw std::invalid_argument("Invalid float register index");
            return idx;
        }
        else
        {
            size_t idx = std::get<reg_i_source>(i).idx_;
            if (idx >= r_.reg_count_per_voice())
                throw std::invalid_argument("Invalid int register index");
            return idx;
        }
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
    std::map<std::string, size_t> event_generator_map_;
};

}
