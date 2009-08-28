#pragma once


// CMessage dialog

class CMessage : public CDialog
{
//	DECLARE_DYNAMIC(CMessage)

public:
	CMessage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMessage();

// Dialog Data
	enum { IDD = IDD_MESSAGES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// Generated message map functions
	//{{AFX_MSG(CMessage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
