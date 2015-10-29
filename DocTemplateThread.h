#pragma once
#ifndef DOCTEMPLATETHREAD_H
#define DOCTEMPLATETHREAD_H

#include "MyMDIChildWnd.h"

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
	void SetMDIClass(CRuntimeClass* myRuntimeClass, CMultiDocTemplate* myMultiDocTemplate, UINT nResource);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	CRuntimeClass*			pRuntimeClass;
	CMultiDocTemplate*	pMultiDocTemplate;
	LPCTSTR							m_szTitle;
	CMyWnd							m_wndMultiDocTemplate;
	HWND								m_hwndParent;
	UINT								m_nResource;
	// own document manager
	CDocManager* m_pDocManager;
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
	CDocTemplate* GetDocTemplate() { return pDocTemplate; }
	void SetTitle(LPCTSTR szTitle) { m_szTitle = szTitle; }
	void SetMDIClass(CRuntimeClass* myRuntimeClass, CDocTemplate* myDocTemplate, UINT nResource);
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	CRuntimeClass*		pRuntimeClass;
	CDocTemplate*			pDocTemplate;
	LPCTSTR						m_szTitle;
	HWND							m_hwndParent;
	UINT							m_nResource;
protected:
	afx_msg void OnPutData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnParseSysex(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//CMainFrame* StartMainframe();

#endif /* DOCTEMPLATETHREAD_H */
