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
	DECLARE_DYNCREATE(CMidiMonChildWnd)
private:
	using CMDIChildWnd::Create;
	
public:
	CMidiMonChildWnd();

	CWnd* CreateView(CCreateContext* pContext, UINT nID);
	BOOL OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext);
	virtual BOOL DestroyWindow();
/*	BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
		CWnd* pParentWnd, CCreateContext* pContext);*/
protected:
	afx_msg LRESULT OnPrepareToClose(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT OnPutData(WPARAM wParam, LPARAM lParam);

	DWORD MonThreadID;
	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pMidiMonWnd;
};

#endif
