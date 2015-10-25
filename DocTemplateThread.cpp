// MidMonThread.cpp : implementation file
//

#include "stdafx.h"
//#include "MainFrm.h"
#include "Mirage Editor.h"
#include "DocTemplateThread.h"
#include "ThreadNames.h"

// CMultiDocTemplateThread

IMPLEMENT_DYNCREATE(CMultiDocTemplateThread, CWinThread)

CMultiDocTemplateThread::CMultiDocTemplateThread()
{
	m_hTemplateThreadStarted = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);
}

CMultiDocTemplateThread::CMultiDocTemplateThread(HWND hwndParent) : m_hwndParent(hwndParent)
{
	m_hTemplateThreadStarted = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);

}

CMultiDocTemplateThread::~CMultiDocTemplateThread()
{
}

void CMultiDocTemplateThread::SetMDIClass(CRuntimeClass* myRuntimeClass, CMultiDocTemplate* myMultiDocTemplate)
{
	pRuntimeClass = myRuntimeClass;
	pMultiDocTemplate = myMultiDocTemplate;
}

BOOL CMultiDocTemplateThread::InitInstance()
{
	MSG uMsg;
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MirageSampDumpTYPE)) //-V668
	{
		delete pMainFrame;
		return FALSE;
	}
	
	m_pMainWnd = pMainFrame;

	pMultiDocTemplate = (CMultiDocTemplate*)pMultiDocTemplate->OpenDocumentFile(NULL);

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	PeekMessage (&uMsg, NULL, 0, 0, PM_NOREMOVE);

	SetEvent(m_hTemplateThreadStarted);
	return TRUE;
}

int CMultiDocTemplateThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMultiDocTemplateThread, CWinThread)
END_MESSAGE_MAP()


// CMultiDocTemplateThread message handlers

// CDocTemplateThread

IMPLEMENT_DYNCREATE(CDocTemplateThread, CWinThread)

CDocTemplateThread::CDocTemplateThread()
{
	m_hTemplateThreadStarted = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);

}

CDocTemplateThread::CDocTemplateThread(HWND hwndParent) : m_hwndParent(hwndParent)
{
	m_hTemplateThreadStarted = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);

}

CDocTemplateThread::~CDocTemplateThread()
{
}

void CDocTemplateThread::SetMDIClass(CRuntimeClass* myRuntimeClass, CDocTemplate* myDocTemplate)
{
	pRuntimeClass = myRuntimeClass;
	pDocTemplate = myDocTemplate;
}

BOOL CDocTemplateThread::InitInstance()
{
	MSG uMsg;

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MirageSampDumpTYPE)) //-V668
	{
		delete pMainFrame;
		return FALSE;
	}
	
	m_pMainWnd = pMainFrame;

	pDocTemplate = (CDocTemplate*)pDocTemplate->OpenDocumentFile(NULL);

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	PeekMessage (&uMsg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(m_hTemplateThreadStarted);
	return TRUE;
}

int CDocTemplateThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CDocTemplateThread, CWinThread)
END_MESSAGE_MAP()
