#pragma once
#include "afxwin.h"
#include "resource.h"
#include "samplerate.h"

// Resample_Dialog dialog

class CResample_Dialog : public CDialog
{
	DECLARE_DYNAMIC(CResample_Dialog)

public:
	CResample_Dialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResample_Dialog();
	LPSTR	*m_lpSrcData;
	unsigned char m_targetpages;
	unsigned char m_currentpages;
	bool	m_resample_cancel_ok;

// Dialog Data
	enum { IDD = IDD_RESAMPLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox mResampleConverterCombo;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedResamplePrelisten();
	CComboBox m_PagesCombo;
};
