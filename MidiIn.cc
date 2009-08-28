/*
You must link with winmm.lib. If using Visual C++, go to Build->Settings. Flip to the Link page,
and add winmm.lib to the library/object modules.

This app inputs MIDI messages and displays information about each one in a console window.

I incorporate a callback into this example.
*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <mmsystem.h>






/*	A buffer to hold incoming System Exclusive bytes. I arbitrarily make this 256 bytes. Note:
	For Win 3.1 16-bit apps, this buffer should be allocated using GlobalAlloc with the
	GMEM_MOVEABLE flag to get a handle of the memory object. Then pass this handle to the
	GlobalLock function to get a pointer to the memory object. To free a data block, use
	GlobalUnlock and GlobalFree. But Win32 doesn't appear to have this limitation.
*/
unsigned char SysXBuffer[256];

/* A flag to indicate whether I'm currently receiving a SysX message */
unsigned char SysXFlag = 0;





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

void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	LPMIDIHDR		lpMIDIHeader;
	unsigned char *	ptr;
	TCHAR			buffer[80];
	unsigned char 	bytes;

	/* Determine why Windows called me */
	switch (uMsg)
	{
		/* Received some regular MIDI message */
		case MIM_DATA:
		{
			/* Display the time stamp, and the bytes. (Note: I always display 3 bytes even for
			Midi messages that have less) */
			sprintf(&buffer[0], "0x%08X 0x%02X 0x%02X 0x%02X\r\n", dwParam2, dwParam1 & 0x000000FF, (dwParam1>>8) & 0x000000FF, (dwParam1>>16) & 0x000000FF);
			_cputs(&buffer[0]);
			break;
		}

		/* Received all or part of some System Exclusive message */
		case MIM_LONGDATA:
		{
			/* If this application is ready to close down, then don't midiInAddBuffer() again */
			if (!(SysXFlag & 0x80))
			{
				/*	Assign address of MIDIHDR to a LPMIDIHDR variable. Makes it easier to access the
					field that contains the pointer to our block of MIDI events */
				lpMIDIHeader = (LPMIDIHDR)dwParam1;

				/* Get address of the MIDI event that caused this call */
				ptr = (unsigned char *)(lpMIDIHeader->lpData);

				/* Is this the first block of System Exclusive bytes? */
				if (!SysXFlag)
				{
					/* Print out a noticeable heading as well as the timestamp of the first block.
						(But note that other, subsequent blocks will have their own time stamps). */
					printf("*************** System Exclusive **************\r\n0x%08X ", dwParam2);

					/* Indicate we've begun handling a particular System Exclusive message */
					SysXFlag |= 0x01;
				}

				/* Is this the last block (ie, the end of System Exclusive byte is here in the buffer)? */
				if (*(ptr + (lpMIDIHeader->dwBytesRecorded - 1)) == 0xF7)
				{
					/* Indicate we're done handling this particular System Exclusive message */
					SysXFlag &= (~0x01);
				}

				/* Display the bytes -- 16 per line */
				bytes = 16;

				while((lpMIDIHeader->dwBytesRecorded--))
				{
					if (!(--bytes))
					{
						sprintf(&buffer[0], "0x%02X\r\n", *(ptr)++);
						bytes = 16;
					}
					else
						sprintf(&buffer[0], "0x%02X ", *(ptr)++);

					_cputs(&buffer[0]);
				}

				/* Was this the last block of System Exclusive bytes? */
				if (!SysXFlag)
				{
					/* Print out a noticeable ending */
					_cputs("\r\n******************************************\r\n");
				}

				/* Queue the MIDIHDR for more input */
				midiInAddBuffer(handle, lpMIDIHeader, sizeof(MIDIHDR));
			}

			break;
		}

		/* Process these messages if you desire */
/*
		case MIM_OPEN:
        case MIM_CLOSE:
        case MIM_ERROR:
        case MIM_LONGERROR:
        case MIM_MOREDATA:

			break;
*/
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
		printf("%s\r\n", &buffer[0]);
	}
	else if (err == MMSYSERR_BADERRNUM)
	{
		printf("Strange error number returned!\r\n");
	}
	else if (err == MMSYSERR_INVALPARAM)
	{
		printf("Specified pointer is invalid!\r\n");
	}
	else
	{
		printf("Unable to allocate/lock memory!\r\n");
	}
}





/* ******************************** main() ******************************** */

int main(int argc, char **argv)
{
	HMIDIIN			handle;
	MIDIHDR			midiHdr;
	unsigned long	err;

	/* Open default MIDI In device */
	if (!(err = midiInOpen(&handle, 0, (DWORD)midiCallback, 0, CALLBACK_FUNCTION)))
	{
		/* Store pointer to our input buffer for System Exclusive messages in MIDIHDR */
		midiHdr.lpData = (LPBYTE)&SysXBuffer[0];

		/* Store its size in the MIDIHDR */
		midiHdr.dwBufferLength = sizeof(SysXBuffer);

		/* Flags must be set to 0 */
		midiHdr.dwFlags = 0;

		/* Prepare the buffer and MIDIHDR */
		err = midiInPrepareHeader(handle, &midiHdr, sizeof(MIDIHDR));
		if (!err)
		{
			/* Queue MIDI input buffer */
			err = midiInAddBuffer(handle, &midiHdr, sizeof(MIDIHDR));
			if (!err)
			{
				/* Start recording Midi */
				err = midiInStart(handle);
				if (!err)
				{
					/* Wait for user to abort recording */
					printf("Press any key to stop recording...\r\n\n");
					_getch();

					/* We need to set a flag to tell our callback midiCallback()
					   not to do any more midiInAddBuffer(), because when we
					   call midiInReset() below, Windows will send a final
					   MIM_LONGDATA message to that callback. If we were to
					   allow midiCallback() to midiInAddBuffer() again, we'd
					   never get the driver to finish with our midiHdr
					*/
					SysXFlag |= 0x80;
					printf("\r\nRecording stopped!\n");
				}

				/* Stop recording */
				midiInReset(handle);
			}
		}

		/* If there was an error above, then print a message */
		if (err) PrintMidiInErrorMsg(err);

		/* Close the MIDI In device */
		while ((err = midiInClose(handle)) == MIDIERR_STILLPLAYING) Sleep(0);
		if (err) PrintMidiInErrorMsg(err);

		/* Unprepare the buffer and MIDIHDR. Unpreparing a buffer that has not been prepared is ok */
	    midiInUnprepareHeader(handle, &midiHdr, sizeof(MIDIHDR));
	}
	else
	{
		printf("Error opening the default MIDI In Device!\r\n");
		PrintMidiInErrorMsg(err);
	}

	return(0);
}