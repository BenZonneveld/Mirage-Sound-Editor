// $Id: Wavapi.cpp,v 1.19 2008/02/23 23:58:51 root Exp $
#include "stdafx.h"
#include "windows.h"
#include "Globals.h"
#include	<vector>
#include	<cassert>


#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "wavesamples.h"
#include "Dialog_Resample.h"
#include "MirageSysex.h"
#include "resource.h"
#include "samplerate.h"
#include "float_cast.h"
#include "Wavapi.h"
#include <math.h>

DWORD StereoToMono(unsigned char *lpDataIn, WORD BlockAlign, WORD wBitsPerSample, DWORD DataSize)
{
	unsigned char *lpDataOut;
	short *	lpDataOut_16;
	short * lpDataIn_16;
	unsigned int c, TargetCount;

	switch (wBitsPerSample)
	{
	case 8:	lpDataOut = (unsigned char *) ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (DWORD)DataSize);
//			memcpy(lpDataOut, lpDataIn, DataSize);
			break;
	case 16:
			lpDataOut_16 = (short *) ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (DWORD)DataSize*sizeof(short));
			lpDataIn_16 = (short *) ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (DWORD)DataSize*sizeof(short));
			memcpy(lpDataOut_16, lpDataIn, DataSize);
			memcpy(lpDataIn_16, lpDataIn, DataSize);
			break;
	}

//	DataWidth = wBitsPerSample >> 3;
	for( c = 0; c <= DataSize ; c += 2 ) 
	{
		TargetCount = c / 2 ;
		switch (wBitsPerSample)
		{
		case 8:	lpDataOut[TargetCount] = ((unsigned char)lpDataIn[c] + (unsigned char)lpDataIn[c+1]) / 2;
				break;
		case 16:
				lpDataOut_16[TargetCount] = ((short)lpDataIn_16[c] + (short)lpDataIn_16[c + 1]) / 2;
				break;
		}
	}
	if ( wBitsPerSample == 8 )
	{
		memcpy(lpDataIn, lpDataOut, TargetCount);
		return (TargetCount);
	} else {
		memcpy(lpDataIn, lpDataOut_16, TargetCount);
		return ( TargetCount );
	}
}

DWORD ConvertTo8Bit(short *lpDataIn, DWORD DataSize)
{
	unsigned char *	lpDataOut;
	unsigned int c;
	unsigned char NewValue;

	lpDataOut = (unsigned char *) ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (DWORD)DataSize);
	for( c = 0; c < DataSize ; c++)
	{
		NewValue = (32767+lpDataIn[c]) >> 8;/*unsigned char(lrint(((unsigned float)(32767+lpDataIn[c]))/256));*/
		lpDataOut[c] = NewValue;
	}
	memcpy(lpDataIn, lpDataOut,DataSize/2);
	return (DataSize/2);
}

DWORD     WINAPI  WAVSize(LPSTR lpWAV)
{
	_WaveSample_ sWav;

	memcpy((unsigned char *)&sWav,lpWAV,(sizeof(sWav.riff_header)+
						sizeof(sWav.waveFormat)+
						sizeof(sWav.sampler)+
						sizeof(sWav.instrument)+
						sizeof(sWav.data_header)
						));

	return sWav.data_header.dataSIZE;
}

DWORD     WINAPI  WAVChannels(LPSTR lpWAV)
{
	_WaveSample_ sWav;

	memcpy((unsigned char *)&sWav,lpWAV,(sizeof(sWav.riff_header)+
						sizeof(sWav.waveFormat)));

	return (sWav.waveFormat.fmtFORMAT.nChannels);
}

BOOL WINAPI SaveWAV(MWAV hWav, CFile& file)
{
	struct _WaveSample_ sWav;
	LPSTR lpWAV;   // Pointer to WAV info structure
	UINT nWAVSize;

	if (hWav == NULL)
		return FALSE;

	/*
	 * Get a pointer to the WAV File memory, the first parts contain
	 * the RIFF header info
	 */
	lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWav);
	if (lpWAV == NULL)
		return FALSE;

	memcpy((unsigned char *)&sWav,lpWAV,sizeof(sWav.riff_header));
	memcpy((unsigned char *)&sWav,lpWAV,sWav.riff_header.riffSIZE+8);

	// Now calculate the size of the wavefile

	nWAVSize = (sWav.riff_header.riffSIZE+8);

	TRY
	{
		//
		// Write the WAV header and the bits
		//
		file.Write(lpWAV, nWAVSize);
	}
	CATCH (CFileException, e)
	{
		::GlobalUnlock((HGLOBAL) hWav);
		THROW_LAST();
	}
	END_CATCH

	::GlobalUnlock((HGLOBAL) hWav);
	return TRUE;
}

