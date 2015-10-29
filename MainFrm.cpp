// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Mirage Editor.h"
//#include "MirageSysex.h"
#include "CMidiReceiver.h"
#include "Globals.h"
#include "PitchTable.h"
#include "MainFrm.h"

#include "MyMDIChildWnd.h"
#include "Midi View.h"
#include "Threadnames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_PROGRESS, OnProgress)
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, &CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, &CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CMDIFrameWnd::OnHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_MESSAGE_PAGE, OnUpdatePage)
	ON_UPDATE_COMMAND_UI(ID_MESSAGE_RATE, OnUpdateRate)
	ON_UPDATE_COMMAND_UI(ID_MESSAGE_GENERAL, OnUpdateGeneric)
	ON_UPDATE_COMMAND_UI(ID_MESSAGE_PITCH, OnUpdatePitch)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,// status line indicator
	ID_MESSAGE_GENERAL,
	ID_MESSAGE_PAGE,
	ID_MESSAGE_PITCH,
	ID_MESSAGE_RATE,
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_nPages = 0;
	m_rate = 0;
	m_GenericMessage.Format("");
	m_Pitch = 0;
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() & ~CBRS_HIDE_INPLACE);

	if (!m_wndSampleToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT) ||
		!m_wndSampleToolBar.LoadToolBar(IDR_SAMPLES))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndSampleToolBar.SetBarStyle(m_wndSampleToolBar.GetBarStyle() & ~CBRS_HIDE_INPLACE);

	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndSampleToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
	m_wndReBar.SetBarStyle(m_wndReBar.GetBarStyle() & ~CBRS_HIDE_INPLACE);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	theApp.m_ThreadId = GetCurrentThreadId();

	CMyMDIChildWnd* pMyMDIChildWnd = new CMyMDIChildWnd;
	pMyMDIChildWnd->Create( _T("Bounce"),
													WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
													rectDefault,
													this);
	
//	theApp.m_pMidMonThread->SetMDIClass(RUNTIME_CLASS(CMyMDIChildWnd), new CMultiDocTemplate(IDR_MidiInputType,
//																			RUNTIME_CLASS(CMidiDoc),
//																			RUNTIME_CLASS(CMyMDIChildWnd),
//																			RUNTIME_CLASS(CMidiView)),IDR_MirageSampDumpTYPE );
//	theApp.m_pMidMonThread->SetTitle("Midi Monitor Thread");
//
////	theApp.m_pMidMonThread->CreateThread();
//	SetThreadName(theApp.m_pMidMonThread->m_nThreadID, "Midi Monitor Thread");
//	theApp.m_pMidMonThread->ResumeThread();
//	WaitForSingleObject(theApp.m_pMidMonThread->m_hTemplateThreadStarted, INFINITE);

//	theApp.m_pMidMonThread = new CMultiDocTemplateThread(GetSafeHwnd());
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return CFrameWnd::OnCreateClient(lpcs, pContext);
}
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnUpdatePage(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
   CString	strPage;
   if ( m_nPages == 0 )
   {
	   strPage.Format("");
   } else {
	   strPage.Format("%d (%02X) Sample Pages", m_nPages, m_nPages);
   }
   pCmdUI->SetText(strPage);
}

void CMainFrame::OnUpdateRate(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
   CString	strRate;
   if ( m_rate == 0 )
   {
	   strRate.Format("");
   } else {
	   strRate.Format("%i Hz", m_rate);
   }
   pCmdUI->SetText(strRate);
}

void CMainFrame::SetGenericMessage(CString Message)
{
	m_GenericMessage = Message;
}

void CMainFrame::OnUpdateGeneric(CCmdUI* pCmdUI)
{
	if (m_GenericMessage.GetLength() == 0)
	{
		pCmdUI->Enable(FALSE);
	} else {
		pCmdUI->Enable();
		pCmdUI->SetText(m_GenericMessage);
	}
}

void CMainFrame::SetPages(int Pages)
{
	m_nPages=Pages;
}

void CMainFrame::SetSampleRate(long Rate)
{
	m_rate=Rate;
}

void CMainFrame::SetPitch(double pitch)
{
	m_Pitch = pitch;
}

void CMainFrame::OnUpdatePitch(CCmdUI* pCmdUI)
{
//	int Index;
//	int factor=1;
  pCmdUI->Enable();
  CString	strPitch;
  if ( m_Pitch == 0.0 )
  {
		strPitch.Format("");
  } else {		
	  strPitch.Format("%.3f Hz", m_Pitch);
  }
  pCmdUI->SetText(strPitch);
}

LRESULT CMainFrame::OnProgress(UINT wParam, LONG lParam)
{
	int progress_value = 0;
	HWND hwndProgress=progress.GetSafeHwnd();
	if (hwndProgress != NULL)
	{
		progress_value = progress.Bar.GetPos() + lParam;
		progress.progress(progress_value);
	}
//	theApp.m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	return 0;
}