/* $Id: wavesamples.h,v 1.15 2008/01/23 22:42:31 root Exp $ */

#include "stdafx.h"
#include "Mirage EditorDoc.h"
#include <mmsystem.h>

#ifndef WAVESAMPLES
#define WAVESAMPLES
#endif

struct _riff_
{
  _TCHAR riffID[4];		//contains identifier "RIFF"
  DWORD riffSIZE;		//File size minus 8 bytes
  _TCHAR riffFORMAT[4];	//contains identifier "WAVE"
};

struct WAVEFORM
{
  WORD wFormatTag;		//format of digital sound (1 for uncompressed)
  WORD nChannels;		//Number of channels (1 for mono and 2 for stereo)
  DWORD nSamplesPerSec;	//Number of samples per second
  DWORD nAvgBytesPerSec;//Average number bytes of data per second
  WORD nBlockAlign;		//Minimal data size for playing
  WORD wBitsPerSample;	//Bits per sample (8 or 16)
};

struct _fmt_
{
  _TCHAR fmtID[4];		//contains identifier: "fmt " (with space)
  DWORD fmtSIZE;		//contains the size of this block (for WAVE PCM 16)
  WAVEFORM fmtFORMAT;	//structure WAVEFORMATEX but without cbSize field
};

struct _data_
{
  _TCHAR dataID[4];	//contains identifier: "data"
  DWORD dataSIZE;	//data size
};

struct SAMPLELOOP{
  DWORD	dwIdentifier;	// Contains a unique number (ie, different than the ID number of any other SampleLoop structure).
  DWORD dwType;			// Loop type: 0 = forward, 1 = Alternating Loop, 2 = Loop Backward, 3-31 = reserved, 32-? = sampler specific.
  DWORD dwStart;		// The startpoint of the loop. In other words, it's the byte offset from the start of waveformData[], where an offset of 0 would be at the start of the waveformData[] array (ie, the loop start is at the very first sample point).
  DWORD dwEnd;			// Bye offset indicating the end of the loop.
  DWORD dwFraction;		// fine-tuning for loop fractional areas between samples.
  DWORD dwPlayCount;	// times to play the loop. A value of 0 specifies an infinite sustain loop (ie, the wave keeps looping until some external force interrupts playback, such as the musician releasing the key that triggered that wave's playback).
};

struct _sampler_ {
  _TCHAR	samplerID[4];		// Contains identifier: "smpl"
  DWORD		samplerSize;		// the number of bytes in the chunk, not counting the 8 bytes used by ID and Size fields nor any possible pad byte needed to make the chunk an even size (ie, chunkSize is the number of remaining bytes in the chunk after the chunkSize field, not counting any trailing pad byte).
  DWORD		Manufacturer;		// MMA Manufacturer ID ( 0x0F for Ensoniq)
  DWORD		Product;			// Product Code for the intended sampler ( 0x01 for the Mirage )
  DWORD		SamplePeriod;		// The period of one sample in nanoseconds (normally 1/nSamplesPerSec from the Format chunk. But note that this field allows finer tuning than nSamplesPerSec). For example, 44.1 KHz would be specified as 22675 (0x00005893).
  DWORD		MIDIUnityNote;		// The MIDI note number at which the instrument plays back the waveform data without pitch modification (ie, at the same sample rate that was used when the waveform was created). This value ranges 0 through 127, inclusive. Middle C is 60.
  DWORD		MIDIPitchFraction;	// The fraction of a semitone up from the specified dwMIDIUnityNote. A value of 0x80000000 is 1/2 semitone (50 cents); a value of 0x00000000 represents no fine tuning between semitones.
  DWORD		SMPTEFormat;		// Should be 0 for the Mirage
  DWORD		SMPTEOffset;		// Shoudl be 0 for the Mirage
  DWORD		cSampleLoops;		// The number (count) of SampleLoop structures that are appended to this chunk.
  DWORD		cbSamplerData;		// Should be 0
  struct	SAMPLELOOP	Loops;
};

#pragma pack (1)
struct _WaveSample_
{
	struct _riff_			riff_header;
	struct _fmt_			waveFormat;
	struct _sampler_		sampler;
	struct _data_			data_header;
	unsigned char		SampleData[65535];	// The actual waveform data
	unsigned char		checksum;			// Checksum formed as a modulo 128 add of each nybble and the pagecount
	unsigned char		samplepages;		// Number of Mirage pages in the sample.
};

typedef short	AudioWord;
typedef unsigned char	AudioByte;

extern struct _WaveSample_ WaveSample;

void CreateRiffWave(int SampleNumber, int UpperLower, BOOL LoopSwitch);
BOOL CreateFromMirage(unsigned char SampleNumber, unsigned char ul_Wavesample);
void PlayWaveData(struct _WaveSample_ WaveData);
LPSTR GetWaveSample(struct _WaveSample_ * Get_sWav, CMirageEditorDoc* pDoc);