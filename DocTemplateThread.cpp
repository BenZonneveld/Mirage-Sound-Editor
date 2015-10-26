// MidMonThread.cpp : implementation file
//

#include "stdafx.h"
//#include "MainFrm.h"
#include "Mirage Editor.h"
#include "DocTemplateThread.h"
#include "ThreadNames.h"

// CMultiDocTemplateThread

IMPLEMENT_DYNCREATE(CMultiDocTemplateThread, CWinThread)

CMultiDocTemplateThread::CMultiDocTemplateThread()
{
	m_hTemplateThreadStarted = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);
}

CMultiDocTemplateThread::CMultiDocTemplateThread(HWND hwndParent) : m_hwndParent(hwndParent)
{
	m_hTemplateThreadStarted = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);

}

CMultiDocTemplateThread::~CMultiDocTemplateThread()
{
}

void CMultiDocTemplateThread::SetMDIClass(CRuntimeClass* myRuntimeClass, CMultiDocTemplate* myMultiDocTemplate)
{
	pRuntimeClass = myRuntimeClass;
	pMultiDocTemplate = myMultiDocTemplate;
}

BOOL CMultiDocTemplateThread::InitInstance()
{
	MSG uMsg;

		CMainFrame* pMainFrame = new CMainFrame;
	//CMDIFrameWnd* pMainFrame = (CMainFrame*)::AfxGetMainWnd(); 
	
	//pMainFrame->CreateNewChild(pRuntimeClass, IDR_MirageSampDumpTYPE);

//	CMainFrame* pMainFrame =  (CMainFrame*) CMainFrame::FromHandle(m_hwndParent);

	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MirageSampDumpTYPE))
	{
		delete pMainFrame;
		return FALSE;
	}
	//CWnd* pParent = CWnd::FromHandle(m_hwndParent);
	//CRect rect;
	//pParent->GetClientRect(&rect);

	//BOOL bReturn = m_wndThread.Create(LPCTSTR(pRuntimeClass),
	//																		m_szTitle,
	//																		WS_CHILD | WS_VISIBLE,
	//																		rect,
	//																		pParent,
	//																		IDC_MONITOR_WND);

	//if ( bReturn )
	//	m_pMainWnd = &m_wndThread;

	m_pMainWnd = pMainFrame;

	pMultiDocTemplate = (CMultiDocTemplate*)pMultiDocTemplate->OpenDocumentFile(NULL);

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	PeekMessage (&uMsg, NULL, 0, 0, PM_NOREMOVE);

	SetEvent(m_hTemplateThreadStarted);
	return TRUE;
}

int CMultiDocTemplateThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMultiDocTemplateThread, CWinThread)
	ON_THREAD_MESSAGE( WM_MM_PUTDATA, OnPutData )
	ON_THREAD_MESSAGE( WM_PARSESYSEX, OnParseSysex )
END_MESSAGE_MAP()

// CMultiDocTemplateThread message handlers
void CMultiDocTemplateThread::OnPutData(WPARAM wParam, LPARAM lParam)
{
	CMidiDoc* pMidiDoc = (CMidiDoc*)pMultiDocTemplate;

	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
	pMidiDoc->PutData(mydata, pcds->dwData);
}

void CMultiDocTemplateThread::OnParseSysex(WPARAM wParam, LPARAM lParam)
{
	CMidiDoc* pMidiDoc = (CMidiDoc*)pMultiDocTemplate;
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
		return;
	}
	pMidiDoc->PutData(LogMessage, io_dir);
	LogMessage.clear();

	if ( BytesRecorded > 47 ) BytesRecorded = 47;

	for(i=0 ; i <= BytesRecorded; i++)
	{
		sprintf(SEMessage,"%02X ",*(ptr+i));
		LogMessage += SEMessage;
		if ( (i+1) % 16 == 0 )
		{
			pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
		}
	}
	if ( i % 16 != 0 )
	{
		pMidiDoc->PutData(LogMessage, io_dir);
	}
}
// CDocTemplateThread

IMPLEMENT_DYNCREATE(CDocTemplateThread, CWinThread)

CDocTemplateThread::CDocTemplateThread()
{
	m_hTemplateThreadStarted = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);

}

CDocTemplateThread::CDocTemplateThread(HWND hwndParent) : m_hwndParent(hwndParent)
{
	m_hTemplateThreadStarted = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);

}

CDocTemplateThread::~CDocTemplateThread()
{
}

void CDocTemplateThread::SetMDIClass(CRuntimeClass* myRuntimeClass, CDocTemplate* myDocTemplate)
{
	pRuntimeClass = myRuntimeClass;
	pDocTemplate = myDocTemplate;
}

