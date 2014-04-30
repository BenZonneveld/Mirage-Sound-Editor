// Mirage Editor.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "afxwin.h"
#include <winuser.h>
#include <winbase.h>
//#include "d3d9.h"
#include <windows.h>
//#include <mmsystem.h>

#include "Mirage Editor.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "Mirage EditorDoc.h"
#include "Mirage EditorView.h"
#include "Dialog_Preferences.h"
#include "Dialog_ReceiveSamples.h"
#include "MirageSysex.h"
#include "wavesamples.h"
#include "Dialog_LoopEdit.h"
#include "Dialog_OrigKey.h"
#include "Dialog_KeyMapper.h"
#include "SendSysex.h"
#include "UpdateCheck.h"
#include "Dialog_Program.h"
#include "Tab_GeneralKeyboard.h"
#include "Tab_SamplingConfig.h"
#include "Tab_Sequencer.h"
#include "Tab_MidiConfig.h"
#include "Dialog_ConfigParams.h"
#include "Tabby.h"

#include "LongMsg.h"
#include "ShortMsg.h"
#include "midi.h"


//#include "MidiWrapper/MIDIInDevice.h"
//#include "midireceive.h"
#include "MidiWrapper/MIDIOutDevice.h"
#include "Globals.h"
#include "Mirage Sysex_Strings.h"
#include "SysexParser.h"
//#include "GetMidi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
FILE	*logfile;
#endif

CProgressDialog	progress;
//COrigKey		GetOriginalKey;
HANDLE			thread_event;
HANDLE			AudioPlayingEvent;
//HANDLE			midi_in_event;
HANDLE			midi_in_event;

unsigned char	SysXBuffer[SYSEXBUFFER];

std::vector <unsigned char> LowerSelectList;
std::vector <unsigned char> UpperSelectList;
std::vector <unsigned char> LoadBank;

// The MultiDocTemplate
//CMultiDocTemplate* pDocTemplate;

// CMirageEditorApp

BEGIN_MESSAGE_MAP(CMirageEditorApp, CWinApp)
//	ON_MESSAGE(WM_GETSAMPLES, OnProgress)
	ON_COMMAND(ID_APP_ABOUT, &CMirageEditorApp::OnAppAbout)
	// Standard file based document commands
//	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	ON_COMMAND(ID_MIRAGE_RECEIVESAMPLE, &CMirageEditorApp::OnMirageReceivesample)
	ON_COMMAND(ID_MIRAGE_PREFERENCES, &CMirageEditorApp::OnMiragePreferences)
	ON_COMMAND(ID_MIRAGE_KEYMAPPING, &CMirageEditorApp::OnMirageKeymapping)
	ON_COMMAND(ID_HELP_REPORTBUG, &CMirageEditorApp::OnHelpReportbug)
	ON_COMMAND(ID_HELP_CHECKFORUPDATES, &CMirageEditorApp::OnHelpCheckforupdates)
#ifdef _DEBUG
	ON_COMMAND(ID_MIRAGE_PROGRAMSETTINGS, &CMirageEditorApp::OnMirageProgramEdit)
#endif
	ON_COMMAND(ID_MIRAGE_SYSTEMPARAMETERS, &CMirageEditorApp::OnMirageConfigParams)
END_MESSAGE_MAP()


// CMirageEditorApp construction

CMirageEditorApp::CMirageEditorApp()
{
	EnableHtmlHelp();
}


// The one and only CMirageEditorApp object

CMirageEditorApp theApp;

// CMirageEditorApp initialization
int CMirageEditorApp::ExitInstance()
{
	CWinApp::ExitInstance();
//	SetEvent(midi_in_event);
	m_InDevice.StopRecording();
	m_InDevice.Close();
	m_OutDevice.Close();

	return 0;
}

BOOL CMirageEditorApp::InitInstance()
{
	HACCEL m_haccel;

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

	m_haccel = LoadAccelerators(AfxGetInstanceHandle(),
								MAKEINTRESOURCE(IDR_MAINFRAME));

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	// Standard initialization
	SetRegistryKey(_T("Synthforum"));
	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	theApp.m_AppInit = true;
	StartMidiInput();
	StartMidiOutput();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	m_pDocTemplate = new CMultiDocTemplate(IDR_MirageSampDumpTYPE,
		RUNTIME_CLASS(CMirageEditorDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CMirageEditorView));

		if (!m_pDocTemplate)
			return FALSE;
	
	m_pDiskImageTemplate = new CMultiDocTemplate(IDR_DiskImageType,
		RUNTIME_CLASS(CMirageEditorDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CMirageEditorView));

	if (!m_pDiskImageTemplate)
		return FALSE;

	m_pDocTemplate->SetContainerInfo(IDR_MirageSampDumpTYPE);
	m_pDiskImageTemplate->SetContainerInfo(IDR_DiskImageType);

	AddDocTemplate(m_pDocTemplate);
	AddDocTemplate(m_pDiskImageTemplate);

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
//	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;

  // Do an autodetect for the Mirage with Masos Booted
  //AutoDetectMirage();
	// The main window has been initialized, so show and update it
	theApp.m_AppInit = false;
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	// Check for updates
	if ( theApp.GetProfileIntA("Settings","AutoCheckForUpdates",true) == 1 )
	{
		OnHelpCheckforupdates();
	}

	AudioPlayingEvent = CreateEvent(
						NULL,               // default security attributes
						TRUE,               // manual-reset event
						FALSE,              // initial state is nonsignaled
						FALSE);

	return TRUE;
}

