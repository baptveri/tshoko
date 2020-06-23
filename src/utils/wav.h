#ifndef WAV_H
#define WAV_H

#include <stdio.h>

typedef struct wav_header_
{
  // FileTypeBlocID Definition
  char  FileTypeBlocID[4];   // "RIFF"                      (4 bytes)
  long  TotBlocSize;         // File BlocSizegth - 8 bytes  (4 bytes)
  char  FileFormatID[8];     // "Wavefmt"                   (8 bytes)
  long  BlocSize;            // Remaining BlocSizegth       (4 bytes)
  short AudioFormat;         // Tag (1 = PCM)               (2 bytes)
  short AudioChannels;       // Mono=1 Stereo=2             (2 bytes)
  long  SamplesPerSec;       // No samples/sec              (4 bytes)
  long  AvgBytesPerSec;      // Average bytes/sec           (4 bytes)
  short BlockAlign;          // Block align                 (2 bytes)
  short BitsPerSample;       // 8 or 16 bit                 (2 bytes)
  char  DataID[4];           // "data"                      (4 bytes)
  long  DataBlocSize;        // Raw data BlocSizegth        (4 bytes)
  // Total header size = 44 bytes
} wav_header_t;

int wav_is_valid(
    FILE *fid,
    short audioChannels,
    long samplesPerSec,
    short bitsPerSample);

void wav_write_header(
    FILE *fid, 
    short audioChannels,
    long samplesPerSec,
    short bitsPerSample);

void wav_write_length_in_header(
    FILE *fid,
    long nbsample,
    short audioChannels,
    short bitsPerSample);

#endif // WAV_H