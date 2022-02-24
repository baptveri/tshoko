// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef CMATH_H
#define CMATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

#include <common/ctypes.h>

#include <math.h>
#include <float.h>

#define Q15_MAX_VALUE ((1<<15)-1)

#define Q15_TO_FLOAT(val) (((float32_t)val)/(float32_t)Q15_MAX_VALUE)
#define FLOAT_TO_Q15(val) ((int16_t)(val*(float32_t)Q15_MAX_VALUE))

#define M_EPSILON (0.000001f)

#define M_32BIT 32
#define M_24BIT 24
#define M_16BIT 16

#define M_MAX(a,b) ((a)<(b)?(b):(a))
#define M_MIN(a,b) ((a)<(b)?(a):(b))
#define M_SGN(a)   ((a)>=0?1:-1)
#define M_ABS(a)   ((a)>=0?(a):-(a))
#define M_SQR(a)   ((a)*(a))
#define M_SQRT(a)  (sqrtf(a))
#define M_RND(a)   (roundf(a))

#define M_MAX_INT(prec) ((1 << ((prec)-1)) - 1)
#define M_INT_TO_FLOAT(val, prec) (((float32_t)val)/(float32_t)M_MAX_INT(prec))

#ifndef M_PI
#define M_PI (3.1415927410125732421875f)
#endif

#define M_DB_MIN    (-888.f)
#define M_DENORM_TH (FLT_MIN)

// Math functions
float32_t db2lin(float32_t val);
float32_t lin2db(float32_t val);
float32_t smooth2z0(float32_t val, float32_t sampleRate);
float32_t ms2samples(float32_t ms, float32_t sampleRate);
float32_t samples2ms(uint32_t samples, float32_t sampleRate);
uint8_t bincount(uint32_t value);

void deinterleave(
    float32_t **dest,
    float32_t *src,
    uint8_t nbChannels,
    uint32_t nbSamples);

void interleave(
    float32_t *dest,
    float32_t **src,
    uint8_t nbChannels,
    uint32_t nbSamples);

// Vectorial operations
static inline void v_add(
    float32_t *dst, float32_t *src1, float32_t *src2, int32_t n)
{
    while(n-- > 0)
        *(dst++) = *(src1++) + *(src2++);
}

static inline void v_mult(
    float32_t *dst, float32_t *src, float32_t val, int32_t n)
{
    while(n-- > 0)
        *(dst++) = *(src++) * val;
}

static inline float32_t v_sum(float32_t *src, int32_t n)
{
    float32_t sum = 0.f;
    while(n-- > 0)
        sum += *(src++);
    return sum;
}

static inline void v_cpy(float32_t *dst, float32_t *src, int32_t n)
{
    while(n-- > 0)
        *(dst++) = *(src++);
}

static inline float32_t v_min(float32_t *src, int32_t n)
{
    float32_t min = FLT_MAX;
    float32_t val;
    while(n-- > 0)
    {
        val = *(src++);
        min = M_MIN(val, min);
    }
    return min;
}

static inline float32_t v_max(float32_t *src, int32_t n)
{
    float32_t max = FLT_MIN;
    float32_t val;
    while(n-- > 0)
    {
        val = *(src++);
        max = M_MAX(val, max);
    }
    return max;
}

static inline void v_max_vect(
    float32_t *dst, float32_t *src1, float32_t *src2, int32_t n)
{
    float32_t val1, val2;
    while(n-- > 0)
    {
        val1 = *(src1++);
        val2 = *(src2++);
        *(dst++) = M_MAX(val1, val2);
    }    
}

static inline void v_set(float32_t *src, float32_t value, uint32_t n)
{
    while(n-- > 0)
        *(src++) = value;
}

void v_sort(float32_t *src, int32_t n);

#ifdef __cplusplus
}
#endif

#endif // CMATH_H