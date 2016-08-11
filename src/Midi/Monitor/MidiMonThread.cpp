#include "stdafx.h"
#include "MidiMon.h"
#include "Midi View.h"
#include "MidiMonThread.h"
#include "../../Mirage Editor.h"
#include "../Nybble.h"

HANDLE CMidiMonThread::m_hEventMidiMonThreadKilled;

IMPLEMENT_DYNCREATE(CMidiMonThread, CWinThread)

BEGIN_MESSAGE_MAP(CMidiMonThread, CWinThread)
	ON_THREAD_MESSAGE(WM_MM_PUTDATA ,OnPutData)
	ON_THREAD_MESSAGE(WM_PARSESYSEX, OnParseSysex)
		//{{AFX_MSG_MAP(CMidiMonThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMidiMonThread::CMidiMonThread()
{
}

CMidiMonThread::CMidiMonThread(HWND hwndParent) : m_hwndParent(hwndParent)
{
		theApp.midi_monitor_started = CreateEvent(	NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);
		m_hEventMidiMonThreadKilled = CreateEvent(NULL,               // default security attributes
																			TRUE,               // manual-reset event
																			FALSE,              // initial state is nonsignaled
																			FALSE);
//		m_pMidiMonitor = theApp.m_pMidiMonitor;
}

CMidiMonThread::~CMidiMonThread()
{
}

void CMidiMonThread::operator delete(void* p)
{
	SetEvent(m_hEventMidiMonThreadKilled);

	CWinThread::operator delete(p);
}

void CMidiMonThread::SetCreateContext(CCreateContext* pContext)
{
	//m_pMidiMonitor = new CMultiDocTemplate(IDR_MidiInputType,
	//										RUNTIME_CLASS(CMidiDoc),
	//										RUNTIME_CLASS(CMidiMonChildWnd),
	//										RUNTIME_CLASS(CMidiView));
	
	m_pContext = pContext;
	m_pMidiDoc = (CMidiDoc*)m_pContext->m_pCurrentDoc;

//	memcpy(m_pContext,pContext, sizeof(CCreateContext));
}

CMidiDoc* CMidiMonThread::GetDocument()
{
	return m_pMidiDoc;
}

int CMidiMonThread::InitInstance()
{
	CMidiMonChildWnd* pParentWnd = (CMidiMonChildWnd*)CWnd::FromHandle(m_hwndParent);
	CRect rect;
	MSG msg;

	PeekMessage(&msg, NULL,NULL,NULL,PM_REMOVE);

	pParentWnd->GetClientRect(&rect);

	// Note: can be a CWnd with PostNcDestroy self cleanup
	CWnd* pView = (CWnd*)m_pContext->m_pNewViewClass->CreateObject();
	if (pView == NULL)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of view type %hs failed.\n",
			m_pContext->m_pNewViewClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CWnd, pView);

	// views are always created with a border!
	if (!pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), pParentWnd, m_nID, m_pContext))
	{
		TRACE(traceAppMsg, 0, "Warning: could not create view for frame.\n");
		return NULL;        // can't continue without a view
	}

	m_pMainWnd = pView;

	SetEvent(theApp.midi_monitor_started);
	return TRUE;
}

int CMidiMonThread::ExitInstance()
{
	delete m_pMidiDoc;
//	delete m_pMidiMonitor;
//	m_pMainWnd->DestroyWindow();

	return CWinThread::ExitInstance();
}

void CMidiMonThread::OnPutData(WPARAM wParam, LPARAM lParam)
{
	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
	m_pMidiDoc->PutData(mydata, pcds->dwData);
	LocalFree(pcds);
}

