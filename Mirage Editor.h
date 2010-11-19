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
#include "Dialog_ProgressBar.h"
#include "Message.h"
#include "MidiReceive.h"
#include "MainFrm.h"
#include "Globals.h"
#include "DiskImage.h"
#include <vector>

// CMirageEditorApp:
// See Mirage Editor.cpp for the implementation of this class
//

class CMirageEditorApp : public CWinApp
{
public:
	CMirageEditorApp();
	void GetSamplesList();
	CMainFrame*	GetMainFrame()
	{	return m_pMainFrame; }
	CMultiDocTemplate*	m_pDocTemplate;
	CMultiDocTemplate*	m_pLoopTemplate;
	CMirageEditorDoc*	m_CurrentDoc;
	bool m_AppInit;
	BOOL MidiOldMode;
	CMainFrame*	m_pMainFrame;
	DWORD	m_ThreadId;
	int RepeatCount;
	CDiskImage DiskImage;
// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
  BOOL  AutoDetectMirage();
// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMirageReceivesample();
	afx_msg void OnMiragePreferences();
//	afx_msg LRESULT GetSamplesList(UINT wParam, LONG lParam);

public:
	afx_msg void OnMirageKeymapping();
	afx_msg void OnHelpReportbug();
	afx_msg void OnHelpCheckforupdates();
};

class CDialogThread : public CWinThread
{
	DECLARE_DYNCREATE(CDialogThread)
	CDialogThread() {};
	virtual BOOL InitInstance();
};

extern	CMirageEditorApp	theApp;

extern	CProgressDialog		progress;
extern	CMessage			MessagePopup;
extern	std::vector <unsigned char> LowerSelectList;
extern	std::vector <unsigned char> UpperSelectList;
extern	std::vector <unsigned char> LoadBank;
extern	HANDLE				thread_event;
extern	HANDLE				AudioPlayingEvent;

//extern CMultiDocTemplate* pDocTemplate;
extern const char *MirageReceivedSysex;
extern int MirageBytesRecorded;

#ifdef _DEBUG
extern	FILE *logfile;
#endif
