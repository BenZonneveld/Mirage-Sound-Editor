#include "stdafx.h"
#include "Mirage Editor.h"
#include "Midi Doc.h"
#include "Midi View.h"
#include "MidiMonitorThread.h"

HANDLE CMidiMonitorThread::m_EventMonitorThreadKilled;

IMPLEMENT_DYNCREATE(CMidiMonitorThread, CWinThread)

BEGIN_MESSAGE_MAP(CMidiMonitorThread, CWinThread)
	//{{AFX_MSG_MAP(CMidiMonitorThread)
	ON_THREAD_MESSAGE( WM_MIDIMONITOR, OnPutData )
	ON_THREAD_MESSAGE( WM_PARSESYSEX, OnParseSysex )
	//}}AFX_MSG_MAP
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
  
	CWnd* pParent = CWnd::FromHandle(m_hwndParent);
	PeekMessage(&msg, NULL, WM_MIDIMONITOR, WM_MIDIMONITOR, PM_REMOVE);

	SetEvent(midi_monitor_started);

	return true;
}

int CMidiMonitorThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

//int CMidiMonitorThread::Run()
//{
//	MSG msg;
//	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
//	TranslateMessage(&msg);
//	DispatchMessage(&msg);
//	return true;
//}

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
	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
	m_pMidiDoc->PutData(mydata, pcds->dwData);
	return;
}

void CMidiMonitorThread::OnParseSysex(WPARAM wParam, LPARAM lParam)
{
	unsigned char * ptr;
	DWORD BytesRecorded;
	BOOL io_dir;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
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
			LogMessage += "Command Code ->";
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
		default:
			LogMessage += "Unknown Mirage Sysex";
			m_pMidiDoc->PutData(LogMessage, io_dir);
	}
	if ( *(ptr) != 0xF0 && *(ptr+(BytesRecorded-1)) != 0xF7 )
	{
		return;
	}

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
}