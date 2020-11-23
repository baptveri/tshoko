// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#include "auto_level.h"

#include <common/dsp_math.h>
#include <modules/tools/biquad.h>

// Defines
#define AUTO_LEVEL_DEFAULT_GAIN 1.f

#define AUTO_LEVEL_DEFAULT_BYPASS 0
#define AUTO_LEVEL_DEFAULT_Z0_RMS (0.99999219f) // = 8 sec at 48kHz
#define AUTO_LEVEL_DEFAULT_NOISE_LEVEL (0.0031622777f) // = -50dBFS
#define AUTO_LEVEL_DEFAULT_NOISE_TIME_SAMPLE (480) // = 10 ms at 48kHz
#define AUTO_LEVEL_DEFAULT_RMS_TARGET (0.158489317f) // = -16dBFS
#define AUTO_LEVEL_DEFAULT_MAX_GAIN (3.98107171f) // = +12 dB
#define AUTO_LEVEL_DEFAULT_MIN_GAIN (0.25118864f) // = -12 dB
#define AUTO_LEVEL_DEFAULT_Z0_GAIN (0.99996875f) // = 2 sec at 48kHz
#define AUTO_LEVEL_DEFAULT_COMP_RELEASE (0.9993752f) // = 0.1 sec at 48kHz
#define AUTO_LEVEL_DEFAULT_COMP_THRESHOLD (1.f) // = 0dBFS

#define AUTO_LEVEL_BASS_FILTER_FREQ         80.f // Hz
#define AUTO_LEVEL_BASS_FILTER_Q            0.5f
#define AUTO_LEVEL_BASS_FILTER_GAIN_DB      0.f  // dB
#define AUTO_LEVEL_BASS_FILTER_TYPE         HIGHPASS

#define AUTO_LEVEL_TREBLE_FILTER_FREQ       1500.f // Hz
#define AUTO_LEVEL_TREBLE_FILTER_Q          1.f
#define AUTO_LEVEL_TREBLE_FILTER_GAIN_DB    4.f  // dB
#define AUTO_LEVEL_TREBLE_FILTER_TYPE       HIGHSHELF

// API functions
uint32_t auto_level_get_size()
{
    return sizeof(auto_level_t);
}

void auto_level_init(auto_level_t* p_auto_level)
{
    // Parameters
    p_auto_level->bypass = AUTO_LEVEL_DEFAULT_BYPASS;
    p_auto_level->z0_rms = AUTO_LEVEL_DEFAULT_Z0_RMS;
    p_auto_level->noise_level = AUTO_LEVEL_DEFAULT_NOISE_LEVEL;
    p_auto_level->noise_time_sample = AUTO_LEVEL_DEFAULT_NOISE_TIME_SAMPLE;
    p_auto_level->rms_target = AUTO_LEVEL_DEFAULT_RMS_TARGET;
    p_auto_level->max_gain = AUTO_LEVEL_DEFAULT_MAX_GAIN;
    p_auto_level->min_gain = AUTO_LEVEL_DEFAULT_MIN_GAIN;
    p_auto_level->z0_slow_gain = AUTO_LEVEL_DEFAULT_Z0_GAIN;
    p_auto_level->z0_release = AUTO_LEVEL_DEFAULT_COMP_RELEASE;
    p_auto_level->comp_threshold = AUTO_LEVEL_DEFAULT_COMP_THRESHOLD;

    // States
    p_auto_level->rms2_l = 0.f;
    p_auto_level->rms2_r = 0.f;
    p_auto_level->noise_counter = 0;
    p_auto_level->slow_gain = AUTO_LEVEL_DEFAULT_GAIN;
    p_auto_level->env_l = 0.f;
    p_auto_level->env_r = 0.f;

    // Filter configurations
    p_auto_level->bass_filter_config.f = AUTO_LEVEL_BASS_FILTER_FREQ;
    p_auto_level->bass_filter_config.q = AUTO_LEVEL_BASS_FILTER_Q;
    p_auto_level->bass_filter_config.g_db = AUTO_LEVEL_BASS_FILTER_GAIN_DB;
    p_auto_level->bass_filter_config.type = AUTO_LEVEL_BASS_FILTER_TYPE;

    p_auto_level->treble_filter_config.f = AUTO_LEVEL_TREBLE_FILTER_FREQ;
    p_auto_level->treble_filter_config.q = AUTO_LEVEL_TREBLE_FILTER_Q;
    p_auto_level->treble_filter_config.g_db = AUTO_LEVEL_TREBLE_FILTER_GAIN_DB;
    p_auto_level->treble_filter_config.type = AUTO_LEVEL_TREBLE_FILTER_TYPE;

    // Init biquads and set coefficients
    biquad_init(
        &p_auto_level->bass_filter_param,
        &p_auto_level->bass_filter_stateL);
    biquad_init(
        &p_auto_level->bass_filter_param,
        &p_auto_level->bass_filter_stateR);
    biquad_init(
        &p_auto_level->treble_filter_param,
        &p_auto_level->treble_filter_stateL);
    biquad_init(
        &p_auto_level->treble_filter_param,
        &p_auto_level->treble_filter_stateR);

    biquad_compute_coeffs(
        &p_auto_level->bass_filter_config,
        &p_auto_level->bass_filter_param);
    biquad_compute_coeffs(
        &p_auto_level->treble_filter_config,
        &p_auto_level->treble_filter_param);
}

