// Copyright (c) 2020 All Rights Reserved Baptiste Vericel

#include <stdlib.h>
#include <string.h>

#include "../../../../src/modules/auto_level/auto_level.h"

#include "ladspa.h"
#include "utils.h"

// The port numbers for the plugin
#define TS_AUTO_LEVEL_BYPASS                0
#define TS_AUTO_LEVEL_Z0_RMS                1
#define TS_AUTO_LEVEL_NOISE_LEVEL           2
#define TS_AUTO_LEVEL_NOISE_TIME_SAMPLE     3
#define TS_AUTO_LEVEL_RMS_TARGET            4
#define TS_AUTO_LEVEL_MAX_GAIN              5
#define TS_AUTO_LEVEL_MIN_GAIN              6
#define TS_AUTO_LEVEL_Z0_GAIN               7
#define TS_AUTO_LEVEL_COMP_RELEASE          8
#define TS_AUTO_LEVEL_COMP_THRESHOLD        9
#define TS_AUTO_LEVEL_IN_L                  10
#define TS_AUTO_LEVEL_IN_R                  11
#define TS_AUTO_LEVEL_OUT_L                 12
#define TS_AUTO_LEVEL_OUT_R                 13

#define TS_AUTO_LEVEL_NB_PORTS              14

// Global Variables
LADSPA_Descriptor* g_tsAutoLevelDescriptor = NULL;

// The structure used to hold port connection information and state
typedef struct {
  LADSPA_Data * m_bypass;

  LADSPA_Data * m_InL;
  LADSPA_Data * m_InR;
  LADSPA_Data * m_OutL;
  LADSPA_Data * m_OutR;
  
  auto_level_t auto_level;
} tshoko_auto_level_t;

// Construct a new plugin instance. */
static LADSPA_Handle instantiateTshokoAutoLevel(
    const LADSPA_Descriptor * Descriptor,
	unsigned long SampleRate)
{
	LADSPA_Handle handle = malloc(sizeof(tshoko_auto_level_t));
	tshoko_auto_level_t * tshoko_auto_level;

	tshoko_auto_level = (tshoko_auto_level_t *)handle;
	auto_level_init(&tshoko_auto_level->auto_level);
	
	return handle;
}

// Connect a port to a data location
static void connectPortToTshokoAutoLevel(
    LADSPA_Handle Instance,
	unsigned long Port,
	LADSPA_Data * DataLocation)
{
    tshoko_auto_level_t * tshoko_auto_level;
    unsigned long uInt32Val;

    tshoko_auto_level = (tshoko_auto_level_t *)Instance;
    switch (Port) 
    {
        case TS_AUTO_LEVEL_BYPASS:
            tshoko_auto_level->m_bypass = DataLocation;
            break;

        case TS_AUTO_LEVEL_Z0_RMS:
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_Z0_RMS,
                (void*)DataLocation);
            break;

        case TS_AUTO_LEVEL_NOISE_LEVEL:
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_NOISE_LEVEL,
                (void*)DataLocation);
            break;

        case TS_AUTO_LEVEL_NOISE_TIME_SAMPLE:
            uInt32Val = (unsigned long)*DataLocation;
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_NOISE_TIME_SAMPLE,
                (void*)&uInt32Val);
            break;

        case TS_AUTO_LEVEL_RMS_TARGET:
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_RMS_TARGET,
                (void*)DataLocation);
            break;

        case TS_AUTO_LEVEL_MAX_GAIN:
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_MAX_GAIN,
                (void*)DataLocation);
            break;

        case TS_AUTO_LEVEL_MIN_GAIN:
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_MIN_GAIN,
                (void*)DataLocation);
            break;

        case TS_AUTO_LEVEL_Z0_GAIN:
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_Z0_GAIN,
                (void*)DataLocation);
            break;

        case TS_AUTO_LEVEL_COMP_RELEASE:
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_COMP_RELEASE,
                (void*)DataLocation);
            break;

        case TS_AUTO_LEVEL_COMP_THRESHOLD:
            auto_level_set(
                &(tshoko_auto_level->auto_level),
                AUTO_LEVEL_COMP_THRESHOLD,
                (void*)DataLocation);
            break;

        case TS_AUTO_LEVEL_IN_L:
            tshoko_auto_level->m_InL = DataLocation;
            break;

        case TS_AUTO_LEVEL_IN_R:
            tshoko_auto_level->m_InR = DataLocation;
            break;

        case TS_AUTO_LEVEL_OUT_L:
            tshoko_auto_level->m_OutL = DataLocation;
            break;

        case TS_AUTO_LEVEL_OUT_R:
            tshoko_auto_level->m_OutR = DataLocation;
            break;
    }
}

