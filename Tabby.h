#if !defined(AFX_TABBY_H__60E75989_9A76_4ADE_8AFF_F5A88D8962A4__INCLUDED_)
#define AFX_TABBY_H__60E75989_9A76_4ADE_8AFF_F5A88D8962A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tabby.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabby window

class CTabby : public CTabCtrl {
	CWnd* m_Parent;
	int m_PreviousTab;
public:
	CTabGeneralKeyboard m_TabOneDialog;
	CTabSamplingConfig m_TabTwoDialog;
	CTabSequencer m_TabThreeDialog;
	CTabMidiConfig m_TabFourDialog;
	CDialog *m_pDialogs[4];
	int m_MaxWidth, m_MaxHeight;

	BOOL CreateTab(CDialog *, UINT, int);

public:
	CTabby();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabby)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTabby() {};

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabby)
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABBY_H__60E75989_9A76_4ADE_8AFF_F5A88D8962A4__INCLUDED_)
