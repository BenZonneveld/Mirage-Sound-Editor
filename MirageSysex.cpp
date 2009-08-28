/* $Id: MirageSysex.cpp,v 1.18 2008/02/23 23:58:51 root Exp $ */

#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "MirageSysex.h"
#include "ChildFrm.h"
#include "Mirage EditorDoc.h"
#include "Mirage EditorView.h"
#include "MidiReceive.h"
#include "SendSysex.h"
#include "wavesamples.h"
#include "float_cast.h"
#include "Mirage Helpers.h"
#include "Tuning.h"
#include "Message.h"
#include <windows.h>
#include <mmsystem.h>

char	DataDumped = 0;
unsigned char	lower_upper_select = 0;
unsigned char	ProgramStatus = 0xFF;
unsigned char	WavesampleStatus = 0xFF;
unsigned char	WavesampleStore = 0;

// Ensoniq Mirage Sysex ID
unsigned char	MirID[] = {0xF0,
							0x0F,
							0x01};  // Mirage Identifier

unsigned char	ProgramDumpReqUpper[]={5, // Length of sysex
										MirID[0],
										MirID[1],
										MirID[2],
										0x13,
										0xF7}; // Upper Program Dump Request

// This command instructs the mirage to dump its current
// configuration parameters
unsigned char	ConfigParmsDumpReq[]={5,
									MirID[0],
									MirID[1],
									MirID[2],
									0x00,
									0xF7}; // Configuration parameters dump request

unsigned char	MirageCommandCode[]={MirID[0],
									MirID[1],
									MirID[2],
									0x01}; // Command Code

unsigned char	SelectLowerSample[]={MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x15 // Lower Sample Select
									};

unsigned char	SelectUpperSample[]={MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x14 // Upper Sample Select
									};

unsigned char	ConfigParmsDump[]={MirID[0],
									MirID[1],
									MirID[2],
									0x02}; // Configuration parameters dump 

unsigned char	ProgramDumpReqLower[]={5,
										MirID[0],
										MirID[1],
										MirID[2],
										0x03,
										0xF7}; // Lower Program Dump Request

// Used to ask the mirage to dump the current wavesample as selected
// by Wavesample select, parameter [26]
unsigned char	WaveDumpReq[] = {5,
									MirID[0],
									MirID[1],
									MirID[2],
									0x04,
									0xF7}; // Wavesample dump request

unsigned char	ProgramDumpLower[] = {MirID[0],
										MirID[1],
										MirID[2],
										0x05};

unsigned char	ProgramDumpUpper[] = {MirID[0],
										MirID[1],
										MirID[2],
										0x15};

unsigned char	WaveDumpData[] = {MirID[0],
									MirID[1],
									MirID[2],
									0x06}; // WaveSample Dump Data

unsigned char	ProgramStatusMessage[] = {MirID[0],
									MirID[1],
									MirID[2],
									0x07}; // Program Dump Data (previous selected sample)

unsigned char	WavesampleStatusMessage[] = {MirID[0],
									MirID[1],
									MirID[2],
									0x08}; // Wavesample Status Message

unsigned char	WavesampleAck[] = {5,
									MirID[0],
									MirID[1],
									MirID[2],
									0x09,
									0xF7}; // Sent by the Mirage when the checksum of the received wavesampledump is good. Also sent when a wavesample function is completed

unsigned char	WavesampleNack[] = {5,
									MirID[0],
									MirID[1],
									MirID[2],
									0x0A,
									0xF7}; // Sent by the Mirage when the checksum of a received dump is bad

unsigned char SampleEnd[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x01,
									0x7F, // End of Commando Code
									0xF7};

unsigned char ValueDown[] = {8,
								MirID[0],
								MirID[1],
								MirID[2],
								0x01, // Command Code
								0x0D, // Select Value
								0x0F, // Value Down
								0x7F, // End of Command Code
								0xF7};

unsigned char ValueUp[] = {8,
								MirID[0],
								MirID[1],
								MirID[2],
								0x01, // Command Code
								0x0D, // Select Value
								0x0E, // Value Down
								0x7F, // End of Command Code
								0xF7};

unsigned char LoopStart[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x02,
									0x7F, // End of Command Code
									0xF7};

unsigned char LoopEnd[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x03,
									0x7F, // End of Command Code
									0xF7};

