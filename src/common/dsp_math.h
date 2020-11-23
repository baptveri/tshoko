// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef DSP_MATH_H
#define DSP_MATH_H

#include <math.h>

#include "dsp_types.h"

#define Q23_MAX_VALUE ((1<<23)-1)
#define Q15_MAX_VALUE ((1<<15)-1)

#define Q15_TO_FLOAT(val) (((float32_t)val)/(float32_t)Q15_MAX_VALUE)
#define FLOAT_TO_Q15(val) ((int16_t)(val*(float32_t)Q15_MAX_VALUE))
#define Q23_TO_FLOAT(val) (((float32_t)val)/(float32_t)Q23_MAX_VALUE)
#define FLOAT_TO_Q23(val) ((int32_t)(val*(float32_t)Q23_MAX_VALUE))

#define M_EPSILON (0.000001f)

#define M_SQRT(a) (sqrtf(a))
#define M_MAX(a,b) (a<b?b:a)
#define M_MIN(a,b) (a<b?a:b)
#define M_ABS(a) (a<0?-a:a)
#define M_SQR(a) (a*a)

#define M_PI (3.1415927410125732421875f)

float32_t db2lin(float32_t val_db);
float32_t lin2db(float32_t val);
float32_t smooth2z0(float32_t val, float32_t samplerate);

#endif // DSP_MATH_H
