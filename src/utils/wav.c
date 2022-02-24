#include "wav.h"

#include <string.h>

// Check wavefile format
//  Return 0 if the wavefile matches the
//  sampling Frequency, Number of channels and bits per samples given in input
int wav_is_valid(
    FILE* fid,
    short audioChannels,
    int samplesPerSec,
    short bitsPerSample)
{
    wav_header_t wav_header;

  // Read the header
  fread((char *)&wav_header, 1, sizeof(wav_header), fid);

  // Check the format
  if(wav_header.SamplesPerSec != samplesPerSec)
  {
    printf("Error: wavefile is not sampled at %dHz !", (int)samplesPerSec);
    return 1;
  }
  if(wav_header.AudioChannels != audioChannels)
  {
    printf("Error: wavefile does not have %d channels !", audioChannels);
    return 1;
  }
  if(wav_header.BitsPerSample != bitsPerSample)
  {
    printf("Error: wavefile is not %d bits !", bitsPerSample);
    return 1;
  }
  return 0;
}


// Write a wavefile header
// SIZE of data is NOT precised
void wav_write_header(
    FILE* fid,
    short audioChannels,
    int samplesPerSec,
    short bitsPerSample)
{
    wav_header_t wav_header;

    strncpy(wav_header.FileTypeBlocID, "RIFF", 4);
    wav_header.TotBlocSize = 0;
    strncpy(wav_header.FileFormatID, "WAVEfmt ", 8);
    wav_header.BlocSize = 16;          // 16 for PCM
    wav_header.AudioFormat = 1;    // 1 for PCM
    wav_header.AudioChannels = audioChannels;
    wav_header.SamplesPerSec = samplesPerSec;

    wav_header.AvgBytesPerSec = samplesPerSec * audioChannels * bitsPerSample / 8;
    wav_header.BlockAlign = audioChannels * bitsPerSample / 8;
    wav_header.BitsPerSample = bitsPerSample;
    strncpy(wav_header.DataID, "data", 4);
    wav_header.DataBlocSize = 0;

    fwrite(&wav_header, sizeof(wav_header), 1, fid);
}

// Write length info in wavefile's header
void wav_write_length_in_header(
    FILE* fid,
    int nbsample,
    short audioChannels,
    short bitsPerSample)
{
    int length1;
    int length2;

    int ptr;

    length1 = nbsample * audioChannels * bitsPerSample + 4 + 16 + 8 + 8;
    // nbsample*nbchannel*nbbitspersample/8
    length2 = nbsample * audioChannels * bitsPerSample / 8; 

    // Write total length
    fseek(fid, 4, SEEK_SET);
    fwrite(&length1, sizeof(int), 1, fid);

    // Write data length
    fseek(fid, 40, SEEK_SET);
    fwrite(&length2, sizeof(int), 1, fid);
}