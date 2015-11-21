// ProgressBar.cpp : implementation file
//

#include "stdafx.h"
#include "..\Mirage Editor.h"
#include "Dialog_ProgressBar.h"
#include "..\Globals.h"
#include "..\Gui\UIThread.h"

CUIThread *m_pUIThread;

CProgressDialog::CProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
	//}}AFX_DATA_INIT
}

void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialog)
	DDX_Control(pDX, IDC_PROGRESS, Bar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers

void CProgressDialog::progress (unsigned int progressValue)
{
	if ( Bar.GetSafeHwnd() != NULL )
	{
		UpdateData();
//	float value = getCropedValue (progressValue);
		Bar.SetPos(progressValue);

		UpdateData (FALSE);
	}
}

BOOL CProgressDialog::OnInitDialog() 
{
	CStatusBar pStatusBar;
	CDialog::OnInitDialog();

//	pStatusBar=theApp.m_pMainFrame->m_wndStatusBar;
	
	Bar.SetRange32 (0, BAR_MAX);
//	Bar.SetParent(CWnd::FromHandle(theApp.m_pMainFrame->GetSafeHwnd()));
//	Bar.SetParent(pStatusBar);

	// Setup the progress in the first pane
//	CRect crPaneRect;
//	pStatusBar.GetStatusBarCtrl().GetRect(1,&crPaneRect);

	// Adjust rectangle, deflate a bit
//	crPaneRect.DeflateRect(0,2,2,2);

//	Bar.MoveWindow(crPaneRect);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CProgressDialog::Abort()
{
	MSG msg;
	while(::PeekMessage(&msg,NULL,NULL,NULL,PM_NOREMOVE))
	{
		AfxGetThread()->PumpMessage();
	}
	return false;
}
