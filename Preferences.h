/* $Id: Preferences.h,v 1.3 2008/01/04 10:40:21 root Exp $ */
#pragma once
#include "afxwin.h"


// CPreferences dialog

class CPreferences : public CDialog
{
	DECLARE_DYNAMIC(CPreferences)

public:
	CPreferences(CWnd* pParent = NULL);   // standard constructor
	
	virtual ~CPreferences();

// Dialog Data
	enum { IDD = IDD_PREFERENCES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CComboBox PrefsOutCombo;
	CComboBox PrefsInCombo;
	afx_msg void OnBnClickedDoResampling();
	afx_msg void OnBnClickedStereo2mono();
	CButton mDoResampling;
	CButton mStereoToMono;
};