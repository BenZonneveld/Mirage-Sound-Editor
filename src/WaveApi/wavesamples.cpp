/* $Id: wavesamples.cpp,v 1.17 2008/02/23 23:58:51 root Exp $ */
#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <MMReg.h>
#include <math.h>
#include <afxwin.h>

#include "../Globals.h"

#include "../Mirage Editor.h"

#include "wavesamples.h"
#include "../Midi/MirageSysex.h"
#include "Wave Doc.h"
#include "Wave View.h"

struct _WaveSample_ WaveSample;

void CreateRiffWave(int SampleNumber,int UpperLower, BOOL LoopSwitch)
{
	// Create RIFF Header
	memcpy(WaveSample.riff_header.riffID,"RIFF",4);
	WaveSample.riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_)+sizeof(_instrument_)+(WaveSample.samplepages * MIRAGE_PAGESIZE);
	memcpy(WaveSample.riff_header.riffFORMAT,"WAVE",4);

	// Create Format Header
	memcpy(WaveSample.waveFormat.fmtID,"fmt ",4);
	WaveSample.waveFormat.fmtSIZE = 16;
	WaveSample.waveFormat.fmtFORMAT.wFormatTag = 1; 
	WaveSample.waveFormat.fmtFORMAT.nChannels = 1; // Mono Samples
	WaveSample.waveFormat.fmtFORMAT.wBitsPerSample = 8; // 8 Bit Samples
	WaveSample.waveFormat.fmtFORMAT.nSamplesPerSec = 30000; // 30 Khz
	WaveSample.waveFormat.fmtFORMAT.nAvgBytesPerSec = 30000;
	WaveSample.waveFormat.fmtFORMAT.nBlockAlign = 256;

	// Create Sampler Header
	memcpy(WaveSample.sampler.samplerID,"smpl",4);
	WaveSample.sampler.samplerSize = 60; // Size of the chunk is 60 bytes
	WaveSample.sampler.Manufacturer = 0x0F; // For Ensoniq
	WaveSample.sampler.Product = 0x01; // For Mirage
	WaveSample.sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)WaveSample.waveFormat.fmtFORMAT.nSamplesPerSec);
	WaveSample.sampler.MIDIUnityNote = 60;
	WaveSample.sampler.MIDIPitchFraction = 0;
	WaveSample.sampler.SMPTEFormat = 0; // Mirage does not use SMPTE offsets
	WaveSample.sampler.SMPTEOffset = 0; // No SMPTE offset for the Mirage
	WaveSample.sampler.cbSamplerData = 0; 
	WaveSample.sampler.cSampleLoops = 1; // The Mirage always has loop points set
	// Now get the loop for this wavesample
	WaveSample.sampler.Loops.dwIdentifier = 0;
	WaveSample.sampler.Loops.dwType = 0;
	WaveSample.sampler.Loops.dwStart = ((ProgramDumpTable[UpperLower].WaveSampleControlBlock[SampleNumber].LoopStart - ProgramDumpTable[UpperLower].WaveSampleControlBlock[SampleNumber].SampleStart) * MIRAGE_PAGESIZE); 
	WaveSample.sampler.Loops.dwEnd = ((ProgramDumpTable[UpperLower].WaveSampleControlBlock[SampleNumber].LoopEnd - ProgramDumpTable[UpperLower].WaveSampleControlBlock[SampleNumber].SampleStart) * MIRAGE_PAGESIZE) + ProgramDumpTable[UpperLower].WaveSampleControlBlock[SampleNumber].LoopEndFine;

	WaveSample.sampler.Loops.dwPlayCount = !LoopSwitch;
	WaveSample.sampler.Loops.dwFraction = 0;

	// Create the instrument chunk
	memcpy(WaveSample.instrument.chunk_id,"inst",4);
	WaveSample.instrument.inst_size = 8; // Size of the chunk is 8 bytes
	WaveSample.instrument.unshifted_note = WaveSample.sampler.MIDIUnityNote;
	WaveSample.instrument.fine_tune = 0;
	WaveSample.instrument.gain = 0;
	WaveSample.instrument.low_note = 0; 
	WaveSample.instrument.high_note = 0x7F;
	WaveSample.instrument.low_velocity = 0x01;
	WaveSample.instrument.high_velocity = 0x7F;
	WaveSample.instrument.padding = 0;

	// Create Data Chunk
	memcpy(WaveSample.data_header.dataID,"data",4);
	if (WaveSample.samplepages == 0x00 )
	{
		WaveSample.data_header.dataSIZE = (0x100 * MIRAGE_PAGESIZE);
	} else {
		WaveSample.data_header.dataSIZE = WaveSample.samplepages * MIRAGE_PAGESIZE;
	}
	WaveSample.data_header.dataSIZE = WaveSample.data_header.dataSIZE;
