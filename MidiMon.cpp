#include "stdafx.h"
#include "MidiMon.h"
#include "MidiMonThread.h"


BEGIN_MESSAGE_MAP(CMidiMonChildWnd, CMDIChildWnd)
END_MESSAGE_MAP()

CMidiMonChildWnd::CMidiMonChildWnd()
{
}

CMidiMonChildWnd::Create(LPCTSTR szTitle, LONG style, const RECT &rect, CMDIFrameWnd *pParent)
{
	if(!CMDIChildWnd::Create(NULL, szTitle, style, rect, parent))
		return FALSE;

#pragma warning(push)
#pragma warning(disable:6014)
	CMidiMonThread pMidiMonThread = new CMidiMonThread(m_hWnd);
#pragma warning(pop)

	pMidiMonThread->CreateThread();

	return TRUE;
}

CMidiMonChildWnd::DestroyWindow()
{
	OnPrepareToClose()

	return CMDIChildWnd::DestroyWindow();
}

LRESULT CMidiMonChildWnd::OnPrepareToClose(WPARAM, LPARAM)
{
	Cwnd pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDR_MidiInputType);
	
	return 0;
}

BEGIN_MESAGE_MAP(CMidiMonWnd, Cwnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP

IMPLEMENT_DYNAMIC(CMidiMonWnd, Cwnd)

BOOL CMidiMonWnd::Create(LPCTSTR szTitle, LONG style, const RECT &rect, CWnd *pParent)
{
	LPCTSTR lpszMidiMonClass = 
		AfxRegisterWndClass(CW_HREDRAW | CW_VREDRAW,
		LoadCursor(NULL, IDC_UPARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL);

	return Cwnd::Create(lpszMidiMonClass, szTitle, style, rect, parent, IDR_MIDIMON_WND);
}

CMidiMonWnd::CMidiMonWnd()
{
	m_pMidiDoc = new CMidiDoc();
}

void CMidiMonWnd::OnDestroy()
{
	Cwnd::OnDestroy();
}

LRESULT CMidiMonWnd::::OnPrepareToClose(WPARAM, LPARAM)
{
	DestroyWindow();
	return 0;
}
