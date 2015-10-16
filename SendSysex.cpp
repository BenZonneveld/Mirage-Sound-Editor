/* $Id: SendSysex.cpp,v 1.10 2008/02/23 23:58:51 root Exp $ */
#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#ifdef _DEBUG
#include "sysex_logger.h"
#endif

#include "MirageSysex.h"

void SendLongData(unsigned char *sysEx, UINT SysXsize)
{
	sysex_logmsg(&sysEx[0],SysXsize, MIDIMON_OUT);
	theApp.m_OutDevice.SendMsg((LPSTR)&sysEx[0],SysXsize);
	theApp.m_pMidiDoc->OnUpdateAllViews();
}

void SendData(unsigned char *sysEx)
{
	theApp.m_OutDevice.SendMsg((LPSTR)&sysEx[1],sysEx[0]);
	sysex_logmsg(&sysEx[1],sysEx[0], MIDIMON_OUT);
	theApp.m_pMidiDoc->OnUpdateAllViews();
}