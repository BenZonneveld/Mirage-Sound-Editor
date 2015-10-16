#include "stdafx.h"
#include "Mirage Editor.h"
//#include "globals.h"
#include "Midi Doc.h"
#include "Midi View.h"
#include "MidiMonitorThread.h"

HANDLE CMidiMonitorThread::m_EventMonitorThreadKilled;

IMPLEMENT_DYNCREATE(CMidiMonitorThread, CWinThread)

BEGIN_MESSAGE_MAP(CMidiMonitorThread, CWinThread)
	//{{AFX_MSG_MAP(CMidiMonitorThread)
	//}}AFX_MSG_MAP
//	ON_THREAD_MESSAGE( WM_MIDIMONITOR, OnPutData )
//	ON_THREAD_MESSAGE( WM_PARSESYSEX, OnParseSysex )
END_MESSAGE_MAP()

CMidiMonitorThread::CMidiMonitorThread()
{
}

CMidiMonitorThread::CMidiMonitorThread(HWND hwndParent) : m_hwndParent(hwndParent)
{
}

CMidiMonitorThread::~CMidiMonitorThread()
{
}

void CMidiMonitorThread::operator delete(void* p)
{
	SetEvent(m_EventMonitorThreadKilled);

	CWinThread::operator delete(p);
}

int CMidiMonitorThread::InitInstance()
{
	MSG msg;
  m_wParam = 0;
	m_lParam = 0;

	MessageFlag = CreateEvent(	NULL,               // default security attributes
															TRUE,               // manual-reset event
															FALSE,              // initial state is nonsignaled
															FALSE);
	m_message = 0;

	PeekMessage(&msg, NULL, WM_MIDIMONITOR, WM_MIDIMONITOR, PM_NOREMOVE);

	SetEvent(theApp.midi_monitor_started);

	return true;
}

int CMidiMonitorThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

void CMidiMonitorThread::ThreadMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
//	WaitForSingleObject(MessageFlag, INFINITE);
	m_wParam = wParam;
	m_lParam = lParam;
	m_message = message;
	SetEvent(MessageFlag);
	return;
}

int CMidiMonitorThread::Run()
{
	while(true)  
	{
		WaitForSingleObject(MessageFlag, INFINITE);
		{
			switch (m_message) 
			{
				case WM_MIDIMONITOR: 
					OnPutData(m_wParam,m_lParam);
					break;
				case WM_PARSESYSEX:
					OnParseSysex(m_wParam,m_lParam);
					break;
			}
		}
	}
	return 0;
}

void CMidiMonitorThread::SetHandle(HWND hwnd)
{
	m_hwndParent=hwnd;
}

void CMidiMonitorThread::SetMidiDoc(CMidiDoc* pMidiDoc)
{
	m_pMidiDoc=pMidiDoc;
}

void CMidiMonitorThread::OnPutData(WPARAM wParam, LPARAM lParam)
{
//	MSG msg;

//	PeekMessage(&msg, NULL, WM_MIDIMONITOR,WM_MIDIMONITOR, PM_REMOVE);

	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)m_lParam;
	mydata=(LPCTSTR)(pcds->lpData);
	m_pMidiDoc->PutData(mydata, pcds->dwData);
}

void CMidiMonitorThread::OnParseSysex(WPARAM wParam, LPARAM lParam)
{
//	MSG msg;
//	GetMessage(&msg, NULL, WM_PARSESYSEX,WM_PARSESYSEX);
	unsigned char * ptr;
	DWORD BytesRecorded;
	BOOL io_dir;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)m_lParam;
	ptr = (unsigned char *)(pcds->lpData);
	io_dir = (BOOL)(pcds->dwData);
	BytesRecorded = pcds->cbData;

	char SEMessage[64];
	std::string LogMessage;

	unsigned char MessageID;
	int i;

	switch (*(ptr+3))
	{
		case CONFIG_PARM_REQ:
			LogMessage += "Config Parameters Request";
			break;
		case COMMAND_CODE:
			LogMessage += "Command Code";
			switch ( *(ptr+4))
			{
				case SELECT_LOWER:
					LogMessage += "Select Lower Wavesample";
					break;
				case SELECT_UPPER:
					LogMessage += "Select Upper Wavesample";
					break;
			}
			break;
		case CONFIG_PARM_DUMP:
			LogMessage += "Config Parameters Dump Data";
			break;
		case LOWER_PRG_DUMP_REQ:
			LogMessage += "Lower Program Dump Request";
			break;
		case UPPER_PRG_DUMP_REQ:
			LogMessage += "Upper Program Dump Request";
			break;
		case WAVE_DUMP_REQ:
			LogMessage += "Wave Dump Request";
			break;
		case PRG_DUMP_LOWER:
			LogMessage += "Lower Program Dump Data";
			break;
		case PRG_DUMP_UPPER:
			LogMessage += "Upper Program Dump Data";
			break;
		case WAVE_DUMP_DATA:
			LogMessage += "Wave Dump Data";
			break;
		case PRG_STATUS_MSG:
			LogMessage += "Program Status Message";
			break;
		case WAVE_STATUS_MSG:
			LogMessage += "Wavesample Status Message";
			break;
		case WAVE_ACK:
			LogMessage += "Wavesample acknowledge";
			break;
		case WAVE_NACK:
			LogMessage += "Wavesample NOT acknowleged";
			break;
		case WAVEDUMPABSREQ:
			LogMessage += "Wavesample Dump Absolute Request";
			break;
		case WAVEDUMPABSDATA:
			LogMessage += "Wavesample Dump Absolute Data";
			break;
		case PRG_PARM_MSG:
			LogMessage += "Program Parameter Message";
			break;
		case SMP_PARM_MSG:
			LogMessage += "Wavesample Parameter Message";
			break;
		case WAVEMANIPCMD:
			LogMessage += "Wavesample Manipulation Function Command";
			break;
		default:
			LogMessage += "Unknown Mirage Sysex";
			sprintf(SEMessage," Message ID: %02X ",*(ptr+3));
			LogMessage += SEMessage;
	}
	if ( *(ptr) != 0xF0 && *(ptr+(BytesRecorded-1)) != 0xF7 )
	{
		ResetEvent(MessageFlag);
		return;
	}
	
	m_pMidiDoc->PutData(LogMessage, io_dir);
	LogMessage.clear();

	sprintf(SEMessage, "System Exclusive Size: %d", BytesRecorded);
	m_pMidiDoc->PutData(LogMessage, io_dir);
	LogMessage.clear();

	if ( BytesRecorded > 47 ) BytesRecorded = 47;

	for(i=0 ; i < BytesRecorded; i++)
	{
		sprintf(SEMessage,"%02X ",*(ptr+i));
		LogMessage += SEMessage;
		if ( (i+1) % 16 == 0 )
		{
			m_pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
		}
	}
	if ( i % 16 != 0 )
		m_pMidiDoc->PutData(LogMessage, io_dir);
	ResetEvent(MessageFlag);
}