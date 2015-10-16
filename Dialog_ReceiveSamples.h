/* $Id: ReceiveSamples.h,v 1.7 2008/01/21 23:25:53 root Exp $ */

#pragma once
#include "afxwin.h"

// CReceiveSamples dialog

class CReceiveSamples : public CDialog
{
	DECLARE_DYNAMIC(CReceiveSamples)

public:
	BOOL Create( UINT nId,  CWnd * pWnd);
	CReceiveSamples(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReceiveSamples();
	BOOL UpdateSampleData();

// Dialog Data
	enum { IDD = IDD_RECEIVE_SAMPLES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedReceiveSamples();
	afx_msg void OnBnClickedReceiveAbort();

public:
	CListBox m_LowerList;
	CListBox m_UpperList;
	virtual BOOL OnInitDialog();
};
