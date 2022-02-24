/*
Authors: Baptiste Vericel <baptiste.vericel@gmail.com>

Copyright (c) Ircam Amplify 2022
*/

#include "cmath.h"

// Math functions
float32_t db2lin(float32_t val)
{
    return powf(10.f, val/20.f);
}

float32_t lin2db(float32_t val)
{
    float32_t db = val==0.f ? M_DB_MIN : 20.f*log10f(val);
    return db;
}

float32_t smooth2z0(float32_t val, float32_t sampleRate)
{
    return expf( -3.f / (val*sampleRate) );
}

float32_t ms2samples(float32_t ms, float32_t sampleRate)
{
    return ms * sampleRate / 1000.f;
}

float32_t samples2ms(uint32_t samples, float32_t sampleRate)
{
    return (float32_t)samples * 1000.f / sampleRate;
}

void deinterleave(
    float32_t **dest,
    float32_t *src,
    uint8_t nbChannels,
    uint32_t nbSamples)
{
    for(uint8_t ch=0; ch<nbChannels; ch++)
    {
        float32_t* chBuffer = dest[ch];
        for(uint32_t i=0; i<nbSamples; i++)
        {
            chBuffer[i] = src[i*nbChannels + ch];
        }
    }
}

void interleave(
    float32_t *dest,
    float32_t **src,
    uint8_t nbChannels,
    uint32_t nbSamples)
{
    for(uint32_t ch=0; ch<nbChannels; ch++)
    {
        float32_t* chBuffer = src[ch];
        for(uint32_t i=0; i<nbSamples; i++)
        {
            float32_t inputValue = chBuffer[i];
            dest[i*nbChannels + ch] = inputValue;
        }
    }
}

void v_sort(float32_t *src, int32_t n)
{
    int32_t i, j;
    float32_t key;
    for (i = 1; i < n; i++)
    {
        key = src[i];
        j = i - 1;
  
        /* Move elements of src[0..i-1], that are
        greater than key, to one position ahead
        of their current position */
        while (j >= 0 && src[j] > key)
        {
            src[j + 1] = src[j];
            j = j - 1;
        }
        src[j + 1] = key;
    }
}
