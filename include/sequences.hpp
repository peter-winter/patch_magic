#pragma once

#include "utils.hpp"
#include "math.hpp"
#include "notes.hpp"

#include <variant>
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>

namespace patch_magic
{

constexpr float inaudible_frequency = 1.0e-8;

struct pause {};
inline constexpr pause x{};

struct note_placeholder
{
    int64_t degree_;
};

struct sequence_item_wrapper;
using sequence_items = std::vector<sequence_item_wrapper>;

class sequence
{
public:
    sequence() = default;

    template<typename... Ts>
    sequence& operator()(Ts&&... args)
    {
        (append(std::forward<Ts>(args)), ...);
        return *this;
    }

    const sequence_items& get_items() const { return items_; }
    
    inline void concat_sequence(const sequence& seq);
    
private:
    inline void append(note n);
    inline void append(pause);
    inline void append(int64_t degree);
    inline void append(const sequence& sub);
    inline void append(sequence&& sub);
    
    sequence_items items_;
};

using sequence_item = std::variant<note, pause, sequence, note_placeholder>;

struct sequence_item_wrapper
{
    sequence_item wrapped_;
};

inline void sequence::append(note n) { items_.emplace_back(sequence_item{std::move(n)}); }
inline void sequence::append(pause) { items_.emplace_back(sequence_item{x}); }
inline void sequence::append(int64_t degree) { items_.emplace_back(sequence_item{note_placeholder{degree}}); }
inline void sequence::append(const sequence& sub) { items_.emplace_back(sequence_item{sub}); }
inline void sequence::append(sequence&& sub) { items_.emplace_back(sequence_item{std::move(sub)}); }

inline void sequence::concat_sequence(const sequence& seq) { items_.insert(items_.end(), seq.get_items().begin(), seq.get_items().end()); }

struct sequence_builder
{
    template<typename... Ts>
    sequence operator()(Ts&&... args) const { return sequence{}(std::forward<Ts>(args)...); }
};

inline constexpr sequence_builder _{};

inline sequence operator + (const sequence& s1, const sequence& s2)
{
    sequence ret;
    ret.concat_sequence(s1);
    ret.concat_sequence(s2);
    return ret;
}

struct note_length_change
{
    float val_;
};

struct base_change
{
    note_from_number base_;
};

using flow_item = std::variant<sequence, note_length_change, base_change>;
using flow = std::vector<flow_item>;

inline flow_item base(note_from_number b) { return base_change{b}; }
inline flow_item note_len(float v) { return note_length_change{v}; }

inline auto operator >>= (flow f, flow_item i)
{
    flow ret(std::move(f));
    ret.emplace_back(std::move(i));
    return ret;
}

inline auto operator >>= (flow f1, flow f2)
{
    flow ret(std::move(f1));
    ret.insert(ret.end(), std::make_move_iterator(f2.begin()), std::make_move_iterator(f2.end()));
    return ret;
}

inline auto operator >>= (flow_item i, flow f)
{
    flow ret{i};
    ret.insert(ret.end(), std::make_move_iterator(f.begin()), std::make_move_iterator(f.end()));
    return ret;
}

inline auto operator >>= (flow_item i1, flow_item i2)
{
    return flow{std::move(i1), std::move(i2)};
}

struct pattern
{
    pattern(std::initializer_list<uint64_t> list):
        p_(list)
    {}
    
    std::vector<uint64_t> p_;
    
    template<typename... Ts>
    inline flow operator ()(Ts&&... args)
    {
        std::vector<std::variant<flow, flow_item>> arg_v{std::forward<Ts>(args)...};
        
        flow ret;
        for (uint64_t x : p_)
        {
            if (x >= arg_v.size())
                throw std::out_of_range("Not enough arguments passed pattern()");
            std::visit([&]<typename T>(T&& v){ ret = (ret >>= std::forward<T>(v)); }, arg_v[x]);
        }
        
        return ret;
    }
};

}
