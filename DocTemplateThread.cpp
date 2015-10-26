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
	//CMainFrame* pMainFrame = new CMainFrame;
	//if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MirageSampDumpTYPE))
	//{
	//	delete pMainFrame;
	//	return FALSE;
	//}
	CWnd* pParent = CWnd::FromHandle(m_hwndParent);
	CRect rect;
	pParent->GetClientRect(&rect);

	BOOL bReturn = m_wndThread.Create(LPCTSTR(pRuntimeClass),
																			m_szTitle,
																			WS_CHILD | WS_VISIBLE,
																			rect,
																			pParent,
																			IDC_MONITOR_WND);

	if ( bReturn )
		m_pMainWnd = &m_wndThread;

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

	CMDIChildWnd* pMDIChildWnd = new CMDIChildWnd;
	if (!pMDIChildWnd || !pMDIChildWnd->Create(NULL, m_szTitle, WS_CHILD | WS_VISIBLE, *m_Rect, NULL)) //-V668
	{
		delete pMDIChildWnd;
		return FALSE;
	}
	
	m_pMainWnd = pMDIChildWnd;

	pDocTemplate = (CDocTemplate*)pDocTemplate->OpenDocumentFile(NULL);

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

BEGIN_MESSAGE_MAP(CDocTemplateThread, CWinThread)
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

#pragma warning(push)
#pragma warning(disable:6014)
	theApp.m_pMidMonThread = new CMultiDocTemplateThread(m_hWnd);
#pragma warning(pop)


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