void auto_level_process(
    auto_level_t *p_auto_level,
    float32_t **p_in,
    float32_t **p_out,
    uint32_t buffer_size)
{
    float32_t* p_inL = p_in[0];
    float32_t* p_inR = p_in[1];
    float32_t* p_outL = p_out[0];
    float32_t* p_outR = p_out[1];

    uint32_t i;
    float32_t rms;
    float32_t target_gain, fast_gain;
    float32_t peak_l, peak_r;
    float32_t env;

    float32_t rms2_l = p_auto_level->rms2_l;
    float32_t rms2_r = p_auto_level->rms2_r;
    float32_t slow_gain = p_auto_level->slow_gain;
    float32_t env_l = p_auto_level->env_l;
    float32_t env_r = p_auto_level->env_r;

    // Filter input signal with bass and treble filter
    // Output buffer is used as temporary buffer
    biquad_process(
        p_inL, 
        p_outL,
        &p_auto_level->bass_filter_param,
        &p_auto_level->bass_filter_stateL,
        buffer_size);
    biquad_process(
        p_outL,
        p_outL,
        &p_auto_level->treble_filter_param,
        &p_auto_level->treble_filter_stateL,
        buffer_size);

    biquad_process(
        p_inL,
        p_outR,
        &p_auto_level->bass_filter_param,
        &p_auto_level->bass_filter_stateR,
        buffer_size);
    biquad_process(
        p_outR,
        p_outR,
        &p_auto_level->treble_filter_param,
        &p_auto_level->treble_filter_stateR,
        buffer_size);

    for(i = 0; i < buffer_size; i++)
    {
        float32_t inL = *p_inL++;
        float32_t inR = *p_inR++;
        float32_t side_chainL = *p_outL;
        float32_t side_chainR = *p_outR;

        // Compute rms
        rms2_l = M_SQR(side_chainL)
            - p_auto_level->z0_rms * (M_SQR(side_chainL) - rms2_l);
        rms2_r = M_SQR(side_chainR)
            - p_auto_level->z0_rms * (M_SQR(side_chainR) - rms2_r);

        rms = M_SQRT(p_auto_level->rms2_l + p_auto_level->rms2_r);

        // Compute Slow Gain
        if(rms < p_auto_level->noise_level)
        {
            if (p_auto_level->noise_counter < p_auto_level->noise_time_sample)
            {
                p_auto_level->noise_counter++;
            }
        }
        else
        {
            p_auto_level->noise_counter = 0;;
        }

        if(p_auto_level->noise_counter < p_auto_level->noise_time_sample)
        {
            target_gain = p_auto_level->rms_target / (rms + M_EPSILON);
        }
        else
        {
            target_gain = 1.f;
        }

        // Clip Gain
        target_gain = M_MAX(target_gain, p_auto_level->min_gain);
        target_gain = M_MIN(target_gain, p_auto_level->max_gain);

        // Smooth Gain
        slow_gain = target_gain  
            - p_auto_level->z0_slow_gain * (target_gain - slow_gain);

        // Compute Fast Gain
        peak_l = M_ABS(inL);
        peak_r = M_ABS(inR);

        if(peak_l > env_l)
        {
            env_l = peak_l;
        }
        else
        {
            env_l = p_auto_level->z0_release * env_l;
        }

        if(peak_r > env_r)
        {
            env_r = peak_r;
        }
        else
        {
            env_r = p_auto_level->z0_release * env_r;
        }

        env = M_MAX(env_l, env_r) * slow_gain;
        if(env > p_auto_level->comp_threshold)
        {
            fast_gain = p_auto_level->comp_threshold / env;
        }
        else
        {
            fast_gain = 1;
        }

        // Compute Output
        *p_outL++ = inL * slow_gain * fast_gain;
        *p_outR++ = inR * slow_gain * fast_gain;
    }

    // Save States
    p_auto_level->rms2_l = rms2_l;
    p_auto_level->rms2_r = rms2_r;
    p_auto_level->slow_gain = slow_gain;
    p_auto_level->env_l = env_l;
    p_auto_level->env_r = env_r;

    return;
}

error_t auto_level_set(
    auto_level_t* p_auto_level,
    auto_level_param_id_t id,
    void* p_data)
{
    error_t error = DSP_NO_ERROR;

    switch(id)
    {
    case AUTO_LEVEL_BYPASS:
        p_auto_level->bypass = *(uint8_t*)p_data;
        break;
    case AUTO_LEVEL_Z0_RMS:
        p_auto_level->z0_rms =*(float32_t *)p_data;
        break;
    case AUTO_LEVEL_NOISE_LEVEL:
        p_auto_level->noise_level = *(float32_t*)p_data;
        break;
    case AUTO_LEVEL_NOISE_TIME_SAMPLE:
        p_auto_level->noise_time_sample = *(uint32_t*)p_data;
        break;
    case AUTO_LEVEL_RMS_TARGET:
        p_auto_level->rms_target = *(float32_t*)p_data;
        break;
    case AUTO_LEVEL_MAX_GAIN:
        p_auto_level->max_gain = *(float32_t*)p_data;
        break;
    case AUTO_LEVEL_MIN_GAIN:
        p_auto_level->min_gain = *(float32_t*)p_data;
        break;
    case AUTO_LEVEL_Z0_GAIN:
        p_auto_level->z0_slow_gain = *(float32_t*)p_data;
        break;
    case AUTO_LEVEL_COMP_RELEASE:
        p_auto_level->z0_release = *(float32_t*)p_data;
        break;
    case AUTO_LEVEL_COMP_THRESHOLD:
        p_auto_level->comp_threshold = *(float32_t*)p_data;
        break;
    default:
        return DSP_PARAM_ID_ERROR;
    }

    return error;
}
