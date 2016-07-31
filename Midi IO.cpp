#include "stdafx.h"
#include "src/Mirage Editor.h"
#include "src\Midi\SendSysex.h"

#include "src/Midi/MidiWrapper/LongMsg.h"
#include "src/Midi/MidiWrapper/ShortMsg.h"
#include "src/Midi/MidiWrapper/midi.h"
#include "src/Midi/SysexParser.h"

#include "src/Midi/Monitor/sysex_logger.h"
unsigned char	SysXBuffer[SYSEXBUFFER];

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

	COPYDATASTRUCT* mycds = (COPYDATASTRUCT*)LocalAlloc(LMEM_FIXED, sizeof(COPYDATASTRUCT));
	mycds->dwData = IO_Dir;
	mycds->cbData = sizeof(TCHAR) * m_midimonitorstring.length();
	mycds->lpData = (LPVOID)m_midimonitorstring.data();
	m_pMidiMonThread->PostThreadMessage(WM_MM_PUTDATA, NULL, (LPARAM)mycds);
}

void CMirageEditorApp::StartMidiOutput()
{
	m_OutDevice.Open(m_OutDevice.GetIDFromName(theApp.GetProfileStringA("Settings", "OutPort", "not connected")) - 1);
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
	if (midi::CMIDIInDevice::GetNumDevs() == 0)
	{
		MessageBox(NULL, "No MIDI input devices available.", "Warning",
			MB_ICONWARNING | MB_OK);
	}
	else {
		if (m_InDevice.GetIDFromName(theApp.GetProfileStringA("Settings", "InPort", "not connected")) > 0)
		{
			m_InDevice.SetReceiver(*this);
			m_InDevice.Open(theApp.m_InDevice.GetIDFromName(theApp.GetProfileStringA("Settings", "InPort", "not connected")) - 1);
			m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer, sizeof(SysXBuffer));
			// Start receiving MIDI events
			m_InDevice.StartRecording();
		}
		PostMidiMonitor(string("Ready to receive data"), MIDIMON_IN);
	}
}

void CMirageEditorApp::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
	char logmsg[64];
	char *CommandName[] = { "Note Off",
		"Note On",
		"Poly Pressure",
		"Ctrl Change",
		"Prg Change",
		"Aftertouch",
		"Pitch Bend" };

	midi::CShortMsg ShortMsg(Msg, TimeStamp);

	unsigned char Command = ShortMsg.GetCommand();
	if (Command == midi::NOTE_OFF ||
		(Command == midi::NOTE_ON && ShortMsg.GetData2() == 0))
	{
		m_LastNote = ShortMsg.GetData1();
	}

	if (Command < 0xF0)
	{
		sprintf(logmsg, "%s Data1: %02X Data2: %02X", CommandName[(Command >> 4) & 0x7], ShortMsg.GetData1(), ShortMsg.GetData2());
		PostMidiMonitor(logmsg, MIDIMON_IN);
	}
	else {
		int i = 1;
	}

	SetEvent(midi_in_event);
}

// Sysex Data
void CMirageEditorApp::ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	// Why is this one running in midi::CMIDIInDevice::MidiInProc

	midi::CLongMsg LongMsg(Msg, BytesRecorded);
	char end_of_sysex = (char)LongMsg.GetMsg()[BytesRecorded - 1];
	char start_of_data = (char)LongMsg.GetMsg()[0];
	if (start_of_data == (char)0xF0) // Start of sysex
	{
		// start filling std::string m_sysex_received
		m_sysex_received.assign(LongMsg.GetMsg(), (size_t)BytesRecorded);
	}
	else {
		// append to std::string m_sysex_received
		m_sysex_received.append(LongMsg.GetMsg(), (size_t)BytesRecorded);
	}

	// TODO: Progress bar
	if (progress)
	{
		//		progress.Bar.StepIt();
	}

	if (end_of_sysex == (char)0xF7)
	{
		m_lSysex_Buffer.push_back(m_sysex_received); // Add the received sysex to the end of the buffer list.
																							 // Now should actually parse the data in the list. Maybe it's best to just send a pointer to the list member?

																							 // Parse the sysex from std:string m_sysex_received
		ParseSysEx(/*(unsigned char*)m_sysex_received.data(),*/ (DWORD)m_sysex_received.size(), m_lSysex_Buffer.size());
		sysex_logmsg((unsigned char*)m_sysex_received.data(), (DWORD)m_sysex_received.size(), m_lSysex_Buffer.size(),MIDIMON_IN);
		// This one might be the one deleting the data...no guarantee the thread has done it's job.
		m_sysex_received.clear();
		SetEvent(midi_in_event);
	}
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer, sizeof(SysXBuffer));
	//	m_InDevice.AddSysExBuffer((LPSTR)&SysXBuffer,sizeof(SysXBuffer));

	memset((void *)LongMsg.GetMsg(), 0, LongMsg.GetLength());
}

void CMirageEditorApp::OnError(DWORD Msg, DWORD TimeStamp)
{
	return;
}

// For long messages:
void CMirageEditorApp::OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp)
{
	m_InDevice.ReleaseBuffer((LPSTR)&SysXBuffer, sizeof(SysXBuffer));
	return;
}

