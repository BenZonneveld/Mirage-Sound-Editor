// Message.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Dialog_OrigKey.h"


// COrigKey dialog

IMPLEMENT_DYNAMIC(COrigKey, CDialog)

COrigKey::COrigKey(CWnd* pParent /*=NULL*/)
	: CDialog(COrigKey::IDD, pParent)
{
	//{{AFX_DATA_INIT(COrigKey)
	//}}AFX_DATA_INIT

}

COrigKey::~COrigKey()
{
}

void COrigKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ORIGKEY, m_Keys);
}

BOOL COrigKey::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_OrigKey = 255;
	m_NoteOff = false;
	m_NoteOn = false;

	/* Create the piano Keyboard */
	m_Keys.Initialize(CMIDIKeyboard::LOW_NOTE, CMIDIKeyboard::HIGH_NOTE);

	/* Attach ourselves to the piano control so we will be notified */
	m_Keys.AttachListener(*this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COrigKey::OnNoteOn(CPianoCtrl &PianoCtrl, unsigned char NoteId)
{
/*	if ( m_OrigKey != ( NoteId - 0x23) && m_OrigKey < 255 )
	{
		COLORREF NoteColor=PianoCtrl.GetNoteOnColor();
		PianoCtrl.SetNoteOnColor(RGB(0, 250, 150));
		m_Keys.NoteOn(m_OrigKey + 0x23);
		PianoCtrl.SetNoteOnColor(NoteColor);
	}*/
	Invalidate(FALSE);
	UpdateWindow();
}

void COrigKey::OnNoteOff(CPianoCtrl &PianoCtrl, unsigned char NoteId)
{
	if ( m_NoteOff == false )
	{
		if ( m_OrigKey < 255 || m_OrigKey != (NoteId - 0x23))
		{
			COLORREF NoteColor=PianoCtrl.GetNoteOnColor();
			PianoCtrl.SetNoteOnColor(RGB(0, 250, 150));

			if ( m_OrigKey < 255)
			{
				m_NoteOff = true;
				m_Keys.NoteOff(m_OrigKey + 0x23);
			}
			m_Keys.NoteOn(NoteId);
			PianoCtrl.SetNoteOnColor(NoteColor);
		}
		m_OrigKey = NoteId - 0x23;
	} else {
		m_NoteOff = false;
	}
	Invalidate(FALSE);
	UpdateWindow();
}

void COrigKey::OnButtonOk()
{
	theApp.m_LastNote = m_OrigKey;

	OnOK();
}

BEGIN_MESSAGE_MAP(COrigKey, CDialog)
		ON_WM_DRAWITEM()
		ON_BN_CLICKED(IDORIGKEYOK, &COrigKey::OnButtonOk)
	//{{AFX_MSG_MAP(COrigKey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// COrigKey message handlers
