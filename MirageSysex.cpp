/* $Id: MirageSysex.cpp,v 1.18 2008/02/23 23:58:51 root Exp $ */

#include "stdafx.h"
#include "Globals.h"

#include "Mirage Editor.h"

#ifdef _DEBUG
#include "sysexdebug.h"
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
#include "MidiWrapper/MIDIInDevice.h"
#include "MidiWrapper/shortmsg.h"
#include "MidiWrapper/Longmsg.h"

static unsigned char	SysXBuffer[SYSEXBUFFER];
MyReceiver	Receiver;
midi::CMIDIInDevice	InDevice(Receiver);
midi::CLongMsg	LongMsg;

// Function called to receive short messages
void MyReceiver::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
    midi::CShortMsg ShortMsg(Msg, TimeStamp);

    fprintf(logfile,"Command: %i\n\
					Channel: %i\n\
					DataByte1: %i\n\
					DateByte2: %i\n\
					TimeStamp: %i\n\n",
					static_cast<int>(ShortMsg.GetCommand()),
					static_cast<int>(ShortMsg.GetChannel()),
					static_cast<int>(ShortMsg.GetData1()),
					static_cast<int>(ShortMsg.GetData2()),
					static_cast<int>(ShortMsg.GetTimeStamp()));
}

void MyReceiver::ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	LongMsg.SetMsg(Msg, BytesRecorded);
	SetEvent(midi_in_event);
}

void StartMidi()
{		
	midi_in_event = CreateEvent(NULL,               // default security attributes
								TRUE,               // manual-reset event
								FALSE,              // initial state is nonsignaled
								FALSE);

	InDevice.Open(theApp.GetProfileIntA("Settings","InPort",0)-1);
	InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	// Start Recording
	InDevice.StartRecording();
}

void StopMidi()
{
	InDevice.StopRecording();

	InDevice.Close();
}

char	DataDumped = 0;
unsigned char	lower_upper_select = 0;
unsigned char	ProgramStatus = 0xFF;
unsigned char	WavesampleStatus = 0xFF;
unsigned char	WavesampleStore = 0;

HANDLE midi_in_event;

// Ensoniq Mirage Sysex ID
unsigned char	MirID[] = {0xF0,
							0x0F,
							0x01};  // Mirage Identifier

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

unsigned char	ProgramDumpReqUpper[]={5, // Length of sysex
										MirID[0],
										MirID[1],
										MirID[2],
										0x13,
										0xF7}; // Upper Program Dump Request

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
	progress.Bar.SetRange32(0,MaxValue);
	for( counter = 0 ; counter < MaxValue ; counter++)
	{
		SendData(Event);
		progress.progress(counter);
		//Sleep(50);
	}
	progress.DestroyWindow();
}