MWAV WINAPI ReadWAVFile(CFile& file)
{
	HMMIO		hmmio;				// file handle for open file 
	MMCKINFO	mmckinfoParent;		// parent chunk information 
	MMCKINFO	mmckinfoSubchunk;	// subchunk information structure 
	DWORD		dwFmtSize;			// size of "FMT" chunk
	DWORD		dwSmplSize;			// size of "SMPL" chunk
	DWORD		dwInstSize;			// size of "inst" chunk
	DWORD		dwDataSize;			// size of "DATA" chunk
	const unsigned char	*lpDataIn;			// Pointer to memory for file to be loaded
	DWORD		dwRiffSize;			// size of "RIFF" chunk?
	MWAV		hWAV;
	LPSTR		pWAV;
	struct	_WaveSample_ sWav;	// The Wavesample structure for the Mirage
	char szFileName[1024];
	// For automatic Resampling:
	SRC_STATE	*src_state ;		// The sample rate converter object
	SRC_DATA	src_data;			// struct to pass audio and control data into the sample rate converter
	int channels = MAX_WAVCHANNELS;	// the number of interleaved channels that the sample rate converter is being asked to process
	int			srcErrorCode;
	int			newRate;
	double		srcRatio = 1.0;
	float		*lpFloatDataIn;		// Float buffer for sample rate conversion
	float		*lpFloatDataOut;
	double		gain = 1.0;
	double		max_gain = 0.0 ;

	// Open the file for reading with buffered I/O 
    // by using the default internal buffer 
	sprintf_s(szFileName,sizeof(szFileName),"%s",file.GetFilePath());
	if(!(hmmio = mmioOpen(szFileName, NULL, 
		MMIO_READ | MMIO_ALLOCBUF))) 
	{ 
		MessageBox(NULL,"Failed to open file.",NULL,MB_ICONERROR | MB_OK); 
		return NULL; 
	}

	// Locate a "RIFF" chunk with a "WAVE" form type to make 
	// sure the file is a waveform-audio file. 
	mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
	if (mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, 
		MMIO_FINDRIFF)) 
	{ 
		MessageBox(NULL,"This is not a waveform-audio file.",NULL,MB_ICONERROR | MB_OK); 
		mmioClose(hmmio, 0); 
		return NULL; 
	}
	dwRiffSize = mmckinfoParent.cksize + 8;

	memcpy((unsigned char *)&sWav.riff_header,(unsigned char *)&mmckinfoParent,sizeof(sWav.riff_header));
	if ( dwRiffSize > sizeof(sWav) )
	{
		hWAV = (MWAV) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (DWORD)dwRiffSize);
	} else {
		hWAV = (MWAV) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (DWORD)sizeof(sWav));
	}

	if (hWAV == NULL)
	{
		MessageBox(NULL,"Unable to allocate memory for file loading.",NULL,MB_ICONERROR | MB_OK); 
		mmioClose(hmmio, 0); 
		return NULL;
	}
	pWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);

	// Find the "FMT" chunk (form type "FMT"); it must be 
	// a subchunk of the "RIFF" chunk. 
	mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' '); 
	if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
		MMIO_FINDCHUNK) != MMSYSERR_NOERROR ) 
	{ 
		MessageBox(NULL, "Waveform-audio file has no \"FMT\" chunk.", NULL, MB_ICONERROR | MB_OK); 
		mmioClose(hmmio, 0); 
		::GlobalUnlock((HGLOBAL) hWAV);
		::GlobalFree((HGLOBAL) hWAV);
		return NULL; 
	} 
 
	// Get the size of the "FMT" chunk.
	dwFmtSize = mmckinfoSubchunk.cksize;
	// Read the "FMT" chunk. 
	if (mmioRead(hmmio, (HPSTR) &sWav.waveFormat.fmtFORMAT, dwFmtSize) != dwFmtSize){ 
        MessageBox(NULL,"Failed to read format chunk.", NULL, MB_ICONERROR | MB_OK); 
        mmioClose(hmmio, 0);
		::GlobalUnlock((HGLOBAL) hWAV);
		::GlobalFree((HGLOBAL) hWAV);
        return NULL; 
    }
	// Put the size in the sWav and the fmtID
	memcpy(sWav.waveFormat.fmtID,"fmt ",4);
	sWav.waveFormat.fmtSIZE = dwFmtSize;

	if ( sWav.waveFormat.fmtFORMAT.wBitsPerSample > 16 )
	{
		MessageBox(NULL,"Wavefile is more than 16 bits, only 8 and 16 bits audio is supported.", NULL, MB_ICONERROR | MB_OK); 
        mmioClose(hmmio, 0);
		::GlobalUnlock((HGLOBAL) hWAV);
		::GlobalFree((HGLOBAL) hWAV);
        return NULL;
	}
	// Ascend out of the "FMT" subchunk. 
	mmioAscend(hmmio, &mmckinfoSubchunk, 0);

	//// Find a smpl subchunk
	//mmckinfoSubchunk.ckid = mmioStringToFOURCC("smpl",0);
	//if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
	//	MMIO_FINDCHUNK) == MMSYSERR_NOERROR ) 
	//{ 
	//	;
	//}
	//// Ascend out of the "FMT" subchunk. 
	//mmioAscend(hmmio, &mmckinfoSubchunk, 0);

	// Find the data subchunk. The current file position should be at 
	// the beginning of the data chunk; however, you should not make 
	// this assumption. Use mmioDescend to locate the data chunk. 
	mmckinfoSubchunk.ckid = mmioStringToFOURCC("data", 0);
	if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
		MMIO_FINDCHUNK) != MMSYSERR_NOERROR ) 
	{ 
		MessageBox(NULL,"Waveform-audio file has no data chunk.", NULL, MB_ICONERROR | MB_OK); 
		mmioClose(hmmio, 0); 
		::GlobalUnlock((HGLOBAL) hWAV);
		::GlobalFree((HGLOBAL) hWAV);
		return NULL; 
	} 
 
	// Get the size of the data subchunk. 
	dwDataSize = mmckinfoSubchunk.cksize;
	if (dwDataSize == 0L){ 
        MessageBox(NULL,"The data chunk contains no data.", NULL, MB_ICONERROR | MB_OK);  
		mmioClose(hmmio, 0); 
		::GlobalUnlock((HGLOBAL) hWAV);
		::GlobalFree((HGLOBAL) hWAV);
		return NULL; 
	}

	// Allocate memory for input data
	lpDataIn = (const unsigned char *) ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (DWORD)(dwDataSize * (sWav.waveFormat.fmtFORMAT.wBitsPerSample/8)));
	 
	// Read the waveform-audio data subchunk. 
	if(mmioRead(hmmio, (HPSTR) lpDataIn, dwDataSize) != dwDataSize){ 
		MessageBox(NULL, "Failed to read data chunk.", NULL, MB_ICONERROR | MB_OK); 
		mmioClose(hmmio, 0);
		::GlobalUnlock((HGLOBAL) hWAV);
		::GlobalFree((HGLOBAL) hWAV);
		::GlobalFree((HGLOBAL) lpDataIn);
		return NULL; 
	}

	// Check if the sample is a mono sample
	if ( sWav.waveFormat.fmtFORMAT.nChannels > MAX_WAVCHANNELS )
	{	
		CString strMsg;
		if ( theApp.GetProfileIntA("Settings","Stereo To Mono",true) == false )
		{
			strMsg.LoadString(IDS_WAV_NOT_MONO);
			MessageBox(NULL, strMsg, NULL, MB_ICONERROR| MB_OK);
			return NULL;
		}
		// We only handle stereo samples:
		if ( sWav.waveFormat.fmtFORMAT.nChannels > 2 )
		{
			MessageBox(NULL, "Sample has more than 2 channels.", NULL, MB_ICONERROR | MB_OK);
			mmioClose(hmmio, 0);
			::GlobalUnlock((HGLOBAL) hWAV);
			::GlobalFree((HGLOBAL) hWAV);
			::GlobalFree((HGLOBAL) lpDataIn);
			return NULL;
		}
		// Convert stereo sample to mono sample
		dwDataSize = StereoToMono((unsigned char *)lpDataIn,
									sWav.waveFormat.fmtFORMAT.nBlockAlign,
									sWav.waveFormat.fmtFORMAT.wBitsPerSample,
									dwDataSize);
		sWav.waveFormat.fmtFORMAT.nChannels = 1;
		sWav.waveFormat.fmtFORMAT.nAvgBytesPerSec = sWav.waveFormat.fmtFORMAT.nAvgBytesPerSec / 2;
		sWav.waveFormat.fmtFORMAT.nBlockAlign = sWav.waveFormat.fmtFORMAT.nBlockAlign / 2;
		sWav.riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_)+dwDataSize-8;
	}

	switch (sWav.waveFormat.fmtFORMAT.wBitsPerSample)
	{
	case 8:
			break;
	case 16:
			dwDataSize = ConvertTo8Bit((short *)lpDataIn, dwDataSize);
			sWav.riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_)+sizeof(_instrument_)+dwDataSize/*-8*/;
			sWav.waveFormat.fmtFORMAT.wBitsPerSample = 8;
			sWav.waveFormat.fmtFORMAT.nBlockAlign = sWav.waveFormat.fmtFORMAT.nBlockAlign / 2;
			sWav.waveFormat.fmtFORMAT.nAvgBytesPerSec = sWav.waveFormat.fmtFORMAT.nAvgBytesPerSec / 2;
			break;
	}
			
	// Resample the data if needed
	if ( dwDataSize > sizeof(sWav.SampleData) ) // Truncate samples which are too Big for the Mirage
	{
		if ( theApp.GetProfileIntA("Settings","DoResampling",true) == false )
		{
			MessageBox(NULL,"Wavefile is to large for the Mirage (max 64 Kb)", NULL, MB_ICONERROR | MB_OK);
			mmioClose(hmmio, 0);
			::GlobalUnlock((HGLOBAL) hWAV);
			::GlobalFree((HGLOBAL) hWAV);
			return NULL;
		}
		CResample_Dialog ResampleDlg;

		// Allocate memory
		lpFloatDataIn = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,(DWORD)dwDataSize*sizeof(float));
		lpFloatDataOut = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(sWav.SampleData)*sizeof(float)); 
		srcRatio = sizeof(sWav.SampleData) / ( 1.0 * dwDataSize );
		src_data.src_ratio = srcRatio;
		src_data.data_in = lpFloatDataIn;
		src_data.data_out = lpFloatDataOut;
		src_data.input_frames = dwDataSize;
		src_data.input_frames_used = 0;
		src_data.output_frames = sizeof(sWav.SampleData);
		src_data.output_frames_gen = 0;
		src_data.end_of_input = 1;
		newRate = (int)floor(srcRatio * sWav.waveFormat.fmtFORMAT.nSamplesPerSec);
		ResampleDlg.m_lpSrcData = (LPSTR *)&src_data;
		ResampleDlg.m_currentpages = 0xFF;

		ResampleDlg.DoModal();

		if (ResampleDlg.m_resample_cancel_ok == false )
		{
			mmioClose(hmmio, 0);
			::GlobalUnlock((HGLOBAL) hWAV);
			::GlobalFree((HGLOBAL) hWAV);
			//::GlobalFree((HGLOBAL) lpDataIn);
			return NULL;
		}

		// Create ProgressBar Window
		progress.Create(CProgressDialog::IDD, NULL);
		progress.SetWindowTextA("Resampling Progress");

