/* Mirage Helpers $Id: Mirage\040Helpers.cpp,v 1.3 2008/02/23 23:58:51 root Exp $ */

#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "MirageSysex.h"
#include "wavesamples.h"
#include "float_cast.h"
#include <stdio.h>
#include <math.h>

void Show_ProgramDump_Data(void)
{
	unsigned char bank;
	unsigned char wavesample;
	unsigned char program;

	for(bank = 0; bank < 2 ; bank++)
	{
#ifdef _DEBUG_
		if (bank == 0 )
		{
			fprintf(logfile,"\n\nLower Bank Data:\n");
		} else {
			fprintf(logfile,"\n\nUpper Bank Data:\n");
		}
#endif
					
		for(wavesample = 0 ; wavesample < 8 ;  wavesample++)
		{
#ifdef _DEBUG_
			fprintf(logfile,"Wavesample %d start: %02X end: %02X TopKey: %d \
pSampleStart: %04X pSampleEnd: %04X\
\nLoop Start: %02X End: %02X End Fine: %02X\
pLoopStart: %04X pLoopEnd: %04X\n",
				wavesample+1,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleStart,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleEnd,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].TopKey,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].pSampleStart,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].pSampleEnd,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].LoopStart,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].LoopEnd,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].LoopEndFine,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].pLoopStart,
				ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].pLoopEnd);
#endif
		}
		for(program = 0 ; program < 4 ; program++)
		{
#ifdef _DEBUG_
			fprintf(logfile,"\nProgram: %d\n", program+1);

			fprintf(logfile,"**Keyboard/Program**\n\
MonoMode: %02X\tFilter Cutoff Frequency: %d\n\
LFO Freq: %d\tFilter Resonance: %d\n\
LFO Depth: %d\tKBD Tracking: %d\n",
							ProgramDumpTable[bank].ProgramParameterBlock[program].MonoModeSwitch,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].FilterCutOff / 2),
							ProgramDumpTable[bank].ProgramParameterBlock[program].LFO_Freq,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].Resonance / 4 ),
							ProgramDumpTable[bank].ProgramParameterBlock[program].LFO_Depth,
							ProgramDumpTable[bank].ProgramParameterBlock[program].FilterKeyboardTracking);
			fprintf(logfile,"**Wavesample**\n\
Initial Wavesample: %d\tMix Mode: %d\n\
Osc 2 Detune: %d\tOsc Mix: %d\n\
Osc. Mix -> Vel. Sens.: %d\n",
							ProgramDumpTable[bank].ProgramParameterBlock[program].InitialWavesample + 1,
							ProgramDumpTable[bank].ProgramParameterBlock[program].MixModeSwitch,
							ProgramDumpTable[bank].ProgramParameterBlock[program].Osc_Detune,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].Osc_Mix / 4),
							(ProgramDumpTable[bank].ProgramParameterBlock[program].MixVelSens / 4));
			fprintf(logfile,"**Filter Envelope**\n\
Attack: %d\tVel.Sens: %d\n\
Peak: %d\tVel.Sens: %d\n\
Decay: %d\tKbd.Scaled: %d\n\
Sustain: %d\tVel.Sens: %d\n\
Release: %d\tVel.Sens: %d\n",
							ProgramDumpTable[bank].ProgramParameterBlock[program].FiltEnvAttack,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].FiltAttackVelo / 4),
							ProgramDumpTable[bank].ProgramParameterBlock[program].FiltEnvPeak,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].FiltPeakVelo / 4),
							ProgramDumpTable[bank].ProgramParameterBlock[program].FiltEnvDecay,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].FiltDecayKeyScale / 4),
							ProgramDumpTable[bank].ProgramParameterBlock[program].FiltEnvSustain,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].FiltSustainVelo / 4),
							ProgramDumpTable[bank].ProgramParameterBlock[program].FiltEnvRelease,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].FiltReleaseVelo / 4));
			fprintf(logfile,"**Amplitude Envelope**\n\
Attack: %d\tVel.Sens: %d\n\
Peak: %d\tVel.Sens: %d\n\
Decay: %d\tKbd.Scaled: %d\n\
Sustain: %d\tVel.Sens: %d\n\
Release: %d\tVel.Sens: %d\n",
							ProgramDumpTable[bank].ProgramParameterBlock[program].AmpEnvAttack,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].AmpAttackVelo / 4),
							ProgramDumpTable[bank].ProgramParameterBlock[program].AmpEnvPeak,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].AmpPeakVelo / 4),
							ProgramDumpTable[bank].ProgramParameterBlock[program].AmpEnvDecay,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].AmpDecayKeyScale / 4),
							ProgramDumpTable[bank].ProgramParameterBlock[program].AmpEnvSustain,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].AmpSustainVelo / 4),
							ProgramDumpTable[bank].ProgramParameterBlock[program].AmpEnvRelease,
							(ProgramDumpTable[bank].ProgramParameterBlock[program].AmpReleaseVelo / 4));
#endif
		}
	}
}

bool CheckForValidWaveSample(unsigned char bank, unsigned char wavesample)
{
	unsigned char prev_sample;

	if ( ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleStart == ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleEnd )
		return false;
	if ( wavesample == 0 && ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleStart < ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleEnd )
		return true;
	for ( prev_sample = 0 ; prev_sample < wavesample ; prev_sample++)
	{
		if ( ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleStart == ProgramDumpTable[bank].WaveSampleControlBlock[prev_sample].SampleEnd )
			return false;
	}
	return true;
}

unsigned char CheckForValidTransmitSample(unsigned char bank, unsigned char wavesample)
{
	return (ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleEnd - ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleStart);
}

unsigned char GetNumberOfPages(struct _WaveSample_ *pWav)
{
	unsigned char Pages;

	Pages = unsigned char ((pWav->data_header.dataSIZE & 0xFF00 ) >> 8);
	if ( Pages == 0 )
		Pages++;

	return (Pages);
}
unsigned char GetChecksum(struct _WaveSample_ * pWav)
{
	int pages = (pWav->samplepages);
	unsigned char *	ptr;
	int SubSum = 0;
	unsigned char CheckSum;
	unsigned char lsNybble;
	unsigned char msNybble;
	int i;

	ptr = (unsigned char *)pWav->SampleData;

	for(i = 0; i < (pages * MIRAGE_PAGESIZE); i++)
	{
		msNybble = (((unsigned char)*(ptr)) & 0xF0) >> 4;
		lsNybble = (unsigned char)*(ptr) & 0x0F;
		SubSum = ((msNybble + lsNybble) % 128) + SubSum;
		ptr++;
	}
	lsNybble = pages & 0x0F;
	msNybble = ( pages & 0xF0 ) >> 4;
	CheckSum = (SubSum + lsNybble + msNybble) % 128;

	return (CheckSum);
}
