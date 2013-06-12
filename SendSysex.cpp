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

void SendData(unsigned char *sysEx)
{
	midi::CMIDIOutDevice outdevice;
	outdevice.Open(outdevice.GetIDFromName(theApp.GetProfileStringA("Settings","OutPort","not connected"))-1);
	outdevice.SendMsg((LPSTR)&sysEx[1],sysEx[0]);
	outdevice.Close();
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
	midi::CMIDIOutDevice outdevice;
	outdevice.Open(outdevice.GetIDFromName(theApp.GetProfileStringA("Settings","OutPort","not connected"))-1);
	outdevice.SendMsg((LPSTR)&sysEx[0],SysXsize);
	outdevice.Close();
}
