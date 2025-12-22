#pragma once

#include "utils.hpp"
#include "math.hpp"
#include "notes.hpp"

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
struct atom
{
    T t_;
};

struct number
{
    int n_;
};

struct tick
{};

template<typename T>
struct pause
{};

using atom_note = atom<notes::note>;
using atom_number = atom<number>;
using atom_tick = atom<tick>;
using pause_note = pause<notes::note>;
using pause_number = pause<number>;
using pause_tick = pause<tick>;

constexpr atom_number _1(number{1});
constexpr atom_number _2(number{2});
constexpr atom_number _3(number{3});
constexpr atom_number _4(number{4});
constexpr atom_number _5(number{5});
constexpr atom_number _6(number{6});
constexpr atom_number _7(number{7});
constexpr atom_number _8(number{8});
constexpr atom_number _9(number{9});
constexpr atom_number _10(number{10});
constexpr atom_number _11(number{11});
constexpr atom_number _12(number{12});

template<typename T>
struct seq_item_wrapper;

template<typename T>
struct par_item_wrapper;

template<typename T>
using seq = std::vector<seq_item_wrapper<T>>;

using seq_number = seq<number>;
using seq_note = seq<notes::note>;
using seq_tick = seq<tick>;

template<typename T>
using par = std::vector<par_item_wrapper<T>>;

using par_number = par<number>;
using par_note = par<notes::note>;
using par_tick = par<tick>;

template<typename T>
using seq_item = std::variant<atom<T>, par<T>, seq<T>, pause<T>>;

template<typename T>
using par_item = std::variant<atom<T>, seq<T>>;

using seq_item_number = seq_item<number>;
using seq_item_note = seq_item<notes::note>;
using seq_item_tick = seq_item<tick>;
using par_item_number = par_item<number>;
using par_item_note = par_item<notes::note>;
using par_item_tick = par_item<tick>;

template<typename T>
struct seq_item_wrapper
{
    seq_item<T> i_;
};

template<typename T>
struct par_item_wrapper
{
    par_item<T> i_;
};

using par_item_wrapper_number = par_item_wrapper<number>;
using par_item_wrapper_note = par_item_wrapper<notes::note>;
using par_item_wrapper_tick = par_item_wrapper<tick>;
using seq_item_wrapper_number = seq_item_wrapper<number>;
using seq_item_wrapper_note = seq_item_wrapper<notes::note>;
using seq_item_wrapper_tick = seq_item_wrapper<tick>;

template<typename T, template<typename> class K>
inline par<T> make_par(const K<T>& i1, const K<T>& i2)
{
    return par<T>{par_item_wrapper<T>{i1}, par_item_wrapper<T>{i2}};
}

template<typename T, template<typename> class K>
inline par<T> copy_par_and_append(const par<T>& i1, const K<T>& i2)
{
    par<T> res(i1);
    res.emplace_back(i2);
    return res;
}

template<typename T>
inline auto operator & (const atom<T>& i1, const atom<T>& i2){ return make_par(i1, i2); }
template<typename T>
inline auto operator & (const seq<T>& i1, const atom<T>& i2){ return make_par(i1, i2); }
template<typename T>
inline auto operator & (const atom<T>& i1, const seq<T>& i2){ return make_par(i1, i2); }
template<typename T>
inline auto operator & (const seq<T>& i1, const seq<T>& i2){ return make_par(i1, i2); }

template<typename T>
inline auto operator & (const par<T>& i1, const atom<T>& i2){ return copy_par_and_append(i1, i2); }
template<typename T>
inline auto operator & (const atom<T>& i1, const par<T>& i2){ return copy_par_and_append(i2, i1); }
template<typename T>
inline auto operator & (const par<T>& i1, const seq<T>& i2){ return copy_par_and_append(i1, i2); }
template<typename T>
inline auto operator & (const seq<T>& i1, const par<T>& i2){ return copy_par_and_append(i2, i1); }


template<typename T, template<typename> class K>
inline auto multiply(const K<T>& x, size_t n)
{
    seq<T> res;
    for (size_t i = 0; i < n; ++i)
        res.emplace_back(seq_item_wrapper<T>{x});
    return res;
}