// Processing
static void runTshokoAutoLevel(
    LADSPA_Handle Instance,
    unsigned long SampleCount) 
{
	//unsigned long i;
	
    tshoko_auto_level_t* tshoko_auto_level = (tshoko_auto_level_t*)Instance;
	
	LADSPA_Data * pinL  = tshoko_auto_level->m_InL;
    LADSPA_Data * pinR  = tshoko_auto_level->m_InR;
    LADSPA_Data * poutL = tshoko_auto_level->m_OutL;
    LADSPA_Data * pOutR = tshoko_auto_level->m_OutR;
	
	float *p_in[2];
	float *p_out[2];
	
	p_in[0] = (float*)pinL;
	p_in[1] = (float*)pinR;
	
	p_out[0] = (float*)poutL;
	p_out[1] = (float*)pOutR;
	
	auto_level_process(&tshoko_auto_level->auto_level,p_in,p_out,SampleCount);

    //for (i = 0; i < SampleCount; i++)
    //{
    //    *(poutL++) = *(pinL++)*0.1f;
    //    *(pOutR++) = *(pinR++)*0.1f;
    //}
}

// Throw away a Tshoko Auto Level
static void  cleanupTshokoAutoLevel(LADSPA_Handle Instance)
{
  free(Instance);
}

// Called automatically when the plugin library is first loaded
ON_LOAD_ROUTINE {

    char ** pcPortNames;
    LADSPA_PortDescriptor * piPortDescriptors;
    LADSPA_PortRangeHint  * psPortRangeHints;

    g_tsAutoLevelDescriptor =  
        (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
  
    if (g_tsAutoLevelDescriptor)
    {
        // General Parameters
        g_tsAutoLevelDescriptor->UniqueID = 1049;
        g_tsAutoLevelDescriptor->Label = strdup("tshoko_auto_Level");
        g_tsAutoLevelDescriptor->Properties = LADSPA_PROPERTY_HARD_RT_CAPABLE;
        g_tsAutoLevelDescriptor->Name  = strdup("Tshoko Auto Level");
        g_tsAutoLevelDescriptor->Maker = strdup("Baptiste Vericel");
        g_tsAutoLevelDescriptor->Copyright = strdup("None");
        g_tsAutoLevelDescriptor->PortCount = TS_AUTO_LEVEL_NB_PORTS;

        // Port descriptors
        piPortDescriptors = (LADSPA_PortDescriptor *)calloc(
            TS_AUTO_LEVEL_NB_PORTS,
            sizeof(LADSPA_PortDescriptor));

        g_tsAutoLevelDescriptor->PortDescriptors =
            (const LADSPA_PortDescriptor *)piPortDescriptors;

        piPortDescriptors[TS_AUTO_LEVEL_BYPASS] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_Z0_RMS] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_NOISE_LEVEL] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_NOISE_TIME_SAMPLE] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_RMS_TARGET] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_MAX_GAIN] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_MIN_GAIN] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_Z0_GAIN] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_COMP_RELEASE] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
        piPortDescriptors[TS_AUTO_LEVEL_COMP_THRESHOLD] =
            LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;

        piPortDescriptors[TS_AUTO_LEVEL_IN_L]  =
            LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
        piPortDescriptors[TS_AUTO_LEVEL_IN_R]  =
            LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
        piPortDescriptors[TS_AUTO_LEVEL_OUT_L] =
            LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
        piPortDescriptors[TS_AUTO_LEVEL_OUT_R] =
            LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;

        // Port names
        pcPortNames = (char **)calloc(TS_AUTO_LEVEL_NB_PORTS, sizeof(char *));

        g_tsAutoLevelDescriptor->PortNames = (const char **)pcPortNames;

        pcPortNames[TS_AUTO_LEVEL_BYPASS] = strdup("Bypass");
        pcPortNames[TS_AUTO_LEVEL_Z0_RMS] = strdup("RMS smoothing coeff");
        pcPortNames[TS_AUTO_LEVEL_NOISE_LEVEL] = strdup("Noise Level");
        pcPortNames[TS_AUTO_LEVEL_NOISE_TIME_SAMPLE] = strdup("Noise Time (samples)");
        pcPortNames[TS_AUTO_LEVEL_RMS_TARGET] = strdup("RMS target");
        pcPortNames[TS_AUTO_LEVEL_MAX_GAIN] = strdup("Max gain (linear)");
        pcPortNames[TS_AUTO_LEVEL_MIN_GAIN] = strdup("Min gain (linear)");
        pcPortNames[TS_AUTO_LEVEL_Z0_GAIN] = strdup("Gain smoothing coeff");
        pcPortNames[TS_AUTO_LEVEL_COMP_RELEASE] = strdup("Limiter release coeff");
        pcPortNames[TS_AUTO_LEVEL_COMP_THRESHOLD] = strdup("Limiter threshold");

        pcPortNames[TS_AUTO_LEVEL_IN_L]   = strdup("Input (Left)");
        pcPortNames[TS_AUTO_LEVEL_IN_R]   = strdup("Input (Right)");
        pcPortNames[TS_AUTO_LEVEL_OUT_L]  = strdup("Output (Left)");
        pcPortNames[TS_AUTO_LEVEL_OUT_R]  = strdup("Output (Right)");

        // Hints
        psPortRangeHints = ((LADSPA_PortRangeHint *) calloc(
            TS_AUTO_LEVEL_NB_PORTS,
            sizeof(LADSPA_PortRangeHint)));

        g_tsAutoLevelDescriptor->PortRangeHints =
            (const LADSPA_PortRangeHint *)psPortRangeHints;

        psPortRangeHints[TS_AUTO_LEVEL_BYPASS].HintDescriptor =
            ( LADSPA_HINT_BOUNDED_BELOW
	        | LADSPA_HINT_DEFAULT_0 );
        psPortRangeHints[TS_AUTO_LEVEL_BYPASS].LowerBound    = 0;

        psPortRangeHints[TS_AUTO_LEVEL_Z0_RMS].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_NOISE_LEVEL].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_NOISE_TIME_SAMPLE].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_RMS_TARGET].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_MAX_GAIN].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_MIN_GAIN].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_Z0_GAIN].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_COMP_RELEASE].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_COMP_THRESHOLD].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_IN_L].HintDescriptor  = 0;
        psPortRangeHints[TS_AUTO_LEVEL_IN_R].HintDescriptor  = 0;
        psPortRangeHints[TS_AUTO_LEVEL_OUT_L].HintDescriptor = 0;
        psPortRangeHints[TS_AUTO_LEVEL_OUT_R].HintDescriptor = 0;

        // Methods
        g_tsAutoLevelDescriptor->instantiate = instantiateTshokoAutoLevel;
        g_tsAutoLevelDescriptor->connect_port = connectPortToTshokoAutoLevel;
        g_tsAutoLevelDescriptor->activate = NULL;
        g_tsAutoLevelDescriptor->run = runTshokoAutoLevel;
        g_tsAutoLevelDescriptor->run_adding = NULL;
        g_tsAutoLevelDescriptor->set_run_adding_gain = NULL;
        g_tsAutoLevelDescriptor->deactivate = NULL;
        g_tsAutoLevelDescriptor->cleanup = cleanupTshokoAutoLevel;
    }
}

static void deleteDescriptor(LADSPA_Descriptor * psDescriptor)
{
    unsigned long lIndex;

    if (psDescriptor)
    {
        free((char *)psDescriptor->Label);
        free((char *)psDescriptor->Name);
        free((char *)psDescriptor->Maker);
        free((char *)psDescriptor->Copyright);

        free((LADSPA_PortDescriptor *)psDescriptor->PortDescriptors);

        for (lIndex = 0; lIndex < psDescriptor->PortCount; lIndex++)
        {
            free((char*)(psDescriptor->PortNames[lIndex]));
        }
            
        free((char **)psDescriptor->PortNames);

        free((LADSPA_PortRangeHint *)psDescriptor->PortRangeHints);

        free(psDescriptor);
    }
}

// Called automatically when the library is unloaded
ON_UNLOAD_ROUTINE
{
    deleteDescriptor(g_tsAutoLevelDescriptor);
}

// Return a descriptor of the requested plugin type
const LADSPA_Descriptor * ladspa_descriptor(unsigned long Index)
{
    // Return the requested descriptor or null if the index is out of range
    switch (Index)
    {
        case 0:
        case 1:
            return g_tsAutoLevelDescriptor;
        default:
            return NULL;
    }
}
