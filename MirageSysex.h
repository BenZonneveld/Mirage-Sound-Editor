/* $Id: MirageSysex.h,v 1.14 2008/05/05 16:13:25 root Exp $ */
#pragma once

#include "Globals.h"
#include "MidiWrapper/MIDIInDevice.h"
#include "MidiWrapper/shortmsg.h"
#include "MidiWrapper/Longmsg.h"
#include <vector>

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
	unsigned char dummy_byte;
	unsigned char master_tune;
	unsigned char pitch_bend_range;
	unsigned char velocity_sensitivity;
	unsigned char up_low_balance;
	unsigned char program_link_switch;
	unsigned char sample_time_adjust;
	unsigned char input_filter_freq;
	unsigned char mic_line_switch;
	unsigned char sample_treshold;
	unsigned char multisample_switch;
	unsigned char omni_mode;
	unsigned char midi_channel;
	unsigned char thru_mode;
	unsigned char mod_enable;
	unsigned char source_start_msb;
	unsigned char source_start_lsb;
	unsigned char source_end_msb;
	unsigned char source_end_lsb;
	unsigned char dest_msb;
	unsigned char dest_lsb;
	unsigned char dest_bank;
	unsigned char scale_start;
	unsigned char scale_end;
	unsigned char external_comp;
	unsigned char baud_rate;
	unsigned char cart_filt_freq;
	unsigned char software_rev;
	unsigned char spare;
};

class MyReceiver : public midi::CMIDIReceiver
{
public:
	// Receives short messages
	void ReceiveMsg(DWORD Msg, DWORD Timestamp);

	// Receives long messages
	void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);

	// Called when an invalid short message is received
  void OnError(DWORD Msg, DWORD TimeStamp) {}
    // Called when an invalid long message is received
  void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
};

extern _program_dump_table_ ProgramDumpTable[];

extern _config_dump_table_ ConfigDump;

extern int MirageOS;

extern HANDLE midi_in_event;
extern HANDLE wave_status_message;

extern std::vector <char> InMsg;
extern unsigned char SysXBuffer[SYSEXBUFFER];
extern MyReceiver Receiver;
extern midi::CMIDIInDevice InDevice;//(Receiver);
extern midi::CLongMsg	LongMsg;
extern midi::CShortMsg ShortMsg;

#endif // Mirage_Sysex

void StopMidi();
BOOL StartMidi();
//void ChangeParameter(const char * Name, unsigned char Parameter, unsigned char Value);
BOOL GetAvailableSamples(void);
BOOL GetSampleParameters(void);
int GetMirageOs(void);
BOOL DoSampleSelect(unsigned char *SampleSelect,unsigned char SampleNumber);
BOOL GetSample(unsigned char *SampleSelect,unsigned char SampleNumber);
BOOL PutSample(unsigned char *SampleSelect, unsigned char SampleNumber,bool LoopOnly);
BOOL GetConfigParms();
