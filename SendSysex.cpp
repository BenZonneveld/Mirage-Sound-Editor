/* $Id: SendSysex.cpp,v 1.10 2008/02/23 23:58:51 root Exp $ */
#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include <windows.h>
#include <mmsystem.h>

HMIDIOUT		midi_out_handle;
MIDIHDR			midiOutHdr;
unsigned char	SysXOutBuffer[SYSEXBUFFER];

void PrintMidiOutErrorMsg(unsigned long err);

void SendData(unsigned char *sysEx)
{
	HMIDIOUT    handle;
	UINT        err;
	HANDLE			midi_out_long_event;

#ifdef _DEBUG
	fprintf(logfile,"Sending %d bytes of sysex data:", sizeof(sysEx));
	for(int i=1 ; i <= sysEx[0]; i++)
	{
		fprintf(logfile,"%02X ", sysEx[i]);
	}
	fprintf(logfile,"\n");
#endif
	midi_out_long_event = CreateEvent(
									NULL,               // default security attributes
									TRUE,               // manual-reset event
									TRUE,              // initial state is nonsignaled
									FALSE);

	/* Open default MIDI Out device */
	if (!midiOutOpen(&handle, theApp.GetProfileIntA("Settings","OutPort",0)-1, 0, 0, CALLBACK_EVENT))
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
			if (err)
			{
				char   errMsg[120];

				midiOutGetErrorText(err, &errMsg[0], 120);
			}

			while (TRUE)
			{
				WaitForSingleObject(midi_out_long_event,SYSEXBUFFER/2);
				if ( (midiOutHdr.dwFlags & MHDR_DONE) == MHDR_DONE )
				{
					break;
				}
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
	HMIDIOUT		midiOuthandle;
	unsigned long	err;
	DWORD			counter;
	HANDLE			midi_out_long_event;

	midi_out_long_event = CreateEvent(
									NULL,               // default security attributes
									TRUE,               // manual-reset event
									TRUE,              // initial state is nonsignaled
									FALSE);

	/* Open default MIDI Out device */
	if (!midiOutOpen(&midiOuthandle, theApp.GetProfileIntA("Settings","OutPort",0)-1, 0, 0, CALLBACK_EVENT))
	{
		for(counter = 0 ; counter < SysXsize ; counter += SYSEXBUFFER )
		{
			/* Store pointer in MIDIHDR */
			midiOutHdr.lpData = (LPSTR)&sysEx[counter];
			/* Store its size in the MIDIHDR */
			if (( SysXsize - counter ) >= SYSEXBUFFER )
			{
				midiOutHdr.dwBufferLength = SYSEXBUFFER;
			} else {
				midiOutHdr.dwBufferLength = SysXsize - counter;
			}
			/* Flags must be set to 0 */
			midiOutHdr.dwFlags = 0;
			/* Prepare the buffer and MIDIHDR */
			err = midiOutPrepareHeader(midiOuthandle,  &midiOutHdr, sizeof(MIDIHDR));

			ResetEvent(midi_out_long_event);

			if (!err)
			{
				/* Output the SysEx message */
				err = midiOutLongMsg(midiOuthandle, &midiOutHdr, sizeof(MIDIHDR));
				while (TRUE)
				{
					WaitForSingleObject(midi_out_long_event,SYSEXBUFFER/2);
					if ( (midiOutHdr.dwFlags & MHDR_DONE) == MHDR_DONE )
					{
						progress.progress(counter);
						break;
					}
				}
			}
			if (err)
			{
				char   errMsg[120];
				midiOutGetErrorText(err, &errMsg[0], 120);
			}

			/* Unprepare the buffer and MIDIHDR */
			while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(midiOuthandle, &midiOutHdr, sizeof(MIDIHDR)))
			{
				/* Short delay while waiting */
				Sleep(1);
			}
		}

		/* Close the MIDI device */
		midiOutClose(midiOuthandle);
	}
}
