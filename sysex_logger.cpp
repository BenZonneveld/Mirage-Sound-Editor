#include "stdafx.h"
#include "Mirage Editor.h"
#include "sysex_logger.h"
#include <mmsystem.h>

void sysex_logmsg(const unsigned char * ptr,DWORD BytesRecorded, BOOL io_dir)
{
	theApp.m_midimonitorstring.assign((const char *)ptr, BytesRecorded);

	COPYDATASTRUCT* mycds = (COPYDATASTRUCT*)LocalAlloc(LMEM_FIXED,sizeof(COPYDATASTRUCT));
	mycds->dwData = io_dir;
	mycds->cbData = BytesRecorded;
	mycds->lpData =  (LPVOID)theApp.m_midimonitorstring.data();
	theApp.m_pMidiMonThread->PostThreadMessage(WM_PARSESYSEX, NULL, (LPARAM)mycds);
}
