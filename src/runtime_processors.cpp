#include <cmath>
#include <cstddef>

#include "runtime_processors.hpp"
#include "runtime.hpp"

namespace patch_magic
{
    
void process_sine(runtime_data& rd, const runtime_op& op)
{
    constexpr float two_pi = 2.0f * 3.14159265358979323846f;
    
    auto& st = std::get<sine_state>(rd.states_[op.state_idx_]);
    
    float freq = op.load_arg0_(rd, op.arg0_idx_);
    st.phase_ += two_pi * freq / rd.sample_rate_;
    if (st.phase_ >= two_pi) 
        st.phase_ -= two_pi;
    op.store_(rd, op.store_idx_, std::sin(st.phase_));
}

void process_vol(runtime_data& rd, const runtime_op& op)
{
    float gain = op.load_arg0_(rd, op.arg0_idx_);
    float in   = op.load_arg1_(rd, op.arg1_idx_);
    op.store_(rd, op.store_idx_, in * gain);
}

}
