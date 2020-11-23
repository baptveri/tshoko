// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#include "biquad.h"

#include <common/common.h>
#include <common/dsp_types.h>

// API functions
void biquad_init(biquad_param_t *p_param, biquad_state_t *p_state)
{
    // Init coefficients (flat config)
    p_param->b0 = 1.f;
    p_param->b1 = 0.f;
    p_param->b2 = 0.f;
    p_param->a1 = 0.f;
    p_param->a2 = 0.f;

    // Init states
    p_state->x1 = 0.f;
    p_state->x2 = 0.f;
    p_state->y1 = 0.f;
    p_state->y2 = 0.f;
}

void biquad_process(
    float32_t *p_in,
    float32_t *p_out,
    biquad_param_t *p_param,
    biquad_state_t *p_state,
    uint32_t buffer_size)
{
    uint32_t i;
    float32_t x, y;

    float32_t x1 = p_state->x1;
    float32_t x2 = p_state->x2;
    float32_t y1 = p_state->y1;
    float32_t y2 = p_state->y2;

    float32_t b0 = p_param->b0;
    float32_t b1 = p_param->b1;
    float32_t b2 = p_param->b2;
    float32_t a1 = p_param->a1;
    float32_t a2 = p_param->a2;

    for(i=0; i<buffer_size; i++)
    {
        x = *p_in++;
        y  = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2;

        *p_out++ = y;
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;
    }

    p_state->x1 = x1;
    p_state->x2 = x2;
    p_state->y1 = y1;
    p_state->y2 = y2;
}

void biquad_set(biquad_param_t *p_param_in, biquad_param_t *p_param_out)
{
    p_param_out->b0 = p_param_in->b0;
    p_param_out->b1 = p_param_in->b1;
    p_param_out->b2 = p_param_in->b2;
    p_param_out->a1 = p_param_in->a1;
    p_param_out->a2 = p_param_in->a2;
}

