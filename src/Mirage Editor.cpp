// Mirage Editor.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "afxwin.h"
#include <winuser.h>
#include <winbase.h>
#include <windows.h>
#include <string>

#include "ThreadNames.h"
#include "Mirage Editor.h"
#include "Gui/MainFrm.h"

#include "WaveApi/Wave Doc.h"

#include "WaveApi/wavesamples.h"

#include "Dialogs/Dialog_Preferences.h"
#include "Dialogs/Dialog_ReceiveSamples.h"
#include "Dialogs/Dialog_LoopEdit.h"
#include "Dialogs/Dialog_OrigKey.h"
#include "Dialogs/Dialog_KeyMapper.h"
#include "Dialogs/Dialog_Program.h"

#include "Gui/Tab_GeneralKeyboard.h"
#include "Gui/Tab_SamplingConfig.h"
#include "Gui/Tab_Sequencer.h"
#include "Gui/Tab_MidiConfig.h"
#include "Dialogs/Dialog_ConfigParams.h"
#include "Gui/Tabby.h"

#include "Midi/SendSysex.h"
#include "UpdateCheck.h"
#include "Midi/MidiWrapper/LongMsg.h"
#include "Midi/MidiWrapper/ShortMsg.h"
#include "Midi/MidiWrapper/midi.h"
#include "Midi/Nybble.h"

#include "Midi/MidiWrapper/MIDIOutDevice.h"
#include "Globals.h"

#include "Midi/Monitor/MidiMon.h"
#include "Midi/Monitor/Midi Doc.h"
#include "Midi/Monitor/Midi View.h"
//#include "MTFrameWnd.h"

#include "Midi/MirageSysex.h"
#include "Midi/Mirage Sysex_Strings.h"
#include "Midi/SysexParser.h"
#include "Midi/Monitor/sysex_logger.h"
CProgressDialog	progress;
HANDLE			thread_event;
HANDLE			AudioPlayingEvent;
HANDLE			midi_in_event;
HANDLE			midi_monitor_started;


unsigned char	SysXBuffer[SYSEXBUFFER];

using std::string;

//std::vector <unsigned char> LowerSelectList;
//std::vector <unsigned char> UpperSelectList;
//std::vector <unsigned char> LoadBank;

// CMirageEditorApp

BEGIN_MESSAGE_MAP(CMirageEditorApp, CWinApp)
	ON_THREAD_MESSAGE( WM_GETSAMPLES, OnGetSamplesList )
	ON_THREAD_MESSAGE( WM_WAVESAMPLERECEIVED, OnGotWaveData)
	ON_COMMAND(ID_APP_ABOUT, &CMirageEditorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	ON_COMMAND(ID_MIRAGE_RECEIVESAMPLE, /*&CMirageEditorApp::*/OnMirageReceivesample)
	ON_COMMAND(ID_MIRAGE_PREFERENCES, &CMirageEditorApp::OnMiragePreferences)
	ON_COMMAND(ID_MIRAGE_KEYMAPPING, &CMirageEditorApp::OnMirageKeymapping)
	ON_COMMAND(ID_HELP_REPORTBUG, &CMirageEditorApp::OnHelpReportbug)
	ON_COMMAND(ID_HELP_CHECKFORUPDATES, &CMirageEditorApp::OnHelpCheckforupdates)
#ifdef _DEBUG
	ON_COMMAND(ID_MIRAGE_PROGRAMSETTINGS, &CMirageEditorApp::OnMirageProgramEdit)
#endif
	ON_COMMAND(ID_MIRAGE_SYSTEMPARAMETERS, &CMirageEditorApp::OnMirageConfigParams)
	ON_COMMAND(ID_WINDOW_MIDIMONITOR, MidiMonitor)
	ON_THREAD_MESSAGE(ID_WINDOW_MIDIMONITOR, MidiMonitorFromView)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_MIDIMONITOR, OnUpdateMidiMonitor)

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

	m_MidiMonitorVisibility = true;
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	//  
	//  Sample dump waveform window
	m_pDocTemplate = new CMultiDocTemplate(IDR_MirageSampDumpTYPE,
		RUNTIME_CLASS(CMirageEditorDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CMirageEditorView));

	// Disk images
	m_pDiskImageTemplate = new CMultiDocTemplate(IDR_DiskImageType,
		RUNTIME_CLASS(CMirageEditorDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CMirageEditorView));

	m_pDocTemplate->SetContainerInfo(IDR_MirageSampDumpTYPE);
	m_pDiskImageTemplate->SetContainerInfo(IDR_DiskImageType);

	AddDocTemplate(m_pDocTemplate);
	AddDocTemplate(m_pDiskImageTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME)) //-V668
	{
		delete pMainFrame;
		return FALSE;
	}

	m_pMainFrame = pMainFrame;
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainFrame->DragAcceptFiles();
	theApp.m_AppInit = false;

	// The main window has been initialized, so show and update it
	m_pMainFrame->ShowWindow(m_nCmdShow);
	m_pMainFrame->UpdateWindow();

	// Do an autodetect for the Mirage with Masos Booted
  //AutoDetectMirage();

	// Midi Monitor

	MidiMonitorView();	

	InitDialogs();

