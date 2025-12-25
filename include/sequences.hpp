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

struct subsequence_item_wrapper;
using subsequence_items = std::vector<subsequence_item_wrapper>;

class subsequence
{
public:
    subsequence() = default;

    template<typename... Ts>
    subsequence& operator()(Ts&&... args)
    {
        (append(std::forward<Ts>(args)), ...);
        return *this;
    }

    const subsequence_items& get_items() const { return items_; }
    inline void resolve_placeholders(note_from_number resolver_base);

private:
    inline void resolve_placeholders(subsequence_item_wrapper& wrapper, note_from_number resolver_base);
    
    inline void append(note n);

    inline void append(pause);
    inline void append(int64_t degree);
    inline void append(const subsequence& nested);
    
    subsequence_items items_;
};

struct subsequence_builder
{
    template<typename... Ts>
    subsequence operator()(Ts&&... args) const { return subsequence{}(std::forward<Ts>(args)...); }
};

inline constexpr subsequence_builder _{};

using subsequence_item = std::variant<note, pause, subsequence, note_placeholder>;

struct subsequence_item_wrapper
{
    subsequence_item item_;
};

inline void subsequence::append(note n)
{
    items_.emplace_back(subsequence_item{std::move(n)});
}

inline void subsequence::append(pause)
{
    items_.emplace_back(subsequence_item{x});
}

inline void subsequence::append(int64_t degree)
{
    items_.emplace_back(subsequence_item{note_placeholder{degree}});
}

inline void subsequence::append(const subsequence& nested)
{
    items_.emplace_back(subsequence_item{nested});
}

inline void subsequence::resolve_placeholders(subsequence_item_wrapper& wrapper, note_from_number resolver_base)
{
    std::visit(
        overloaded {
            [&](note_placeholder& pl)
            {
                note resolved = (resolver_base != nullptr)
                    ? resolver_base(pl.degree_)
                    : note{inaudible_frequency};
                wrapper.item_.emplace<note>(std::move(resolved));
            },
            [&](subsequence& sub) { sub.resolve_placeholders(resolver_base); },
            [](auto&) {}
        }, 
        wrapper.item_
    );
}

inline void subsequence::resolve_placeholders(note_from_number resolver_base)
{
    for (auto& wrapper : items_)
        resolve_placeholders(wrapper, resolver_base);
}

using sequence_item = std::variant<note, pause, subsequence>;

struct sequence_item_wrapper;
using sequence_items = std::vector<sequence_item_wrapper>;

class sequence
{
public:
    sequence() = default;

    sequence(note_from_number base, float length = 1.0f):
        base_(base),
        note_length_(length)
    {}

    template<typename... Ts>
    sequence& operator()(Ts&&... args)
    {
        (append(std::forward<Ts>(args)), ...);
        return *this;
    }

    const sequence_items& get_items() const { return items_; }

    float note_length() const { return note_length_; }
    
    constexpr float total_duration() const
    {
        return items_.size() * note_length_;
    }
    
private:
    void append(note n)
    {
        items_.emplace_back(sequence_item{std::move(n)});
    }

    void append(pause)
    {
        items_.emplace_back(sequence_item{x});
    }

    void append(int64_t degree)
    {
        if (base_ != nullptr)
            items_.emplace_back(sequence_item{base_(degree)});
        else
            items_.emplace_back(sequence_item{note{inaudible_frequency}});
    }

    void append(const subsequence& sub)
    {
        auto resolved = sub;
        resolved.resolve_placeholders(base_);
        items_.emplace_back(sequence_item{std::move(resolved)});
    }
    
    sequence_items items_;
    note_from_number base_{nullptr};
    float note_length_{1.0f};
};

struct sequence_item_wrapper
{
    sequence_item item_;
};

using sequences = std::vector<sequence>;

class flow
{
public:
    flow() = default;

    explicit flow(note_from_number base):
        current_base_(base)
    {}

    explicit flow(float initial_note_length):
        current_note_length_(initial_note_length)
    {}

    flow(note_from_number base, float initial_note_length):
        current_base_(base),
        current_note_length_(initial_note_length)
    {}

    template<typename... Ts>
    flow& operator()(Ts&&... args)
    {
        sequences_.emplace_back(current_base_, current_note_length_);
        sequences_.back()(std::forward<Ts>(args)...);
        return *this;
    }

    flow& base(note_from_number new_base)
    {
        current_base_ = new_base;
        return *this;
    }

    flow& note_length(float length_seconds)
    {
        current_note_length_ = length_seconds;
        return *this;
    }

    const sequences& get_sequences() const { return sequences_; }
    
    constexpr float total_duration() const
    {
        float duration = 0.0f;
        for (const auto& seq : sequences_)
        {
            duration += seq.total_duration();
        }
        return duration;
    }

private:
    sequences sequences_;
    note_from_number current_base_{nullptr};
    float current_note_length_{1.0f};
};
}