//	RemoveZeroSamples(&WaveSample);
}

BOOL CreateFromMirage(unsigned char SampleNumber, unsigned char ul_Wavesample)
{
	MWAV hWAV;
	LPSTR pWAV;

	char titlestring[20];

	if ( ul_Wavesample == 0 )
	{
		sprintf_s(titlestring,sizeof(titlestring),"LowerWaveSample %d", SampleNumber+1);
	}
	else
	{
		sprintf_s(titlestring,sizeof(titlestring),"UpperWaveSample %d", SampleNumber+1);
	}

	CMirageEditorDoc* pDoc=NULL; 
	CMirageEditorApp* pApp = (CMirageEditorApp*)AfxGetApp();
	pDoc = (CMirageEditorDoc *)pApp->m_pDocTemplate->OpenDocumentFile(NULL);
	pDoc->SetTitle(titlestring);

	hWAV = (MWAV) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (DWORD)sizeof(WaveSample));
	if (hWAV == 0)
	{
		return false;
	}
	pWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);

	memcpy(pWAV,(unsigned char *)&WaveSample,sizeof(WaveSample));
	
	pDoc->CreateNewFromMirage(hWAV);
	::GlobalUnlock((HGLOBAL) hWAV);
	// Wait for screen updates 
	return true;
}

