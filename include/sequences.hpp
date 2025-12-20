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

struct simple
{
    int n_;
};

constexpr simple _1(1);
constexpr simple _2(2);
constexpr simple _3(3);
constexpr simple _4(4);
constexpr simple _5(5);

using simple_items = std::vector<simple>;

struct parallel
{
    simple_items items_;
};

struct item_wrapper;
using sequence = std::vector<item_wrapper>;

using item = std::variant<simple, parallel, sequence>;

struct item_wrapper
{
    item i_;
};

struct seq_builder
{
    template<typename... T>
    auto operator() (T... t) const { return sequence{item_wrapper{item{t}}...}; }
};

constexpr seq_builder _;

inline parallel operator & (simple s1, simple s2) { return parallel{simple_items{s1, s2}}; }
inline parallel operator & (parallel p, simple s) { parallel res(p); p.items_.emplace_back(s); return res; }
inline parallel operator & (simple s, parallel p) { return p & s; }


inline item operator & (item i1, item i2)
{
    return std::visit(
        [](auto v1, auto v2) -> item { return item{v1 & v2}; },
        i1, i2
    );
}

inline sequence operator & (sequence seq, simple s)
{
    sequence res;
    for (size_t i; i < seq.size(); ++i)
        res.emplace_back(item_wrapper{seq[i].i_ & s});
    return res;
}

inline sequence operator & (sequence seq, parallel p)
{
    sequence res;
    for (size_t i; i < seq.size(); ++i)
        res.emplace_back(item_wrapper{seq[i].i_ & p});
    return res;
}

inline sequence operator & (simple s, sequence seq) { return seq & s; }
inline sequence operator & (parallel p, sequence seq) { return seq & p; }

inline sequence operator & (sequence seq1, sequence seq2)
{
    sequence res;
    for (size_t i; i < seq1.size(); ++i)
        for (size_t j; j < seq2.size(); ++j)
            res.emplace_back(seq1[i].i_ & seq2[j].i_);
    return res;
}

inline sequence operator * (simple s, size_t n)
{
    sequence res; 
    for (size_t i; i < n; ++i)
        res.emplace_back(item_wrapper{s}); 
        
    return res;
}

inline sequence operator * (parallel p, size_t n)
{
    sequence res; 
    for (size_t i; i < n; ++i)
        res.emplace_back(item_wrapper{p}); 
        
    return res;
}

inline sequence operator * (size_t n, simple s) { return s * n; }
inline sequence operator * (size_t n, parallel p) { return p * n; }

inline sequence operator * (sequence seq, size_t n)
{
    sequence res; 
    for (size_t i; i < n; ++i)
        for (size_t j; j < seq.size(); ++j)
            res.emplace_back(seq[j]);
        
    return res;
}

inline sequence operator * (size_t n, sequence seq) { return seq * n; }

inline sequence operator * (sequence seq, simple s)
{
    sequence res; 
    for (size_t i; i < seq.size(); ++i)
    {
        for (size_t j; j < seq.size(); ++j)
            res.emplace_back(seq[j]);
        res.emplace_back(item_wrapper{s});
    }
        
    return res;
}

inline sequence operator * (sequence seq, parallel p)
{
    sequence res; 
    for (size_t i; i < seq.size(); ++i)
    {
        for (size_t j; j < seq.size(); ++j)
            res.emplace_back(seq[j]);
        res.emplace_back(item_wrapper{p});
    }
        
    return res;
}

inline sequence operator * (simple s, sequence seq) { return seq * s; }
inline sequence operator * (parallel p, sequence seq) { return seq * p; }

inline item operator * (item i1, item i2)
{
    return std::visit(
        [](auto v1, auto v2) -> item { return item{v1 * v2}; },
        i1, i2
    );
}

inline sequence operator * (sequence seq1, sequence seq2)
{
    sequence res;
    for (size_t i; i < seq1.size(); ++i)
        for (size_t j; j < seq2.size(); ++j)
            res.emplace_back(seq1[i].i_ * seq2[j].i_);
    return res;
}

}

}
