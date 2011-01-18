#pragma once


// CMirageMemLayout dialog

class CMirageMemLayout : public CDialog
{
	DECLARE_DYNAMIC(CMirageMemLayout)

public:
	CMirageMemLayout(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMirageMemLayout();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DISK_BANK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CRect m_MemoryRect[16];
	CWnd m_CwndSample[16];

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnStnClickedMapLwave1();
	afx_msg void OnStnClickedMapLwave2();
	afx_msg void OnStnClickedMapLwave3();
	afx_msg void OnStnClickedMapLwave4();
	afx_msg void OnStnClickedMapLwave5();
	afx_msg void OnStnClickedMapLwave6();
	afx_msg void OnStnClickedMapLwave7();
	afx_msg void OnStnClickedMapLwave8();
	afx_msg void OnStnClickedMapUwave1();
	afx_msg void OnStnClickedMapUwave2();
	afx_msg void OnStnClickedMapUwave3();
	afx_msg void OnStnClickedMapUwave4();
	afx_msg void OnStnClickedMapUwave5();
	afx_msg void OnStnClickedMapUwave6();
	afx_msg void OnStnClickedMapUwave7();
	afx_msg void OnStnClickedMapUwave8();
};
