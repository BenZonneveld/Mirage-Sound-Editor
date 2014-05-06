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

void SendLongData(unsigned char *sysEx, UINT SysXsize)
{
	theApp.m_OutDevice.SendMsg((LPSTR)&sysEx[0],SysXsize);
}

void SendData(unsigned char *sysEx)
{
	theApp.m_OutDevice.SendMsg((LPSTR)&sysEx[1],sysEx[0]);
}