BOOL CDocTemplateThread::InitInstance()
{
	MSG uMsg;
//	CMainFrame* pMainFrame = new CMainFrame;
	CMDIFrameWnd* pMainFrame = new CMDIFrameWnd;
	pMainFrame->LoadFrame(IDR_MirageSampDumpTYPE);

//	CMainFrame* pMainFrame =  (CMainFrame*) CMainFrame::FromHandle(m_hwndParent);

//	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MirageSampDumpTYPE))
//	{
//		delete pMainFrame;
//		return FALSE;
//	}
	//CWnd* pParent = CWnd::FromHandle(m_hwndParent);
	//CRect rect;
	//pParent->GetClientRect(&rect);

	//BOOL bReturn = m_wndThread.Create(LPCTSTR(pRuntimeClass),
	//																		m_szTitle,
	//																		WS_CHILD | WS_VISIBLE,
	//																		rect,
	//																		pParent,
	//																		IDC_MONITOR_WND);

	//if ( bReturn )
	//	m_pMainWnd = &m_wndThread;

	m_pMainWnd = pMainFrame;

//	pDocTemplate = (CDocTemplate*)pDocTemplate->OpenDocumentFile(NULL);

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	PeekMessage (&uMsg, NULL, 0, 0, PM_NOREMOVE);

	SetEvent(m_hTemplateThreadStarted);
	return TRUE;
}

int CDocTemplateThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

// CMultiDocTemplateThread message handlers
void CDocTemplateThread::OnPutData(WPARAM wParam, LPARAM lParam)
{
	CMidiDoc* pMidiDoc = (CMidiDoc*)pDocTemplate;

	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
	pMidiDoc->PutData(mydata, pcds->dwData);
}

void CDocTemplateThread::OnParseSysex(WPARAM wParam, LPARAM lParam)
{
	CMidiDoc* pMidiDoc = (CMidiDoc*)pDocTemplate;
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
		return;
	}
	pMidiDoc->PutData(LogMessage, io_dir);
	LogMessage.clear();

	if ( BytesRecorded > 47 ) BytesRecorded = 47;

	for(i=0 ; i <= BytesRecorded; i++)
	{
		sprintf(SEMessage,"%02X ",*(ptr+i));
		LogMessage += SEMessage;
		if ( (i+1) % 16 == 0 )
		{
			pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
		}
	}
	if ( i % 16 != 0 )
	{
		pMidiDoc->PutData(LogMessage, io_dir);
	}
}
BEGIN_MESSAGE_MAP(CDocTemplateThread, CWinThread)
	ON_THREAD_MESSAGE( WM_MM_PUTDATA, OnPutData )
	ON_THREAD_MESSAGE( WM_PARSESYSEX, OnParseSysex )
END_MESSAGE_MAP()

// CMyMDIChildWnd
IMPLEMENT_DYNCREATE(CMyMDIChildWnd, CMDIChildWnd)

CMenu CMyMDIChildWnd::menu;

CMyMDIChildWnd::CMyMDIChildWnd()
{
}

BOOL CMyMDIChildWnd::Create(LPCTSTR szTitle, LONG style /* = 0 */,
	const RECT& rect /* = rectDefault */,
	CMDIFrameWnd* parent /* = NULL */)
{
	// Setup the shared menu
	if (menu.m_hMenu == NULL)
		menu.LoadMenu(IDR_MirageSampDumpTYPE);
	m_hMenuShared = menu.m_hMenu;

	if (!CMDIChildWnd::Create(NULL, szTitle, style, rect, parent))
		return FALSE;

	// The default PostNcDestroy handler will delete this CBounceMDIChildWnd
	// object when destroyed.  When Windows destroys the CBounceMDIChildWnd
	// window, it will also destroy the CBounceWnd, which is the child
	// window of the MDI child window, executing in the separate thread.
	// Finally, when the child CBounceWnd window is destroyed, the
	// CWinThread object will be automatically destroyed, as explained
	// in the comment for CBounceThread::InitInstance in mtbounce.cpp.
//
//#pragma warning(push)
//#pragma warning(disable:6014)
//	theApp.m_pMidMonThread = new CMultiDocTemplateThread(m_hWnd);
//#pragma warning(pop)


	return TRUE;
}

BEGIN_MESSAGE_MAP(CMyMDIChildWnd, CMDIChildWnd)
END_MESSAGE_MAP()

// CMyWnd

IMPLEMENT_DYNCREATE(CMyWnd, CWnd)

BOOL CMyWnd::Create(LPCTSTR szTitle, LONG style, const RECT &rect, CWnd *pParent)
{
		LPCTSTR lpszBounceClass =
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
			LoadCursor(NULL, IDC_UPARROW),
			(HBRUSH)(COLOR_WINDOW+1),
			NULL);

	return CWnd::Create(lpszBounceClass, szTitle, style, rect, pParent,
		IDC_MONITOR_WND);
}

CMyWnd::CMyWnd()
{
}

BEGIN_MESSAGE_MAP(CMyWnd, CWnd)
END_MESSAGE_MAP()
