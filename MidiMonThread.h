#ifndef MIDIMONTHREAD_H
#define MIDIMONTHREAD_H

#include "Midi View.h"

class CMidiMonThread : public CWinThread
{
	DECLARE_DYNCREATE(CMidiMonThread)

protected:
	CMidiMonThread(); // protected constructor used by dynamic creation
public:
	CMidiMonThread(HWND hwndParent);
	void operator delete(void* p);
	void SetID(UINT nID) { m_nID = nID; }
	void SetCreateContext(CCreateContext* pContext);
	CCreateContext* GetCreateContext() { return m_pContext; }
// Attributes
public:
	static HANDLE m_hEventMidiMonThreadKilled;


protected:
	HWND								m_hwndParent;
	CMidiDoc*						m_pMidiDoc;
	CMultiDocTemplate*	m_pMidiMonitor;
//	CMidiMonWnd	m_wndMidiMon;
//	CMidiView		m_wndMidiView;
// Operations
public:

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
protected:
	virtual ~CMidiMonThread();
	CCreateContext	m_Context;
	CCreateContext* m_pContext;
	UINT m_nID;
	DECLARE_MESSAGE_MAP()
};

#endif /* MIDIMONTHREAD_H */
