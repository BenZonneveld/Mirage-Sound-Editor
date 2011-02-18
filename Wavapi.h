// $Id: Wavapi.h,v 1.8 2008/01/30 22:50:43 root Exp $

#ifndef _INC_WAVAPI
#define _INC_WAVAPI
#include	<vector>

/* Handle to a WAV */
DECLARE_HANDLE(MWAV);

DWORD	StereoToMono(unsigned char *lpDataIn, WORD BlockAlign, WORD wBitsPerSample, DWORD DataSize);
DWORD	ConvertTo8Bit(short *lpDataIn, DWORD DataSize);

/* Function prototypes */
DWORD	WINAPI	WAVSize(LPSTR lpWAV);
DWORD	WINAPI	WAVChannels(LPSTR lpWAV);
BOOL	WINAPI	SaveWAV(MWAV hWav, CFile& file);
MWAV	WINAPI	ReadWAVFile(CFile& file);
void RemoveZeroSamples(struct _WaveSample_ * sWav);
void ResizeRiff(struct _WaveSample_ * sWav, DWORD NewSize);

#endif //!_INC_WAVAPI

