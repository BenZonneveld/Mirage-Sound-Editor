#ifndef MIDIMONCHILDWINDOW_H
#define MIDIMONCHILDWINDOW_H
#include "stdafx.h"
#include "Midi Doc.h"
#include "MidiMon.h"

#define IDC_MIDIMON_WND 1

struct COnCmdMsg
{
	int m_nID;
	int m_nCode;
	void* m_pExtra;
	AFX_CMDHANDLERINFO* m_pHandlerInfo;
};

class CMidiMonChildWnd : public CMDIChildWnd
{
private:
	using CMDIChildWnd::Create;
	
public:
	CMidiMonChildWnd();
	BOOL Create(LPCTSTR szTitle, LONG style = 0,
					const RECT& rect = rectDefault,
					CMDIFrameWnd* pParent = NULL);
	virtual BOOL DestroyWindow();
	DWORD GetChildThreadId()
		{ return MonThreadID; }
	CWnd* GetMidiMonWnd();
protected:
//	static CMenu NEAR menu;     // menu for all MIDIMON windows

	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg LRESULT OnPrepareToClose(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT OnPutData(WPARAM wParam, LPARAM lParam);

	DWORD MonThreadID;
	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pMidiMonWnd;
};

class CMidiMonWnd : public CWnd
{
private:
	using CWnd::Create;

	DECLARE_DYNAMIC(CMidiMonWnd)

public:
	CMidiMonWnd();
	BOOL Create(LPCTSTR szTitle, LONG style, const RECT& rect, CWnd* pParent);
	CMidiDoc* GetMidiDoc()
		{ return m_pMidiDoc; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiMonWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMidiDoc *m_pMidiDoc;

	virtual void OnDraw(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnPutData(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	CFont m_ftTimes;
	int m_nLineHt;
};

#endif