BOOL GetAvailableSamples(void)
{
	StartMidiReceiveData();
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Available Lower Samples");
	progress.Bar.SetRange32(0,1);

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

	StopMidiReceiveData();
	progress.DestroyWindow();
	StartMidiReceiveData();

	DataDumped = -1;	
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Available Upper Samples");
	progress.Bar.SetRange32(0,1);

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

	StopMidiReceiveData();
	StartMidiReceiveData();

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
	// Start Recording
	StartMidi();

//	StartMidiReceiveData();
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Configuration Data");

	SendData(ConfigParmsDumpReq);
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,250);
		if (wait_state == WAIT_TIMEOUT)
		{
			progress.DestroyWindow();
			MessageBox(NULL,"You must boot the Mirage with MASOS\nfor this program to work correctly.\n", "Error", MB_ICONERROR);
			StopMidi();
			return (0);
		} else {
			break;
		}
	}
	progress.DestroyWindow();

	sysexerror((const unsigned char *)LongMsg.GetMsg(),LongMsg.GetLength(),"normal");
	/* Stop receiving midi data */
	StopMidi();
	return (1);
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
	StartMidi();

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
	while(true)
	{
		wait_state = WaitForSingleObject(midi_in_event,250);
#ifdef _DEBUG
		//OutputDebugString("Stopped waiting for Program Status\n");
#endif
		ProgramStatus = LongMsg.GetMsg()[4] -1;
		sysexerror((const unsigned char*)LongMsg.GetMsg(),LongMsg.GetLength(),"debug");

		if (wait_state == WAIT_TIMEOUT )
		{
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			StopMidi();
			return false;
		} else {
			fprintf(logfile,"Expecting Wavesample Status\n");
//			InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
			ResetEvent(midi_in_event);
			break;
		}
	}
	
	StopMidi();
	Sleep(10);
	StartMidi();
	Sleep(10);
	SendData(SampleNumberSelect);
	/*
	/* Get Wavesample Status Message
	/* This is the response from the Mirage with the current selected wavesample
	/* We check if this is the correct sample we are expecting
	/* Value is put into "WavesampleStatus"
	*/
	while(true)
	{
		wait_state = WaitForSingleObject(midi_in_event,1000);
		sysexerror((const unsigned char*)LongMsg.GetMsg(),LongMsg.GetLength(),"debug");
#ifdef _DEBUG
		fprintf(logfile,"Stopped Waiting for Wavesample Status, status: %02X\n",WavesampleStatus);
		OutputDebugString("Stopped waiting for Wavesample Status\n");
#endif
		if (wait_state == WAIT_TIMEOUT )
		{
#ifdef _DEBUG
		fprintf(logfile,"Raising Error\n");
		OutputDebugString("Raising Error\n");
#endif
			MessageBox(NULL,"Error while getting Wavesample Status.\nMIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			StopMidi();
			return false;
		} else {
			break;
		}
	}

	/*
	/* Determine lower or upper sample expected
	/* And the expected sample number
	*/
#ifdef _DEBUG
	fprintf(logfile,"WavesampleStatus: %02X\n" , WavesampleStatus);
	sysexdump(SampleSelect,"send sampleselect");
#endif
	
	WavesampleStatus = LongMsg.GetMsg()[4];
	ExpectedWavesample = SampleNumber;
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
				StopMidi();
				return false;
			}
			break;
		}
	}
	if ( ExpectedWavesample != SelectedWavesample )
	{
		sprintf_s(errorstring,sizeof(errorstring),"Selected sample %d but got %d from Mirage.",SelectedWavesample,ExpectedWavesample);
		MessageBox(NULL,(CString)errorstring, "Error", MB_ICONERROR);
		StopMidi();
		return false;
	}
	StopMidi();
	return true;
}

BOOL GetSample(unsigned char *SampleSelect, unsigned char SampleNumber)
{
	short pages;
	int	pagecount=0;
	BOOL LoopSwitch;
	DWORD wait_state;
	byte trycount=0;

	midi_in_event = CreateEvent(NULL,               // default security attributes
								TRUE,               // manual-reset event
								FALSE,              // initial state is nonsignaled
								FALSE);

	sysexdump(SampleSelect,"Transmitting (sampleselect)");

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

	StartMidiReceiveData();
	/* Now Request the selected sample from the Mirage */
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Sample Data");
	progress.Bar.SetRange32(0, pages*MIRAGE_PAGESIZE);
retry:
	DataDumped = -1;

	SendData(WaveDumpReq);
	ResetEvent(midi_in_event);
	while(true)
	{
		wait_state = WaitForSingleObject(midi_in_event,62500);
		if (wait_state == WAIT_TIMEOUT )
		{
			MessageBox(NULL,"MIDI timeout while getting sample\n", "Error", MB_ICONERROR);
			StopMidiReceiveData();
			return false;
		} else {
//			pagecount++;
//			progress.progress(MIRAGE_PAGESIZE*pagecount);
//			if ( DataDumped != -1 )
				break;
		}
	}
	StopMidiReceiveData();

	/* Remember to switch the loop back on */
	if ( LoopSwitch == TRUE )
	{
		SendData(LoopOn);
	}

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
			goto retry;
		}
	} else {
		SendData(WavesampleAck);
	}
	progress.DestroyWindow();

	CreateRiffWave(WavesampleStore, (ul_Wavesample >> 4), LoopSwitch );
	CreateFromMirage(WavesampleStore,ul_Wavesample);

	return true;
}

BOOL PutSample(unsigned char *SampleSelect,unsigned char SampleNumber, bool LoopOnly)
{
	_WaveSample_ *pWav;
	unsigned char TransmitSamplePages;
	byte *TransmitSample;
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

	if (DoSampleSelect(SampleSelect,SampleNumber) == false)
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
#ifdef _DEBUG
	fprintf(logfile, "Preparing to transmit %i sample pages\n",TransmitSamplePages);
#endif

	if ( LoopOnly )
		goto LoopOnly;

	TransmitSample = (byte *)malloc((TransmitSamplePages * MIRAGE_PAGESIZE * 2)+ 9);
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

#ifdef _DEBUG
	fprintf(logfile, "Starting the actual Transmit\nSysEx size: %i\n",counter2);
#endif

	/* Now Transmit the sample */
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Transmitting Sample");
	progress.Bar.SetRange32(0,(counter2+1));

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