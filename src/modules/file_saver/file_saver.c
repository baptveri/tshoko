// Copyright (c) 2022 All Rights Reserved Baptiste Vericel

#include "file_saver.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <common/cmath.h>
#include <utils/wav.h>

// Defines
#define FILE_SAVER_DEFAULT_BYPASS 0

#define FILE_SAVER_NB_CHANNELS          (2)     // Stereo
#define FILE_SAVER_DEFAULT_SR           (44100) // Hz
#define FILE_SAVER_BIT_PER_SAMPLES      (16)    // bits
#define FILE_SAVER_DEFAULT_CHUNK_SEC    (5)    // sec
#define FILE_SAVER_DEFAULT_FLUSH_SEC    (30)   // sec

#define FILE_SAVER_DEFAULT_CHUNK_SAMPLE (FILE_SAVER_DEFAULT_SR * FILE_SAVER_DEFAULT_CHUNK_SEC)
#define FILE_SAVER_DEFAULT_FLUSH_SAMPLE (FILE_SAVER_DEFAULT_SR * FILE_SAVER_DEFAULT_FLUSH_SEC)

// Local function declaration
void file_saver_flush(file_saver_t *p_file_saver);

// API functions
uint32_t file_saver_get_size()
{
    return sizeof(file_saver_t);
}

void file_saver_init(file_saver_t* p_file_saver)
{
    uint32_t size;

    // Parameters
    p_file_saver->bypass = FILE_SAVER_DEFAULT_BYPASS;
    p_file_saver->chunk_size_sample = FILE_SAVER_DEFAULT_CHUNK_SAMPLE;
    p_file_saver->flush_interval_sample = FILE_SAVER_DEFAULT_FLUSH_SAMPLE;

    // States
    size = FILE_SAVER_NB_CHANNELS * p_file_saver->chunk_size_sample * sizeof(int16_t);
    p_file_saver->flush_buffer = (int16_t *) malloc(size);

    p_file_saver->flush_counter = 0;
    p_file_saver->flush_idx = 0;
    p_file_saver->file_idx = 0;
}

void file_saver_process(
    file_saver_t *p_file_saver,
    float32_t **p_in,
    float32_t **p_out,
    uint32_t buffer_size)
{
    float32_t* p_inL = p_in[0];
    float32_t* p_inR = p_in[1];
    float32_t* p_outL = p_out[0];
    float32_t* p_outR = p_out[1];
    int16_t * p_flush = p_file_saver->flush_buffer;

    uint32_t counter = p_file_saver->flush_counter;
    uint32_t idx = p_file_saver->flush_idx;
    
    // Copy input to output
    for(int i = 0; i < buffer_size; i++)
    {
        float32_t inL = *p_inL++;
        float32_t inR = *p_inR++;

        *p_outL++ = inL;
        *p_outR++ = inR;

        if(counter >= p_file_saver->flush_interval_sample)
        {
            if(idx < p_file_saver->chunk_size_sample)
            {
                p_flush[2*idx]   = FLOAT_TO_Q15(inL);
                p_flush[2*idx+1] = FLOAT_TO_Q15(inR);
                idx++;
            }
            else
            {
                counter = idx;
                idx = 0;
                file_saver_flush(p_file_saver);
            }
        }
        else
        {
            idx = 0;
            counter++;
        }
    }

    // Save States
    p_file_saver->flush_counter = counter;
    p_file_saver->flush_idx = idx;

    return;
}

error_t file_saver_set(
    file_saver_t* p_file_saver,
    file_saver_param_id_t id,
    void* p_data)
{
    error_t error = E_NO_ERROR;

    switch(id)
    {
    case FILE_SAVER_BYPASS:
        p_file_saver->bypass = *(uint8_t*)p_data;
        break;
    case FILE_SAVER_CHUNK_LENGTH:
        p_file_saver->chunk_size_sample = *(uint32_t*)p_data;
        break;
    case FILE_SAVER_FLUSH_INTERVAL:
        p_file_saver->flush_interval_sample = *(uint32_t*)p_data;
        break;
    default:
        return E_DSP_PARAM_ID_ERROR;
    }

    return error;
}

// Local functions
void file_saver_flush(file_saver_t *p_file_saver)
{
    char year[10];
    char month[10];
    char hour[10];
    char min[10];
    char sec[10];
    char fileidx[10];
    char filename[1024];

    FILE *file;
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(year, "%d", tm.tm_year + 1900);
    sprintf(month, "%d", tm.tm_mon + 1);
    sprintf(hour, "%d", tm.tm_hour);
    sprintf(min, "%d", tm.tm_min);
    sprintf(sec, "%d", tm.tm_sec);
    sprintf(fileidx, "%d", p_file_saver->file_idx);

    p_file_saver->file_idx++;
    
    // Create filename
    strcpy(filename, "pending/");
    strcat(filename, year);
    strcat(filename, "_");
    strcat(filename, month);
    strcat(filename, "_");
    strcat(filename, hour);
    strcat(filename, "_");
    strcat(filename, min);
    strcat(filename, "_");
    strcat(filename, sec);
    strcat(filename, "_");
    strcat(filename, fileidx);
    strcat(filename, ".wav");

    // Create output file
    file = fopen(filename, "wb+");

    wav_write_header(
        file,
        FILE_SAVER_NB_CHANNELS,
        FILE_SAVER_DEFAULT_SR,
        FILE_SAVER_BIT_PER_SAMPLES);

    fwrite(
        p_file_saver->flush_buffer,
        sizeof(int16_t),
        p_file_saver->chunk_size_sample * FILE_SAVER_NB_CHANNELS,
        file);

    wav_write_length_in_header(
        file,
        p_file_saver->chunk_size_sample,
        FILE_SAVER_NB_CHANNELS,
        FILE_SAVER_BIT_PER_SAMPLES);

    fclose(file);
}
