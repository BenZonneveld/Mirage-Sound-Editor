#include "MidiWrapper/MIDIInDevice.h"
#include "MidiWrapper/shortmsg.h"
#include "MidiWrapper/Longmsg.h"
#include "globals.h"
#include <vector>

//extern HANDLE				midi_in_expected; 

class MyReceiver : public midi::CMIDIReceiver
{
public:
	// Receives short messages
	void ReceiveMsg(DWORD Msg, DWORD Timestamp);

	// Receives long messages
	void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);

	// Called when an invalid short message is received
  void OnError(DWORD Msg, DWORD TimeStamp) {}
    // Called when an invalid long message is received
  void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);
};

extern std::vector <char> InMsg;
extern unsigned char SysXBuffer[SYSEXBUFFER];
extern MyReceiver Receiver;
extern midi::CMIDIInDevice InDevice;//(Receiver);
extern midi::CLongMsg	LongMsg;
extern midi::CShortMsg ShortMsg;

extern HANDLE midi_in_event;