unsigned char LoopEndFine[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x04,
									0x7F, // End of Command Code
									0xF7};


unsigned char LoopOn[]={11,
						MirID[0],
						MirID[1],
						MirID[2],
						0x01,
						0x0C,
						0x06,
						0x05,
						0x0D,
						0x0E, // Up Arrow
						0x7F,
						0xF7};

unsigned char LoopOff[]={11,
						MirID[0],
						MirID[1],
						MirID[2],
						0x01,
						0x0C,
						0x06,
						0x05,
						0x0D,
						0x0F, // Down Arrow
						0x7F,
						0xF7};

unsigned char TuningCourse[]={9,
							MirID[0],
							MirID[1],
							MirID[2],
							0x01, // Commando Code
							0x0C, // Parameter 
							0x06, 
							0x07,
							0x7F, // End of Command Code
							0xF7};

unsigned char TuningFine[]={9,
								MirID[0],
								MirID[1],
								MirID[2],
								0x01, // Commando Code
								0x0C, // Parameter 
								0x06, 
								0x08,
								0x7F, // End of Command Code
								0xF7};

/* For setting the original key */
unsigned char LastMidiKey;

#pragma pack (1)
struct _program_dump_table_ ProgramDumpTable[2];

#pragma pack(1)
struct _config_dump_table_ ConfigDump;

int MirageOS;

int do_timeout(unsigned char lang)
{
	unsigned int	timeout_counter = 0;
	while ( (DataDumped < 0) && timeout_counter < MIDI_TIMEOUT )
	{
		if (sysex_mode == WAVE_DATA )
			progress.progress(pagecount/ MIRAGE_PAGESIZE);
		if (lang == LONG_TIMEOUT)
		{
			Sleep(25);
		} else {
			Sleep(1);
		}
		timeout_counter++;
	}

	if ( timeout_counter < MIDI_TIMEOUT )
	{
		return 0;
	}
	return 1;
}

void DoParameterChange(const char * Name,unsigned char * Event, unsigned char MaxValue)
{
	unsigned char counter;
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA(Name);
	progress.Bar.SetRange((short)0,(short)MaxValue);
	for( counter = 0 ; counter < MaxValue ; counter++)
	{
		SendData(Event);
		progress.progress(counter);
		Sleep(100);
	}
	progress.DestroyWindow();
}

BOOL GetAvailableSamples(void)
{
	StartMidiReceiveData();
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Available Lower Samples");

	DataDumped = -1;
	SendData(ProgramDumpReqLower);
	if (do_timeout(SHORT_TIMEOUT))
	{
		progress.DestroyWindow();
		MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		/* Stop receiving midi data */
		StopMidiReceiveData();
		return false;
	}

	progress.DestroyWindow();

	DataDumped = -1;	
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Available Upper Samples");

	SendData(ProgramDumpReqUpper);
	if (do_timeout(SHORT_TIMEOUT))
	{
		progress.DestroyWindow();
		MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		/* Stop receiving midi data */
		StopMidiReceiveData();
		return false;
	}
	progress.DestroyWindow();

	/* Stop receiving midi data */
	StopMidiReceiveData();

	return true;
}

BOOL GetSampleParameters(void)
{
	StartMidiReceiveData();

	DataDumped = -1;
	SendData(ProgramDumpReqLower);
	if (do_timeout(SHORT_TIMEOUT))
	{
		MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		/* Stop receiving midi data */
		StopMidiReceiveData();
		return false;
	}

	DataDumped = -1;	

	SendData(ProgramDumpReqUpper);
	if (do_timeout(SHORT_TIMEOUT))
	{
		MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		/* Stop receiving midi data */
		StopMidiReceiveData();
		return false;
	}

	/* Stop receiving midi data */
	StopMidiReceiveData();

	return true;
}

int GetMirageOs(void)
{
	StartMidiReceiveData();
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Configuration Data");

	DataDumped = -1;
	SendData(ConfigParmsDumpReq);
	if (do_timeout(SHORT_TIMEOUT))
	{
		progress.DestroyWindow();
		MessageBox(NULL,"You must boot the Mirage with MASOS\nfor this program to work correctly.\n", "Error", MB_ICONERROR);
		StopMidiReceiveData();
		return (0);
	}
	progress.DestroyWindow();

	/* Stop receiving midi data */
	StopMidiReceiveData();
	return (1);
}

