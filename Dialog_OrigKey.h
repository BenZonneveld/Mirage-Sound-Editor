#pragma once
#include "afxwin.h"
#include "MIDIKeyboard.h"
#include "MIDIInDevice.h"
#include "ShortMsg.h"

// COrigKey dialog

class COrigKey : public CDialog, public CPianoCtrlListener/*, public midi::CMIDIReceiver*/
{
	DECLARE_DYNAMIC(COrigKey)

public:
	COrigKey(CWnd* pParent = NULL);   // standard constructor
	virtual ~COrigKey();

	void OnNoteOn(CPianoCtrl &PianoCtrl, unsigned char NoteId);
	void OnNoteOff(CPianoCtrl &PianoCtrl, unsigned char NoteId);
// Dialog Data
	enum { IDD = IDD_ORIG_KEY };

protected:
	CMIDIKeyboard m_Keys;
	unsigned char m_OrigKey;
	BOOL m_NoteOn;
	BOOL m_NoteOff;
//  midi::CMIDIInDevice m_InDevice;	
	// Generated message map functions
	//{{AFX_MSG(COrigKey)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOk();
	//}}AFX_MSG
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
};
