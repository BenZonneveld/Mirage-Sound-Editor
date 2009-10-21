// Mirage Editor.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "afxwin.h"

//#include "d3d9.h"
#include <windows.h>
//#include <mmsystem.h>

#include "Mirage Editor.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "Mirage EditorDoc.h"
#include "Mirage EditorView.h"
#include "Preferences.h"
#include "ReceiveSamples.h"
#include "MirageSysex.h"
#include "wavesamples.h"
#include "LoopDialog.h"
#include "Message.h"
//#include "MidiWrapper/MIDIInDevice.h"
//#include "MidiWrapper/MIDIOutDevice.h"

#ifdef _DEBUG
#define new DEBUG_NEW
FILE	*logfile;
#endif

CProgressDialog progress;
CMessage	MessagePopup;

// The MultiDocTemplate
//CMultiDocTemplate* pDocTemplate;

// CMirageEditorApp

BEGIN_MESSAGE_MAP(CMirageEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMirageEditorApp::OnAppAbout)
	// Standard file based document commands
//	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	ON_COMMAND(ID_MIRAGE_RECEIVESAMPLE, &CMirageEditorApp::OnMirageReceivesample)
	ON_COMMAND(ID_MIRAGE_PREFERENCES, &CMirageEditorApp::OnMiragePreferences)
END_MESSAGE_MAP()


// CMirageEditorApp construction

CMirageEditorApp::CMirageEditorApp()
{
	EnableHtmlHelp();
}


// The one and only CMirageEditorApp object

CMirageEditorApp theApp;

// CMirageEditorApp initialization

BOOL CMirageEditorApp::InitInstance()
{
//	CScrollView();
#ifdef _DEBUG
	fopen_s(&logfile,"mirage_midi_in.log","a+");
	
	fprintf(logfile,"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
#endif
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("MirageEditor"));
	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)
	theApp.m_AppInit = true;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	m_pDocTemplate = new CMultiDocTemplate(IDR_MirageSampDumpTYPE,
		RUNTIME_CLASS(CMirageEditorDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CMirageEditorView));

	if (!m_pDocTemplate)
		return FALSE;

//	pDocTemplate->SetContainerInfo(IDR_MirageSampDumpTYPE_CNTR_IP);
//	AddDocTemplate(pDocTemplate);
	m_pDocTemplate->SetContainerInfo(IDR_MirageSampDumpTYPE_CNTR_IP);
	AddDocTemplate(m_pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}

	m_pMainFrame = pMainFrame;
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
//	EnableShellOpen();
//	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	theApp.m_AppInit = false;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CMirageEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMirageEditorApp message handlers


void CMirageEditorApp::OnMirageReceivesample()
{
	if(GetMirageOs())
	{
		if(GetAvailableSamples())
		{
			CReceiveSamples ReceiveDlg;
			ReceiveDlg.DoModal();
		}
	}
}

void CMirageEditorApp::OnMiragePreferences()
{
	CPreferences PreferencesDlg;
	PreferencesDlg.DoModal();
}
