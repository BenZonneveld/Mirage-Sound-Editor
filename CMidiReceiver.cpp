#include "stdafx.h"
#include "Globals.h"
#include "CMidiReceiver.h"
#include "MidiWrapper/MIDIInDevice.h"
#include "MidiWrapper/shortmsg.h"
#include "MidiWrapper/Longmsg.h"
#include "MidiWrapper/midi.h"
#ifdef NDEBUG
#include "Mirage Editor.h"
#endif

unsigned char	SysXBuffer[SYSEXBUFFER];
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
#ifdef _NDEBUG
		unsigned char MessageID = ReceivedMsg[4];
		CString SEMessage;
		switch (MessageID)
		{
			case CONFIG_PARM_REQ:
					SEMessage.Format("Config Parameters Request");
					break;
			case COMMAND_CODE:
					SEMessage.Format("Command Code");
					switch ( ReceivedMsg[5] )
					{
						case SELECT_LOWER:
																SEMessage.Format("Select Lower Wavesample");
																break;
						case SELECT_UPPER:
																SEMessage.Format("Select Upper Wavesample");
																break;
					}
					break;
			case CONFIG_PARM_DUMP:
					SEMessage.Format("Config Parameters Dump Data");
					break;
			case LOWER_PRG_DUMP_REQ:
					SEMessage.Format("Lower Program Dump Request");
					break;
			case UPPER_PRG_DUMP_REQ:
					SEMessage.Format("Upper Program Dump Request");
					break;
			case WAVE_DUMP_REQ:
					SEMessage.Format("Wave Dump Request");
					break;
			case PRG_DUMP_LOWER:
					SEMessage.Format("Lower Program Dump Data");
					break;
			case PRG_DUMP_UPPER:
					SEMessage.Format("Upper Program Dump Data");
					break;
			case WAVE_DUMP_DATA:
					SEMessage.Format("Wave Dump Data");
					break;
			case PRG_STATUS_MSG:
					SEMessage.Format("Program Status Message");
					break;
			case WAVE_STATUS_MSG:
					SEMessage.Format("Wavesample Status Message");
					break;
			case WAVE_ACK:
					SEMessage.Format("Wavesample acknowledge");
					break;
			case WAVE_NACK:
					SEMessage.Format("Wavesample NOT acknowleged");
			default:
					SEMessage.Format("Unknown Mirage Sysex");
		}
		fprintf(logfile,"LongMsg %d bytes of %s data\n", InMsg.size(),SEMessage);
#endif
		InMsg.clear();
		free(ReceivedMsg);
		SetEvent(midi_in_event);		
	}
}

void MyReceiver::OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	return;
}