resample:
		src_unchar_to_float_array(lpDataIn, lpFloatDataIn, (int)dwDataSize);
		if(src_is_valid_ratio(src_data.src_ratio) == 0)
		{
			MessageBox(NULL,"Sample rate change out of valid range", NULL, MB_ICONERROR | MB_OK);
			goto src_out2;
		}
		/* Initialize the sample rate converter. */
		if ((src_state = src_new (theApp.GetProfileIntA("Settings","SampleRateConverter",0), channels, &srcErrorCode)) == NULL)
			goto src_out;
		if ((srcErrorCode=src_process(src_state, &src_data)))
		{
src_out:	MessageBox(NULL,src_strerror(srcErrorCode), NULL, MB_ICONERROR | MB_OK);
src_out2:	mmioClose(hmmio, 0);
			::GlobalUnlock((HGLOBAL) hWAV);
			::GlobalFree((HGLOBAL) hWAV);
			::GlobalFree((HGLOBAL) lpDataIn);
			src_state = src_delete (src_state) ;
			return NULL;
		}
maxgain:
		max_gain = 0.0;
		max_gain = apply_gain(src_data.data_out, src_data.output_frames_gen, channels, max_gain, gain);
		// Redo resample if the gain is too large
		if (max_gain > 1.0)
		{	
			gain = 1.0 / max_gain;
			progress.SetWindowTextA("Changing gain and redoing Resampling");
			goto resample;
		}
		if ( max_gain < 1.0 )
		{
			gain = 1.0 / max_gain;
			goto maxgain;
		}
		src_state = src_delete (src_state);
		src_float_to_unchar_array (lpFloatDataOut, (unsigned char *)&sWav.SampleData, src_data.output_frames_gen);
		/* int src_simple (SRC_DATA *data, int converter_type, int channels) ; */
		dwDataSize = sizeof(sWav.SampleData);
		sWav.waveFormat.fmtFORMAT.nSamplesPerSec = newRate;
		sWav.waveFormat.fmtFORMAT.nAvgBytesPerSec = newRate;
		progress.DestroyWindow();
	}
	else
	{
		memcpy((unsigned char *)&sWav.SampleData,lpDataIn,dwDataSize);
	}
	::GlobalFree((HGLOBAL) lpDataIn); 

	memcpy(sWav.data_header.dataID,"data",4);
	sWav.data_header.dataSIZE = dwDataSize;
	mmioClose(hmmio, 0);

	// To Read the smpl chunk we have to reopen the damn file
	// Must be some Microsucks glitch or my fantastic programming skills..
	if(!(hmmio = mmioOpen(szFileName, NULL, 
		MMIO_READ | MMIO_ALLOCBUF))) 
	{ 
		MessageBox(NULL,"Failed to open file.",NULL,MB_ICONERROR | MB_OK); 
		return NULL; 
	}

	// Locate a "RIFF" chunk with a "WAVE" form type to make 
	// sure the file is a waveform-audio file. 
	mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E'); 
	if (mmioDescend(hmmio, (LPMMCKINFO) &mmckinfoParent, NULL, 
		MMIO_FINDRIFF)) 
	{ 
		MessageBox(NULL,"This is not a waveform-audio file.",NULL,MB_ICONERROR | MB_OK); 
		mmioClose(hmmio, 0); 
		::GlobalUnlock((HGLOBAL) hWAV);
		::GlobalFree((HGLOBAL) hWAV);
		return NULL; 
	}
	// Find the "SMPL" subchunk. The current file position should be at
	// the beginning of the smpl chunk; however we should not make this 
	// assumption. The SMPL chunk is not a mandatory chunk after all.
	memset(&sWav.sampler, 0, sizeof(sWav.sampler));
	mmckinfoSubchunk.ckid = mmioFOURCC('s', 'm', 'p', 'l');
	if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
		MMIO_FINDCHUNK) != MMSYSERR_NOERROR ) 
	{
		sWav.sampler.Manufacturer = 0x0F; // Ensoniq
		sWav.sampler.Product = 0x01; // Mirage
		sWav.sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)sWav.waveFormat.fmtFORMAT.nSamplesPerSec);
		sWav.sampler.MIDIUnityNote = 60;
		sWav.sampler.MIDIPitchFraction = 0;
		sWav.sampler.SMPTEFormat = 0; // Mirage does not use SMPTE offsets
		sWav.sampler.SMPTEOffset = 0; // No SMPTE offset for the Mirage
		sWav.sampler.cbSamplerData = 0; 
		sWav.sampler.cSampleLoops = 1; // The Mirage always has loop points set
		// Now set the loop for this wavesample
		sWav.sampler.Loops.dwIdentifier = 0;
		sWav.sampler.Loops.dwType = 0;
		sWav.sampler.Loops.dwStart = sWav.data_header.dataSIZE - 0x100; 
		sWav.sampler.Loops.dwEnd = sWav.data_header.dataSIZE - 16;
		sWav.sampler.Loops.dwPlayCount = 1;
		sWav.sampler.Loops.dwFraction = 0;
	}
	else
	{
		// Get the size of the "smpl" chunk.
		dwSmplSize = mmckinfoSubchunk.cksize;
		// Read the "smpl" chunk. 
		if (mmioRead(hmmio, (HPSTR) &sWav.sampler+8, dwSmplSize) != dwSmplSize){ 
		    MessageBox(NULL,"Failed to read sampler chunk.", NULL, MB_ICONERROR | MB_OK); 
		    mmioClose(hmmio, 0); 
			::GlobalUnlock((HGLOBAL) hWAV);
			::GlobalFree((HGLOBAL) hWAV);
		    return NULL; 
		}
		sWav.sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)sWav.waveFormat.fmtFORMAT.nSamplesPerSec);
		sWav.sampler.Loops.dwStart = lrint(sWav.sampler.Loops.dwStart * srcRatio)&0xFF00;
		sWav.sampler.Loops.dwEnd = lrint(sWav.sampler.Loops.dwEnd * srcRatio);
	}

	// Put the size in sWav and the smplID
	memcpy(sWav.sampler.samplerID,"smpl",4);
	sWav.sampler.samplerSize = 60; // Size of the chunk is 60 bytes

	memset(&sWav.instrument, 0, sizeof(sWav.instrument));
	mmckinfoSubchunk.ckid = mmioFOURCC('i', 'n', 's', 't');
	if (mmioDescend(hmmio, &mmckinfoSubchunk, &mmckinfoParent, 
		MMIO_FINDCHUNK) != MMSYSERR_NOERROR ) 
	{
		sWav.instrument.unshifted_note = sWav.sampler.MIDIUnityNote;
		sWav.instrument.fine_tune = 0;
		sWav.instrument.gain = 0;
		sWav.instrument.low_note = 0; 
		sWav.instrument.high_note = 0x7F;
		sWav.instrument.low_velocity = 0x01;
		sWav.instrument.high_velocity = 0x7F;
		sWav.instrument.padding = 0;
	}
	else
	{
		// Get the size of the "smpl" chunk.
		dwInstSize = mmckinfoSubchunk.cksize;
		// Read the "inst" chunk. 
		if (mmioRead(hmmio, (HPSTR) &sWav.instrument+8, dwInstSize ) != dwInstSize ){ 
		    MessageBox(NULL,"Failed to read sampler chunk.", NULL, MB_ICONERROR | MB_OK); 
		    mmioClose(hmmio, 0); 
			::GlobalUnlock((HGLOBAL) hWAV);
			::GlobalFree((HGLOBAL) hWAV);
		    return NULL; 
		}
	}

	// Put the size in sWav and the smplID
	memcpy(sWav.instrument.chunk_id,"inst",4);
	sWav.instrument.inst_size = 8; // Size of the chunk is 8 bytes

	mmioClose(hmmio, 0);

	sWav.sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)sWav.waveFormat.fmtFORMAT.nSamplesPerSec);

