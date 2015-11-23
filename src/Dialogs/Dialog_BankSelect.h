#pragma once


// CBankSelect dialog

class CBankSelect : public CDialog
{
	DECLARE_DYNAMIC(CBankSelect)

public:
	CBankSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBankSelect();

// Dialog Data
	enum { IDD = IDD_BANK_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_Bank[3];
	virtual BOOL OnInitDialog();
	afx_msg void OnBnBankSelectOk();
};