//	WaitForSingleObject(midi_monitor_started,INFINITE);
	// Check for updates
	if ( theApp.GetProfileIntA("Settings","AutoCheckForUpdates",1) == 1 )
	{
		OnHelpCheckforupdates();
	}

	StartMidiInput();
	StartMidiOutput();

	AudioPlayingEvent = CreateEvent(
						NULL,               // default security attributes
						TRUE,               // manual-reset event
						FALSE,              // initial state is nonsignaled
						FALSE);

	return TRUE;
}

void CMirageEditorApp::InitDialogs()
{
	m_ReceiveDlg = NULL;
	m_ReceiveDlg = new CReceiveSamples(theApp.GetMainWnd());
	m_ReceiveDlg->Create(CReceiveSamples::IDD, theApp.GetMainWnd());
}

void CMirageEditorApp::MidiMonitorView()
{ //-V668
	// Midi monitor window
	m_pMidiMonitor = new CMultiDocTemplate(IDR_MirageSampDumpTYPE,
											RUNTIME_CLASS(CMidiDoc),
											RUNTIME_CLASS(CMidiMonChildWnd),
											RUNTIME_CLASS(CMidiView));
//	AddDocTemplate(m_pMidiMonitor);

	CMidiDoc* pMidiDoc = new CMidiDoc;
	pMidiDoc->SetTitle(_T("Midi Monitor"));

	m_pMidiMonFrame = (CFrameWnd*)(m_pMidiMonitor->CreateNewFrame(pMidiDoc, NULL));
	m_pMidiMonFrame->ShowWindow(SW_SHOW);
	Sleep(250);

//	m_pMidiMonFrame->ShowWindow(SW_MINIMIZE);
//	m_pMidiMonFrame->ShowWindow(SW_RESTORE);

}

void CMirageEditorApp::PostMidiMonitor(string Data, BOOL IO_Dir)
{
	m_midimonitorstring = Data;

	COPYDATASTRUCT* mycds = (COPYDATASTRUCT*)LocalAlloc(LMEM_FIXED,sizeof(COPYDATASTRUCT));
	mycds->dwData = IO_Dir;
	mycds->cbData = sizeof(TCHAR) * m_midimonitorstring.length();
	mycds->lpData =  (LPVOID)m_midimonitorstring.data();
	m_pMidiMonThread->PostThreadMessage(WM_MM_PUTDATA, NULL, (LPARAM)mycds);
}

void CMirageEditorApp::StartMidiOutput()
{
	m_OutDevice.Open(m_OutDevice.GetIDFromName(theApp.GetProfileStringA("Settings","OutPort","not connected"))-1);
	PostMidiMonitor(string("Ready to transmit data"), MIDIMON_OUT);
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
		PostMidiMonitor(string("Ready to receive data"), MIDIMON_IN);
	}
}

void CMirageEditorApp::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
	char logmsg[64];
	char *CommandName[]={"Note Off",
											"Note On",
											"Poly Pressure",
											"Ctrl Change",
											"Prg Change",
											"Aftertouch",
											"Pitch Bend"};

	midi::CShortMsg ShortMsg(Msg,TimeStamp);

	unsigned char Command = ShortMsg.GetCommand();
	if ( Command == midi::NOTE_OFF || 
				(Command == midi::NOTE_ON && ShortMsg.GetData2() == 0) )
	{
		m_LastNote = ShortMsg.GetData1();
	}

	if ( Command < 0xF0 )
	{
		sprintf(logmsg,"%s Data1: %02X Data2: %02X",CommandName[(Command>>4)&0x7], ShortMsg.GetData1(), ShortMsg.GetData2());
		PostMidiMonitor(logmsg, MIDIMON_IN);
	} else {
		int i=1;
	}

	SetEvent(midi_in_event);
}

