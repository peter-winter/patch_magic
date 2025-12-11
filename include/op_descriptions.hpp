#pragma once

namespace patch_magic
{

template<typename Op>
struct op_descr
{};

template<>
struct op_descr<sine>
{
    using rt_state = sine_state;
    loaded_op::process_fn process_fn_{process_sine};
};

template<>
struct op_descr<vol>
{
    using rt_state = stateless;
    loaded_op::process_fn process_fn_{process_vol};
};

constexpr auto op_descriptions = std::make_tuple(
    op_descr<sine>{},
    op_descr<vol>{}
);

}
