/* $Id: SendSysex.cpp,v 1.10 2008/02/23 23:58:51 root Exp $ */
#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#ifdef _DEBUG
#include "sysexdebug.h"
#endif

#include "MidiWrapper/MIDIInDevice.h"
#include "MidiWrapper/MIDIOutDevice.h"
#include "MirageSysex.h"

#include <windows.h>
#include <mmsystem.h>

HMIDIOUT		midi_out_handle;
MIDIHDR			midiOutHdr;
unsigned char	SysXOutBuffer[SYSEXBUFFER];

void PrintMidiOutErrorMsg(unsigned long err);

//void SendData(unsigned char *sysEx)
//{
//  midi::CLongMsg OutLongMsg;
//  midi::CMIDIOutDevice OutDevice;
//
//  //HANDLE			midi_out_long_event;
//  //midi_out_long_event = CreateEvent(NULL,               // default security attributes
//		//							                  TRUE,               // manual-reset event
//		//							                  TRUE,              // initial state is nonsignaled
//		//							                  FALSE);
//
//  OutDevice.Open(theApp.GetProfileIntA("Settings","OutPort",0)-1);
//  OutLongMsg.SetMsg((const char*)&ConfigParmsDumpReq+1,ConfigParmsDumpReq[0]);
//  OutLongMsg.SendMsg(OutDevice);
//  OutDevice.Close();
//}

void SendData(unsigned char *sysEx)
{
#ifdef OLD_MIDI
	HMIDIOUT    handle;
	UINT        err;
	HANDLE			midi_out_long_event;

	midi_out_long_event = CreateEvent(
									NULL,               // default security attributes
									TRUE,               // manual-reset event
									TRUE,              // initial state is nonsignaled
									FALSE);

	/* Open default MIDI Out device */
	if (!midiOutOpen(&handle, midi::CMIDIOutDevice::GetIDFromName(theApp.GetProfileStringA("Settings","OutPort","not connected"))-1, 0, 0, CALLBACK_EVENT))
	{
		/* Store pointer in MIDIHDR */
		midiOutHdr.lpData = (LPSTR)&sysEx[1];

		/* Store its size in the MIDIHDR */
		midiOutHdr.dwBufferLength = sysEx[0];

		/* Flags must be set to 0 */
		midiOutHdr.dwFlags = 0;

		/* Prepare the buffer and MIDIHDR */
		err = midiOutPrepareHeader(handle,  &midiOutHdr, sizeof(MIDIHDR));

		ResetEvent(midi_out_long_event);

		if (!err)
		{
			/* Output the SysEx message */
			err = midiOutLongMsg(handle, &midiOutHdr, sizeof(MIDIHDR));
			while (TRUE)
			{
				WaitForSingleObject(midi_out_long_event,sysEx[0]+2);
				if ( (midiOutHdr.dwFlags & MHDR_DONE) == MHDR_DONE )
				{
					break;
				}
			}

			if (err)
			{
				char   errMsg[120];

				midiOutGetErrorText(err, &errMsg[0], 120);
			}

			/* Unprepare the buffer and MIDIHDR */
			while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(handle, &midiOutHdr, sizeof(MIDIHDR)))
			{
				/* Should put a delay in here rather than a busy-wait */		    
			}
		}

		/* Close the MIDI device */
		midiOutClose(handle);
	}
#else
	midi::CMIDIOutDevice outdevice;
	outdevice.Open(outdevice.GetIDFromName(theApp.GetProfileStringA("Settings","OutPort","not connected"))-1);
	outdevice.SendMsg((LPSTR)&sysEx[1],sysEx[0]);
	outdevice.Close();
#endif
}

void PrintMidiOutErrorMsg(unsigned long err)
{
#define BUFFERSIZE 200
	char	buffer[BUFFERSIZE];

	
	if (!(err = midiOutGetErrorText(err, &buffer[0], BUFFERSIZE)))
	{
		MessageBox(NULL,&buffer[0],"Error", MB_ICONERROR);
	}
	else if (err == MMSYSERR_BADERRNUM)
	{
		MessageBox(NULL, "Strange error number returned!\n", "Error", MB_ICONERROR);
	}
	else if (err == MMSYSERR_INVALPARAM)
	{
		MessageBox(NULL, "Specified pointer is invalid!\n", "Error", MB_ICONERROR);
	}
	else
	{
		MessageBox(NULL, "Unable to allocate/lock memory!\n", "Error", MB_ICONERROR);
	}
}

void SendLongData(byte *sysEx, UINT SysXsize)
{
	midi::CMIDIOutDevice outdevice;
	outdevice.Open(outdevice.GetIDFromName(theApp.GetProfileStringA("Settings","OutPort","not connected"))-1);
	outdevice.SendMsg((LPSTR)&sysEx,SysXsize);
	outdevice.Close();
}
