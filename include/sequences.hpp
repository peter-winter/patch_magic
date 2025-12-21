#pragma once

#include "utils.hpp"
#include "math.hpp"

#include <variant>
#include <vector>
#include <cstdint>

namespace patch_magic
{
    
namespace sequences
{

using freq_f = float (*)(uint32_t);
using progression_sequence = std::vector<freq_f>;

template<typename T>
struct simple
{
    T t_;
};

struct number
{
    int n_;
};

struct note
{
    float freq_;
};

using simple_note = simple<note>;
using simple_number = simple<number>;

constexpr simple_number _1(number{1});
constexpr simple_number _2(number{2});
constexpr simple_number _3(number{3});
constexpr simple_number _4(number{4});
constexpr simple_number _5(number{5});
constexpr simple_number _6(number{6});
constexpr simple_number _7(number{7});
constexpr simple_number _8(number{8});
constexpr simple_number _9(number{9});
constexpr simple_number _10(number{10});
constexpr simple_number _11(number{11});
constexpr simple_number _12(number{12});

template<typename T>
using simple_items = std::vector<simple<T>>;

template<typename T>
struct parallel
{
    simple_items<T> items_;
};

using parallel_number = parallel<number>;
using parallel_note = parallel<note>;

template<typename T>
struct item_wrapper;

template<typename T>
using sequence = std::vector<item_wrapper<T>>;

using number_sequence = sequence<number>;
using note_sequence = sequence<note>;

template<typename T>
using item = std::variant<simple<T>, parallel<T>, sequence<T>>;

using number_item = item<number>;
using note_item = item<note>;

template<typename T>
struct item_wrapper
{
    item<T> i_;
};

template<typename T>
inline parallel<T> operator & (simple<T> s1, simple<T> s2) { return parallel<T>{simple_items<T>{s1, s2}}; }

template<typename T>
inline parallel<T> operator & (parallel<T> p, simple<T> s) { parallel<T> res(p); p.items_.emplace_back(s); return res; }

template<typename T>
inline parallel<T> operator & (simple<T> s, parallel<T> p) { return p & s; }


template<typename T>
inline item<T> operator & (item<T> i1, item<T> i2)
{
    return std::visit(
        [](auto v1, auto v2) -> item<T> { return item<T>{v1 & v2}; },
        i1, i2
    );
}

template<typename T>
inline sequence<T> operator & (sequence<T> seq, simple<T> s)
{
    sequence<T> res = seq;
    for (const auto& it : seq)
        res.emplace_back(item_wrapper<T>{it.i_ & s});
    return res;
}

template<typename T>
inline sequence<T> operator & (sequence<T> seq, parallel<T> p)
{
    sequence<T> res;
    for (const auto& it : seq)
        res.emplace_back(item_wrapper<T>{it.i_ & p});
    return res;
}

template<typename T>
inline sequence<T> operator & (simple<T> s, sequence<T> seq) { return seq & s; }

template<typename T>
inline sequence<T> operator & (parallel<T> p, sequence<T> seq) { return seq & p; }

template<typename T>
inline sequence<T> operator * (simple<T> s, size_t n)
{
    sequence<T> res; 
    for (size_t i; i < n; ++i)
        res.emplace_back(item_wrapper<T>{s}); 
        
    return res;
}

template<typename T>
inline sequence<T> operator * (parallel<T> p, size_t n)
{
    sequence<T> res; 
    for (size_t i = 0; i < n; ++i)
        res.emplace_back(item_wrapper<T>{p}); 
        
    return res;
}

template<typename T>
inline sequence<T> operator * (size_t n, simple<T> s) { return s * n; }

template<typename T>
inline sequence<T> operator * (size_t n, parallel<T> p) { return p * n; }

template<typename T>
inline sequence<T> operator * (sequence<T> seq, size_t n)
{
    sequence<T> res; 
    for (size_t i = 0; i < n; ++i)
        for (const auto& it : seq)
            res.emplace_back(it);
        
    return res;
}

template<typename T>
inline sequence<T> operator * (size_t n, sequence<T> seq) { return seq * n; }

template<typename T>
inline item<T> operator * (item<T> i1, item<T> i2)
{
    return std::visit(
        [](auto v1, auto v2) -> item<T> { return item<T>{v1 * v2}; },
        i1, i2
    );
}

template<typename T>
inline sequence<T> operator * (sequence<T> seq1, sequence<T> seq2)
{
    sequence<T> res;
    for (size_t i; i < seq2.size(); ++i)
    {
        sequence<T> inner = seq1;
        inner.emplace_back(seq2[i].i_);
        res.emplace_back(item_wrapper<T>{inner});
    }
    return res;
}

inline simple_note operator | (simple_number sn, freq_f f)
{
    return simple_note{note{f(sn.t_.n_)}};
}

inline parallel_note operator | (parallel_number pn, freq_f f)
{
    parallel_note res;
    for (const auto& s : pn.items_)
        res.items_.emplace_back(simple_note{note{f(s.t_.n_)}});
    return res;
    
}

inline note_item operator | (number_item i, freq_f f)
{
    return std::visit(
        [](auto v) -> note_item { return note_item{v | f}; },
        i
    );
}

inline note_sequence operator | (number_sequence seq, freq_f f)
{
    note_sequence res;
    for (const auto& it : seq)
        res.emplace_back(item_wrapper<note>{it.i_ | f})
    return res;
}


inline note_sequence operator | (number_sequence seq, progression_sequence p_seq)
{
    note_sequence res;
    for (const auto f : p_seq)
    {
        note_sequence inner = seq | f;
        res.emplace_back(item_wrapper<note>{inner});
    }
    
    return res;
}

struct number_seq_builder
{
    template<typename... T>
    auto operator() (T... t) const { return sequence<number>{item_wrapper<number>{item<number>{t}}...}; }
};

struct note_seq_builder
{
    template<typename... T>
    auto operator() (T... t) const { return sequence<note>{item_wrapper<note>{item<note>{t}}...}; }
};

struct progression_seq_builder
{
    template<typename... T>
    auto operator() (T... t) const { return progression_sequence{t...}; }
};

constexpr number_seq_builder _;
constexpr note_seq_builder _n;
constexpr progression_seq_builder _p;

}

}
