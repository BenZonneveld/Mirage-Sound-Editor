/* $Id: MirageSysex.cpp,v 1.18 2008/02/23 23:58:51 root Exp $ */

#include "stdafx.h"
#include "Globals.h"

#include "Mirage Editor.h"

#ifdef _DEBUG
#include "sysexdebug.h"
#endif

#include "Nybble.h"
#include "ChildFrm.h"
#include "Mirage EditorDoc.h"
#include "Mirage EditorView.h"
#include "SendSysex.h"
#include "wavesamples.h"
#include "float_cast.h"
#include "Mirage Helpers.h"
#include "Tuning.h"
#include "Message.h"
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include "MidiWrapper/MIDIInDevice.h"
#include "MidiWrapper/shortmsg.h"
#include "MidiWrapper/Longmsg.h"
#include "MirageSysex.h"

static unsigned char	SysXBuffer[SYSEXBUFFER];
BOOL ProgressActive = FALSE;
MyReceiver	Receiver;
midi::CMIDIInDevice	InDevice(Receiver);
midi::CLongMsg	LongMsg;
midi::CShortMsg ShortMsg;

std::vector <char> InMsg;

// Function called to receive short messages
void MyReceiver::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
    midi::CShortMsg InShortMsg(Msg, TimeStamp);

	ShortMsg.SetMsg(InShortMsg.GetCommand(),
					InShortMsg.GetChannel(),
					InShortMsg.GetData1(),
					InShortMsg.GetData2());
/*    fprintf(logfile,"Command: %i\n\
					Channel: %i\n\
					DataByte1: %i\n\
					DateByte2: %i\n\
					TimeStamp: %i\n\n",
					static_cast<int>(ShortMsg.GetCommand()),
					static_cast<int>(ShortMsg.GetChannel()),
					static_cast<int>(ShortMsg.GetData1()),
					static_cast<int>(ShortMsg.GetData2()),
					static_cast<int>(ShortMsg.GetTimeStamp()));
*/
	SetEvent(midi_in_event);
}

void MyReceiver::ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	int vectorstart = InMsg.size();
	int mp=0;

	InMsg.resize(InMsg.size()+BytesRecorded);
	for ( int c=vectorstart; c < InMsg.size(); c++)
	{
		InMsg[c] = (unsigned char)*(Msg + mp);
		mp++;
	}

	if ( (unsigned char)*(Msg + (BytesRecorded - 1)) != 0xf7 )
	{
		InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
		return;
	} else {
		char *ReceivedMsg;
		ReceivedMsg=(char *) malloc(InMsg.size());
		for(int c=0; c < InMsg.size(); c++)
		{
			ReceivedMsg[c] = InMsg[c];
		}
		LongMsg.SetMsg(ReceivedMsg, InMsg.size());
		InMsg.clear();
		free(ReceivedMsg);
		SetEvent(midi_in_event);		
	}
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

unsigned char ParmChange[]={9,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									0x0C, // Parameter 
									0x06, 
									0x01,
									0x7F, // End of Commando Code
									0xF7};

unsigned char GetCurrentValue[] = {7,
								MirID[0],
								MirID[1],
								MirID[2],
								0x01, // Command Code
								0x0D, // Select Value
								0x7F, // End of Command Code
								0xF7};

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

unsigned char ParmCurValue[9]={
								MirID[0],
								MirID[1],
								MirID[2],
								0x0E, // Wavesample Parameter Message
								0x00, // ms nybble is bank number, ls nybble is sample number
								0x00, // Parameter Number
								0x00, // Value LS nybble
								0x00, // Value MS nybble
								0xF7};

unsigned char ReceivedParmNumber;
unsigned char ReceivedParmValue[100];


/* For setting the original key */
unsigned char LastMidiKey;

#pragma pack (1)
struct _program_dump_table_ ProgramDumpTable[2];

#pragma pack(1)
struct _config_dump_table_ ConfigDump;

int MirageOS;