void CMirageEditorApp::StartMidiOutput()
{
	m_OutDevice.Open(m_OutDevice.GetIDFromName(theApp.GetProfileStringA("Settings","OutPort","not connected"))-1);
}

void CMirageEditorApp::StartMidiInput()
{
	midi_in_event = CreateEvent(NULL,               // default security attributes
															TRUE,               // manual-reset event
															FALSE,              // initial state is nonsignaled
															FALSE);
  // If there are any MIDI input devices available, open one and begin
  // recording.
  if(midi::CMIDIInDevice::GetNumDevs() == 0)
	{
    MessageBox(NULL,"No MIDI input devices available.", "Warning", 
		MB_ICONWARNING | MB_OK);
	} else {
		if (m_InDevice.GetIDFromName(theApp.GetProfileStringA("Settings","InPort","not connected")) > 0 )
		{
			m_InDevice.SetReceiver(*this);
			m_InDevice.Open(theApp.m_InDevice.GetIDFromName(theApp.GetProfileStringA("Settings","InPort","not connected"))-1);
			m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
			// Start receiving MIDI events
			m_InDevice.StartRecording();
		}
	}
}

void CMirageEditorApp::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
	midi::CShortMsg ShortMsg(Msg,TimeStamp);

	unsigned char Command = ShortMsg.GetCommand();
	if ( Command == midi::NOTE_OFF || 
				(Command == midi::NOTE_ON && ShortMsg.GetData2() == 0) )
	{
		m_LastNote = ShortMsg.GetData1();
	}
	SetEvent(midi_in_event);
}

void CMirageEditorApp::ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	midi::CLongMsg LongMsg(Msg, BytesRecorded);

	ParseSysEx((unsigned char *)LongMsg.GetMsg(),LongMsg.GetLength());
	memset((void *)LongMsg.GetMsg(),0,LongMsg.GetLength());

	m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	SetEvent(midi_in_event);
//  Why did I do this?
	Sleep(1);
	ResetEvent(midi_in_event);
}

void CMirageEditorApp::OnError(DWORD Msg, DWORD TimeStamp)
{
	return;
}

// For long messages:
void CMirageEditorApp::OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{

	m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	return;
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
			CReceiveSamples ReceiveDlg;
			if ( ReceiveDlg.DoModal() == IDOK )
			{
				GetSamplesList();
			}
//		}
//	}
	return;
}

void CMirageEditorApp::OnMiragePreferences()
{
	CPreferences PreferencesDlg;
	PreferencesDlg.DoModal();
}

void CMirageEditorApp::OnMirageKeymapping()
{
	CKeyMapper KeyMapperDlg;
	KeyMapperDlg.DoModal();
}


void CMirageEditorApp::GetSamplesList()
{
	char *sysexconstruct = NULL;
	int i;

	/* Construct the select sample front pannel command */
	unsigned char SelectSample[]={7,
																MirID[0],
																MirID[1],
																MirID[2],
																0x01, // Commando Code
																0x15, // Lower Sample Select
																0x7F,
																0xF7}; // Lower sample select
	for ( i = 0 ; i < LowerSelectList.size(); i++ )
	{
		GetSample(SelectSample, LowerSelectList[i]);
	}

	// Construct the select sample front pannel command 
	SelectSample[5]=0x14; // Upper Sample Select

	for ( i = 0 ; i < UpperSelectList.size(); i++ )
	{
		GetSample(SelectSample,i);
	}
	LowerSelectList.clear();
	UpperSelectList.clear();
}

void CMirageEditorApp::OnHelpReportbug()
{
	SHELLEXECUTEINFO sei;
	::ZeroMemory(&sei,sizeof(SHELLEXECUTEINFO));

	sei.cbSize = sizeof( SHELLEXECUTEINFO );

	sei.lpVerb = TEXT( "open"); // Set Verb

	sei.lpFile = _T(MANTIS_URL); // Target to open

	sei.nShow = SW_NORMAL; // Show Normal

	ShellExecuteEx(&sei);
}

void CMirageEditorApp::OnHelpCheckforupdates()
{
	CUpdateCheck checkUpdate;
	checkUpdate.Check(IDS_UPDATE);
}

void CMirageEditorApp::OnMirageProgramEdit()
{
	CMirProgram MiragePgmDlg;

	MiragePgmDlg.DoModal();
}

void CMirageEditorApp::OnMirageConfigParams()
{
	CConfigParams ConfigParmsDlg;

	ConfigParmsDlg.DoModal();
}