/* $Id: MidiReceive.cpp,v 1.12 2008/02/23 23:58:51 root Exp $ */
#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "MirageSysex.h"
#include "Nybble.h"
#include "wavesamples.h"
//#include "MidiReceive.h"
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <string.h>

#ifdef _DEBUG
#include "sysexdebug.h"
#endif

/*	A buffer to hold incoming System Exclusive bytes. I arbitrarily make this 256 bytes. Note:
	For Win 3.1 16-bit apps, this buffer should be allocated using GlobalAlloc with the
	GMEM_MOVEABLE flag to get a handle of the memory object. Then pass this handle to the
	GlobalLock function to get a pointer to the memory object. To free a data block, use
	GlobalUnlock and GlobalFree. But Win32 doesn't appear to have this limitation.
*/
//#define			SYSEXBUFFER	132*1024
static unsigned char	SysXBuffer[SYSEXBUFFER];
unsigned char	sysex_mode;
unsigned char	* sysex_ptr;
unsigned char	* pLastKey;
unsigned int	pagecount; 

static CRITICAL_SECTION s_critical_section;

void PrintMidiInErrorMsg(unsigned long err);
/* A flag to indicate whether I'm currently receiving a SysX message */
unsigned char SysXFlag = 0;
unsigned int gSysexLength = 0;

HMIDIIN			midi_in_handle;
MIDIHDR			midiInHdr;

/*************************** midiCallback() *****************************
 * Here's my callback that Windows calls whenever 1 of 4 possible things
 * happen:
 *
 * 1).	I open a MIDI In Device via midiInOpen(). In this case, the
 *		uMsg arg to my callback will be MIM_OPEN. The handle arg will
 *		be the same as what is returned from midiInOpen(). The
 *		dwInstance arg is whatever I passed to midiInOpen() as its
 *		dwInstance arg.
 *
 * 2).	I close a MIDI In Device via midiInClose(). In this case, the
 *		uMsg arg to my callback will be MIM_CLOSE. The handle arg will
 *		be the same as what was passed to midiInClose(). The
 *		dwInstance arg is whatever I passed to midiInClose() as its
 *		dwInstance arg when I initially opened this handle.
 *
 * 3).	One, regular (ie, everything except System Exclusive messages) MIDI
 *		message has been completely input. In this case, the uMsg arg to my
 *		callback will be MIM_DATA. The handle arg will be the same as what
 *		is passed to midiInOpen(). The dwInstance arg is whatever I passed
 *		to midiInOpen() as its dwInstance arg when I initially opened this
 *		handle. The dwParam1 arg is the bytes of the MIDI Message packed
 *		into an unsigned long in the same format that is used by
 *		midiOutShort(). The dwParam2 arg is a time stamp that the device
 *		driver created when it recorded the MIDI message.
 *
 * 4).	midiInOpen has either completely filled a MIDIHDR's memory buffer
 *		with part of a System Exclusive message (in which case we had better
 *		continue queuing the MIDIHDR again in order to grab the remainder
 *		of the System Exclusive), or the MIDIHDR's memory buffer contains the
 *		remainder of a System Exclusive message (or the whole message if it
 *		happened to fit into the memory buffer intact). In this case, the
 *		uMsg arg to my callback will be MIM_LONGDATA. The handle arg will be
 *		the same as what is passed to midiInOpen(). The dwInstance arg is
 *		whatever I passed to midiInOpen() as its dwInstance arg when I
 *		initially opened this handle. The dwParam1 arg is a pointer to the
 *		MIDIHDR whose memory buffer contains the System Exclusive data. The
 *		dwParam2 arg is a time stamp that the device driver created when it
 *		recorded the MIDI message.
 *
 * 5).	This callback is not processing data fast enough such that the MIDI
 *		driver (and possibly the MIDI In port itself) has had to throw away
 *		some incoming, regular MIDI messages. In this case, the uMsg arg to my
 *		callback will be MIM_MOREDATA. The handle arg will be the same as what
 *		is passed to midiInOpen(). The dwInstance arg is whatever I passed
 *		to midiInOpen() as its dwInstance arg when I initially opened this
 *		handle. The dwParam1 arg is the bytes of the MIDI Message that was
 *		not handled (by an MIM_DATA call) packed into an unsigned long in the
 *		same format that is used by midiOutShort(). The dwParam2 arg is a time
 *		stamp that the device driver created when it recorded the MIDI message.
 *		In handling a series of these events, you should store the MIDI data
 *		in a global buffer, until such time as you receive another MIM_DATA
 *		(which indicates that you can now do the more time-consuming processing
 *		that you obviously were doing in handling MIM_DATA).
 *		NOTE: Windows sends an MIM_MOREDATA event only if you specify the
 *		MIDI_IO_STATUS flag to midiInOpen().
 *
 * 6).	An invalid, regular MIDI message was received. In this case, the uMsg
 *		arg to my callback will be MIM_ERROR. The handle arg will be the same
 *		as what is passed to midiInOpen(). The dwInstance arg is whatever I
 *		passed to midiInOpen() as its dwInstance arg when I initially opened
 *		this handle. The dwParam1 arg is the bytes of the MIDI Message that was
 *		not handled (by an MIM_DATA call) packed into an unsigned long in the
 *		same format that is used by midiOutShort(). The dwParam2 arg is a time
 *		stamp that the device driver created when it recorded the MIDI message.
 *
 * 7).	An invalid, System Exclusive message was received. In this case, the uMsg
 *		arg to my callback will be MIM_LONGERROR. The handle arg will be the same
 *		as what is passed to midiInOpen(). The dwInstance arg is whatever I
 *		passed to midiInOpen() as its dwInstance arg when I initially opened
 *		this handle. The dwParam1 arg is a pointer to the MIDIHDR whose memory
 *		buffer contains the System Exclusive data. The dwParam2 arg is a time
 *		stamp that the device driver created when it recorded the MIDI message.
 *
 * The time stamp is expressed in terms of milliseconds since your app
 * called midiInStart().
 *************************************************************************/

