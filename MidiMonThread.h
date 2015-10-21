#ifndef MIDIMONTHREAD_H
#define MIDIMONTHREAD_H

class CMidiMonThread : public CWinThread
{
	DECLARE_DYNCREATE(CMidiMonThread)

protected:
	CMidiMonThread(); // protected constructor used by dynamic creation
public:
	CMidiMonThread(HWND hwndParent);
	void operator delete(void* p);

// Attributes
public:
	static HANDLE m_hEventMidiMonThreadKilled;

protected:
	HWND				m_hwndParent;
	CMidiMonWnd	m_wndMidiMon;

// Operations
public:

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
protected:
	virtual ~CMidiMonThread();

	DECLARE_MESSAGE_MAP()
};

#endif /* MIDIMONTHREAD_H */
