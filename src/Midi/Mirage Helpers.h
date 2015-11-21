/* $Id: Mirage\040Helpers.h,v 1.3 2008/02/23 23:58:51 root Exp $ */
#ifndef WAVESAMPLES
#include "../WaveApi/wavesamples.h"
#endif

void Show_ProgramDump_Data(void);
bool CheckForValidWaveSample(unsigned char bank, unsigned char wavesample);
unsigned char CheckForValidTransmitSample(unsigned char bank, unsigned char wavesample);
unsigned char GetNumberOfPages(struct _WaveSample_ *pWav);
unsigned char GetChecksum(struct _WaveSample_ * pWav);