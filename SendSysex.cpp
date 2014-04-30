/* $Id: SendSysex.cpp,v 1.10 2008/02/23 23:58:51 root Exp $ */
#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#ifdef _DEBUG
#include "sysexdebug.h"
#endif

#include "MirageSysex.h"

void PrintMidiOutErrorMsg(unsigned long err);

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
	theApp.m_OutDevice.SendMsg((LPSTR)&sysEx[0],SysXsize);
}

void SendData(unsigned char *sysEx)
{
	theApp.m_OutDevice.SendMsg((LPSTR)&sysEx[1],sysEx[0]);
}