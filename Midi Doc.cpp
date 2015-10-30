// MidiDoc.cpp : implementation file
//

#include "stdafx.h"
#include <string>
#include "Mirage Editor.h"
#include "Midi Doc.h"

// CMidiDoc

IMPLEMENT_DYNCREATE(CMidiDoc, CDocument)

CMidiDoc::CMidiDoc()
{
	m_MaxQueue = 1000;
	for ( int i=0 ; i < 100 ; i++ )
	{
		m_MidiData.push_back("Monitor Started");
		m_MidiIO.push_back(FALSE);
	}
}

BOOL CMidiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

CMidiDoc::~CMidiDoc()
{
}


BEGIN_MESSAGE_MAP(CMidiDoc, CDocument)
END_MESSAGE_MAP()


// CMidiDoc diagnostics

#ifdef _DEBUG
void CMidiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CMidiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CMidiDoc serialization

void CMidiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif
void CMidiDoc::OnUpdateAllViews()
{
	for(POSITION pos = GetFirstViewPosition(); pos != NULL; )
	{
		CView* pView = GetNextView(pos);
		pView->Invalidate();
		pView->UpdateWindow();
	} // end of loop through views
}

// CMidiDoc commands
void CMidiDoc::PutData(string Data, BOOL IO_Dir)
{
	if ( ! Data.empty() )
	{
		int queSize = m_MidiData.size();
		if ( queSize > m_MaxQueue )
		{
			m_MidiData.pop_front();
			m_MidiIO.pop_front();
		}
		m_MidiData.push_back(Data);
		m_MidiIO.push_back(IO_Dir);
//		ResetEvent(theApp.m_MidiMonitorThread->MessageFlag);
//		OnUpdateAllViews();
	}
}

string CMidiDoc::GetData(int line)
{
	return m_MidiData.at(line);
}

BOOL CMidiDoc::GetIO(int line)
{
	return m_MidiIO.at(line);
}

int CMidiDoc::GetSize()
{
	return m_MidiData.size();
}

//void CMidiDoc::OnPutData(WPARAM wParam, LPARAM lParam)
//{
////	MSG msg;
//
////	PeekMessage(&msg, NULL, WM_MIDIMONITOR,WM_MIDIMONITOR, PM_REMOVE);
//
//	string mydata;
//	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)m_lParam;
//	mydata=(LPCTSTR)(pcds->lpData);
//	theApp.m_pMidiDoc->PutData(mydata, pcds->dwData);
//}
//
//void CMidiDoc::OnParseSysex(WPARAM wParam, LPARAM lParam)
//{
////	MSG msg;
////	GetMessage(&msg, NULL, WM_PARSESYSEX,WM_PARSESYSEX);
//	unsigned char * ptr;
//	DWORD BytesRecorded;
//	BOOL io_dir;
//	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)m_lParam;
//	ptr = (unsigned char *)(pcds->lpData);
//	io_dir = (BOOL)(pcds->dwData);
//	BytesRecorded = pcds->cbData;
//
//	char SEMessage[64];
//	std::string LogMessage;
//
//	unsigned char MessageID;
//	int i;
//
//	switch (*(ptr+3))
//	{
//		case CONFIG_PARM_REQ:
//			LogMessage += "Config Parameters Request";
//			break;
//		case COMMAND_CODE:
//			LogMessage += "Command Code";
//			switch ( *(ptr+4))
//			{
//				case SELECT_LOWER:
//					LogMessage += "Select Lower Wavesample";
//					break;
//				case SELECT_UPPER:
//					LogMessage += "Select Upper Wavesample";
//					break;
//			}
//			break;
//		case CONFIG_PARM_DUMP:
//			LogMessage += "Config Parameters Dump Data";
//			break;
//		case LOWER_PRG_DUMP_REQ:
//			LogMessage += "Lower Program Dump Request";
//			break;
//		case UPPER_PRG_DUMP_REQ:
//			LogMessage += "Upper Program Dump Request";
//			break;
//		case WAVE_DUMP_REQ:
//			LogMessage += "Wave Dump Request";
//			break;
//		case PRG_DUMP_LOWER:
//			LogMessage += "Lower Program Dump Data";
//			break;
//		case PRG_DUMP_UPPER:
//			LogMessage += "Upper Program Dump Data";
//			break;
//		case WAVE_DUMP_DATA:
//			LogMessage += "Wave Dump Data";
//			break;
//		case PRG_STATUS_MSG:
//			LogMessage += "Program Status Message";
//			break;
//		case WAVE_STATUS_MSG:
//			LogMessage += "Wavesample Status Message";
//			break;
//		case WAVE_ACK:
//			LogMessage += "Wavesample acknowledge";
//			break;
//		case WAVE_NACK:
//			LogMessage += "Wavesample NOT acknowleged";
//			break;
//		case WAVEDUMPABSREQ:
//			LogMessage += "Wavesample Dump Absolute Request";
//			break;
//		case WAVEDUMPABSDATA:
//			LogMessage += "Wavesample Dump Absolute Data";
//			break;
//		case PRG_PARM_MSG:
//			LogMessage += "Program Parameter Message";
//			break;
//		case SMP_PARM_MSG:
//			LogMessage += "Wavesample Parameter Message";
//			break;
//		case WAVEMANIPCMD:
//			LogMessage += "Wavesample Manipulation Function Command";
//			break;
//		default:
//			LogMessage += "Unknown Mirage Sysex";
//			sprintf(SEMessage," Message ID: %02X ",*(ptr+3));
//			LogMessage += SEMessage;
//	}
//	if ( *(ptr) != 0xF0 && *(ptr+(BytesRecorded-1)) != 0xF7 )
//	{
//		ResetEvent(MessageFlag);
//		return;
//	}
//}