/* $Id: MidiReceive.h,v 1.4 2008/01/23 22:42:32 root Exp $ */
#pragma once
#include "afxwin.h"
#include "Mirage Editor.h"
#include "MidiWrapper/MIDIInDevice.h"

extern unsigned char	sysex_mode;
extern unsigned int	pagecount;

extern	HMIDIIN			midi_in_handle;
extern	MIDIHDR			midiInHdr;
extern short closed;

int StartMidiReceiveData();
int StopMidiReceiveData(void);

void StartMidi();
void StopMidi();

using midi::CMIDIInDevice;
using midi::CMIDIReceiver;

class MyReceiver : public CMIDIReceiver
{
public:
	// Receives short messages
	void ReceiveMsg(DWORD Msg, DWORD Timestamp);

	// Receives long messages
	void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);

	// Called when an invalid short message is received
    void OnError(DWORD Msg, DWORD TimeStamp) {}
    // Called when an invalid long message is received
    void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp) {}
};
