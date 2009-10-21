// Mirage Editor.h : main header file for the Mirage Editor application
// $Id: Mirage\040Editor.h,v 1.9 2008/05/05 16:13:25 root Exp $
#pragma once

//#include "d3d9.h"

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
//#include <mmsystem.h>
#include "Mirage EditorDoc.h"
#include "Mirage EditorView.h"
#include "ProgressBar.h"
#include "Message.h"
#include "MidiReceive.h"
#include "MainFrm.h"

// CMirageEditorApp:
// See Mirage Editor.cpp for the implementation of this class
//

class CMirageEditorApp : public CWinApp
{
public:
	CMirageEditorApp();
	CMainFrame*	GetMainFrame()
	{	return m_pMainFrame; }
	CMultiDocTemplate*	m_pDocTemplate;
	CMultiDocTemplate*	m_pLoopTemplate;
	CMirageEditorDoc*	m_CurrentDoc;
	bool m_AppInit;
	CMainFrame*	m_pMainFrame;
// Overrides
public:
	virtual BOOL InitInstance();

protected:

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMirageReceivesample();
	afx_msg void OnMiragePreferences();
};

extern CMirageEditorApp theApp;

extern CProgressDialog progress;
extern CMessage		MessagePopup;
//extern CMultiDocTemplate* pDocTemplate;

#ifdef _DEBUG
extern FILE *logfile;
#endif