void ParseSysEx(unsigned char* LongMessage)
{
	unsigned char	sysex_byte;
	unsigned char	* sysex_ptr = NULL;
	unsigned char * ptr = NULL;
	int byte_counter = 0;
	char MessageID;
	int sysexlength;

	MessageID=*(LongMessage+3);
	switch(MessageID)
	{
		case CONFIG_PARM_DUMP:
			sysex_ptr = (unsigned char*)&ConfigDump;
			break;
		case PRG_DUMP_LOWER:
		case PRG_DUMP_UPPER:
			lower_upper_select = ((*(LongMessage+3) & 0xF0 ) >>4);
			sysex_ptr = (unsigned char *)&ProgramDumpTable[lower_upper_select];
			break;
		case WAVE_DUMP_DATA:
			sysex_ptr = ((unsigned char *)&WaveSample.SampleData);
			memset(sysex_ptr,0, sizeof(WaveSample.SampleData));
			ptr = LongMessage; 
			sysexlength = LongMsg.GetLength();
			if ( *(LongMessage) == 0xF0 )
			{
				LongMessage += 4; /* First 4 bytes are the sysex header */
				/* Next two bytes are the pagecount */
				WaveSample.samplepages = de_nybblify(*(LongMessage),*(LongMessage+1));
				byte_counter += 6;
				LongMessage += 2;
			}
			while ( byte_counter < sysexlength )
			{
				/* Reconstruct the byte from the nybbles and copy it to the correct structure*/
				sysex_byte = de_nybblify(*(LongMessage),*(LongMessage+1));
				memcpy(sysex_ptr++, &sysex_byte,1);
				LongMessage += 2;
				byte_counter += 2;
			}
			LongMessage = ptr;
			/* Set flag to indicate we are finished with processing the data */			
			WaveSample.checksum = (unsigned char)*(LongMessage + (sysexlength - 2 ));
			return;
			break;
		case SMP_PARM_MSG:
			sysex_ptr = ((unsigned char *)&ParmCurValue);
			memset(sysex_ptr,0,sizeof(ParmCurValue));
			ptr = LongMessage; 
			sysexlength = LongMsg.GetLength();
			if ( *(LongMessage) == 0xF0 )
			{
				/* the 5th byte is the parameter number */
				ReceivedParmNumber = *(LongMessage+5);
				ReceivedParmValue[ReceivedParmNumber] = de_nybblify(*(LongMessage+6),*(LongMessage+7));
			}
			return;
			break;
		case PRG_STATUS_MSG:
			return;
			break;
		case WAVE_STATUS_MSG:
			return;
			break;
		case WAVE_ACK:
			return;
			break;
		case WAVE_NACK:
			return;
			break;
		default:
			;;
	}
	if ( sysex_ptr == NULL )
		return;
	if ( *(LongMessage) == 0xF0 )
	{
		LongMessage = LongMessage + 4; /* First 4 bytes are the sysex header */
		byte_counter += 4;
	}
	sysexlength = LongMsg.GetLength();
	while ( byte_counter <= sysexlength )
	{
		/* Reconstruct the byte from the nybbles and copy it to the correct structure*/
		sysex_byte = de_nybblify(*(LongMessage),*(LongMessage+1));
		LongMessage += 2;
		memcpy(sysex_ptr, &sysex_byte,1);
		sysex_ptr++;
		byte_counter += 2;
	}
}

void ChangeParameter(const char * Name, unsigned char Parameter, unsigned char Value)
{
	unsigned char ParmDecimal;
	unsigned char ParmDigit;
	int no_parms;
	bool progress_val_set = false;
	int progress_value;

	if (Parameter == 60 || Parameter == 61 )
	{
		no_parms = 7;
	} else {
		no_parms = 1;
	}
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA(Name);

	ParmDecimal = Parameter/10;
	ParmDigit = Parameter-(10*ParmDecimal);

	ParmChange[6] = ParmDecimal;
	ParmChange[7] = ParmDigit;
	SendData(ParmChange);
ParmChangeLoop:
	StartMidi();
	SendData(GetCurrentValue);
	for(int c=0; c<no_parms ; c++)
	{
		while(true)
		{
			DWORD wait_state = WaitForSingleObject(midi_in_event,12);
			if (wait_state == WAIT_TIMEOUT)
			{
				break;
			} else {
				ParseSysEx((unsigned char *)LongMsg.GetMsg());
				break;
			}
		}
		if ( (c+1) < no_parms)
		{
			InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
			ResetEvent(midi_in_event);
		}
	}
	StopMidi();
	// Update the progressbar
	if (progress_val_set == false )
	{
		progress.Bar.SetRange32(0,Value);
		progress_val_set = true;
	} else {
		if ( ReceivedParmValue[Parameter] > Value )
		{
			progress_value = ReceivedParmValue[Parameter] - Value;
		} else {
			progress_value = Value - ReceivedParmValue[Parameter];
		}
		progress.progress(progress_value);
	}

	if ( ReceivedParmValue[Parameter] > Value )
	{
		SendData(ValueDown);
		goto ParmChangeLoop;
	}
	if ( ReceivedParmValue[Parameter] < Value )
	{
		SendData(ValueUp);
		goto ParmChangeLoop;
	}
	progress.DestroyWindow();
}

BOOL GetAvailableSamples(void)
{
	//StartMidiReceiveData();
	StartMidi();
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Available Lower Samples");
	progress.Bar.SetRange32(0,1);

	SendData(ProgramDumpReqLower);
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,PROGDUMP_TIMEOUT);
		if (wait_state == WAIT_TIMEOUT)
		{
			progress.DestroyWindow();
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			/* Stop receiving midi data */
			//StopMidiReceiveData();
			StopMidi();
			return false;
		} else {
			break;
		}
	}
	StopMidi();
	ParseSysEx((unsigned char *)LongMsg.GetMsg());

