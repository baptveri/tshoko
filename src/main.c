#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <common/common.h>
#include <common/dsp_math.h>
#include <common/dsp_types.h>
#include <utils/getopt.h>
#include <utils/wav.h>

#include <modules/auto_level/auto_level.h>

#define NB_CHANNELS 2
#define BUFFER_SIZE 64

#define BIT_PER_SAMPLES 16

#define INPUT_GAIN  0.5f
#define OUTPUT_GAIN 1.f

int main(int argc, char* argv[])
{
	// Local Variables
	uint32_t i, channel;
	uint32_t frame_counter;

	FILE *input_file, *output_file;

	int16_t input_file_buffer[BUFFER_SIZE*NB_CHANNELS];
	int16_t output_file_buffer[BUFFER_SIZE*NB_CHANNELS];
	
	float32_t input_buffer[BUFFER_SIZE* NB_CHANNELS];
	float32_t output_buffer[BUFFER_SIZE* NB_CHANNELS];
	float32_t *p_in[NB_CHANNELS];
	float32_t *p_out[NB_CHANNELS];

	bool is_input_wav  = false;
	bool is_output_wav = false;

	auto_level_t auto_level;

	// Variables for program arguments parsing
	int c;

	// Variables for filenames and path string manipulation
	char drive[_MAX_DRIVE], dir[_MAX_DIR];
	char filename[_MAX_FNAME], ext[_MAX_EXT];
	char outpathname[_MAX_PATH];

	// Initialization
	input_file = NULL;
	output_file = NULL;
	frame_counter = 0;

	// --------------------------------------------------------------------- //
	// Parse program arguments
	while((c = getopt(argc, argv, "i:o:")) != -1)
	{
		switch(c)
		{
		case 'i': // Input file
			// Open first input file
			if((input_file = fopen(optarg, "rb")) == NULL)
			{
				printf("\nError opening audio input file  %s !!\n", optarg);   
				system("pause");
				exit(0);
			}
			printf("\n Audio input file:  %s\n", optarg);

			// If it's a wavefile, read the header, and check the format
			if(strstr(optarg, ".wav"))
			{
				if(wav_is_valid(
					input_file,
					NB_CHANNELS,
					SAMPLE_RATE,
					BIT_PER_SAMPLES) != 0)
				{
					exit(0);
				}
				else
				{
					is_input_wav = true;
				}
			}
			break;

		case 'o':  // Audio Output file
			// Get output path and filename for future use
			_splitpath(optarg, drive, dir, filename, ext);
			strcpy(outpathname, dir);
			strcat(outpathname, filename);
			strcat(outpathname, ext);

			/* Create audio output file  */
			if ((output_file = fopen(outpathname, "wb+")) == NULL)
			{
				printf("\nError creating audio output file %s !!\n",
					   outpathname);

				system("pause");
				exit(0);
			}
			printf("Audio output file:  %s\n", outpathname);
			// Check if it's a wavefile, and write a generic header
			if (strstr(optarg, ".wav"))
			{
				wav_write_header(
					output_file,
					NB_CHANNELS,
					SAMPLE_RATE,
					BIT_PER_SAMPLES);
					
				is_output_wav = true;
			}
			break;

		default:
			abort();
		}
	}

	// Check that input, output files and EOL have been specified
	if (input_file == NULL)
	{
		printf("Audio input file unspecified\n");
		return 1;
	}
	if (output_file == NULL)
	{
		printf("Audio output file unspecified\n");
		return 1;
	}

	// Initialize Processing
	auto_level_init(&auto_level);

	// --------------------------------------------------------------------- //
	// Processing

	// Buffer pointers initialization
	for (i = 0; i < NB_CHANNELS; i++)
	{
		p_in[i]  = &input_buffer [i*BUFFER_SIZE];
		p_out[i] = &output_buffer[i*BUFFER_SIZE];
	}

	// Open Processing

	// Process loop
	while (1)
	{
		frame_counter++;

		// Read Input Sample from file
		fread(
			input_file_buffer,
			sizeof(int16_t), 
			BUFFER_SIZE*NB_CHANNELS, 
			input_file);

		if (feof(input_file))
			break;

		// Modifying sample format from input and remove interleaving
		for (channel=0; channel<NB_CHANNELS; channel++)
		{
			for (i=0; i<BUFFER_SIZE; i++)
			{
				input_buffer[i+channel*BUFFER_SIZE] = INPUT_GAIN *
					Q15_TO_FLOAT(input_file_buffer[NB_CHANNELS*i+channel]);
			}
				
		}

		// Processing
		//memcpy(p_out[0], p_in[0], sizeof(float32_t)*BUFFER_SIZE);
		//memcpy(p_out[1], p_in[1], sizeof(float32_t)*BUFFER_SIZE);
		auto_level_process(&auto_level,p_in,p_out,BUFFER_SIZE);

		// Interleave for output
		for (channel=0; channel<NB_CHANNELS; channel++)
		{
			for (i=0; i<BUFFER_SIZE; i++)
			{
				float32_t value = OUTPUT_GAIN *
					output_buffer[i + channel * BUFFER_SIZE];
				value = M_MAX(value, -1.f);
				value = M_MIN(value, 1.f);
				output_file_buffer[NB_CHANNELS*i+channel] = FLOAT_TO_Q15(value);
			}

		}

		// Writing audio output into file
		fwrite(
			output_file_buffer,
			sizeof(int16_t),
			BUFFER_SIZE*NB_CHANNELS,
			output_file);
	}

	// Close processing


	// Add the size info in the output wavefile if needed
	if (is_output_wav)
	{
		wav_write_length_in_header(
			output_file,
			frame_counter*SAMPLE_RATE,
			NB_CHANNELS,
			BIT_PER_SAMPLES);
	}
		

	// Close files
	fclose(input_file);
	fclose(output_file);
};