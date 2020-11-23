// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef AUTO_LEVEL_H
#define AUTO_LEVEL_H

#include <common/dsp_error.h>
#include <common/dsp_types.h>
#include <modules/tools/biquad.h>

// Structure definitions
typedef struct auto_level
{
    // UI Parameters
    uint8_t bypass;
    float32_t z0_rms;
    float32_t noise_level;
    uint32_t noise_time_sample;
    float32_t rms_target;
    float32_t max_gain;
    float32_t min_gain;
    float32_t z0_slow_gain;
    float32_t z0_release;
    float32_t comp_threshold;

    // Filter Parameters
    biquad_config_t bass_filter_config;
    biquad_config_t treble_filter_config;
    biquad_param_t bass_filter_param;
    biquad_param_t treble_filter_param;

    // States
    float32_t rms2_l;
    float32_t rms2_r;
    uint32_t noise_counter;
    float32_t slow_gain;
    float32_t env_l;
    float32_t env_r;
    biquad_state_t bass_filter_stateL;
    biquad_state_t bass_filter_stateR;
    biquad_state_t treble_filter_stateL;
    biquad_state_t treble_filter_stateR;
}auto_level_t;

// Enums
typedef enum
{
    AUTO_LEVEL_BYPASS = 0,
    AUTO_LEVEL_Z0_RMS,
    AUTO_LEVEL_NOISE_LEVEL,
    AUTO_LEVEL_NOISE_TIME_SAMPLE,
    AUTO_LEVEL_RMS_TARGET,
    AUTO_LEVEL_MAX_GAIN,
    AUTO_LEVEL_MIN_GAIN,
    AUTO_LEVEL_Z0_GAIN,
    AUTO_LEVEL_COMP_RELEASE,
    AUTO_LEVEL_COMP_THRESHOLD,

    AUTO_LEVEL_NB_PARAMS
}auto_level_param_id_t;


// API functions
uint32_t auto_level_get_size();

void auto_level_init(auto_level_t* p_auto_level);

void auto_level_process(
    auto_level_t* p_auto_level,
    float32_t** p_in,
    float32_t** p_out,
    uint32_t buffer_size);

error_t auto_level_set(
    auto_level_t* p_auto_level,
    auto_level_param_id_t id,
    void *p_data);

#endif // AUTO_LEVEL_H
