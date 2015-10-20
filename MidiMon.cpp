#include "stdafx.h"
#include "Globals.h"
#include "Midi Doc.h"
#include "MidiMon.h"
#include "MidiMonThread.h"


BEGIN_MESSAGE_MAP(CMidiMonChildWnd, CMDIChildWnd)
END_MESSAGE_MAP()

CMidiMonChildWnd::CMidiMonChildWnd()
{
}

BOOL CMidiMonChildWnd::Create(LPCTSTR szTitle, LONG style, const RECT &rect, CMDIFrameWnd *pParent)
{
	if(!CMDIChildWnd::Create(NULL, szTitle, style, rect, pParent))
		return FALSE;

#pragma warning(push)
#pragma warning(disable:6014)
	CMidiMonThread* pMidiMonThread = new CMidiMonThread(m_hWnd);
#pragma warning(pop)

	pMidiMonThread->CreateThread();

	return TRUE;
}

BOOL CMidiMonChildWnd::DestroyWindow()
{
	OnPrepareToClose();

	return CMDIChildWnd::DestroyWindow();
}

LRESULT CMidiMonChildWnd::OnPrepareToClose(WPARAM, LPARAM)
{
	CWnd* pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
	
	return 0;
}

BEGIN_MESSAGE_MAP(CMidiMonWnd, CWnd)
	//{{AFX_MSG_MAP(CMidiMonWnd)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MM_PUTDATA , CMidiMonWnd::OnPutData)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMidiMonWnd, CWnd)

BOOL CMidiMonWnd::Create(LPCTSTR szTitle, LONG style, const RECT &rect, CWnd *pParent)
{
	LPCTSTR lpszMidiMonClass = 
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
		LoadCursor(NULL, IDC_UPARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL);

	return CWnd::Create(lpszMidiMonClass, szTitle, style, rect, pParent, IDC_MIDIMON_WND);
}

CMidiMonWnd::CMidiMonWnd()
{
	m_pMidiDoc = new CMidiDoc();
}

LRESULT CMidiMonWnd::OnPutData(WPARAM wParam, LPARAM lParam)
{
//	MSG msg;

//	PeekMessage(&msg, NULL, WM_MIDIMONITOR,WM_MIDIMONITOR, PM_REMOVE);

	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
//	theApp.m_pMidiDoc->PutData(mydata, pcds->dwData);
	return true;
}