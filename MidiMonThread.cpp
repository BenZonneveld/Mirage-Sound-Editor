#include "stdafx.h"
#include "MidiMon.h"
#include "Midi View.h"
#include "MidiMonThread.h"
#include "Mirage Editor.h"

HANDLE CMidiMonThread::m_hEventMidiMonThreadKilled;

IMPLEMENT_DYNCREATE(CMidiMonThread, CWinThread)

BEGIN_MESSAGE_MAP(CMidiMonThread, CWinThread)
	ON_THREAD_MESSAGE(WM_MM_PUTDATA ,CMidiMonThread::OnPutData)
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
	m_pContext->m_pCurrentDoc = m_pMidiDoc;

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

	PeekMessage(&msg, NULL,NULL,NULL,PM_NOREMOVE);

	pParentWnd->GetClientRect(&rect);
//	CMidiMonChildWnd* myChildWnd = pParentWnd->MDIGetActive();
	//CMidiMonChildWnd* MonChild = CMidiMonChildWndMDIGetActive();
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
}