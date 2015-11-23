#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// PasteMulti dialog

class CPasteMulti : public CDialog
{
	DECLARE_DYNAMIC(CPasteMulti)

public:
	CPasteMulti(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPasteMulti();

// Dialog Data
	enum { IDD = IDD_MULTIPLE_COPIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeltaposRepeatSpin(NMHDR *pNMHDR, LRESULT *pResult);
//	virtual BOOL OnInitDialog();
	CEdit RepeatValue;
	CSpinButtonCtrl SpinButton;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
