/* $Id: MirageSysex.cpp,v 1.18 2008/02/23 23:58:51 root Exp $ */

#include "stdafx.h"
#include "Globals.h"

#include "Mirage Editor.h"

#ifdef _DEBUG
#include "sysexdebug.h"
#endif

#include "Nybble.h"
#include "Mirage EditorDoc.h"
#include "Mirage EditorView.h"
#include "SendSysex.h"
#include "wavesamples.h"
#include "float_cast.h"
#include "Mirage Helpers.h"
#include "Tuning.h"
#include "Dialog_OrigKey.h"
#include "Dialog_TxSamParms.h"
//#include <windows.h>
//#include <mmsystem.h>
#include "MirageSysex.h"
#include "MirageParameters.h"
#include "Mirage Sysex_Strings.h"
//#include "CMidiReceiver.h"
//#include "GetMidi.h"
#include "LongMsg.h"

midi::CLongMsg LongMsg;

char	DataDumped = 0;
unsigned char	lower_upper_select = 0;
unsigned char	ProgramStatus = 0xFF;
unsigned char	WavesampleStatus = 0xFF;
unsigned char	WavesampleStore = 0;

//HANDLE midi_in_event;

/* For setting the original key */
unsigned char LastMidiKey;

#pragma pack (1)
struct _program_dump_table_ ProgramDumpTable[2];

#pragma pack(1)
struct _config_dump_table_ ConfigDump;

int MirageOS;

BOOL GetAvailableSamples(void)
{
	DWORD wait_state;
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Available Lower Samples");
	progress.Bar.SetRange32(0,1);

	ResetEvent(midi_in_event);
	SendData(ProgramDumpReqLower);
	wait_state = WaitForSingleObject(midi_in_event,10000);
	progress.DestroyWindow();

	if (wait_state == WAIT_TIMEOUT)
	{
#ifdef NDEBUG
			MessageBox(NULL,"No Response from ProgramDumpReqLower in MirageSysex -> GetAvailableSamples\n", "Error", MB_ICONERROR);
#else
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
#endif
		return false;
	}

	// Should be aprox 625 bytes of Program Dump Data
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Available Upper Samples");
	progress.Bar.SetRange32(0,1);

//	ExpectSysex(ProgramDumpUpper);
	ResetEvent(midi_in_event);
	SendData(ProgramDumpReqUpper);

	wait_state = WaitForSingleObject(midi_in_event,10000);
	progress.DestroyWindow();

	if (wait_state == WAIT_TIMEOUT)
	{
#ifdef NDEBUG
		MessageBox(NULL,"No Response from ProgramDumpReqLower in MirageSysex -> GetAvailableSamples\n", "Error", MB_ICONERROR);
#else
		MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
#endif
		return false;
	}

	return true;
}

BOOL GetSampleParameters(void)
{
	/*
	 * Get the sample parameters for the lower programs
	 */
	ResetEvent(midi_in_event);
	SendData(ProgramDumpReqLower);
	DWORD wait_state = WaitForSingleObject(midi_in_event,PROGDUMP_TIMEOUT);
	if (wait_state == WAIT_TIMEOUT)
	{
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			return false;
	}

	/*
	 * Get the sample parameters for the upper programs
	 */
	ResetEvent(midi_in_event);
	SendData(ProgramDumpReqUpper);

	wait_state = WaitForSingleObject(midi_in_event,PROGDUMP_TIMEOUT);
	if (wait_state == WAIT_TIMEOUT)
	{
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			return false;
	}

	return true;
}