void CMidiMonThread::OnParseSysex(WPARAM wParam, LPARAM lParam)
{
	char ConfigParams[][29] = {
		"Dummy Byte              ",
		"Master Tune             ",
		"Pitch Bend Range        ",
		"Velocity Sensitivity    ",
		"Upper/Lower Balance     ",
		"Program Link Switch     ",
		"Sample Time Adjust      ",
		"Input Filter Frequency  ",
		"Mic/Line Switch         ",
		"Sampling Threshold      ",
		"Multisample Switch      ",
		"Omni Mode Flag          ",
		"Midi Channel            ",
		"Thru Mode Switch        ",
		"Mod Enable Switch       ",
		"Source Start MSB        ",
		"Source Start LSB        ",
		"Source End MSB          ",
		"Source End LSB          ",
		"Destination MSB         ",
		"Destination LSB         ",
		"Destination Bank        ",
		"Scale Start Factor      ",
		"Scale End Factor        ",
		"External Computer Switch",
		"Baud Rate Switch        ",
		"Cartridge Filter Freq   ",
		"Software Version        ",
		"Spare                   " };

	unsigned char ulprogsam = 0;
	unsigned char paramnumber = 0;
	unsigned char paramvalue = 0;
	unsigned char * ptr;
	DWORD BytesRecorded;
	BOOL io_dir;
	string SysExList;
	char SEMessage[128];
	std::string LogMessage;

	unsigned char MessageID;
	int i;

	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	
	ptr = (unsigned char*)LocalAlloc(LMEM_FIXED,pcds->cbData);
	if (pcds->dwData >= 128)
	{
		pcds->dwData -= 128;
		SysExList.assign(theApp.m_lSysex_Buffer.at((int)pcds->lpData-1));
//		theApp.m_lSysex_Buffer.RemoveAt((POSITION &)pcds->lpData);

		if (SysExList.c_str() == NULL)
			goto MonThreadEnd;
		
		memcpy(ptr,SysExList.c_str(), pcds->cbData);
	}
	else {
		memcpy(ptr, pcds->lpData, pcds->cbData);
	}
	io_dir = (BOOL)(pcds->dwData);
	BytesRecorded = pcds->cbData;


	// Simple check for complete valid sysex
	if ( *(ptr) != 0xF0 && *(ptr+(BytesRecorded-1)) != 0xF7 )
	{
		LocalFree(ptr);
		LocalFree(pcds);
		return;
	}

	ulprogsam = *(ptr+4);

	switch (*(ptr+3))
	{
		case CONFIG_PARM_REQ:
			LogMessage += "Config Parameters Request"; // Only 5 bytes, simple request
			break;

		case COMMAND_CODE:
			LogMessage += "Command Code"; // Needs expanding 
			switch ( *(ptr+4))
			{
				case SELECT_LOWER:
					LogMessage += " Select Lower Wavesample";
					break;
				case SELECT_UPPER:
					LogMessage += " Select Upper Wavesample";
					break;
			}
			break;

		case CONFIG_PARM_DUMP:
			LogMessage += "Config Parameters Dump Data"; // Needs more info
			m_pMidiDoc->PutData(LogMessage, io_dir);
//			LogMessage.clear();
			sprintf(SEMessage, "");
			ptr += 4;
			for ( int i=0 ; i < 29 ; i++ )
			{
				paramvalue = de_nybblify(*(ptr), *(ptr + 1) );
				if ( i == 4 || i == 7 || i == 9 )
				{
					sprintf(SEMessage,"%sParam # %d, value %03d %s ", SEMessage, i+20, paramvalue/2, ConfigParams[i]);
				} else {
					sprintf(SEMessage,"%sParam # %d, value %03d %s ", SEMessage, i+20, paramvalue, ConfigParams[i]);
				}
				if ( i%2 == 1  )
				{
					m_pMidiDoc->PutData(SEMessage, io_dir);
					sprintf(SEMessage, "");
				}
				ptr += 2;
			}
			m_pMidiDoc->PutData(SEMessage, io_dir);
			break;

		case LOWER_PRG_DUMP_REQ:
			LogMessage += "Lower Program Dump Request"; // Only 5 bytes, simple request
			break;

		case UPPER_PRG_DUMP_REQ:
			LogMessage += "Upper Program Dump Request"; // Only 5 bytes, simple request
			break;

		case WAVE_DUMP_REQ:
			LogMessage += "Wave Dump Request"; // Only 5 bytes, simple request. The sample is selected via wavesample select
			break;

		case PRG_DUMP_LOWER:
			LogMessage += "Lower Program Dump Data"; // Needs more info
			break;

		case PRG_DUMP_UPPER:
			LogMessage += "Upper Program Dump Data"; // Needs more info
			break;

		case WAVE_DUMP_DATA:
			LogMessage += "Wave Dump Data"; // No extra info needed.
			paramvalue = de_nybblify(*(ptr+4),*(ptr+5));
			paramnumber = *(ptr+(BytesRecorded-2));
			sprintf(SEMessage, " for %02X sample pages with checksum %02X", paramvalue, paramnumber);
			LogMessage += SEMessage;
			break;

		case PRG_STATUS_MSG:
			LogMessage += "Program Status Message"; // Send when a program number is changed from the front panel
			m_pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
			if ( (ulprogsam&0xF0) == 0 )
			{
				sprintf(SEMessage, "Selected lower program %d", (ulprogsam&0xF)+1);
			} else {
				sprintf(SEMessage, "Selected upper program %d", (ulprogsam&0xF)+1);
			}
			LogMessage += SEMessage;
			break;

		case WAVE_STATUS_MSG:
			LogMessage += "Wavesample Status Message"; // Send when a new wavesample is selected from the front panel
			m_pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
			if ( (ulprogsam&0xF0) == 0 )
			{
				sprintf(SEMessage, "Selected lower sample %d", (ulprogsam&0xF)+1);
			} else {
				sprintf(SEMessage, "Selected upper sample %d", (ulprogsam&0xF)+1);
			}
			LogMessage += SEMessage;
			break;

		case WAVE_ACK:
			LogMessage += "Wavesample acknowledge"; // Sent by mirage when the checksum of a received sample is good. 
																							// Also sent when a wavesample function is completed
			break;

		case WAVE_NACK:
			LogMessage += "Wavesample NOT acknowleged";
			break;

		case WAVEDUMPABSREQ:
			LogMessage += "Wavesample Dump Absolute Request";
			break;

		case WAVEDUMPABSDATA:
			LogMessage += "Wavesample Dump Absolute Data";
			break;

		case PRG_PARM_MSG:
			LogMessage += "Program Parameter Message"; // Sent whenever a program parameter is changed from the front panel.
			/* +4 = 000N00PP  where N=0 Lower, N=1 Upper, PP = Program Number
				 +5 = parameter #
				 +6 = LS Nybble
				 +7 = MS Nybble
			*/
			paramnumber = *(ptr+5);
			paramvalue = (*(ptr+6) + (*(ptr+7)<<4));
			
			if ( (ulprogsam&0xF0) == 0 )
			{
				sprintf(SEMessage, " for lower program %d ", (ulprogsam&0xF)+1);
			} else {
				sprintf(SEMessage, " for upper program %d ", (ulprogsam&0xF)+1);
			}
			LogMessage += SEMessage;
			m_pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
			sprintf(SEMessage,"Parameter %02d : %02d", paramnumber, paramvalue);
			LogMessage += SEMessage;
			break;

		case SMP_PARM_MSG:
			LogMessage += "Wavesample Parameter Message"; // Sent whenever a sample parameter is changed from the front panel. 
			/* +4 = 000N0SSS  where N=0 Lower, N=1 Upper, SSS = wavesample #
				 +5 = parameter #
				 +6 = LS Nybble
				 +7 = MS Nybble
			*/
			paramnumber = *(ptr+5);
			paramvalue = (*(ptr+6) + (*(ptr+7)<<4));
			
			if ( (ulprogsam&0xF0) == 0 )
			{
				sprintf(SEMessage, " for lower sample #%d ", (ulprogsam&0xF)+1);
			} else {
				sprintf(SEMessage, " for upper sample #%d ", (ulprogsam&0xF)+1);
			}
			LogMessage += SEMessage;
			m_pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
			if ( paramnumber < 69 )
			{
				sprintf(SEMessage,"Parameter %02d : %02X", paramnumber, paramvalue);
			} else {
					sprintf(SEMessage,"Parameter %02d : %02d", paramnumber, paramvalue);
			}
			LogMessage += SEMessage;
			break;

		case WAVEMANIPCMD:
			LogMessage += "Wavesample Manipulation Function Command";
			break;

		default:
			LogMessage += "Unknown Mirage Sysex";
			sprintf(SEMessage," Message ID: %02X ",*(ptr+3));
			LogMessage += SEMessage;
	}
	
	m_pMidiDoc->PutData(LogMessage, io_dir);
	LogMessage.clear();

	if ( BytesRecorded >= 64 )
	{
		sprintf(SEMessage, "System Exclusive Size: %d", BytesRecorded);
		m_pMidiDoc->PutData(SEMessage, io_dir);
		LogMessage.clear();
	}

	if ( BytesRecorded > 47 ) BytesRecorded = 47;

	for(i=0 ; i < BytesRecorded; i++)
	{
		sprintf(SEMessage,"%02X ",*(ptr+i));
		LogMessage += SEMessage;
		if ( (i+1) % 16 == 0 )
		{
			m_pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
		}
	}
	if ( i % 16 != 0 )
		m_pMidiDoc->PutData(LogMessage, io_dir);
MonThreadEnd:	
	LocalFree(ptr);
	LocalFree(pcds);
}
