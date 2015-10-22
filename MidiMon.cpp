#include "stdafx.h"
#include "Globals.h"
#include "Resource.h"
#include "Midi Doc.h"
#include "MidiMon.h"
#include "MidiMonThread.h"
#include "ThreadNames.h"

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
	CMidiMonThread* pMidiMonThread = new CMidiMonThread(m_hWnd);
#pragma warning(pop)

	pMidiMonThread->CreateThread();
	SetThreadName(pMidiMonThread->m_nThreadID, "MIDI Monitor");
	MonThreadID = pMidiMonThread->m_nThreadID;
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

void CMidiMonWnd::OnPaint()
{
  CPaintDC dc(this);
	CDC dcMem;

	dcMem.CreateCompatibleDC(&dc);

	CSize sizeTotal;
	POINT scrollPos;
	CRect rect;


	sizeTotal.cx = 50;
	sizeTotal.cy = m_nLineHt * m_pMidiDoc->GetSize();

	dcMem.SetTextColor(RGB(127,127,0));
	dcMem.SetBkColor(::GetSysColor(COLOR_WINDOW));
	GetClientRect(rect);
	dcMem.DrawText(_T("Hello, World!"), -1, rect,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	scrollPos.x = 0;
	scrollPos.y = m_pMidiDoc->GetSize();

//	SetScrollSizes(MM_TEXT, sizeTotal);

//	ScrollToPosition(scrollPos);

	CFont *pOldFont;
	
	int ct;

	dcMem.SetBkMode(TRANSPARENT);
	
	int start = 0; //GetDeviceScrollPosition().y /m_nLineHt -1;
	if (start < 0) start = 0;

	CRect rcClient;

	GetClientRect(&rcClient);

	ct = start + rcClient.Height()/m_nLineHt + 3;

	if( ct > m_pMidiDoc->GetSize() ) ct=m_pMidiDoc->GetSize();
	
	pOldFont = dcMem.SelectObject(&m_ftTimes);
	
	for(int i=start;i<ct;i++)
//	for(int i=ct-1;i>=start;i--)
	{
		if ( m_pMidiDoc->GetIO(i) )
		{
			dcMem.SetTextColor(RGB(127,0,0));
		} else {
			dcMem.SetTextColor(RGB(0,127,0));
		}
		dcMem.TextOut(0,i * m_nLineHt,&m_pMidiDoc->GetData(i)[0]);
	}
	
	dcMem.SelectObject(pOldFont);
	dcMem.DeleteDC();
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