// CMidiMonitorThread
#ifndef MIDIMONITORTHREAD_H
#define MIDIMONITORTHREAD_H

#include "Midi Doc.h"
#include "Midi View.h"

class CMidiMonitorThread : public CWinThread
{
	DECLARE_DYNCREATE(CMidiMonitorThread)
protected:
	CMidiMonitorThread(); // protected constructor used by dynamic creation
public:
	CMidiMonitorThread(HWND hwndParent);
	void operator delete(void* p);
// methods
public:
	void SetHandle(HWND hwnd);
//	void SetMidiDoc(CMidiDoc* pMidiDoc);
	void ThreadMessage(UINT message, WPARAM wParam, LPARAM lParam);
	// atributes
public:
	static HANDLE m_EventMonitorThreadKilled;
	HANDLE	MessageFlag;
//	void SetMaxQue(int size) { theApp.m_pMidiDoc->SetMaxQue(size); }
//	void EnableMidiMonitor();

protected:
	HWND	m_hwndParent;
//	CBounceWnd m_wndBounce;
// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	CMidiDoc*						m_pMidiDoc;
	CMultiDocTemplate*	m_pMidiDocTemplate;
	WPARAM	m_wParam;
	LPARAM	m_lParam;
	UINT		m_message;
	virtual ~CMidiMonitorThread();
	virtual int Run();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPutData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnParseSysex(WPARAM wParam, LPARAM lParam);
};

#endif /* MIDIMONITORTHREAD_H */