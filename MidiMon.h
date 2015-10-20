#ifndef MIDIMONCHILDWINDOW_H
#define MIDIMONCHILDWINDOW_H
#include "stdafx.h"
#include "Midi Doc.h"
#include "MidiMon.h"

#define IDC_MIDIMON_WND 1

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
protected:
	afx_msg LRESULT OnPrepareToClose(WPARAM wParam = 0, LPARAM lParam = 0);
	DECLARE_MESSAGE_MAP()
};

class CMidiMonWnd : public CWnd
{
private:
	using CWnd::Create;

	DECLARE_DYNAMIC(CMidiMonWnd)

public:
	CMidiMonWnd();
	BOOL Create(LPCTSTR szTitle, LONG style, const RECT& rect, CWnd* pParent);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMidiMonWnd)
	//}}AFX_VIRTUAL

// Implementation
protected:
	CMidiDoc *m_pMidiDoc;

	afx_msg LRESULT OnPutData(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#endif
