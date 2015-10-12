// CMidiMonitorThread
#ifndef MIDIMONITORTHREAD_H
#define MIDIMONITORTHREAD_H
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
	void SetMidiDoc(CMidiDoc* pMidiDoc);

	// atributes
public:
	static HANDLE m_EventMonitorThreadKilled;

protected:
	HWND	m_hwndParent;

private:
	CMidiDoc*						m_pMidiDoc;

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	virtual ~CMidiMonitorThread();
//	virtual int Run();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPutData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnParseSysex(WPARAM wParam, LPARAM lParam);
};

#endif /* MIDIMONITORTHREAD_H */