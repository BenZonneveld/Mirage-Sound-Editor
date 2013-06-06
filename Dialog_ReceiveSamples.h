/* $Id: ReceiveSamples.h,v 1.7 2008/01/21 23:25:53 root Exp $ */

#pragma once
#include "afxwin.h"

// CReceiveSamples dialog

class CReceiveSamples : public CDialog
{
	DECLARE_DYNAMIC(CReceiveSamples)

public:
	CReceiveSamples(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReceiveSamples();

// Dialog Data
	enum { IDD = IDD_RECEIVE_SAMPLES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_LowerList;
	CListBox m_UpperList;
	afx_msg void OnBnClickedReceiveSamples();
	afx_msg void OnBnClickedReceiveAbort();
	virtual BOOL OnInitDialog();
};
