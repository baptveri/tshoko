#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sndfile.h"
#include <common/cmath.h>
#include <utils/wav.h>

#include "modules/file_saver/file_saver.h"

#define BUFFER_SIZE (1024)

#define BYTE 8
#define SF_FORMAT_TO_BITRES(format) ((format & 0xF) * BYTE)

#define DISPLAY_PROGRESS

// Available options
const char opt_help = 'h';
const char opt_input = 'i';
const char opt_output = 'o';

// Local functions declaration
int32_t parseOptions(
    int argc,
    char *argv[],
    char **inputFileName,
    char **outputFileName);

// Main
int main(int argc, char *argv[])
{
    // Local Variables
    char *inFileName;
    char *outFileName;
    SNDFILE *inFile;
    SF_INFO inFileInfo;
    float32_t *fileInputBuffer = NULL;
    float32_t *fileOutputBuffer = NULL;
    float32_t **inputBuffers = NULL;
    float32_t **outputBuffers = NULL;
    uint32_t bufferSize = BUFFER_SIZE;
    int64_t nbReadSamples;
    int64_t totalSamplesRead = 0;
    int32_t frameIdx = 0;
    uint32_t sampleIdx = 0;
    uint32_t size;
    file_saver_t* p_file_saver;
    bool endOfFile = false;

    // Configuration according to command line arguments
    int32_t res;
    res = parseOptions(argc, argv, &inFileName, &outFileName);
    if(res)
    {
        return 0;
    }

    // Open wav file
    inFile = sf_open(inFileName, SFM_READ, &inFileInfo);

    if (inFile == 0)
    {
        printf("Cannot open %s \n", inFileName);
        return -1;
    }

    // Create metasoundquali
    uint32_t nbChannels = (uint32_t)inFileInfo.channels;
    uint64_t nbSamples = (uint64_t)inFileInfo.frames;
    uint32_t sampleRate = (uint32_t)inFileInfo.samplerate;

    // Allocate memory for local buffer
    size = bufferSize * nbChannels * sizeof(float32_t);
    fileInputBuffer = (float32_t*)malloc(size);
    size = nbSamples * nbChannels * sizeof(float32_t);
    fileOutputBuffer = (float32_t*)malloc(size);

    inputBuffers = (float32_t**)malloc(nbChannels * sizeof(float32_t*));
    outputBuffers = (float32_t**)malloc(nbChannels * sizeof(float32_t*));
    for(uint32_t chIdx = 0; chIdx < nbChannels; chIdx++)
    {
        size = bufferSize * sizeof(float32_t);
        inputBuffers[chIdx]  = (float32_t*)malloc(size);
        outputBuffers[chIdx] = (float32_t*)malloc(size);
    }

    // Initialize processing
    size = file_saver_get_size();
    p_file_saver = (file_saver_t *) malloc(size);
    file_saver_init(p_file_saver);

#ifdef DISPLAY_PROGRESS
    int8_t prevPercent = 0;
#endif

    // Processing
    while(!endOfFile)
    {
        nbReadSamples = sf_readf_float(inFile, fileInputBuffer, bufferSize);
        totalSamplesRead += nbReadSamples;
        
        if((int64_t)totalSamplesRead >= nbSamples)
        {
            endOfFile = true;
        }

        deinterleave(
            inputBuffers, 
            fileInputBuffer, 
            nbChannels, 
            (uint32_t)nbReadSamples);

        file_saver_process(
            p_file_saver,
            inputBuffers,
            outputBuffers,
            nbReadSamples);

        // memcpy(
        //     outputBuffers[0],
        //     inputBuffers[0],
        //     nbReadSamples*sizeof(float32_t));

        // memcpy(
        //     outputBuffers[1],
        //     inputBuffers[1],
        //     nbReadSamples*sizeof(float32_t));

        interleave(
            &fileOutputBuffer[sampleIdx],
            outputBuffers,
            nbChannels,
            (uint32_t)nbReadSamples);

        // Display progress bar
#ifdef DISPLAY_PROGRESS
        uint8_t barLength = 40;
        float32_t progress = (float32_t)totalSamplesRead / (float32_t)nbSamples;
        uint8_t width = (uint8_t)(progress * barLength);
        int8_t percent = (int8_t)(progress * 100.0);

        if(percent > prevPercent)
        {
            printf("\rProcessing: [");
            for (uint8_t i = 0; i < barLength; ++i)
            {
                if(i < width)
                {
                    printf("=");
                }
                else if(i == width)
                {
                    printf(">");
                }
                else
                {
                    printf(" ");
                }  
            }
            printf("] %d%%", (int)percent);

            prevPercent = percent;
        }
#endif

        // Update frame index
        frameIdx++;
        sampleIdx += (uint32_t)nbReadSamples * nbChannels;
    }

    SNDFILE *outFile;
    SF_INFO outFileInfo;
    outFileInfo.frames = inFileInfo.frames;
    outFileInfo.samplerate = inFileInfo.samplerate;
    outFileInfo.channels = (int)nbChannels;
    outFileInfo.format = inFileInfo.format;
    outFile = sf_open(outFileName, SFM_WRITE, &outFileInfo);
    sf_write_float(
        outFile,
        fileOutputBuffer,
        (sf_count_t) (nbSamples * nbChannels));
    sf_write_sync(outFile);
    sf_close(outFile);
    sf_close(inFile);

    // Free Memory
    for(uint32_t chIdx = 0; chIdx < nbChannels; chIdx++)
    {
        free(inputBuffers[chIdx]);
        free(outputBuffers[chIdx]);
    }
    free(inputBuffers);
    free(outputBuffers);

    free(fileInputBuffer);
    free(fileOutputBuffer);

    // Newline
#ifdef DISPLAY_PROGRESS
    printf("\r");
    for (uint8_t i = 0; i < 60; ++i)
    {
        printf(" ");
    }
    printf("\rDone\n");
#endif

    return 0;
}

