#include "stdafx.h"
#include "Mirage Editor.h"
#include "sysex_logger.h"
#include <mmsystem.h>

void sysex_logmsg(const unsigned char * ptr,DWORD BytesRecorded, BOOL io_dir)
{
//	strcpy_s(theApp.m_midimonitorstring.c_str(), (size_t)BytesRecorded, ptr);

	theApp.m_midimonitorstring.assign((const char *)ptr, BytesRecorded);

	theApp.cds.dwData = io_dir; // can be anything
	theApp.cds.cbData = BytesRecorded;
	theApp.cds.lpData =  (LPVOID)theApp.m_midimonitorstring.c_str();
//	theApp.m_pMidiDoc->PutData(string("SysexMsg"), io_dir);
//	theApp.m_pMidiDoc->ParseSysex(NULL, (LPARAM)(LPVOID)&theApp.cds);
//	theApp.m_MidiMonitorThread->PostThreadMessage(WM_PARSESYSEX, NULL, (LPARAM)(LPVOID)&theApp.cds);
	theApp.m_MidiMonitorThread->ThreadMessage(WM_PARSESYSEX, NULL, (LPARAM)(LPVOID)&theApp.cds);
}
