// This file might be obsoleted

#include "stdafx.h"
#include "Globals.h"
//#include "CMidiReceiver.h"
#include "MidiWrapper/MIDIInDevice.h"
#include "MidiWrapper/shortmsg.h"
#include "MidiWrapper/Longmsg.h"
#include "MidiWrapper/midi.h"
#ifdef NDEBUG
#include "Mirage Editor.h"
#endif

//unsigned char	SysXBuffer[SYSEXBUFFER];
MyReceiver	Receiver;
midi::CMIDIInDevice	InDevice(Receiver);
midi::CLongMsg	LongMsg;
midi::CShortMsg ShortMsg;
HANDLE				midi_in_expected; 
std::vector <char> InMsg;

// Function called to receive short messages

void MyReceiver::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
  midi::CShortMsg InShortMsg(Msg, TimeStamp);

	ShortMsg.SetMsg(InShortMsg.GetCommand(),
	                InShortMsg.GetChannel(),
					        InShortMsg.GetData1(),
					        InShortMsg.GetData2());
	SetEvent(midi_in_event);
}

void MyReceiver::ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	int vectorstart = InMsg.size();
	int mp=0;
	DWORD ThreadID;

  if ( BytesRecorded == 0 || BytesRecorded == 0xDDDDDDDD || (DWORD)Msg == 0 || (DWORD)Msg == BytesRecorded)
    return;

	InMsg.resize(InMsg.size()+BytesRecorded);
	
	// Send a message for the progressbar
//	::PostMessage(progress.GetSafeHwnd(),WM_PROGRESS,(WPARAM)BytesRecorded, (LPARAM)BytesRecorded);

	for ( int c=vectorstart; c < InMsg.size(); c++)
	{
		InMsg[c] = (unsigned char)*(Msg + mp);
		mp++;
	}

	if ( (unsigned char)*(Msg + (BytesRecorded - 1)) != midi::END_OF_EXCLUSIVE )
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
		
		LongMsg.SetMsg(ReceivedMsg,BytesRecorded);

		InMsg.clear();
		free(ReceivedMsg);
		SetEvent(midi_in_event);		
	}
}

void MyReceiver::OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	return;
}