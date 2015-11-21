#pragma once
#include "afxwin.h"
#include "../PianoKeys/MIDIKeyboard.h"
#include "../Midi/MirageSysex.h"
#include "../Midi/MidiWrapper/MIDIInDevice.h"
#include "../Midi/MidiWrapper/LongMsg.h"

// CKeyMapper dialog

class CKeyMapper : public CDialog, public CPianoCtrlListener, public midi::CMIDIReceiver
{
	DECLARE_DYNAMIC(CKeyMapper)

public:
	CKeyMapper(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKeyMapper();

	// Receives short messages
	void ReceiveMsg(DWORD Msg, DWORD TimeStamp);
	// Calles when an invalid short message is received
	void OnError(DWORD Msg, DWORD TimeStamp) {}
	// Receives long messages
	void ReceiveMsg(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp) {}
	// Called when an invalid long message is received
	void OnError(LPSTR Msg, DWORD BytesRecorded, DWORD TimeStamp) {}

	char *NoteName(unsigned char NoteId, int *octave); 
	void OnNoteOn(CPianoCtrl &PianoCtrl, unsigned char NoteId);
	void OnNoteOff(CPianoCtrl &PianoCtrl, unsigned char NoteId);
// Dialog Data
	enum { IDD = IDD_KEYMAPPER };

protected:
	CMIDIKeyboard m_Keys;
  midi::CMIDIInDevice m_InDevice;
	
	unsigned char m_TopKey;
	int m_mapping_sample;
	CRect m_LastKeyRect[16];
	unsigned char m_lastkey[16];
	CWnd m_CwndSample[16];

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void CKeyMapper::OnStnClickedLower1Map();
	afx_msg void CKeyMapper::OnStnClickedLower2Map();
	afx_msg void CKeyMapper::OnStnClickedLower3Map();
	afx_msg void CKeyMapper::OnStnClickedLower4Map();
	afx_msg void CKeyMapper::OnStnClickedLower5Map();
	afx_msg void CKeyMapper::OnStnClickedLower6Map();
	afx_msg void CKeyMapper::OnStnClickedLower7Map();
	afx_msg void CKeyMapper::OnStnClickedLower8Map();
	afx_msg void CKeyMapper::OnStnClickedUpper1Map();
	afx_msg void CKeyMapper::OnStnClickedUpper2Map();
	afx_msg void CKeyMapper::OnStnClickedUpper3Map();
	afx_msg void CKeyMapper::OnStnClickedUpper4Map();
	afx_msg void CKeyMapper::OnStnClickedUpper5Map();
	afx_msg void CKeyMapper::OnStnClickedUpper6Map();
	afx_msg void CKeyMapper::OnStnClickedUpper7Map();
	afx_msg void CKeyMapper::OnStnClickedUpper8Map();
	afx_msg void OnBnClickedOk();
};
