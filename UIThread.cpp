#include "stdafx.h"
#include "UIThread.h"
#include "Mirage Editor.h"
#include "MirageSysex.h"
#include "Dialog_ProgressBar.h"

/////////////////////////////////////////////////////////////////////////////
// CUIThread

IMPLEMENT_DYNCREATE(CUIThread, CWinThread)

BEGIN_MESSAGE_MAP(CUIThread, CWinThread)
	//{{AFX_MSG_MAP(CUIThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CUIThread::CUIThread()
{
	m_bKill = FALSE;
	m_bRunning = FALSE;
}

CUIThread::~CUIThread()
{
}

BOOL CUIThread::InitInstance()
{
	return TRUE;
}

int CUIThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

void CUIThread::Kill()
{
	if(m_bKill) return;

	m_bKill = TRUE;
	progress.PostMessage(WM_COMMAND, IDCANCEL); //IDC_RECEIVE_ABORT
}

int CUIThread::Run()
{
	m_bKill = FALSE;
	m_bRunning = TRUE;

	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA(m_Text);
	progress.Bar.SetRange32(0,m_Range);
	while ( m_bKill == FALSE )
	{
		Sleep(1);
	}

	m_bKill = TRUE;
	m_bRunning = FALSE;

//	if ( m_pParent)
//		m_pParent->PostMessage(WM_INFORM_CLOSE);

	return 0;
}

BOOL CUIThread::IsRunning()
{
	return m_bRunning;
}

void CUIThread::SetParent(CWnd *pParent)
{
	m_pParent = pParent;
}

void CUIThread::SetProgressText(LPCTSTR ProgressText)
{
	m_Text = ProgressText;
}

void CUIThread::SetProgressRange(unsigned int Range)
{
	m_Range = Range;
}
