// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef DSP_TYPES_H
#define DSP_TYPES_H

#ifndef _WIN32
#include <adi_types.h>
#else
#include <stdint.h>
#include <stdbool.h>

typedef float  float32_t;
typedef double float64_t;
#endif

#endif // DSP_TYPES_H