BOOL DoSampleSelect(unsigned char *SampleSelect,unsigned char SampleNumber)
{
	unsigned char ExpectedWavesample;
	unsigned char SelectedWavesample;
	unsigned char ul_Wavesample;
	unsigned char * pSample = NULL;
	int trycount=0;
	char errorstring[50];
	DWORD wait_state;

	unsigned char SampleNumberSelect[]={7,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									(SampleNumber+1),
									0x7F,
									0xF7}; // Select Sample

	// Start Recording
	ProgramStatus = 0xFF;
	WavesampleStatus = 0xFF;

	ResetEvent(midi_in_event);
	SendData(SampleSelect);
	/*
	/* Now check the response from the Mirage
	/* Get Program Status Message (That's what the MASOS documentation calls it.
	/* This seems like a bug in the manual because the Mirage actualy returns
	/* The PREVIOUSLY selected wavesample
	/* Value is put into "ProgramStatus" 
	*/
	wait_state = WaitForSingleObject(midi_in_event,250);
	if (wait_state == WAIT_TIMEOUT )
	{
		MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		return false;
	}
// ACCESS VIOLATION	ProgramStatus = LongMsg.GetMsg()[4] -1;
	
	Sleep(10);
	ResetEvent(midi_in_event);
	SendData(SampleNumberSelect);
	/*
	/* Get Wavesample Status Message
	/* This is the response from the Mirage with the current selected wavesample
	/* We check if this is the correct sample we are expecting
	/* Value is put into "WavesampleStatus"
	*/
	wait_state = WaitForSingleObject(midi_in_event,1000);
	if (wait_state == WAIT_TIMEOUT )
	{
		MessageBox(NULL,"Error while getting Wavesample Status.\nMIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		return false;
	}

	/*
	/* Determine lower or upper sample expected
	/* And the expected sample number
	*/
	
//	WavesampleStatus = LongMsg.GetMsg()[4];
	WavesampleStatus = theApp.m_WavesampleStatus;
	ExpectedWavesample = SampleNumber;
	ul_Wavesample = (WavesampleStatus & 0xF0) >> 1;
	SelectedWavesample = WavesampleStatus & 0x0F;
	WavesampleStore = SelectedWavesample; // + ul_Wavesample; // Where to store the received sample

	switch(SampleSelect[5])
	{
	case 0x15: // Lower Sample
		{
			if ( ul_Wavesample != 0 )
			{
				MessageBox(NULL,"Selected Lower Sample, but got Upper Sample?!","Error", MB_ICONERROR);
				return false;
			}
			break;
		}
	case 0x14: // Upper Sample
		{
			if ( ul_Wavesample != 8 )
			{
				MessageBox(NULL,"Selected Upper Sample, but got Lower Sample?!","Error", MB_ICONERROR);
				return false;
			}
			break;
		}
	}
	if ( ExpectedWavesample != SelectedWavesample )
	{
		sprintf_s(errorstring,sizeof(errorstring),"Selected sample %d but got %d from Mirage.",SelectedWavesample,ExpectedWavesample);
		MessageBox(NULL,(CString)errorstring, "Error", MB_ICONERROR);
		return false;
	}
	return true;
}


BOOL GetSample(unsigned char *SampleSelect, unsigned char SampleNumber)
{
	short pages;
	int	pagecount=0;
	BOOL LoopSwitch;
	DWORD wait_state;
	byte trycount=0;

#ifdef NDEBUG
	sysexdump(SampleSelect,"Transmitting (sampleselect)");
#endif

	if (DoSampleSelect(SampleSelect,SampleNumber) == false)
		return false;
	unsigned char bank = (0x15-SampleSelect[5]);
	unsigned char ul_Wavesample = (WavesampleStatus & 0xF0) >> 1;

	pages = 1 + (ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleEnd - ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleStart);

	/* If there is a loop enabled disable this before receiving the sample */
	if (ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopSwitch == 1 )
	{
		LoopSwitch = true;
		SendData(LoopOff);
	} else {
		LoopSwitch = false;
	}

	/* Now Request the selected sample from the Mirage */
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Sample Data");
retry:
	progress.Bar.SetRange32(0, pages*MIRAGE_PAGESIZE);
	// Reset the progress
	progress.progress(0);

	ResetEvent(midi_in_event);
	SendData(WaveDumpReq);

	wait_state = WaitForSingleObject(midi_in_event, (2*pages*MIRAGE_PAGESIZE));
	if (wait_state == WAIT_TIMEOUT )
	{
		MessageBox(NULL,"MIDI timeout while getting sample\n", "Error", MB_ICONERROR);
		progress.DestroyWindow();
		return false;
	}
	progress.DestroyWindow();

	if(WaveSample.checksum != GetChecksum(&WaveSample))
	{
		progress.DestroyWindow();
		trycount++;
		SendData(WavesampleNack);
		if ( trycount > 3 )
		{
			MessageBox(NULL,"Sample checksum not correct.","ERROR",MB_ICONERROR);
			return false;
		} else {
//			progress.Create(CProgressDialog::IDD, NULL);
			progress.SetWindowTextA("Getting Sample Data (retry)");
			goto retry;
		}
	} else {
		SendData(WavesampleAck);
	}
	progress.DestroyWindow();
	
	/* Remember to switch the loop back on */
	if ( LoopSwitch == TRUE )
	{
		SendData(LoopOn);
	}

	CreateRiffWave(WavesampleStore, (ul_Wavesample >> 4), LoopSwitch );
	CreateFromMirage(WavesampleStore,ul_Wavesample);

	return true;
}

BOOL PutSample(unsigned char *SampleSelect,unsigned char SampleNumber, bool LoopOnly)
{
	COrigKey		GetOriginalKey;
	CTxSamParms TxSamParams;
	_WaveSample_ *pWav;
	unsigned char TransmitSamplePages;
	DWORD		DataSize;
//	unsigned char *TransmitSample;
	unsigned char LsNybble;
	unsigned char MsNybble;
	unsigned int counter;
	unsigned int counter2;
	unsigned char bank;
	unsigned int samplerate;
	unsigned char tuning_fine;
	unsigned char tuning_course;
	unsigned char OriginKey;
	unsigned char LastKey;
	unsigned char TargetLoopStart;
	unsigned char TargetLoopEnd;
	unsigned char TargetLoopFine;
	_program_dump_table_ PrgDump;

	/* Do The sample select */
	switch (SampleSelect[5])
	{
	case 0x15:
		bank = 0;
		break;
	case 0x14:
		bank = 1;
		break;
	}

	if (DoSampleSelect(SampleSelect,SampleNumber) == false)
		return false;
	
	/* Get the document */
	MWAV hWAV = theApp.m_CurrentDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return false;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	/* Get the number of pages to transmit */
	TransmitSamplePages = GetNumberOfPages(pWav);

	if ( ! LoopOnly )
	{
		TxSamParams.DoModal();
		TxSamParams.DestroyWindow();
	}

	/* Get the original key */
	ResetEvent(midi_in_event);

	if ( theApp.GetProfileIntA("Settings","TxSampleParams",true) == 1 && ! LoopOnly )
	{
		GetOriginalKey.DoModal();
		GetOriginalKey.DestroyWindow();

		LastKey = theApp.m_LastNote;
	}

	if ( LoopOnly )
		goto LoopOnly;

	DataSize=(((TransmitSamplePages+1) * MIRAGE_PAGESIZE) * 2) + 8;
//	TransmitSample=(unsigned char *)malloc(DataSize);

//	memset(TransmitSample, 0, DataSize-1);

	for ( counter = 0 ; counter < 4; counter++)
	{
		TransmitSample[counter] = WaveDumpData[counter];
	}

	/* Construct Pagecount */
	TransmitSample[4] = TransmitSamplePages & 0x0F;
	TransmitSample[5] = (TransmitSamplePages & 0xF0) >> 4;

	/* Construct Pages */
	counter2 = 6;
	int temp=0xFF + (TransmitSamplePages * MIRAGE_PAGESIZE);

	for(counter = 0 ; counter <= unsigned int(0xFF + (TransmitSamplePages * MIRAGE_PAGESIZE)); counter++)
	{
		if ( counter < pWav->data_header.dataSIZE )
		{
			if (pWav->SampleData[counter] > 0 )
			{
				LsNybble = pWav->SampleData[counter] & 0x0F;
				MsNybble = (pWav->SampleData[counter] & 0xF0) >> 4;
			} else {
				LsNybble = 1;
				MsNybble = 0;
			}
		} else {
			LsNybble = 0;
			MsNybble = 8;
			pWav->SampleData[counter] = 0x80;
		}
		TransmitSample[counter2] = LsNybble;
		TransmitSample[(counter2+1)] = MsNybble;
		counter2 += 2;
	}

//	pWav->data_header.dataSIZE = (MIRAGE_PAGESIZE * TransmitSamplePages) + 255;
	pWav->samplepages = TransmitSamplePages;
	/* Get Checksum */
//	counter2++;
	TransmitSample[counter2] = GetChecksum(pWav);
	counter2++;
	TransmitSample[counter2] = 0xF7;

	/* Now Transmit the sample */
//	progress.Create(CProgressDialog::IDD, NULL);
//	progress.SetWindowTextA("Transmitting Sample");
//	progress.Bar.SetRange32(0,(counter2+1));

	ResetEvent(midi_in_event);
	SendLongData(TransmitSample, counter2+1);

//	progress.DestroyWindow();

	/* Get the OS version again to confirm sample is transmitted
	 * actually a workaround for some midi interfaces which
	 * return immediately while data is still being transmitted
	 */
	GetConfigParms(2*MIRAGE_PAGESIZE * TransmitSamplePages);
	DWORD wait_state = WaitForSingleObject(midi_in_event,2*MIRAGE_PAGESIZE * TransmitSamplePages); // Wait 10 seconds for a response from the mirage

	if ( theApp.GetProfileIntA("Settings","TxSampleParams",true) == false && ! LoopOnly )
		return true;
LoopOnly:

//	unsigned char SampleNumber = (SampleSelect[6]-1);
	
	GetSampleParameters();
	memcpy(&PrgDump,&ProgramDumpTable[bank],sizeof(ProgramDumpTable[bank]));

	unsigned char CurSampleEnd=PrgDump.WaveSampleControlBlock[SampleNumber].SampleEnd;
	unsigned char CurSampleStart=PrgDump.WaveSampleControlBlock[SampleNumber].SampleStart;
	
	TargetLoopStart = unsigned char(pWav->sampler.Loops.dwStart >> 8);
	TargetLoopEnd = unsigned char((pWav->sampler.Loops.dwEnd & 0xFF00) >> 8);
	TargetLoopFine = unsigned char(pWav->sampler.Loops.dwEnd & 0x00FF);

	/* Set sample endpoint */
	if ( (CurSampleEnd - CurSampleStart) > TransmitSamplePages )
	{
		/* It's not possible to move the sample end point before the loop end, so change the loop first */
		/* Set Loop Start Point */
		ChangeParameter("Setting Loop Startpoint", 62, CurSampleStart);

		/* Set Loop End Point */
		ChangeParameter("Setting Loop Endpoint",63, CurSampleStart+1);
	}


//		SampleStartEnd("Setting Sample Endpoint", 61, CurSampleStart+TransmitSamplePages);
		ChangeParameter("Setting Sample Endpoint", 61, CurSampleStart+TransmitSamplePages);

 	ResetEvent(midi_in_event);
	SendData(LoopOn);
	wait_state = WaitForSingleObject(midi_in_event,100);

	ResetEvent(midi_in_event);
	SendData(LoopOff);
	wait_state = WaitForSingleObject(midi_in_event,100);

	/* Next check if we have to set the looppoints */
	if ( pWav->sampler.Loops.dwPlayCount == 0 ) /* Check if Loop is enabled */
	{
		///* Set Loop Start Point */
		ChangeParameter("Setting Loop Startpoint", 62, CurSampleStart+TargetLoopStart);

		///* Set Loop End Point */
		ChangeParameter("Setting Loop Endpoint",63, CurSampleStart+TargetLoopEnd);

		///* Set Loop End Fine */
		ChangeParameter("Setting Loop End Fine point",64, CurSampleStart+TargetLoopFine);
		SendData(LoopOn);
	} // Loop on/off Detect

	/* If we received the sample from the Mirage we do not change the tuning parameters */
	if ( theApp.m_CurrentDoc->FromMirage() != true )
	{	
		samplerate = pWav->waveFormat.fmtFORMAT.nSamplesPerSec;
		tuning_course = 255;

		if ( samplerate >= 5000 && samplerate < 10000 && tuning_course == 255 )
		{
			tuning_course = 0;
			samplerate = samplerate * 4;
		} else 
		if ( samplerate >= 10000 && samplerate < 20000 && tuning_course == 255 )
		{
			tuning_course = 1;
			samplerate = samplerate * 2;
		}
		else 
		if ( samplerate >= 20000 && samplerate < 40000 && tuning_course == 255 )
		{
			tuning_course = 2;
		}
		else 
		if ( samplerate >= 40000 && samplerate < 80000 && tuning_course == 255 )
		{
			tuning_course = 3;
			samplerate = samplerate / 2;
		}
		else 
		if ( samplerate >= 80000 && samplerate < 160000 && tuning_course == 255 )
		{
			tuning_course = 4;
			samplerate = samplerate / 4;
		}
		else 
		if ( samplerate >= 160000 && samplerate < 320000 && tuning_course == 255 )
		{
			tuning_course = 5;
			samplerate = samplerate / 8;
		}

		for (tuning_fine = 0 ; tuning_fine < 256 ; tuning_fine++)
		{
			if ( (Tuning[tuning_fine] >= samplerate) )
			{
				if (Tuning[tuning_fine] == samplerate )
				{
					break;
				} else {
					if ( samplerate < ((Tuning[tuning_fine]+Tuning[tuning_fine-1]) / 2) ) 
						tuning_fine--;
					break;
				}
			}
		}
		
		if ( LastKey < 0x45 )
		{
			OriginKey = 0x45 - LastKey;

OctaveDown:
			if (OriginKey < 12 )
			{
				if ( ((short)tuning_fine + (OriginKey * KEY_TRANSPOSE )) > 255 )
					tuning_course++;
				tuning_fine = tuning_fine + (unsigned char)(OriginKey * KEY_TRANSPOSE );
			} else {
				tuning_course = tuning_course + (OriginKey / 12);
				OriginKey = OriginKey - ((OriginKey / 12) * 12);
				goto OctaveDown;
			}
		}
		if ( LastKey > 0x45 )
		{
			OriginKey = LastKey - 0x45;
OctaveUp:
			if (OriginKey < 12 )
			{	
				if ( ((short)tuning_fine - (OriginKey * KEY_TRANSPOSE )) < 0 )
					tuning_course--;
				tuning_fine = tuning_fine - (unsigned char)(OriginKey * KEY_TRANSPOSE );
			} else {
				tuning_course = tuning_course - (OriginKey / 12);
				OriginKey = OriginKey - ((OriginKey / 12) * 12);
				goto OctaveUp;
			}
		}
		unsigned char Current_Fine_Tune = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].FineTune;
		unsigned char Current_Tune_Course = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].CoarseTune;


		/* Course Tuning */
		if ( tuning_course > 0 && tuning_course <= 7 )
		{
			ChangeParameter("Setting Course Tuning",67,tuning_course);
		}

		/* Fine Tuning */
		ChangeParameter("Setting Fine Tuning",68,tuning_fine);
	}

	progress.DestroyWindow();

	return true;
}

