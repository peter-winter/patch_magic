#pragma once

#include "utils.hpp"
#include "math.hpp"
#include "notes.hpp"

#include <variant>
#include <vector>
#include <cstdint>

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

struct repeat_marker
{
    uint64_t n_;
};

struct repeat_done_marker
{};

struct alt_marker
{};

struct alt_done_marker
{};

struct note_length_change
{
    float val_;
};

struct base_change
{
    note_from_number base_;
};

using flow_item = std::variant<sequence, repeat_marker, repeat_done_marker, alt_marker, alt_done_marker, note_length_change, base_change>;
using flow_items = std::vector<flow_item>;

class flow
{
public:
    flow() = default;

    explicit flow(note_from_number start_base):
        start_base_(start_base)
    {}

    explicit flow(float start_note_length):
        start_note_length_(start_note_length)
    {}

    flow(note_from_number start_base, float start_note_length):
        start_base_(start_base),
        start_note_length_(start_note_length)
    {}

    template<typename... Ts>
    flow& operator()(Ts&&... args)
    {
        (items_.emplace_back(std::forward<Ts>(args)), ...);                    
        return *this;
    }
    
    flow& base(note_from_number new_base)
    {
        items_.emplace_back(base_change{new_base});
        return *this;
    }

    flow& note_length(float length_seconds)
    {
        items_.emplace_back(note_length_change{length_seconds});
        return *this;
    }
    
    flow& rep(uint64_t n)
    {
        items_.emplace_back(repeat_marker{n});
        return *this;
    }
    
    flow& per()
    {
        items_.emplace_back(repeat_done_marker{});
        return *this;
    }
    
    flow& alt()
    {
        items_.emplace_back(alt_marker{});
        return *this;
    }
    
    flow& tla()
    {
        items_.emplace_back(alt_done_marker{});
        return *this;
    }

    float get_start_note_length() const { return start_note_length_; }
    note_from_number get_start_base() const { return start_base_; }
    const flow_items& get_items() const { return items_; }    

    size_t item_count() const { return items_.size(); }
    
private:
    flow_items items_;
    note_from_number start_base_{nullptr};
    float start_note_length_{1.0f};
};
}
