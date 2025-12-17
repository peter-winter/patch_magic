#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "math.hpp"
#include "note_data.hpp"

namespace patch_magic
{

enum class env_stage : uint8_t
{
    off,
    delay,
    attack,
    hold,
    decay,
    sustain,
    release
};

class env_state
{
public:
    static constexpr float min_attack_sec  = 0.008f;
    static constexpr float min_release_sec = 0.008f;
    static constexpr float min_decay_sec   = 0.008f;

    void capture_parameters(uint32_t sample_rate, float del_t, float att_t, float hold_t, float dec_t, float sus_level, float rel_t)
    {
        sample_rate_ = sample_rate;
        del_t_ = std::max(0.0f, del_t);
        att_t_ = std::max(min_attack_sec, att_t);
        hold_t_ = std::max(0.0f, hold_t);
        rel_t_ = std::max(min_release_sec, rel_t);

        sus_target_ = sus_level;

        dec_t_ = (sus_target_ != 1.0f)
            ? std::max(min_decay_sec, dec_t)
            : std::max(0.0f, dec_t);
    }
    
    void set_active(bool active) { active_ = active; }
    bool active() const { return active_; }

    void reset()
    {
        del_t_ = 0.0f;
        att_t_ = 0.0f;
        hold_t_ = 0.0f;
        dec_t_ = 0.0f;
        sus_target_ = 1.0f;
        rel_t_ = 0.0f;
        stage_ = env_stage::off;
        sus_at_rel_ = 0.0f;
        time_ = 0.0f;
        active_ = false;
    };
    
    float calculate_gain()
    {
        float out = 0.0f;
        
        switch (stage_)
        {
            case env_stage::off:
            {
                if (active_)
                {
                    stage_ = del_t_ > 0.0f ? env_stage::delay : env_stage::attack;
                }
                break;
            }

            case env_stage::delay:
            {
                if (time_ >= del_t_)
                {
                    stage_ = env_stage::attack;
                    time_ -= del_t_;
                }
                break;
            }

            case env_stage::attack:
            {
                out = exp_segment(0.0f, 1.0f, att_t_, hold_t_ > 0.0f ? env_stage::hold : env_stage::decay);
                break;
            }

            case env_stage::hold:
            {
                out = 1.0f;
                if (time_ >= hold_t_)
                {
                    stage_ = env_stage::decay;
                    time_ -= hold_t_;
                }
                break;
            }

            case env_stage::decay:
            {
                out = exp_segment(1.0f, sus_target_, dec_t_, env_stage::sustain);
                break;
            }

            case env_stage::sustain:
            {
                time_ = 0.0f;
                out = sus_target_;

                if (!active_)
                {
                    sus_at_rel_ = out;
                    stage_ = env_stage::release;
                }
                break;
            }

            case env_stage::release:
            {
                if (active_)
                {
                    stage_ = (del_t_ > 0.0f ? env_stage::delay : env_stage::attack);
                    break;
                }

                out = exp_segment(sus_at_rel_, 0.0f, rel_t_, env_stage::off);
                break;
            }
        }

        time_ += 1.0f / sample_rate_;
        
        if (out < 0.0)
            return 0.0f;
        return out;
    }
    
private:
    float exp_segment(float start, float target, float time_sec, env_stage next_stage)
    {
        if (time_sec <= 0.0f)
        {
            stage_ = next_stage;
            return target;
        }

        if (time_ >= time_sec)
        {
            stage_ = next_stage;
            time_ -= time_sec;
            return target;
        }

        float p = time_ / time_sec;
        return start + (target - start) * (1.0f - pow2(1.0f - p));
    }
    
    uint32_t sample_rate_;
    float del_t_{0.0f};
    float att_t_{0.0f};
    float hold_t_{0.0f};
    float dec_t_{0.0f};
    float sus_target_{1.0f};
    float rel_t_{0.0f};

    env_stage stage_{env_stage::off};
    float sus_at_rel_{0.0f};
    
    float time_{0.0f};
    bool active_{false};
};

inline float calc_env_ar(env_state& s, sample_rate_wrapper sample_rate, const note_data& nd, float att_t, float rel_t) noexcept
{
    if (!s.active() && nd.active_)
        s.capture_parameters(sample_rate.value_, 0.0f, att_t, 0.0f, 0.0f, 1.0f, rel_t);
    s.set_active(nd.active_);
    return s.calculate_gain();
}


}

