#include "stdafx.h"
#include "Globals.h"
#include "Resource.h"
#include "Midi Doc.h"
#include "MidiMon.h"
#include "MidiMonThread.h"
#include "ThreadNames.h"
#include "Mirage Editor.h"

IMPLEMENT_DYNCREATE(CMidiMonChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CMidiMonChildWnd, CMDIChildWnd)
	ON_MESSAGE(WM_MM_PUTDATA ,OnPutData)
END_MESSAGE_MAP()

CMidiMonChildWnd::CMidiMonChildWnd()
{
}

BOOL CMidiMonChildWnd::Create(LPCTSTR szTitle, LONG style /* = 0 */, const RECT& rect /* = rectDefault */, CMDIFrameWnd* pParent /* = NULL */)
{
	// Setup the shared menu
	//if (menu.m_hMenu == NULL)
	//	menu.LoadMenu(IDR_MirageSampDumpTYPE);
	//m_hMenuShared = menu.m_hMenu;

	if(!CMDIChildWnd::Create(NULL, szTitle, style, rect, pParent))
		return FALSE;

#pragma warning(push)
#pragma warning(disable:6014)
	theApp.m_pMidiMonThread = new CMidiMonThread(m_hWnd);
#pragma warning(pop)
	theApp.midi_monitor_started = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);

	theApp.m_pMidiMonThread->CreateThread();
	SetThreadName(theApp.m_pMidiMonThread->m_nThreadID, "MIDI Monitor");
	MonThreadID = theApp.m_pMidiMonThread->m_nThreadID;
	return TRUE;
}

BOOL CMidiMonChildWnd::DestroyWindow()
{
	OnPrepareToClose();

	return CMDIChildWnd::DestroyWindow();
}

CWnd* CMidiMonChildWnd::GetMidiMonWnd()
{
	return (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
}

LRESULT CMidiMonChildWnd::OnPrepareToClose(WPARAM, LPARAM)
{
	CWnd* pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
	
	return 0;
}

LRESULT CMidiMonChildWnd::OnPutData(WPARAM wParam, LPARAM lParam)
{
	m_pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
	if (m_pMidiMonWnd == NULL)
		return FALSE; // child CBounceWnd not created yet.

	return (BOOL)m_pMidiMonWnd->SendMessage(WM_MM_PUTDATA, wParam, lParam);
}

BEGIN_MESSAGE_MAP(CMidiMonWnd, CWnd)
	//{{AFX_MSG_MAP(CMidiMonWnd)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MM_PUTDATA ,OnPutData)
  ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMidiMonWnd, CWnd)

BOOL CMidiMonWnd::Create(LPCTSTR szTitle, LONG style, const RECT &rect, CWnd *pParent)
{
	LPCTSTR lpszMidiMonClass = 
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
		NULL,
		(HBRUSH)(COLOR_WINDOW+1),
		NULL);

	return CWnd::Create(lpszMidiMonClass, szTitle, style, rect, pParent, IDC_MIDIMON_WND);
}

CMidiMonWnd::CMidiMonWnd()
{
	m_pMidiDoc = new CMidiDoc();
}

int CMidiMonWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CDC* pDC = GetDC();
	
	m_ftTimes.CreatePointFont(110,"Courier New",pDC);
	
	CFont *pOldFont;
	pOldFont = pDC->SelectObject(&m_ftTimes);

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	m_nLineHt = tm.tmHeight + tm.tmExternalLeading;

	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	return 0;
}

LRESULT CMidiMonWnd::OnPutData(WPARAM wParam, LPARAM lParam)
{
	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
	m_pMidiDoc->PutData(mydata, pcds->dwData);
	OnPaint();
	return true;
}