template<typename T>
inline seq<T> operator * (const atom<T>& x, size_t n) { return multiply(x, n); }
template<typename T>
inline seq<T> operator * (size_t n, const atom<T>& x) { return multiply(x, n); }
template<typename T>
inline seq<T> operator * (const par<T>& x, size_t n) { return multiply(x, n); }
template<typename T>
inline seq<T> operator * (size_t n, const par<T>& x) { return multiply(x, n); }
template<typename T>
inline seq<T> operator * (const seq<T>& x, size_t n) { return multiply(x, n); }
template<typename T>
inline seq<T> operator * (size_t n, const seq<T>& x) { return multiply(x, n); }
template<typename T>
inline seq<T> operator * (const pause<T>& x, size_t n) { return multiply(x, n); }
template<typename T>
inline seq<T> operator * (size_t n, const pause<T>& x) { return multiply(x, n); }

template<typename T>
inline seq<T> operator * (const seq<T>& i1, const seq<T>& i2)
{
    seq<T> res;
    for (const auto& x : i2)
    {
        seq<T> inner(i1);
        inner.emplace_back(x.i_);
        res.emplace_back(seq_item_wrapper<T>{inner});
    }
    return res;
}

inline atom_note operator | (const atom_number& i, const freq_f& f) { return atom_note{notes::note{f(i.t_.n_)}}; }
inline atom_note operator | (const freq_f& f, const atom_number& i) { return i | f; }

inline pause_note operator | (const pause_number&, const freq_f&) { return pause_note{}; }
inline pause_note operator | (const freq_f&, const pause_number&) { return pause_note{}; }

inline seq_item_note operator | (const seq_item_number& i, const freq_f& f)
{
    return std::visit([&f](const auto& x) -> seq_item_note { return seq_item_note(x | f); }, i);
}

inline par_item_note operator | (const par_item_number& i, const freq_f& f)
{
    return std::visit([&f](const auto& x) -> par_item_note { return par_item_note(x | f); }, i);
}

inline par_note operator | (const par_number& i, const freq_f& f)
{
    par_note res;
    for (const auto& x : i)
        res.emplace_back(par_item_wrapper_note{ x.i_ | f });
    return res;
}
inline par_note operator | (const freq_f& f, const par_number& i) { return i | f; }

inline seq_note operator | (const seq_number& i, const freq_f& f)
{
    seq_note res;
    for (const auto& x : i)
        res.emplace_back(seq_item_wrapper_note{ x.i_ | f });
    return res;
}
inline seq_note operator | (const freq_f& f, const seq_number& i) { return i | f; }


inline seq_note operator | (const seq_number& i, const progression_sequence& p_seq)
{
    seq_note res;
    for (const auto& f : p_seq)
    {
        seq_note inner = i | f;
        res.emplace_back(seq_item_wrapper_note{inner});
    }
    return res;
}

struct number_seq_builder
{
    template<typename... T>
    auto operator() (const T&... t) const { return seq_number{seq_item_wrapper_number{seq_item_number(t)}...}; }
};

struct note_seq_builder
{
    auto make_arg(notes::note n) const { return atom_note{n}; }

    template<typename T>
    T make_arg(const T& t) const { return t; }

    template<typename... T>
    auto operator() (const T&... t) const { return seq_note{seq_item_wrapper_note{seq_item_note(make_arg(t))}...}; }
};

struct progression_seq_builder
{
    template<typename... T>
    auto operator() (const T&... t) const { return progression_sequence{t...}; }
};

constexpr number_seq_builder _;
constexpr note_seq_builder _n;
constexpr progression_seq_builder _p;
constexpr pause_note _x_;
constexpr pause_number _x;

struct sound_seq_builder
{
    template<typename... T>
    auto operator() (T... t) const { return seq_tick{seq_item_wrapper_tick{seq_item_tick(t)}...}; }
};

constexpr sound_seq_builder _s;

constexpr pause_tick x;
constexpr atom_tick o;

}

}
