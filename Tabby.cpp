// Tabby.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Tab_GeneralKeyboard.h"
#include "Tab_SamplingConfig.h"
#include "Tab_Sequencer.h"
#include "Tab_MidiConfig.h"
#include "Tabby.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTabby, CTabCtrl)
	//{{AFX_MSG_MAP(CTabby)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CTabby::CTabby() : m_PreviousTab(0), m_MaxWidth(0), m_MaxHeight(0) {
m_pDialogs[0] = &m_TabOneDialog;
m_pDialogs[1] = &m_TabTwoDialog;
m_pDialogs[2] = &m_TabThreeDialog;
m_pDialogs[3] = &m_TabFourDialog;
}

void CTabby::PreSubclassWindow() {
	CRect DisplayArea;
	int i, n;
CTabCtrl::PreSubclassWindow();
m_Parent = GetParent();
if (InsertItem(0, "Keyboard")==-1)
	MessageBox("InsertItem 0 failed");
if (InsertItem(1, "Sampling")==-1)
	MessageBox("InsertItem 1 failed");
if (InsertItem(2, "Sequencer")==-1)
	MessageBox("InsertItem 2 failed");
if (InsertItem(3, "Midi ")==-1)
	MessageBox("InsertItem 3 failed");
if (!CreateTab(m_pDialogs[0], IDD_TAB_GENERAL_KEYBOARD, 0))		// initially visible and enabled
	MessageBox("The First Dialog was not created");
if (!CreateTab(m_pDialogs[1], IDD_TAB_SAMPLING_CONFIG, 1))		// initially hidden and disabled
	MessageBox("The Second Dialog was not created");
if (!CreateTab(m_pDialogs[2], IDD_TAB_CONFIGURATION, 2))		// initially hidden and disabled
	MessageBox("The Third Dialog was not created");
if (!CreateTab(m_pDialogs[3], IDD_TAB_MIDI_CONFIGURATION, 3))		// initially hidden and disabled
	MessageBox("The Fourth Dialog was not created");
SetCurSel(0);
// get our position and size
// GetWindowRect receives the screen coordinates of the corners.
GetWindowRect(&DisplayArea);
m_Parent->ScreenToClient(&DisplayArea);		// to our parent's coordinates
AdjustRect(FALSE, &DisplayArea);		// Get current display area
DisplayArea.right = m_MaxWidth + DisplayArea.left;
DisplayArea.bottom = m_MaxHeight + DisplayArea.top;
AdjustRect(TRUE, &DisplayArea);
MoveWindow(&DisplayArea);		// resize ourself
AdjustRect(FALSE, &DisplayArea);
for (i=0,n=(sizeof m_pDialogs/sizeof m_pDialogs[0]);i<n;++i)
	m_pDialogs[i]->MoveWindow(DisplayArea.left, DisplayArea.top,
		DisplayArea.Width(), DisplayArea.Height(), FALSE);
}

// Create the dialogs that are the tab control's contents.
// The parent must be the same parent as the tab control's.
// Therefore they will be repositioned later. Until
// then we gather the maximum sizes so the tab control can
// be adjusted in size so the tab dialogs will fit.
BOOL CTabby::CreateTab(CDialog *pDlg, UINT nId, int x) {
	CRect WindowRect;
	int tw, th;
if (!pDlg->Create(nId, m_Parent))
	return FALSE;
pDlg->GetWindowRect(&WindowRect);
tw = WindowRect.Width();
m_MaxWidth = tw < m_MaxWidth ? m_MaxWidth : tw;
th = WindowRect.Height();
m_MaxHeight = th < m_MaxHeight ? m_MaxHeight : th;
return TRUE;
}

void CTabby::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) {
*pResult = 0;
if (m_PreviousTab == GetCurSel())
	return;
m_pDialogs[m_PreviousTab]->EnableWindow(FALSE);
m_pDialogs[m_PreviousTab]->ShowWindow(SW_HIDE);
m_PreviousTab = GetCurSel();
m_pDialogs[m_PreviousTab]->EnableWindow(TRUE);
m_pDialogs[m_PreviousTab]->ShowWindow(SW_SHOW);
}
