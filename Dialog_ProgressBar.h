#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDialogProgress dialog
// $Id: ProgressBar.h,v 1.5 2008/01/23 22:42:34 root Exp $

#define BAR_MAX 1000000

class CProgressDialog : public CDialog//, public NLMISC::IProgressCallback
{
// Construction
public:
	CProgressDialog(CWnd* pParent = NULL);   // standard constructor
	void	MakeThread(LPCSTR ProgressTitle, UINT Range);
	void	KillThread();
// Dialog Data
	//{{AFX_DATA(CProgressDialog)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	Bar;
	//}}AFX_DATA

	BOOL	Abort();
	/// \from IProgressCallback
	virtual void progress (unsigned int progressValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CProgressDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
