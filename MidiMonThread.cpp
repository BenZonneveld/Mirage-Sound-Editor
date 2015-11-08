#include "stdafx.h"
#include "MidiMon.h"
#include "Midi View.h"
#include "MidiMonThread.h"
#include "Mirage Editor.h"

HANDLE CMidiMonThread::m_hEventMidiMonThreadKilled;

IMPLEMENT_DYNCREATE(CMidiMonThread, CWinThread)

BEGIN_MESSAGE_MAP(CMidiMonThread, CWinThread)
	ON_THREAD_MESSAGE(WM_MM_PUTDATA ,OnPutData)
	ON_THREAD_MESSAGE(WM_PARSESYSEX, OnParseSysex)
		//{{AFX_MSG_MAP(CMidiMonThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMidiMonThread::CMidiMonThread()
{
}

CMidiMonThread::CMidiMonThread(HWND hwndParent) : m_hwndParent(hwndParent)
{
		theApp.midi_monitor_started = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);
}

CMidiMonThread::~CMidiMonThread()
{
}

void CMidiMonThread::operator delete(void* p)
{
	SetEvent(m_hEventMidiMonThreadKilled);

	CWinThread::operator delete(p);
}

void CMidiMonThread::SetCreateContext(CCreateContext* pContext)
{
	m_pMidiMonitor = new CMultiDocTemplate(IDR_MidiInputType,
											RUNTIME_CLASS(CMidiDoc),
											RUNTIME_CLASS(CMidiMonChildWnd),
											RUNTIME_CLASS(CMidiView));

	m_pMidiDoc = new CMidiDoc;
	m_pMidiDoc->SetTitle(_T("Midi Monitor"));

	//m_Context.m_pNewViewClass = pContext->m_pNewViewClass;
	//m_Context.m_pCurrentDoc = /*pContext->m_pCurrentDoc; */m_pMidiDoc;
	//m_Context.m_pNewDocTemplate = /*pContext->m_pNewDocTemplate; */m_pMidiMonitor;
	//m_Context.m_pLastView = pContext->m_pLastView;
	//m_Context.m_pCurrentFrame = pContext->m_pCurrentFrame;
	//m_pContext = &m_Context;
	
	m_pContext = pContext;
	m_pMidiDoc = (CMidiDoc*)m_pContext->m_pCurrentDoc;

//	memcpy(m_pContext,pContext, sizeof(CCreateContext));
}

CMidiDoc* CMidiMonThread::GetDocument()
{
	return m_pMidiDoc;
}

int CMidiMonThread::InitInstance()
{
	CMidiMonChildWnd* pParentWnd = (CMidiMonChildWnd*)CWnd::FromHandle(m_hwndParent);
	CRect rect;
	MSG msg;

	PeekMessage(&msg, NULL,NULL,NULL,PM_REMOVE);

	pParentWnd->GetClientRect(&rect);

	// Note: can be a CWnd with PostNcDestroy self cleanup
	CWnd* pView = (CWnd*)m_pContext->m_pNewViewClass->CreateObject();
	if (pView == NULL)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of view type %hs failed.\n",
			m_pContext->m_pNewViewClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CWnd, pView);

	// views are always created with a border!
	if (!pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), pParentWnd, m_nID, m_pContext))
	{
		TRACE(traceAppMsg, 0, "Warning: could not create view for frame.\n");
		return NULL;        // can't continue without a view
	}

	m_pMainWnd = pView;

	SetEvent(theApp.midi_monitor_started);
	return TRUE;
}

int CMidiMonThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

void CMidiMonThread::OnPutData(WPARAM wParam, LPARAM lParam)
{
	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
	m_pMidiDoc->PutData(mydata, pcds->dwData);
	LocalFree(pcds);
}

void CMidiMonThread::OnParseSysex(WPARAM wParam, LPARAM lParam)
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
		LocalFree(pcds);
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
	LocalFree(pcds);
}
