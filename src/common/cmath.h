// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef CMATH_H
#define CMATH_H

#include <math.h>

#include <common/ctypes.h>

#define Q15_MAX_VALUE ((1<<15)-1)

#define Q15_TO_FLOAT(val) (((float32_t)val)/(float32_t)Q15_MAX_VALUE)
#define FLOAT_TO_Q15(val) ((int16_t)(val*(float32_t)Q15_MAX_VALUE))

#define M_EPSILON (0.000001f)

#define M_SQRT(a) (sqrtf(a))
#define M_MAX(a,b) (a<b?b:a)
#define M_MIN(a,b) (a<b?a:b)
#define M_ABS(a) (a<0?-a:a)
#define M_SQR(a) (a*a)

#endif // CMATH_H