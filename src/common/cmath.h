// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef CMATH_H
#define CMATH_H

#include <common/ctypes.h>

#define Q15_MAX_VALUE ((1<<15)-1)

#define Q15_TO_FLOAT(val) (((float32_t)val)/(float32_t)Q15_MAX_VALUE)
#define FLOAT_TO_Q15(val) ((int16_t)(val*(float32_t)Q15_MAX_VALUE))

#endif // CMATH_H