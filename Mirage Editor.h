// Mirage Editor.h : main header file for the Mirage Editor application
// $Id: Mirage\040Editor.h,v 1.9 2008/05/05 16:13:25 root Exp $
#pragma once

#ifndef MIRAGE_EDITOR_H
#define MIRAGE_EDITOR_H
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "Wave Doc.h"
#include "Wave View.h"
#include "Dialog_ProgressBar.h"
#include "Dialog_OrigKey.h"
#include "MainFrm.h"
#include "Globals.h"
#include "DiskImage.h"
#include "MIDIInDevice.h"
#include "MIDIOutDevice.h"
#include "MidiMonThread.h"
#include "LongMsg.h"
#include "ShortMsg.h"

// For Midi Monitor
#include "MidiMonThread.h"

#include "Dialog_ReceiveSamples.h"
#include <vector>
#include <string>

// CMirageEditorApp:
// See Mirage Editor.cpp for the implementation of this class
//
using std::vector;
using std::string;

#define MIDIMON_OUT true
#define MIDIMON_IN false

class CMirageEditorApp : public CWinApp, public midi::CMIDIReceiver
{
public:
	void CMirageEditorApp::StartMidiInput();
	void CMirageEditorApp::StartMidiOutput();

	// Receive show messages (we ignore these for sample reception
	void ReceiveMsg(DWORD Msg, DWORD TimeStamp);

	// Receive Long messages
	void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);

	// Called when an invalid short message is received
	void OnError(DWORD Msg, DWORD TimeStamp);

	// Called when an invalid long message is received
	void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp);

	CMirageEditorApp();
	HWND GethWnd()
	{ return m_pMainFrame->GetSafeHwnd(); }
	CMainFrame*	GetMainFrame()
	{	return m_pMainFrame; }

	CMultiDocTemplate*	m_pDocTemplate;
	CMultiDocTemplate*	m_pDiskImageTemplate;
	CMirageEditorDoc*		m_CurrentDoc;

	bool m_AppInit;
	CMainFrame*	m_pMainFrame;
	DWORD	m_ThreadId;

	COPYDATASTRUCT cds;
	// For the Midi Monitor
	void PostMidiMonitor(string Data, BOOL IO_Dir);
	std::string m_midimonitorstring;
	CMidiMonThread*			m_pMidiMonThread;
	CMultiDocTemplate*	m_pMidiMonitor;
	BOOL m_MidiMonitorVisibility;
	CFrameWnd* m_pMidiMonFrame;
	HANDLE				midi_monitor_started;

	int RepeatCount; // For Multiple Copy function
	// For Future Diskimage handling
	CDiskImage DiskImage;
	std::vector <unsigned char> m_LoadBank;

	// Dialogs
	CReceiveSamples *m_ReceiveDlg;
	std::vector <unsigned char> m_UpperSelectList;
	std::vector <unsigned char> m_LowerSelectList;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
protected:
//	std::vector <char> m_sysex_buffer; 
	std::string m_sysex_buffer;
	void	InitDialogs();
	void  MidiMonitorView();
  BOOL  AutoDetectMirage();
// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMirageReceivesample();
	afx_msg void OnMiragePreferences();
	afx_msg void MidiMonitor();
	afx_msg void OnGetSamplesList(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGotWaveData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateMidiMonitor(CCmdUI *pCmdUI);							

public:
	midi::CMIDIInDevice m_InDevice;
	midi::CMIDIOutDevice m_OutDevice;
	unsigned char m_LastNote;
	unsigned char m_WavesampleStatus;
	afx_msg void OnMirageKeymapping();
	afx_msg void OnHelpReportbug();
	afx_msg void OnHelpCheckforupdates();
	afx_msg void OnMirageProgramEdit();
	afx_msg void OnMirageConfigParams();
};

class CDialogThread : public CWinThread
{
	DECLARE_DYNCREATE(CDialogThread)
	CDialogThread() {};
	virtual BOOL InitInstance();
};

extern	CMirageEditorApp	theApp;

extern	CProgressDialog		progress;
//extern	std::vector <unsigned char> LowerSelectList;
//extern	std::vector <unsigned char> UpperSelectList;
//extern	std::vector <unsigned char> LoadBank;
extern	HANDLE				thread_event;
extern	HANDLE				AudioPlayingEvent;
extern	HANDLE				midi_in_event;

#endif /* MIRAGE_EDITOR_H */