// Local functions implementation
int32_t parseOptions(int argc,
    char *argv[],
    char **inputFileName,
    char **outputFileName)
{
    *inputFileName = 0;
    *outputFileName = 0;

    // No argument and help display 
    if(argc == 1)
    {
        printf("%s -%s inputfile -%s outputfile\n",
            argv[0], &opt_input, &opt_output);
        printf("%s -%s for details\n\n", argv[0], &opt_help);
        // std::cout << argv[0] << " " 
        //             << "-" << opt_input << " inputfile "
        //             << "-" << opt_output << " outputfile" << std::endl
        //             << argv[0] << " -" << opt_help << " for details" << std::endl
        //             << std::endl;
        return -1;
    }
    else if(argc == 2 
        && argv[1][0] == '-'
        && argv[1][1] == opt_help
        && argv[1][2] == 0)
    {
        printf("-%s input: input audio file\n", &opt_input);
        printf("-%s ouput: output audio file\n", &opt_output);
        // std::cout << '-' << opt_input << " input: input audio file"
        //           << std::endl
        //           << '-' << opt_output << " ouput: output audio file"
        //           << std::endl;
        return -1;
    }

    // Loop over arguments
    for(int ai = 1; ai < argc; ai++)
    {
        bool ok = true;
        char** dest = 0;
        if(*argv[ai] == '-' && argv[ai][1] && !argv[ai][2])
        {
            switch (argv[ai][1])
            {
            case opt_input:
                dest = inputFileName;
                break;
            case opt_output:
                dest = outputFileName;
                break;
            default:
                ok = false;
            }
            if(!ok)
            {
                fprintf(stderr, "Unexpected argument %s\n", argv[ai]);
                // std::cerr << "Unexpected argument " << argv[ai] << std::endl;
                return -1;
            }
            if(dest)
            {
                if (*dest)
                {
                    fprintf(stderr, "%s already specified\n", argv[ai]);
                    //std::cerr << argv[ai] << " already specified" << std::endl;
                    return -1;
                }
                if (++ai == argc || !*argv[ai])
                {
                    fprintf(
                        stderr,
                        "Missing argument after -%s\n",
                        argv[ai - 1]);
                    //std::cerr << "Missing argument after -"
                    //          << argv[ai - 1] << std::endl;
                    return -1;
                }
                *dest = argv[ai];
            }

        }
    }

    // Check for missing io files arguments
    if(!*inputFileName)
    {
        fprintf(stderr, "Mising input file argument\n");
        //std::cerr << "Mising input file argument" << std::endl;
        return -1;
    }
    if(!*outputFileName)
    {
        fprintf(stderr, "Mising output file argument\n");
        //std::cerr << "Mising output file argument" << std::endl;
        return -1;
    }

    return 0;
}