//	StopMidiReceiveData();
	progress.DestroyWindow();
	StartMidi();

	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Getting Available Upper Samples");
	progress.Bar.SetRange32(0,1);

	SendData(ProgramDumpReqUpper);
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,PROGDUMP_TIMEOUT);
		if (wait_state == WAIT_TIMEOUT)
		{
			progress.DestroyWindow();
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			/* Stop receiving midi data */
			StopMidi();
			return false;
		} else {
			break;
		}
	}
	progress.DestroyWindow();
	/* Stop receiving midi data */
	StopMidi();
	ParseSysEx((unsigned char *)LongMsg.GetMsg());

	return true;
}

BOOL GetSampleParameters(void)
{
	StartMidi();

	DataDumped = -1;
	SendData(ProgramDumpReqLower);
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,PROGDUMP_TIMEOUT);
		if (wait_state == WAIT_TIMEOUT)
		{
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			/* Stop receiving midi data */
			StopMidi();
			return false;
		} else {
			break;
		}
	}
	
	ParseSysEx((unsigned char *)LongMsg.GetMsg());

	StopMidi();
	StartMidi();

	SendData(ProgramDumpReqUpper);
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,PROGDUMP_TIMEOUT);
		if (wait_state == WAIT_TIMEOUT)
		{
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			/* Stop receiving midi data */
			StopMidi();
			return false;
		} else {
			break;
		}
	}

	ParseSysEx((unsigned char *)LongMsg.GetMsg());

	/* Stop receiving midi data */
	StopMidi();

	return true;
}

int GetMirageOs(void)
{
	// Start Recording
	StartMidi();

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
		ProgramStatus = LongMsg.GetMsg()[4] -1;
		sysexerror((const unsigned char*)LongMsg.GetMsg(),LongMsg.GetLength(),"debug");

		if (wait_state == WAIT_TIMEOUT )
		{
			MessageBox(NULL,"MIDI In timeout, check connection and cables!\n", "Error", MB_ICONERROR);
			StopMidi();
			return false;
		} else {
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
		if (wait_state == WAIT_TIMEOUT )
		{
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
				StopMidi();
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

	StartMidi();
	/* Now Request the selected sample from the Mirage */
	progress.Create(CProgressDialog::IDD, NULL);
	ProgressActive = TRUE;
	progress.SetWindowTextA("Getting Sample Data");
	progress.Bar.SetRange32(0, pages*MIRAGE_PAGESIZE);
retry:

	SendData(WaveDumpReq);
	ResetEvent(midi_in_event);
	while(true)
	{
		wait_state = WaitForSingleObject(midi_in_event,62500);
		if (wait_state == WAIT_TIMEOUT )
		{
			MessageBox(NULL,"MIDI timeout while getting sample\n", "Error", MB_ICONERROR);
			StopMidi();
			return false;
		} else {
				break;
		}
	}
	ParseSysEx((unsigned char *)LongMsg.GetMsg());
	StopMidi();

	/* Remember to switch the loop back on */
	if ( LoopSwitch == TRUE )
	{
		SendData(LoopOn);
	}

	if(WaveSample.checksum != GetChecksum(&WaveSample))
	{
		progress.DestroyWindow();
		ProgressActive = FALSE;
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
	unsigned char TargetLoopStart;
	unsigned char TargetLoopEnd;
	unsigned char TargetLoopFine;
	_program_dump_table_ PrgDump;

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
	StartMidi();
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,INFINITE);
		break;
	}
	StopMidi();
	LastKey = ShortMsg.GetData1();

	MessagePopup.DestroyWindow();

	TransmitSamplePages = GetNumberOfPages(pWav);

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
	StartMidi();
	SendData(ConfigParmsDumpReq);
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,PROGDUMP_TIMEOUT);
		if (wait_state == WAIT_TIMEOUT)
		{		
			MessageBox(NULL,"Error while transmitting sample to the Mirage.","ERROR",MB_ICONERROR);
			return false;
		} else {
			break;
		}
	}
	ParseSysEx((unsigned char *)LongMsg.GetMsg());
	StopMidi();

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
	ChangeParameter("Setting Sample Endpoint", 61, CurSampleStart+TransmitSamplePages);

	/* Next check if we have to set the looppoints */
	if ( pWav->sampler.Loops.dwPlayCount == 0 ) /* Check if Loop is enabled */
	{
		/* Set Loop Start Point */
		ChangeParameter("Setting Loop Startpoint", 62, CurSampleStart+TargetLoopStart);

		/* Set Loop End Point */
		ChangeParameter("Setting Loop Endpoint",63, CurSampleStart+TargetLoopEnd);

		/* Set Loop End Fine */
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
//		unsigned char Current_Fine_Tune = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].FineTune;
//		unsigned char Current_Tune_Course = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].CoarseTune;

		/* Course Tuning */
		ChangeParameter("Setting Course Tuning",67,tuning_course);

		/* Fine Tuning */
		ChangeParameter("Setting Fine Tuning",68,tuning_fine);
	}

	progress.DestroyWindow();

	return true;
}