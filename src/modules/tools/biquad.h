// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef BIQUAD_H
#define BIQUAD_H

#include <common/dsp_error.h>
#include <common/dsp_math.h>
#include <common/dsp_types.h>

// Enums
typedef enum
{
    FLAT = 0,
    LOWPASS,
    HIGHPASS,
    BANDPASS,
    PEAK,
    LOWSHELF,
    HIGHSHELF,
    ALLPASS,

    BIQUAD_NB_TYPE
} biquad_type_t;

// Structure Definitions
typedef struct biquad_param
{
    float32_t b0;
    float32_t b1;
    float32_t b2;
    float32_t a1;
    float32_t a2;
}biquad_param_t;

typedef struct biquad_state
{
    float32_t x1;
    float32_t x2;
    float32_t y1;
    float32_t y2;
}biquad_state_t;

typedef struct biquad_config
{
    float32_t f;
    float32_t q;
    float32_t g_db;
    biquad_type_t type;
}biquad_config_t;

// Functions
void biquad_init(biquad_param_t *p_param, biquad_state_t *p_state);
void biquad_set(biquad_param_t *p_param_in, biquad_param_t *p_param_out);

void biquad_process(
    float32_t *p_in,
    float32_t *p_out,
    biquad_param_t *p_param,
    biquad_state_t *p_state,
    uint32_t buffer_size);

error_t biquad_compute_coeffs(
    biquad_config_t *p_config,
    biquad_param_t *p_param);

#endif // BIQUAD_H
