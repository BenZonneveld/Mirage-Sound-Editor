/* $Id: MirageSysex.h,v 1.14 2008/05/05 16:13:25 root Exp $ */
#pragma once

#include "../Globals.h"


#ifndef Mirage_Sysex
#define Mirage_Sysex

// Global to indicate we are finished whith processing received sysex data
extern char DataDumped; 
extern unsigned char lower_upper_select;
extern unsigned char ProgramStatus;
extern unsigned char WavesampleStatus;
extern unsigned char WavesampleStore;

// Ensoniq Mirage Sysex ID
/* For setting the original key */
extern unsigned char LastMidiKey;

/* Each Wavesample Control Block contains pointers which are used internally by the Mirage Operating System */
struct _sample_control_block_
{
	unsigned short	pSampleStart;
	unsigned short	pSampleEnd;
	unsigned short	pLoopStart;
	unsigned short	pLoopEnd;
	unsigned char	LoopSwitch;
	unsigned char	CoarseTune;
	unsigned char	FineTune;
	unsigned char	RelativeAmplitude;
	unsigned char	RelativeFilterFreq;
	unsigned char	MaximumFilterFreq;
	unsigned char	TopKey;
	unsigned char	SampleStart;
	unsigned char	SampleEnd;
	unsigned char	LoopStart;
	unsigned char	LoopEnd;
	unsigned char	LoopEndFine;
	unsigned char	FreeRunFlag;
	unsigned char	spare[3];
};

/* The segment list is a set op operating system variables which control the digital oscillators */
/* According to the documentation the should not be changed externally and sent back to the mirage */
struct _segment_list_
{
	unsigned char os_vars[32];
	/*
	 * [0x00] Sample Start Page
	 * [0x01] Sample 
	 * [0x02] 
	 * [0x03] 
	 * [0x04] 
	 * [0x05] 
	 * [0x06] 
	 * [0x07] 
	 * [0x08] 
	 * [0x09] 
	 * [0x0a]
	 * [0x0b]
	 * [0x0c]
	 * [0x0d] 
	 * [0x0e] 0x80
	 * [0x0f] 0x3f
	 * [16]
	 */
};

struct _program_parameter_block_
{
	unsigned char	MonoModeSwitch;
	unsigned char	LFO_Freq;
	unsigned char	LFO_Depth;
	unsigned char	Osc_Detune;
	unsigned char	Osc_Mix;
	unsigned char	MixVelSens;
	unsigned char	FilterCutOff;
	unsigned char	Resonance;
	unsigned char	FilterKeyboardTracking;
	unsigned char	spare1;
	unsigned char	InitialWavesample;
	unsigned char	MixModeSwitch;
	unsigned char	FiltEnvAttack;
	unsigned char	FiltEnvPeak;
	unsigned char	FiltEnvDecay;
	unsigned char	FiltEnvSustain;
	unsigned char	FiltEnvRelease;
	unsigned char	FiltAttackVelo;
	unsigned char	FiltPeakVelo;
	unsigned char	FiltDecayKeyScale;
	unsigned char	FiltSustainVelo;
	unsigned char	FiltReleaseVelo;
	unsigned char	AmpEnvAttack;
	unsigned char	AmpEnvPeak;
	unsigned char	AmpEnvDecay;
	unsigned char	AmpEnvSustain;
	unsigned char	AmpEnvRelease;
	unsigned char	AmpAttackVelo;
	unsigned char	AmpPeakVelo;
	unsigned char	AmpDecayKeyScale;
	unsigned char	AmpSustainVelo;
	unsigned char	AmpReleaseVelo;
	unsigned char	spare2[4];
};

#pragma pack (1)
struct _program_dump_table_
{
	unsigned char	SoundRevLevel;
	struct _sample_control_block_ WaveSampleControlBlock[8];
	struct _segment_list_ WaveSampleSegmentList[9]; // 1 Spare Segment List !!
	struct _program_parameter_block_ ProgramParameterBlock[4];
};

#pragma pack (1)
struct _config_dump_table_
{
	unsigned char dummy_byte;						// Parameter Number
	unsigned char master_tune;					//	21
	unsigned char pitch_bend_range;			//	22
	unsigned char velocity_sensitivity;	//	23
	unsigned char up_low_balance;				//	24
	unsigned char program_link_switch;	//	25
	unsigned char sample_time_adjust;		//	73
	unsigned char input_filter_freq;		//	74
	unsigned char mic_line_switch;			//	75
	unsigned char sample_treshold;			//	76
	unsigned char multisample_switch;		//	77
	unsigned char omni_mode;						//	81
	unsigned char midi_channel;					//	82
	unsigned char thru_mode;						//	83
	unsigned char mod_enable;						//	84 
//			0:	Only MIDI key information is transmitted or received. 
//			1:	Key and controller information are both transmitted and received.
//			2:	Same as value 1 with the addition that program changes are both transmitted and received.
//			3:	Same as value 2 with the addition that to change a program on the 
//					Mirage you must also press the MIDI +1/yes button on instruments
//					such as the DX-7. This setting allows you to change sounds on your
//					controller instrument without changing sounds on the Mirage.
	unsigned char source_start_msb;			//	85	
	unsigned char source_start_lsb;			//	86
	unsigned char source_end_msb;				//	87
	unsigned char source_end_lsb;				//	88
	unsigned char dest_msb;							//	89
	unsigned char dest_lsb;							//	90
	unsigned char dest_bank;						//	94
	unsigned char scale_start;					//	95
	unsigned char scale_end;						//	96
	unsigned char external_comp;				//	91
	unsigned char baud_rate;						//	92
	unsigned char cart_filt_freq;				//	93
	unsigned char software_rev;					//	97
	unsigned char spare;
};

extern _program_dump_table_ ProgramDumpTable[];

extern _config_dump_table_ ConfigDump;

extern int MirageOS;

extern HANDLE wave_status_message;

struct _WaSaRe_
{
//	byte	trycount;
	BOOL	LoopSwitch;
	unsigned char	ul_Wavesample;
};

//extern _WaSaRe_ WaveSampleReceive;

#endif // Mirage_Sysex


//void ChangeParameter(const char * Name, unsigned char Parameter, unsigned char Value);
BOOL GetAvailableSamples(void);
BOOL GetSampleParameters(void);
//int GetMirageOs(void);
BOOL DoSampleSelect(unsigned char *SampleSelect,unsigned char SampleNumber);
BOOL GetSample(unsigned char *SampleSelect,unsigned char SampleNumber);
BOOL GotSample();
BOOL PutSample(unsigned char *SampleSelect, unsigned char SampleNumber,bool LoopOnly);
BOOL GetConfigParms(unsigned int TimeOut = 5000);
BOOL SendConfigParms();
void ChangeParameter(const char * Name, unsigned char Parameter, unsigned char Value);