// Sysex Data
void CMirageEditorApp::ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp) 
{
// Why is this one running in midi::CMIDIInDevice::MidiInProc

	midi::CLongMsg LongMsg(Msg, BytesRecorded);
	char end_of_sysex = (char)LongMsg.GetMsg()[BytesRecorded-1];
	char start_of_data = (char)LongMsg.GetMsg()[0];
	if ( start_of_data == (char)0xF0 ) // Start of sysex
	{
		// start filling std::string m_sysex_buffer
		m_sysex_buffer.assign(LongMsg.GetMsg(), (size_t)BytesRecorded);
	} else {
		// append to std::string m_sysex_buffer
		m_sysex_buffer.append(LongMsg.GetMsg(), (size_t)BytesRecorded);
	}

	// TODO: Progress bar
	if ( progress )
	{
//		progress.Bar.StepIt();
	}

	if ( end_of_sysex == (char)0xF7 )
	{
		// Parse the sysex from std:string m_sysex_buffer
		ParseSysEx((unsigned char*)m_sysex_buffer.data(), (DWORD)m_sysex_buffer.size());
		sysex_logmsg((unsigned char*)m_sysex_buffer.data(), (DWORD)m_sysex_buffer.size(), MIDIMON_IN);
		// This one might be the one deleting the data...no guarantee the thread has done it's job.
		m_sysex_buffer.clear();
		SetEvent(midi_in_event);
	}
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
//	m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
	
	memset((void *)LongMsg.GetMsg(),0,LongMsg.GetLength());
}

void CMirageEditorApp::OnError(DWORD Msg, DWORD TimeStamp)
{
	return;
}

// For long messages:
void CMirageEditorApp::OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));
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

// CMirageEditorApp message handlers

void CMirageEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CMirageEditorApp::OnMirageReceivesample()
{
	m_ReceiveDlg->UpdateSampleData();
	m_ReceiveDlg->ShowWindow(SW_SHOW);

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

void CMirageEditorApp::OnGetSamplesList(WPARAM wParam, LPARAM lParam)
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
	for ( i = 0 ; i < m_LowerSelectList.size(); i++ )
	{
		GetSample(SelectSample, m_LowerSelectList[i]);
	}

	// Construct the select sample front pannel command 
	SelectSample[5]=0x14; // Upper Sample Select

	for ( i = 0 ; i < m_UpperSelectList.size(); i++ )
	{
		GetSample(SelectSample,m_UpperSelectList[i]);
	}
	m_LowerSelectList.clear();
	m_UpperSelectList.clear();
}

void CMirageEditorApp::OnGotWaveData(WPARAM wParam, LPARAM lParam)
{
	unsigned char	sysex_byte;
	unsigned char	*sysex_ptr = NULL;
	int byte_counter = 0;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	unsigned char* MyLongMessage = (unsigned char*)LocalAlloc(LMEM_FIXED,pcds->cbData);
	memcpy(MyLongMessage, pcds->lpData, pcds->cbData);

//	LocalFree(LongMessage);

	DWORD BytesRecorded = pcds->cbData;
	sysex_ptr = ((unsigned char *)&WaveSample.SampleData);
	memset(sysex_ptr,0, sizeof(WaveSample.SampleData));
	unsigned char* ptr = MyLongMessage; 
	if ( *(ptr) == 0xF0 )
	{
		ptr += 4; /* First 4 bytes are the sysex header */
		/* Next two bytes are the pagecount */
		WaveSample.samplepages = de_nybblify(*(ptr),*(ptr+1));
		byte_counter += 6;
		ptr += 2;
	}

	DWORD MaxCount = ((BytesRecorded - 8)/2);
	while ( byte_counter < MaxCount )
	{
		/* Reconstruct the byte from the nybbles and copy it to the correct structure*/
		sysex_byte = de_nybblify(*(ptr),*(ptr+1));
		memcpy(sysex_ptr, &sysex_byte,1);
		sysex_ptr++;
		ptr += 2;
		byte_counter++;
	}

	WaveSample.checksum = (unsigned char)*(ptr - 1);
	
	theApp.m_smiragesysex.clear();

	GotSample();
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

void CMirageEditorApp::MidiMonitorFromView(WPARAM, LPARAM)
{
	MidiMonitor();
}

void CMirageEditorApp::MidiMonitor()
{
	m_MidiMonitorVisibility = !m_MidiMonitorVisibility;

	ASSERT_VALID(m_pMidiMonFrame);
	ASSERT(::IsWindow(m_pMidiMonFrame->m_hWnd));

  if (m_pMidiMonFrame != NULL)
  {
		switch (m_MidiMonitorVisibility)
		{
			case true:
									m_pMidiMonFrame->ShowWindow(SW_SHOW);
									break;
			case false:
									m_pMidiMonFrame->ShowWindow(SW_HIDE);
									break;
		}
  }
}

void CMirageEditorApp::OnUpdateMidiMonitor(CCmdUI *pCmdUI)
{
	CWnd * pMainWindow = AfxGetMainWnd();
	CMenu * pTopLevelMenu = pMainWindow->GetMenu();

	CMenu * pType = pTopLevelMenu->GetSubMenu(5);
	pType->CheckMenuItem(ID_WINDOW_MIDIMONITOR,theApp.m_MidiMonitorVisibility ? MF_CHECKED:MF_UNCHECKED);
}