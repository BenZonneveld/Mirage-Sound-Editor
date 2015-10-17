#include "stdafx.h"
#include "MidiMon.h"
#include "MidiMonThread.h"

HANDLE CMidiMonThread::m_hEventMidiMonThreadKilled;

IMPLEMENT_DYNCREATE(CMidiMonThread, CWinThread)

CMidiMonThread::CMidiMonThread()
{
}

CMidiMonThread::CMidiMonThread(HWND hwndParent) : m_hwndParent(hwndParent)
{
}

CMidiMonThread::~CMidiMonThread()
{
}

void CMidiMonThread::operator delete(void* p)
{
	SetEvent(m_hEventMidiMonThreadKilled);

	CWinThread::operator delete(p);
}

int CMidiMonThread::InitInstance()
{
	CWnd* pParent = CWnd::FromHandle(m_hwndParent);
	CRect rect;
	pParent->GetClientRect(&rect);

	BOOL bReturn = m_wndMidiMon.Create(_T("Midi Monitor"),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL, Rect, pParent);

	if (bReturn)
		m_pMainWnd = &m_wndMidiMon;

	return bReturn;
}

int CMidiMonThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMidiMonThread, CWinThread)
		//{{AFX_MSG_MAP(CMidiMonThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
