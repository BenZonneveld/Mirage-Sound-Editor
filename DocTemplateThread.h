#pragma once


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
//	void SetMDIClass(CRuntimeClass* myRuntimeClass, CDocTemplate* myDocTemplate);
	void SetMDIClass(CRuntimeClass* myRuntimeClass, CMultiDocTemplate* myMultiDocTemplate);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	CRuntimeClass*				pRuntimeClass;
	CMultiDocTemplate*		pMultiDocTemplate;
	HWND m_hwndParent;
protected:
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
	void SetMDIClass(CRuntimeClass* myRuntimeClass, CDocTemplate* myDocTemplate);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	CRuntimeClass*				pRuntimeClass;
	CDocTemplate*					pDocTemplate;
	HWND m_hwndParent;
protected:
	DECLARE_MESSAGE_MAP()
};

