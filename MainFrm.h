// MainFrm.h : interface of the CMainFrame class
//


#pragma once

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void SetPages(int Pages);
	void SetSampleRate(long Rate);
	void SetGenericMessage(CString Message);
	CString GetGenericMessage()
		{ return m_GenericMessage; }
	void SetPitch(double pitch);
	// Implementation
public:
	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CToolBar			m_wndToolBar;
	CToolBar			m_wndSampleToolBar;
	CReBar				m_wndReBar;
	CDialogBar		m_wndDlgBar;
	unsigned char	m_nPages;
	long					m_rate;
	double				m_Pitch;
	CString				m_GenericMessage;
// Generated message map functions
protected:
	afx_msg LRESULT OnProgress(UINT wParam, LONG lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdatePage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGeneric(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePitch(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
};
