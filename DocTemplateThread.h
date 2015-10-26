#pragma once
#ifndef DOCTEMPLATETHREAD_H
#define DOCTEMPLATETHREAD_H

#define IDC_MONITOR_WND 1

class CMyMDIChildWnd:  public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMyMDIChildWnd)

private:
	using CMDIChildWnd::Create;

public:
	CMyMDIChildWnd();
	BOOL Create(LPCTSTR szTitle, LONG style = 0,
			const RECT& rect = rectDefault,
			CMDIFrameWnd* pParent = NULL);
// Attributes
public:
//	virtual BOOL DestroyWindow();
// Operations
public:

// Overrides

protected:
	static CMenu	NEAR	menu;

protected:
	DECLARE_MESSAGE_MAP()
};

class CMyWnd : public CWnd
{
private:
	using CWnd::Create;

	DECLARE_DYNCREATE(CMyWnd);

public:
	CMyWnd();
	BOOL Create(LPCTSTR szTitle, LONG style, const RECT& rect, CWnd* pParent);

protected:

	DECLARE_MESSAGE_MAP()
};

// CMultiDocTemplateThread

class CMultiDocTemplateThread : public CWinThread
{
	DECLARE_DYNCREATE(CMultiDocTemplateThread)

protected:
	CMultiDocTemplateThread();           // protected constructor used by dynamic creation
	virtual ~CMultiDocTemplateThread();

public:
	HANDLE m_hTemplateThreadStarted;
	CMultiDocTemplateThread(HWND hwndParent);
	CMultiDocTemplate* GetMultiDocTemplate() { return pMultiDocTemplate; }
	void SetTitle(LPCTSTR szTitle) { m_szTitle = szTitle; }
//	void SetRect(const RECT& rect) { m_Rect = &rect; }
	void SetMDIClass(CRuntimeClass* myRuntimeClass, CMultiDocTemplate* myMultiDocTemplate);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	const RECT					m_Rect;
	CRuntimeClass*			pRuntimeClass;
	CMultiDocTemplate*	pMultiDocTemplate;
	LPCTSTR							m_szTitle;
	CWnd							m_wndThread;
	HWND								m_hwndParent;
protected:
	afx_msg void OnPutData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnParseSysex(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

class CDocTemplateThread : public CWinThread
{
	DECLARE_DYNCREATE(CDocTemplateThread)

protected:
	CDocTemplateThread();           // protected constructor used by dynamic creation
	virtual ~CDocTemplateThread();

public:
	HANDLE	m_hTemplateThreadStarted;
	CDocTemplateThread(HWND hwndParent);
	void SetTitle(LPCTSTR szTitle) { m_szTitle = szTitle; }
	void SetRect(const RECT& rect) { m_Rect = &rect; }
	void SetMDIClass(CRuntimeClass* myRuntimeClass, CDocTemplate* myDocTemplate);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	const RECT*				m_Rect;
	CRuntimeClass*		pRuntimeClass;
	CDocTemplate*			pDocTemplate;
	LPCTSTR						m_szTitle;
	HWND							m_hwndParent;
protected:
//	afx_msg void OnPutData(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif /* DOCTEMPLATETHREAD_H */