BOOL DoSampleSelect(unsigned char *SampleSelect)
{
	unsigned char ExpectedWavesample;
	unsigned char SelectedWavesample;
	unsigned char ul_Wavesample;
	unsigned char * pSample = NULL;
	char errorstring[50];

	StartMidiReceiveData();
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Selecting Sample");
	DataDumped = -1;

	ProgramStatus = 0xFF;
	WavesampleStatus = 0xFF;
	SendData(SampleSelect);
	/*
	/* Now check the response from the Mirage
	/* Get Program Status Message (That's what the MASOS documentation calls it.
	/* This seems like a bug in the manual because the Mirage actualy returns
	/* The PREVIOUSLY selected wavesample
	/* Value is put into "ProgramStatus" 
	*/
	if (do_timeout(SHORT_TIMEOUT))
	{
		progress.DestroyWindow();
		MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		StopMidiReceiveData();
		return false;
	}
//	DataDumped = -1;
	progress.DestroyWindow();

	/*
	/* Get Wavesample Status Message
	/* This is the response from the Mirage with the current selected wavesample
	/* We check if this is the correct sample we are expecting
	/* Value is put into "WavesampleStatus"
	*/
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Get Wavesample Status");
	if (do_timeout(SHORT_TIMEOUT))
	{
		progress.DestroyWindow();
		MessageBox(NULL,"Error while getting Wavesample Status.\nMIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		StopMidiReceiveData();
		return false;
	}
	progress.DestroyWindow();
	/*
	/* Determine lower or upper sample expected
	/* And the expected sample number
	*/
	ExpectedWavesample = (SampleSelect[6] - 1);
	ul_Wavesample = (WavesampleStatus & 0xF0) >> 1;
	SelectedWavesample = WavesampleStatus & 0x0F;
	WavesampleStore = SelectedWavesample + ul_Wavesample; // Where to store the received sample

	switch(SampleSelect[5])
	{
	case 0x15: // Lower Sample
		{
			if ( ul_Wavesample != 0 )
			{
				MessageBox(NULL,"Selected Lower Sample, but got Upper Sample?!","Error", MB_ICONERROR);
				StopMidiReceiveData();
				return false;
			}
			break;
		}
	case 0x14: // Upper Sample
		{
			if ( ul_Wavesample != 8 )
			{
				MessageBox(NULL,"Selected Upper Sample, but got Lower Sample?!","Error", MB_ICONERROR);
				StopMidiReceiveData();
				return false;
			}
			break;
		}
	}
	if ( ExpectedWavesample != SelectedWavesample )
	{
		sprintf_s(errorstring,sizeof(errorstring),"Selected sample %d but got %d from Mirage.",SelectedWavesample,ExpectedWavesample);
		MessageBox(NULL,(CString)errorstring, "Error", MB_ICONERROR);
		StopMidiReceiveData();
		return false;
	}
	StopMidiReceiveData();
	return true;
}

BOOL GetSample(unsigned char *SampleSelect)
{
	short pages;

	if (DoSampleSelect(SampleSelect) == false)
		return false;
	unsigned char bank = (0x15-SampleSelect[5]);
	unsigned char ul_Wavesample = (WavesampleStatus & 0xF0) >> 1;

	pages = 1 + (ProgramDumpTable[bank].WaveSampleControlBlock[(SampleSelect[6] - 1)].SampleEnd - ProgramDumpTable[bank].WaveSampleControlBlock[(SampleSelect[6] - 1)].SampleStart);


	StartMidiReceiveData();
	/* Now Request the selected sample from the Mirage */
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Sample Data");
	progress.Bar.SetRange(short(0), pages);
	DataDumped = -1;
	SendData(WaveDumpReq);
	if (do_timeout(LONG_TIMEOUT))
	{
		progress.DestroyWindow();
		MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
		StopMidiReceiveData();
		return false;
	}

	if(WaveSample.checksum != GetChecksum(&WaveSample))
	{
		progress.DestroyWindow();
		MessageBox(NULL,"Sample checksum not correct.","ERROR",MB_ICONERROR);
		SendData(WavesampleNack);
		return false;
	} else {
		SendData(WavesampleAck);
	}
	progress.DestroyWindow();
	CreateRiffWave(WavesampleStore, (ul_Wavesample >> 4) );
	CreateFromMirage(WavesampleStore,ul_Wavesample);

	StopMidiReceiveData();
	return true;
}

BOOL PutSample(unsigned char *SampleSelect, bool LoopOnly)
{
	_WaveSample_ *pWav;
	unsigned char TransmitSamplePages;
	unsigned char *TransmitSample;
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

	if (DoSampleSelect(SampleSelect) == false)
		return false;
	
	MWAV hWAV = theApp.m_CurrentDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return false;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	MessagePopup.Create(CMessage::IDD, NULL);

	LastMidiKey = 255;
	StartMidiReceiveData();
	DataDumped = -1;
	while (DataDumped < 0)
	{
		Sleep(10);
	}
	StopMidiReceiveData();
	LastKey = LastMidiKey;

	MessagePopup.DestroyWindow();

//	GetWaveSample(&sWav, theApp.m_CurrentDoc);

	TransmitSamplePages = GetNumberOfPages(pWav);

	if ( LoopOnly )
		goto LoopOnly;

	TransmitSample = (unsigned char *)malloc((TransmitSamplePages * MIRAGE_PAGESIZE * 2)+ 9);
	memset(TransmitSample, 0x80, (TransmitSamplePages * MIRAGE_PAGESIZE * 2)+ 9);

	for ( counter = 0 ; counter < 4; counter++)
	{
		TransmitSample[counter] = WaveDumpData[counter];
	}

	/* Construct Pagecount */
	TransmitSample[4] = TransmitSamplePages & 0x0F;
	TransmitSample[5] = (TransmitSamplePages & 0xF0) >> 4;

	/* Construct Pages */
	counter2 = 6;
	for(counter = 0 ; counter < unsigned int(TransmitSamplePages * MIRAGE_PAGESIZE); counter++)
	{
		if ( counter < pWav->data_header.dataSIZE )
		{
			LsNybble = pWav->SampleData[counter] & 0x0F;
			MsNybble = (pWav->SampleData[counter] & 0xF0) >> 4;
		} else {
			LsNybble = 0;
			MsNybble = 8;
			pWav->SampleData[counter] = 0x80;
		}
		TransmitSample[counter2] = LsNybble;
		TransmitSample[(counter2+1)] = MsNybble;
		counter2 += 2;
	}

	pWav->data_header.dataSIZE = (MIRAGE_PAGESIZE * TransmitSamplePages) + 255;
	pWav->samplepages = TransmitSamplePages;
	/* Get Checksum */
	TransmitSample[counter2] = GetChecksum(pWav);
	TransmitSample[counter2+1] = 0xF7;

	/* Now Transmit the sample */
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Transmitting Sample");
	progress.Bar.SetRange((short)0,(short)(counter2+1));

	SendLongData(TransmitSample, counter2);

	free(TransmitSample);

	progress.DestroyWindow();

	/* Get the OS version again to confirm sample is transmitted
	 * actually a workaround for some midi interfaces which
	 * return immediately while data is still being transmitted
	 */
	StartMidiReceiveData();
	DataDumped = -1;
	SendData(ConfigParmsDumpReq);
	if (do_timeout(LONG_TIMEOUT))
	{
		MessageBox(NULL,"Error while transmitting sample to the Mirage.","ERROR",MB_ICONERROR);
		return false;
	}
	StopMidiReceiveData();

LoopOnly:
	switch (SampleSelect[5])
	{
	case 0x15:
		bank = 0;
		break;
	case 0x14:
		bank = 1;
		break;
	}

	unsigned char SampleNumber = (SampleSelect[6]-1);
	
	unsigned char CurSampleEnd = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleEnd;
	unsigned char CurSampleStart = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleStart;
	
	unsigned char TargetLoopStart = unsigned char(pWav->sampler.Loops.dwStart >> 8);
	unsigned char TargetLoopEnd = unsigned char((pWav->sampler.Loops.dwEnd & 0xFF00) >> 8);
	unsigned char TargetLoopFine = unsigned char(pWav->sampler.Loops.dwEnd & 0x00FF);

	/* Set sample endpoint */
	if ( CurSampleEnd > (CurSampleStart + TransmitSamplePages) )
	{
		SendData(SampleEnd);
		DoParameterChange("Setting Sample Endpoint", ValueDown,(CurSampleEnd - (CurSampleStart + TransmitSamplePages)) );
		SendData(LoopOff);
		SendData(LoopOn);
		SendData(LoopOff);
	}

	GetSampleParameters();

	unsigned char CurLoopStart = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopStart;
	unsigned char CurLoopEnd = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopEnd;

	/* Next check if we have to set the looppoints */
	if ( pWav->sampler.Loops.dwPlayCount == 0 ) /* Check if Loop is enabled */
	{
		SendData(LoopOn);
		/* Set Loop Start Point */
		if ( CurLoopStart > TargetLoopStart)
		{
			SendData(LoopStart);
			DoParameterChange("Setting Loop Startpoint", ValueDown, CurLoopStart - TargetLoopStart);
		}
		if ( CurLoopStart < TargetLoopStart)
		{
			SendData(LoopStart);
			DoParameterChange("Setting Loop Startpoint", ValueUp, TargetLoopStart - CurLoopStart );
		}

		/* Set Loop End Point */
		if ( CurLoopEnd > TargetLoopEnd )
		{
			SendData(LoopEnd);
			DoParameterChange("Setting Loop Endpoint", ValueDown, CurLoopEnd - TargetLoopEnd);
		}
		if ( CurLoopEnd < TargetLoopEnd )
		{
			SendData(LoopEnd);
			DoParameterChange("Setting Loop Endpoint", ValueUp, TargetLoopEnd - CurLoopEnd);
		}

		GetSampleParameters();
		unsigned char CurLoopFine = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopEndFine;

		/* Set Loop End Fine */
		if ( CurLoopFine > TargetLoopFine )
		{
			SendData(LoopEndFine);
			DoParameterChange("Setting Loop Endpoint Fine", ValueDown, CurLoopFine - TargetLoopFine);
		}
		if ( CurLoopFine < TargetLoopFine )
		{
			SendData(LoopEndFine);
			DoParameterChange("Setting Loop Endpoint Fine", ValueUp, TargetLoopFine - CurLoopFine);
		}
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
		}
		if ( samplerate >= 10000 && samplerate < 20000 && tuning_course == 255 )
		{
			tuning_course = 1;
			samplerate = samplerate * 2;
		}
		if ( samplerate >= 20000 && samplerate < 40000 && tuning_course == 255 )
		{
			tuning_course = 2;
		}
		if ( samplerate >= 40000 && samplerate < 80000 && tuning_course == 255 )
		{
			tuning_course = 3;
			samplerate = samplerate / 2;
		}
		if ( samplerate >= 80000 && samplerate < 160000 && tuning_course == 255 )
		{
			tuning_course = 4;
			samplerate = samplerate / 4;
		}
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
				tuning_fine = tuning_fine + (OriginKey * KEY_TRANSPOSE );
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
				tuning_fine = tuning_fine - (OriginKey * KEY_TRANSPOSE );
			} else {
				tuning_course = tuning_course - (OriginKey / 12);
				OriginKey = OriginKey - ((OriginKey / 12) * 12);
				goto OctaveUp;
			}
		}
		unsigned char Current_Fine_Tune = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].FineTune;
		unsigned char Current_Tune_Course = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].CoarseTune;

		if ( Current_Tune_Course > tuning_course )
		{
			SendData(TuningCourse);
			DoParameterChange("Setting Course Tuning", ValueDown, Current_Tune_Course - tuning_course);
		}
		if ( Current_Tune_Course < tuning_course )
		{
			SendData(TuningCourse);
			DoParameterChange("Setting Course Tuning", ValueUp, tuning_course - Current_Tune_Course);
		}
		if ( Current_Fine_Tune > tuning_fine )
		{
			SendData(TuningFine);
			DoParameterChange("Setting Fine Tuning", ValueDown, Current_Fine_Tune - tuning_fine);
		}
		if ( Current_Fine_Tune < tuning_fine )
		{
			SendData(TuningFine);
			DoParameterChange("Setting Fine Tuning", ValueUp, tuning_fine - Current_Fine_Tune);
		}
	}

	progress.DestroyWindow();

	return true;
}