void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD timestamp)
{
	LPMIDIHDR		lpMIDIHeader;
	unsigned char *	ptr;
	unsigned char * ptr_save;
//	unsigned char 	bytes;
	/* For regular midi messages */
	unsigned char	status_byte = 0;
	unsigned char	midi_channel = 0;
	unsigned char	data_1 = 0;
	unsigned char	data_2 = 0;
	char			*status_string = NULL;
	/* For sysex handling */
	unsigned char	sysex_byte;
	int nLower, nUpper;
	unsigned short byte_counter = 0;
	/* For receiving samples */

	if ( midiInHdr.dwUser != 0 )
	{
		status_string=(char *)malloc(25);

		/*if ( *(midiInHdr.lpData) == 0xF0 && uMsg != MIM_LONGDATA)
		{
			fprintf(logfile,"Forcing uMsg to MIM_LONGDATA\n");
			uMsg = MIM_LONGDATA;
		}*/
		/* Determine why Windows called me */
		switch (uMsg)
		{
			/* Received some regular MIDI message */
			case MIM_DATA:
			{
				/* Display the time stamp, and the bytes. (Note: I always display 3 bytes even for
				Midi messages that have less) */
				status_byte = LOBYTE(LOWORD(dwParam1)) & 0xF0;
				switch(status_byte)
				{
					case 0x80:
					{
						status_string = "Note Off";
						break;
					}
					case 0x90:
					{
						status_string = "Note On";
						break;
					}
					case 0xA0:
					{
						status_string = "Poly Pressure";
						break;
					}
					case 0xB0:
					{
						status_string = "Control Change";
						break;
					}
					case 0xC0:
					{
						status_string = "Program Change";
						break;
					}
					case 0xD0:
					{
						status_string = "Aftertouch";
						break;
					}
					case 0XE0:
					{				
						status_string = "Pitch Bend";
						break;
					}
					default:
					{
						status_string = "";
						break;
					}
				}

				midi_channel = (LOBYTE(LOWORD(dwParam1)) & 0x0F) + 1;
				data_1 = HIBYTE(LOWORD(dwParam1));
				data_2 = LOBYTE(HIWORD(dwParam1));
				if ( status_byte == 0x80 && LastMidiKey == 255 )
				{
					LastMidiKey = data_1;
					DataDumped = 1;
				}
				midi::CShortMsg InShortMsg(dwParam1, timestamp);
				ShortMsg.SetMsg(InShortMsg.GetCommand(),
	                InShortMsg.GetChannel(),
					        InShortMsg.GetData1(),
					        InShortMsg.GetData2());
				SetEvent(midi_in_event);

				if ( status_byte == 0x80 || status_byte == 0x90 )
				{

#ifdef _DEBUG
					fprintf(logfile, "MIM_DATA: 0x%08X Status Byte:0x%02X Channel: %d Note:0x%02X Velocity: %d %s\n", timestamp, status_byte, midi_channel, data_1, data_2, status_string);
					printf("MIM_DATA: 0x%08X Status Byte:0x%02X Channel: %d Note:0x%02X Velocity: %d\n", timestamp, status_byte, midi_channel, data_1, data_2);
#endif
				} else {
#ifdef _DEBUG
					fprintf(logfile, "MIM_DATA: 0x%08X Status Byte:0x%02X Channel: %d 0x%02X 0x%02X\n", timestamp, status_byte, midi_channel, data_1, data_2);
#endif
				}
				break;
			}
			
			/* Received all or part of some System Exclusive message */
			case MIM_LONGDATA:
			{
				/*	Assign address of MIDIHDR to a LPMIDIHDR variable. Makes it easier to access the
					field that contains the pointer to our block of MIDI events */
				lpMIDIHeader = (LPMIDIHDR)dwParam1;
				ptr = (unsigned char *)(lpMIDIHeader->lpData+lpMIDIHeader->dwOffset);

				/* Get address of the MIDI event that caused this call */
				if ( *(ptr) == 0xF0 )
				{
					sysex_ptr = NULL;
				}
				
				/* Is this the first block of System Exclusive bytes? */
				if (!SysXFlag)
				{
					/* Indicate we've begun handling a particular System Exclusive message */
					SysXFlag |= 0x01;
				}

				/* Is this the last block (ie, the end of System Exclusive byte is here in the buffer)? */
				if (*(ptr + (lpMIDIHeader->dwBytesRecorded - 1)) == 0xF7)
				{
					/* Indicate we're done handling this particular System Exclusive message */
					SysXFlag &= (~0x01);
				}

	/*			if ( midiInHdr.dwUser == 1 )
				{
					unsigned long err = midiInAddBuffer(midi_in_handle,&midiInHdr, sizeof(MIDIHDR));
					if (err) PrintMidiInErrorMsg(err);
				}*/
				memcpy((void *)MirageReceivedSysex,ptr,lpMIDIHeader->dwBytesRecorded);
				LongMsg.SetMsg(MirageReceivedSysex,lpMIDIHeader->dwBytesRecorded);
				SetEvent(midi_in_event);
			}
			break;
		}
	}
}

