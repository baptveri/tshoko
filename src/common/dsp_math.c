// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#include "dsp_math.h"

// Functions
float32_t db2lin(float32_t val_db)
{
    return powf(10.f, val_db/20.f);
}

float32_t lin2db(float32_t val)
{
    return 20.f*log10f(val);
}

float32_t smooth2z0(float32_t val, float32_t samplerate)
{
    return expf( -3.f / (val*samplerate) );
}