BOOL GetConfigParms(unsigned int TimeOut)
{
	ResetEvent(midi_in_event);
	SendData(ConfigParmsDumpReq);
	DWORD wait_state = WaitForSingleObject(midi_in_event,TimeOut); // Wait 10 seconds for a response from the mirage
	if (wait_state == WAIT_TIMEOUT)
	{		
		MessageBox(NULL,"Error while transmitting to the Mirage.","ERROR",MB_ICONERROR);
		return false;
	}
	return TRUE;
}

BOOL SendConfigParms()
{
	unsigned char LsNybble;
	unsigned char MsNybble;
	unsigned char TXByteCount=0;
	unsigned char ConfigParmsTX[64];
	unsigned char *configdump;
	int c=0;

	configdump=(unsigned char*)malloc(sizeof(ConfigDump));
	memcpy(configdump,&ConfigDump,sizeof(ConfigDump));
	memcpy(ConfigParmsTX+1,ConfigParmsDump,4);

	for(c=0; c < sizeof(ConfigDump);c++)
	{
		LsNybble = configdump[c] & 0x0F;
		MsNybble = (configdump[c] & 0xF0) >> 4;
		ConfigParmsTX[5+TXByteCount]=LsNybble;
		ConfigParmsTX[6+TXByteCount]=MsNybble;
		TXByteCount += 2;
	}

	TXByteCount += 4;
	ConfigParmsTX[TXByteCount]=0xF7;
	ConfigParmsTX[0]=TXByteCount;
	
	SendLongData(ConfigParmsTX+1,ConfigParmsTX[0]);

	return TRUE;
}
