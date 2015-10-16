#include "stdafx.h"
#include "Mirage Editor.h"
#include "sysex_logger.h"
#include <mmsystem.h>

void sysex_logmsg(const unsigned char * ptr,DWORD BytesRecorded, BOOL io_dir)
{
	theApp.m_midimonitorstring.assign((const char *)ptr, BytesRecorded);

	theApp.cds.dwData = io_dir; // can be anything
	theApp.cds.cbData = BytesRecorded;
	theApp.cds.lpData =  (LPVOID)theApp.m_midimonitorstring.data();
	theApp.m_MidiMonitorThread->ThreadMessage(WM_PARSESYSEX, NULL, (LPARAM)(LPVOID)&theApp.cds);
}
