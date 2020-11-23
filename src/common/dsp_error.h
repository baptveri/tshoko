// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef DSP_ERROR_H
#define DSP_ERROR_H

typedef enum
{
	DSP_NO_ERROR           = 0x0000,

	DSP_PARAM_ID_ERROR     = 0x0100,
	DSP_PARAM_VALUE_ERROR  = 0x0101,

	BIQUAD_TYPE_ERROR      = 0x0200,
} error_t;

#endif // DSP_ERROR_H