error_t biquad_compute_coeffs(
    biquad_config_t *p_config,
    biquad_param_t *p_param)
{
    error_t error = DSP_NO_ERROR;

    float32_t wc = 2.f * M_PI * p_config->f / SAMPLE_RATE;
    float32_t q  = p_config->q;
    float32_t g_db = p_config->g_db;
    float32_t b0, b1, b2, a0, a1, a2;

    switch(p_config->type)
    {
    // Flat:
    //
    // H(p) = 1
    //
    case FLAT:
        b0 = 1.f;
        b1 = 0.f;
        b2 = 0.f;
        a1 = 0.f;
        a2 = 0.f;
        break;

    // Lowpass:
    //                 wc^2
    //  H(p) = ---------------------
    //          p^2 + wc/q p + wc^2
    case LOWPASS:
    {
        float32_t alpha = sinf(wc)/(2.f*q);
        float32_t cos_wc = cosf(wc);

        b0 = (1.f - cos_wc) / 2.f;
        b1 = (1.f - cos_wc);
        b2 = (1.f - cos_wc) / 2.f;

        a0 =  1.f + alpha;
        a1 = -2.f * cos_wc;
        a2 =  1.f - alpha;

        break;
    }

    // Highpass:
    //                 p^2
    //  H(p) = ---------------------
    //          p^2 + wc/q p + qc^2
    case HIGHPASS:
    {
        float32_t alpha = sinf(wc) / (2.f*q);
        float32_t cos_wc = cosf(wc);

        b0 =  (1.f + cos_wc) / 2.f;
        b1 = -(1.f + cos_wc);
        b2 =  (1.f + cos_wc) / 2.f;

        a0 =  1.f + alpha;
        a1 = -2.f * cos_wc;
        a2 =  1.f - alpha;

        b0 *= 2.f * (1.f + alpha) / (1.f + cos_wc);
        b1 *= 2.f * (1.f + alpha) / (1.f + cos_wc);
        b2 *= 2.f * (1.f + alpha) / (1.f + cos_wc);

        a0 *= 2.f * (1.f + alpha) / (1.f + cos_wc);
        a1 *= 2.f * (1.f + alpha) / (1.f + cos_wc);
        a2 *= 2.f * (1.f + alpha) / (1.f + cos_wc);

        break;
    }

    // Bandpass:
    //                wc/q p
    //  H(p) = ---------------------
    //          p^2 + wc/q p + wc^2
    case BANDPASS:
    {
        float32_t alpha = sinf(wc) / (2.f*q);

        b0 =        alpha;  b1 =  0.f          ;  b2 =     - alpha;
        a0 =  1.f + alpha;  a1 = -2.f * cosf(wc);  a2 = 1.f - alpha;

        break;
    }

    // Peak:
    //          p^2 + a wc/q p + wc^2
    //  H(p) = -----------------------
    //          p^2 + wc/a*q p + wc^2
    case PEAK:
    {
        float32_t a = sqrtf(db2lin(g_db));
        float32_t alpha = sinf(wc) / (2.f*q);

        b0 = 1 + a*alpha;  b1 = -2*cosf(wc);  b2 = 1 - a*alpha;
        a0 = 1 + alpha/a;  a1 = -2*cosf(wc);  a2 = 1 - alpha/a;

        break;
    }

    // LowShelf:
    //            p^2 + sqrt(a) wc/s p + a wc^2
    //  H(p) = a -------------------------------
    //            a p^2 + sqrt(a) wc/s p + wc^2
    case LOWSHELF:
    {
        float32_t a = sqrtf(db2lin(g_db));
        float32_t s = 1.f/sqrtf((a + 1.f/a) * (1.f/q - 1.f) + 2.f);
        float32_t alpha = sqrtf(a) * sinf(wc) / s;
        float32_t cos_wc = cosf(wc);

        b0 =      a*((a+1.f) - (a-1.f)*cos_wc + alpha);
        b1 =  2.f*a*((a-1.f) - (a+1.f)*cos_wc        );
        b2 =      a*((a+1.f) - (a-1.f)*cos_wc - alpha);

        a0 =        ((a+1.f) + (a-1.f)*cos_wc + alpha);
        a1 =   -2.f*((a-1.f) + (a+1.f)*cos_wc        );
        a2 =        ((a+1.f) + (a-1.f)*cos_wc - alpha);

        break;
    }

    // HighShelf:
    //            p^2 + sqrt(a) wc/s p + a wc^2
    //  H(p) = a -------------------------------
    //            a p^2 + sqrt(a) wc/s p + wc^2
    case HIGHSHELF:
    {
        float32_t a = sqrtf(db2lin(g_db));
        float32_t s = 1.f/sqrtf((a + 1.f/a) * (1.f/q - 1.f) + 2.f);
        float32_t alpha = sqrtf(a) * sinf(wc) / s;
        float32_t cos_wc = cosf(wc);

        b0 =      a*((a+1.f) + (a-1.f)*cos_wc + alpha);
        b1 = -2.f*a*((a-1.f) + (a+1.f)*cos_wc        );
        b2 =      a*((a+1.f) + (a-1.f)*cos_wc - alpha);

        a0 =        ((a+1.f) - (a-1.f)*cos_wc + alpha);
        a1 =    2.f*((a-1.f) - (a+1.f)*cos_wc        );
        a2 =        ((a+1.f) - (a-1.f)*cosf(wc) - alpha);

        break;
    }

    // Allpass:
    //          p^2 + -wc/q p + wc^2
    //  H(p) = ----------------------
    //          p^2 +  wc/q p + wc^2
    case ALLPASS:
    {
        float32_t alpha = sinf(wc) / (2.f*q);
        float32_t cos_wc = cosf(wc);

        b0 = 1.f - alpha;  b1 = -2.f*cos_wc;  b2 = 1.f + alpha;
        a0 = 1.f + alpha;  a1 = -2.f*cos_wc;  a2 = 1.f - alpha;

        break;
    }

    default:
        error = BIQUAD_TYPE_ERROR;
    }

    p_param->b0 = b0/a0;
    p_param->b1 = b1/a0;
    p_param->b2 = b2/a0;
    p_param->a1 = a1/a0;
    p_param->a2 = a2/a0;

    return error;
}