/*********************** PrintMidiInErrorMsg() **************************
 * Retrieves and displays an error message for the passed MIDI In error
 * number. It does this using midiInGetErrorText().
 *************************************************************************/

void PrintMidiInErrorMsg(unsigned long err)
{
#define BUFFERSIZE 200
	char	buffer[BUFFERSIZE];

	
	if (!(err = midiInGetErrorText(err, &buffer[0], BUFFERSIZE)))
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

//BOOL StartMidiReceiveData(bool state)
//{
//	InitializeCriticalSection(&s_critical_section);		
//	EnterCriticalSection(&s_critical_section);
//	unsigned long	err;
//
//	if ( midi::CMIDIInDevice::GetNumDevs() == 0 )
//		return FALSE;
//	/* Open default MIDI In device */
//	if (!(err = midiInOpen(&midi_in_handle,
//							midi::CMIDIInDevice::GetIDFromName(theApp.GetProfileStringA("Settings","InPort","not connected"))-1,
//							(DWORD)&(midiCallback),
//							NULL,
//							CALLBACK_FUNCTION|MIDI_IO_STATUS)))
//	{
//		/* Store pointer to our input buffer for System Exclusive messages in MIDIHDR */
//		midiInHdr.lpData = (LPSTR)&SysXBuffer;
//
//		/* Store its size in the MIDIHDR */
//		midiInHdr.dwBufferLength = sizeof(SysXBuffer);
//
//		/* Flags must be set to 0 */
//		midiInHdr.dwFlags = 0;
//
//		/* set dwUser to 1 to indicate we are receiving data */
//		midiInHdr.dwUser = 1;
//
//		/* Prepare the buffer and MIDIHDR */
//		err = midiInPrepareHeader(midi_in_handle, &midiInHdr, sizeof(MIDIHDR));
//		if (!err)
//		{
//			/* Queue MIDI input buffer */
//			err = midiInAddBuffer(midi_in_handle, &midiInHdr, sizeof(MIDIHDR));
//			if (!err)
//			{
//				/* Start recording Midi */
//				err = midiInStart(midi_in_handle);
//
//				if (!err)
//				{
//					return TRUE;
//				}
//
//			}
//		}
//
//		/* If there was an error above, then print a message */
//		if (err) PrintMidiInErrorMsg(err);
//	}
//	else
//	{
////		printf("Error opening the default MIDI In Device!\n");
//		PrintMidiInErrorMsg(err);
//	}
//	LeaveCriticalSection(&s_critical_section);
//	return FALSE;
//}
//
//void StopMidiReceiveData(void)
//{
//	unsigned long err;
//
//	if ( midiInHdr.dwUser == 0 )
//		return;
//
//	midiInHdr.dwUser = 0;
//
//	/* Reset midi port */
//	err = midiInReset(midi_in_handle);
//
//	/* Stop recording */
//	err = midiInStop(midi_in_handle);
//
//	while(MIDIERR_STILLPLAYING == midiInUnprepareHeader(midi_in_handle, &midiInHdr, sizeof(MIDIHDR)))
//	{
//		break;
//	}
//
//	/* Close the MIDI In device */
//	err = midiInClose(midi_in_handle);
//	if (err) PrintMidiInErrorMsg(err);
//
//	return;
//}