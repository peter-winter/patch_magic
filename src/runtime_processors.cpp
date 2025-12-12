#include <cmath>
#include <cstddef>

#include "runtime_processors.hpp"
#include "runtime.hpp"
#include "math.hpp"

namespace patch_magic
{
    
void process_sine(runtime_data& rd, const runtime_op& op)
{
    auto& st = std::get<sine_state>(rd.states_[op.state_idx_]);
    
    float freq = op.load_arg0_(rd, op.arg0_idx_);
    double inc = two_pi * freq / rd.sample_rate_;
    st.phase_ += inc;
    op.store_(rd, op.store_idx_, std::sin(st.phase_));
}

void process_vol(runtime_data& rd, const runtime_op& op)
{
    float gain = op.load_arg0_(rd, op.arg0_idx_);
    float in   = op.load_arg1_(rd, op.arg1_idx_);
    op.store_(rd, op.store_idx_, in * gain);
}

}
