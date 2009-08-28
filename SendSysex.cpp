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

#ifdef _DEBUG
	int i;
#endif
//	MIDIOUTCAPS		moutCaps;

#ifdef _DEBUG
//	midiOutGetDevCaps(theApp.GetProfileIntA("Settings","OutPort",0)-1, &moutCaps, sizeof(moutCaps));
//	fprintf(logfile,"Midioutdevice used: %s\n", moutCaps.szPname);
	fprintf(logfile,"Sending %d bytes of sysex data:", sizeof(sysEx));
	for(i=1 ; i <= sysEx[0]; i++)
	{
		fprintf(logfile,"%02X ", sysEx[i]);
	}
	fprintf(logfile,"\n");
#endif

	/* Open default MIDI Out device */
	if (!midiOutOpen(&handle, theApp.GetProfileIntA("Settings","OutPort",0)-1, 0, 0, CALLBACK_NULL))
	{
		/* Store pointer in MIDIHDR */
		midiOutHdr.lpData = (LPSTR)&sysEx[1];

		/* Store its size in the MIDIHDR */
		midiOutHdr.dwBufferLength = sysEx[0];

		/* Flags must be set to 0 */
		midiOutHdr.dwFlags = 0;

		/* Prepare the buffer and MIDIHDR */
		err = midiOutPrepareHeader(handle,  &midiOutHdr, sizeof(MIDIHDR));
		if (!err)
		{
			/* Output the SysEx message */
			err = midiOutLongMsg(handle, &midiOutHdr, sizeof(MIDIHDR));
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

		Sleep( 4 * sysEx[0]);
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

void SendLongData(unsigned char *sysEx, UINT SysXsize)
{
	HMIDIOUT		midiOuthandle;
	unsigned long	err;
	DWORD			counter;

	/* Open default MIDI Out device */
	if (!midiOutOpen(&midiOuthandle, theApp.GetProfileIntA("Settings","OutPort",0)-1, 0, 0, CALLBACK_NULL))
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

			if (!err)
			{
				/* Output the SysEx message */
				err = midiOutLongMsg(midiOuthandle, &midiOutHdr, sizeof(MIDIHDR));
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
		
			progress.progress(counter);
		}

		/* Close the MIDI device */
		midiOutClose(midiOuthandle);
	}
}