//	RemoveZeroSamples(&sWav);
	
	if ( dwRiffSize > sizeof(sWav) )
	{
		// Update the Riff Header!
		sWav.riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_)+sizeof(_instrument_)+sizeof(sWav.SampleData);
		memcpy(pWAV,(unsigned char*)&sWav, sizeof(sWav));
	} else {
		// Update the Riff Header!
		sWav.riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_)+sizeof(_instrument_)+dwDataSize;
		memcpy(pWAV,(unsigned char*)&sWav, dwRiffSize);
	}

	::GlobalUnlock((HGLOBAL) hWAV);
	return hWAV;
}

void RemoveZeroSamples(struct _WaveSample_ * sWav)
{
	int zerofix;
	/* The value 0 has a special function: 
	 * it acts as a marker which tells the 
	 * oscillators where to stop.
	 * So we replace all 0 values with 1.
	 */
	for(zerofix = 0 ; zerofix <= int(sWav->data_header.dataSIZE); zerofix++)
	{
		if ( sWav->SampleData[zerofix] == 0 )
			sWav->SampleData[zerofix] = 1;
	}
}

/*void	uchar_to_vector (std::vector <float> &v,  long len, const unsigned char *in)
{
	assert (&v != 0);
	assert (len > 0);
	char NewValue;

	v.resize (len);
	for (long pos = 0; pos < len; pos++)
	{
		using namespace std;

		NewValue = in[pos]-128;
		v [pos] = static_cast <float> ((float)NewValue/(float)0x80);
	}
}

void	short_to_vector (std::vector <float> &v,  long len, const short *in)
{
	assert (&v != 0);
	assert (len > 0);
	short NewValue;

	v.resize (len);
	for (long pos = 0; pos < len; pos++)
	{
		using namespace std;

		NewValue = in[pos];
		v [pos] = static_cast <float> ((float)NewValue/(float)0x80);
	}
}

int vector_to_uchar (std::vector <float> &v, long len, unsigned char *out)
{
	assert (&v != 0);
	assert (len > 0);
	float scaled_value ;

	long maxsize;

	maxsize=( 65535 <= v.size() ) ? 65535 : v.size();

	for (long pos = 0 ; pos < maxsize ; pos++ )
	{
		using namespace std;

		scaled_value = float((v [pos] + 1) * 128.0f) ;
		if ( scaled_value < 0 )
		{
			scaled_value = 0;
		}
		if ( scaled_value > 255 )
		{
			scaled_value = 255;
		}
		out [pos] = (unsigned char)(lrintf (scaled_value)) ;
	}
	return (maxsize);
}

int vector_to_short (std::vector <float> &v, long len, short *out)
{
	assert (&v != 0);
	assert (len > 0);
	float scaled_value ;

	long maxsize;

	maxsize=( 65535 <= v.size() ) ? 65535 : v.size();

	for (long pos = 0 ; pos < maxsize ; pos++ )
	{
		using namespace std;

		scaled_value = float((v [pos]) * 32767.0f) ;
		if ( scaled_value < -32768 )
		{
			scaled_value = 0;
		}
		if ( scaled_value > 32767 )
		{
			scaled_value = 32767;
		}
		out [pos] = (unsigned char)(lrintf (scaled_value) ) ;
	}
	return (maxsize);
}

int	AverageSamplesPeriod(struct _WaveSample_ * sWav, int range_start, int range_end)
{
	int p;
	int vp = 0;
	std::vector <int> period;
	int start = -1;
	int average = 0, max = 0, temp = 0;

	//period.resize(1);
	for (p = range_start; p<range_end; p++)
	{
		// Try to detect a Wavesample zero crossing
		if ( sWav->SampleData[p] > 0x80 && (sWav->SampleData[(p-1)] < 0x80 || sWav->SampleData[p-1] == 0x80)  )
		{
			if ( start > -1 )
			{
				if ( p - start > 0 ) 
				{
					vp = period.size();
					period.resize(period.size()+1);
					// Size detected
					period [vp] = p - start;
					// Set max;
					if (period [vp] > max )
						max = period[vp];
				}
			}
			start = p;
		}
	}

	// Now determine the average from the vector
	p=0;
	for ( vp=0; vp < period.size(); vp++)
	{
		if ( (2*(period[vp]/3)) < (max/2))
		{
			vp++;
			temp = period[vp]+period[vp-1];
			if ( (2*(temp/3)) < (max/2))
			{
				continue;
			} else {
				average += temp;
				p++;
			}
		} else {
			average += period[vp];
			p++;
		}
	}

	if (p > 0 )
		average = average / p;
	return (average);
}*/

void ResizeRiff(struct _WaveSample_ * sWav, DWORD NewSize)
{
	DWORD Oldsize = sWav->data_header.dataSIZE;
	DWORD Resize = Oldsize-NewSize;

	sWav->riff_header.riffSIZE = sWav->riff_header.riffSIZE - Resize;

	sWav->data_header.dataSIZE = sWav->data_header.dataSIZE - Resize;
}