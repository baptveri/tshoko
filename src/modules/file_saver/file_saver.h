// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#ifndef FILE_SAVER_H
#define FILE_SAVER_H

#include <common/cerror.h>
#include <common/ctypes.h>



// Structure definitions
typedef struct file_saver
{
    // UI Parameters
    uint8_t bypass;
    uint32_t chunk_size_sample;
    uint32_t flush_interval_sample;   

    // States
    int16_t *flush_buffer;
    uint32_t flush_counter;
    uint32_t flush_idx;
    uint32_t file_idx;
}file_saver_t;

// Enums
typedef enum
{
    FILE_SAVER_BYPASS = 0,
    FILE_SAVER_CHUNK_LENGTH,
    FILE_SAVER_FLUSH_INTERVAL,

    FILE_SAVER_NB_PARAMS
}file_saver_param_id_t;


// API functions
uint32_t file_saver_get_size();

void file_saver_init(file_saver_t* p_file_saver);

void file_saver_process(
    file_saver_t* p_file_saver,
    float32_t** p_in,
    float32_t** p_out,
    uint32_t buffer_size);

error_t file_saver_set(
    file_saver_t* p_file_saver,
    file_saver_param_id_t id,
    void *p_data);

#endif // FILE_SAVER_H
