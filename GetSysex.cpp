#include "stdafx.h"
#include "Globals.h"
#include "Mirage Editor.h"
#include "CMidiReceiver.h"
#include "SysexParser.h"
#include "Mirage Sysex_Strings.h"

BOOL StartMidi()
{		
	if(midi::CMIDIInDevice::GetNumDevs() > 0)
	{
		if (theApp.m_InDevice.GetIDFromName(theApp.GetProfileStringA("Settings","InPort","not connected")) > 0 )
		{
			theApp.m_InDevice.SetReceiver(Receiver);
			theApp.m_InDevice.Open(theApp.m_InDevice.GetIDFromName(theApp.GetProfileStringA("Settings","InPort","not connected"))-1);
			theApp.m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
			// Start receiving MIDI events
			theApp.m_InDevice.StartRecording();
		
			midi_in_event = CreateEvent(NULL,               // default security attributes
																	TRUE,               // manual-reset event
																	FALSE,              // initial state is nonsignaled
																	FALSE);
			return TRUE;
		}
		return FALSE;
	} else {
		return FALSE;
	}
}

void StopMidi()
{
	theApp.m_InDevice.StopRecording();
	theApp.m_InDevice.Close();
	CloseHandle(midi_in_event);
}

char unsigned ExpectNote()
{
	if (!StartMidi())
		return false;
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,INFINITE);
		break;
	}

	/* Wait for key release */
	ResetEvent(midi_in_event);
	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,INFINITE);
		break;
	}
	StopMidi();
	return ShortMsg.GetData1();
}

DWORD WINAPI DoMidiReceive(LPVOID Parameter)
{
	unsigned char *received;
	unsigned char *sysex;

	sysex=reinterpret_cast<unsigned char *>(Parameter);

DoMidiBegin:
//	if(!StartMidi())
//		return NO_MIDI;

	while(true)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,INFINITE);
		if (wait_state == WAIT_TIMEOUT)
		{
			progress.DestroyWindow();
			MessageBox(NULL,"No Response in Expect Sysex\n", "Error", MB_ICONERROR);
			/* Stop receiving midi data */
			StopMidi();
			return MIDI_DATA_TIMEOUT;
		} else {
			break;
		}
	}

//	StopMidi();
//	received=(unsigned char*)malloc(LongMsg.GetLength());
	received=(unsigned char*)LongMsg.GetMsg();
	if ( received[3] == sysex[3] )
	{
		ParseSysEx((unsigned char *)LongMsg.GetMsg(),LongMsg.GetLength());
		memset((void *)LongMsg.GetMsg(),0,LongMsg.GetLength());
		SetEvent(midi_in_expected);
//		free(received);
		return MIDI_EXPECTED_SUCCESS;
	} else {
		goto DoMidiBegin;
	}
}

BOOL ExpectSysex(char unsigned * sysex)
{
	HANDLE hThread;
	DWORD  dwThreadId;
	DWORD	 dwExitCode;

	CloseHandle(midi_in_expected);
	midi_in_expected = CreateEvent(NULL,               // default security attributes
															TRUE,               // manual-reset event
															FALSE,              // initial state is nonsignaled
															FALSE);
	hThread = CreateThread(NULL,					// default security attributes
												0,							// use default stack size  
												DoMidiReceive,  // thread function name
												sysex,					// argument to thread function
												0,							// use default creation flags 
												&dwThreadId);		// returns the thread identifier 

//	Sleep(10);
	if (GetExitCodeThread(hThread, &dwExitCode) == TRUE )
	{
		return TRUE;
	}

	if (hThread == NULL) 
  {
		return FALSE;
  }
	return TRUE;
}