UINT __cdecl  PlayWaveData(/*MWAV hWAV*/ LPVOID parameter)
{
	MMRESULT	mResult;
	HANDLE		hData  = NULL;  // handle of waveform data memory 
	HPSTR		lpData = NULL;  // pointer to waveform data memory 
	HWAVEOUT	hWaveOut; 
	HGLOBAL		hWaveHdr; 
	LPWAVEHDR	lpWaveHdr; 
	UINT		wResult; 
	WAVEFORMAT	*pFormat; 
	DWORD		dwDataSize; 
	MWAV		hWAV;
	DWORD		ThreadEvent;
	_WaveSample_ WaveData;
		
	ThreadEvent=WaitForSingleObject(AudioPlayingEvent,1);

	if ( ThreadEvent == WAIT_OBJECT_0 )
		return 1;

	SetEvent(AudioPlayingEvent);

	hWAV = reinterpret_cast<MWAV>(parameter);
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	
	memcpy((unsigned char *)&WaveData,lpWAV,sizeof(WaveData.riff_header));
	memcpy((unsigned char *)&WaveData,lpWAV,WaveData.riff_header.riffSIZE+8);

	dwDataSize = WaveData.data_header.dataSIZE-16; // Mirage Adds 16 bytes of zero to indicate end of data
	pFormat = (WAVEFORMAT *)&WaveData.waveFormat.fmtFORMAT;
	lpData = (HPSTR)&WaveData.SampleData;

	// Open a waveform device for output using window callback. 

	pFormat->nBlockAlign = 1;

	if ((mResult = waveOutOpen((LPHWAVEOUT)&hWaveOut, WAVE_MAPPER, 
					(LPWAVEFORMATEX)pFormat,
					0L, 0L, CALLBACK_EVENT|WAVE_ALLOWSYNC)) != MMSYSERR_NOERROR )
	{ 
		MessageBox(NULL, 
					"Failed to open waveform output device.", 
					NULL, MB_OK | MB_ICONEXCLAMATION);
		ResetEvent(AudioPlayingEvent);
		return 1; 
	} 
 
	// Allocate and lock memory for the header. 

	hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, 
		(DWORD) sizeof(WAVEHDR)); 
	if (hWaveHdr == NULL) 
	{ 
		MessageBox(NULL, "Not enough memory for header.", 
					NULL, MB_OK | MB_ICONEXCLAMATION);
		ResetEvent(AudioPlayingEvent);
		return 1; 
	} 
 
	lpWaveHdr = (LPWAVEHDR) GlobalLock(hWaveHdr); 
	if (lpWaveHdr == NULL) 
	{ 
		GlobalUnlock(hData); 
		GlobalFree(hData); 
		MessageBox(NULL, 
			"Failed to lock memory for header.", 
			NULL, MB_OK | MB_ICONEXCLAMATION);
		ResetEvent(AudioPlayingEvent);
		return 1; 
	} 
 
	// After allocation, set up and prepare header. 
 
	lpWaveHdr->lpData = lpData; 
	lpWaveHdr->dwBufferLength = dwDataSize; 
	lpWaveHdr->dwFlags = 0; 
	lpWaveHdr->dwLoops = 0L; 
	waveOutPrepareHeader(hWaveOut, lpWaveHdr, sizeof(WAVEHDR)); 

	// Now the data block can be sent to the output device. The 
	// waveOutWrite function returns immediately and waveform 
	// data is sent to the output device in the background. 
 
	wResult = waveOutWrite(hWaveOut, lpWaveHdr, sizeof(WAVEHDR)); 
	if (wResult != 0) 
	{ 
		waveOutUnprepareHeader(hWaveOut, lpWaveHdr, 
								sizeof(WAVEHDR)); 
		GlobalUnlock( hData); 
		GlobalFree(hData);
		switch (wResult)
		{
			case MMSYSERR_INVALHANDLE:
					MessageBox(NULL, "Invalid wave handle.",
								NULL, MB_OK | MB_ICONEXCLAMATION);
					break;
			case MMSYSERR_NODRIVER:
					MessageBox(NULL, "No wave device driver is present.",
								NULL, MB_OK | MB_ICONEXCLAMATION);
					break;
			case MMSYSERR_NOMEM:
					MessageBox(NULL, "Unable to allocate or lock memory.",
								NULL, MB_OK | MB_ICONEXCLAMATION);
					break;
			case WAVERR_UNPREPARED:
					MessageBox(NULL, "The data block pointed to by the pwh parameter hasn't been prepared.",
								NULL, MB_OK | MB_ICONEXCLAMATION);
					break;

			default:
					MessageBox(NULL, "Failed to write block to device.", 
								NULL, MB_OK | MB_ICONEXCLAMATION); 
		}
		ResetEvent(AudioPlayingEvent);
		return 1; 
	}
	while (TRUE)
	{
		WaitForSingleObject(AudioPlayingEvent,2);
		if ( (lpWaveHdr->dwFlags & WHDR_DONE) == WHDR_DONE)
		{
			waveOutClose(hWaveOut);
/* 			switch(waveOutClose(hWaveOut))
			{
				case MMSYSERR_INVALHANDLE:
					break;
				case MMSYSERR_NODRIVER:
					break;
				case MMSYSERR_NOMEM:
					break;
				case WAVERR_STILLPLAYING:
					break;
				case MMSYSERR_NOERROR: // No Errors
					break;
			}*/
			break;
		}
	}

	GlobalUnlock( hData); 
	GlobalFree(hData);
	ResetEvent(AudioPlayingEvent);
	return 0;
}

LPSTR GetWaveSample(struct _WaveSample_ * Get_sWav, CMirageEditorDoc* pDoc)
{
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return NULL;
	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV != NULL)
	{
		LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
		memcpy((unsigned char *)Get_sWav,lpWAV,sizeof(Get_sWav->riff_header));
		memcpy((unsigned char *)Get_sWav,lpWAV,Get_sWav->riff_header.riffSIZE+8);
		::GlobalUnlock((HGLOBAL) hWAV);
		return (lpWAV);
	}
	return NULL;
}