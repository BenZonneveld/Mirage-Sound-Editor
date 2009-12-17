#pragma once
#include "afxwin.h"

// CTransmitSamples dialog

class CTransmitSamples : public CDialog
{
	DECLARE_DYNAMIC(CTransmitSamples)

public:
	CTransmitSamples(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTransmitSamples();

// Dialog Data
	enum { IDD = IDD_SEND_SAMPLES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_LowerList;
	afx_msg void OnBnClickedTransmitSamples();
	afx_msg void OnBnClickedTransmitAbort();
	virtual BOOL OnInitDialog();
};
