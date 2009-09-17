/* $Id: wavesamples.cpp,v 1.17 2008/02/23 23:58:51 root Exp $ */
#include "stdafx.h"
#include <windows.h>
#include <mmsystem.h>
#include <MMReg.h>
#include <math.h>
#include <afxwin.h>

#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "wavesamples.h"
#include "MirageSysex.h"
#include "Mirage EditorDoc.h"
#include "Mirage EditorView.h"

struct _WaveSample_ WaveSample;

void CreateRiffWave(int SampleNumber,int UpperLower)
{
	// Create RIFF Header
	memcpy(WaveSample.riff_header.riffID,"RIFF",4);
	WaveSample.riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_)+(WaveSample.samplepages * MIRAGE_PAGESIZE);
	memcpy(WaveSample.riff_header.riffFORMAT,"WAVE",4);

#ifdef _DEBUG
	fprintf(logfile,"sizeof WaveSample structure: %d\n", sizeof(WaveSample));
#endif

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

	WaveSample.sampler.Loops.dwPlayCount = 1 - ProgramDumpTable[UpperLower].WaveSampleControlBlock[SampleNumber].LoopSwitch;
	WaveSample.sampler.Loops.dwFraction = 0;

	// Create Data Chunk
	memcpy(WaveSample.data_header.dataID,"data",4);
	WaveSample.data_header.dataSIZE = WaveSample.samplepages * MIRAGE_PAGESIZE;
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
	::GlobalUnlock((HGLOBAL) hWAV);
	pDoc->InitWAVData();
	pDoc->CreateNewFromMirage(hWAV);
	pDoc->ResetZoom();
	pDoc->DisplayTypeWavedraw();
	pDoc->SetFromMirage();
	return true;
}

void PlayWaveData(struct _WaveSample_ WaveData)
{
	HANDLE		hData  = NULL;  // handle of waveform data memory 
	HPSTR		lpData = NULL;  // pointer to waveform data memory 
	HWAVEOUT	hWaveOut; 
	HGLOBAL		hWaveHdr; 
	LPWAVEHDR	lpWaveHdr; 
	UINT		wResult; 
	WAVEFORMAT	*pFormat; 
	DWORD		dwDataSize; 

	dwDataSize = WaveData.data_header.dataSIZE-16; // Mirage Adds 16 bytes of zero to indicate end of data
	pFormat = (WAVEFORMAT *)&WaveData.waveFormat.fmtFORMAT;
	lpData = (HPSTR)&WaveData.SampleData;

	// Open a waveform device for output using window callback. 

	if (waveOutOpen((LPHWAVEOUT)&hWaveOut, WAVE_MAPPER, 
					(LPWAVEFORMATEX)pFormat,
					0L, 0L, CALLBACK_NULL))
	{ 
		MessageBox(NULL, 
					"Failed to open waveform output device.", 
					NULL, MB_OK | MB_ICONEXCLAMATION); 
		return; 
	} 
 
	// Allocate and lock memory for the header. 

	hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, 
		(DWORD) sizeof(WAVEHDR)); 
	if (hWaveHdr == NULL) 
	{ 
		MessageBox(NULL, "Not enough memory for header.", 
					NULL, MB_OK | MB_ICONEXCLAMATION); 
		return; 
	} 
 
	lpWaveHdr = (LPWAVEHDR) GlobalLock(hWaveHdr); 
	if (lpWaveHdr == NULL) 
	{ 
		GlobalUnlock(hData); 
		GlobalFree(hData); 
		MessageBox(NULL, 
			"Failed to lock memory for header.", 
			NULL, MB_OK | MB_ICONEXCLAMATION); 
		return; 
	} 
 
	// After allocation, set up and prepare header. 
 
	lpWaveHdr->lpData = lpData; 
	lpWaveHdr->dwBufferLength = dwDataSize; 
	lpWaveHdr->dwFlags = 0L; 
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
		MessageBox(NULL, "Failed to write block to device", 
					NULL, MB_OK | MB_ICONEXCLAMATION); 
		return; 
	}
	GlobalUnlock( hData); 
	GlobalFree(hData); 
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