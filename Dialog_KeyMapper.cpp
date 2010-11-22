// CKeyMapper.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Dialog_KeyMapper.h"
#include "SendSysex.h"
#include "Mirage Sysex_Strings.h"
#include "MirageSysex.h"
#include "MirageParameters.h"
#include <vector>
#include "ShortMsg.h"
#include "midi.h"
#include "memdc.h"
//#include "MIDIKeyboard.h"

// CKeyMapper dialog

IMPLEMENT_DYNAMIC(CKeyMapper, CDialog)

CKeyMapper::CKeyMapper(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyMapper::IDD, pParent)
{

}

CKeyMapper::~CKeyMapper()
{
    if(m_InDevice.IsOpen())
    {
        if(m_InDevice.IsRecording())
        {
            m_InDevice.StopRecording();
        }   

        m_InDevice.Close();
    }
}

// Receives MIDI short message
void CKeyMapper::ReceiveMsg(DWORD Msg, DWORD TimeStamp)
{
    midi::CShortMsg ShortMsg(Msg, TimeStamp);
    unsigned char Command = ShortMsg.GetCommand();
    CString Text;

    // 
    // If this is a note-on or note-off event, notify piano control so
    // that it can update its display
    //

    if(Command == midi::NOTE_ON && ShortMsg.GetData2() > 0)
    {
        unsigned char Note = ShortMsg.GetData1();

        // Range checking
        if(Note >= m_Keys.GetLowNote() && 
            Note <= m_Keys.GetHighNote())
        {
            m_Keys.NoteOn(Note);
        }
    }
    else if(Command == midi::NOTE_OFF || (Command == midi::NOTE_ON && 
        ShortMsg.GetData2() == 0))
    {
        unsigned char Note = ShortMsg.GetData1();

        // Range checking
        if(Note >= m_Keys.GetLowNote() && 
            Note <= m_Keys.GetHighNote())
        {
            m_Keys.NoteOff(Note);
        }
    }
}


char * CKeyMapper::NoteName(unsigned char NoteId,int *octave)
{
	std::vector <char *> noteString;
	noteString.resize(12);
	noteString[0] = "C";
	noteString[1] = "C#";
	noteString[2] = "D";
	noteString[3] = "D#";
	noteString[4] = "E";
	noteString[5] = "F";
	noteString[6] = "F#";
	noteString[7] = "G";
	noteString[8] = "G#";
	noteString[9] = "A";
	noteString[10] = "A#";
	noteString[11] = "B";
	*octave = (NoteId/12);
	int noteIndex=(NoteId%12);

	return noteString[noteIndex];
}

void CKeyMapper::OnNoteOn(CPianoCtrl &PianoCtrl, unsigned char NoteId)
{
	int lastkey;

	if ( m_mapping_sample != -1 )
	{
		lastkey = NoteId - 0x23;
		m_lastkey[m_mapping_sample] = lastkey;
		Invalidate(FALSE);
		UpdateWindow();
	}
}

void CKeyMapper::OnNoteOff(CPianoCtrl &PianoCtrl, unsigned char NoteId)
{
	int lastkey;

	if ( m_mapping_sample != -1 )
	{
		lastkey = NoteId - 0x23;
		m_lastkey[m_mapping_sample] = lastkey;
		Invalidate(FALSE);
		UpdateWindow();
	}
}

void CKeyMapper::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MIDI_KEYS, m_Keys);
	DDX_Control(pDX, IDC_LOWER1_MAP, m_CwndSample[0]);
	DDX_Control(pDX, IDC_LOWER2_MAP, m_CwndSample[1]);
	DDX_Control(pDX, IDC_LOWER3_MAP, m_CwndSample[2]);
	DDX_Control(pDX, IDC_LOWER4_MAP, m_CwndSample[3]);
	DDX_Control(pDX, IDC_LOWER5_MAP, m_CwndSample[4]);
	DDX_Control(pDX, IDC_LOWER6_MAP, m_CwndSample[5]);
	DDX_Control(pDX, IDC_LOWER7_MAP, m_CwndSample[6]);
	DDX_Control(pDX, IDC_LOWER8_MAP, m_CwndSample[7]);
	DDX_Control(pDX, IDC_UPPER1_MAP, m_CwndSample[8]);
	DDX_Control(pDX, IDC_UPPER2_MAP, m_CwndSample[9]);
	DDX_Control(pDX, IDC_UPPER3_MAP, m_CwndSample[10]);
	DDX_Control(pDX, IDC_UPPER4_MAP, m_CwndSample[11]);
	DDX_Control(pDX, IDC_UPPER5_MAP, m_CwndSample[12]);
	DDX_Control(pDX, IDC_UPPER6_MAP, m_CwndSample[13]);
	DDX_Control(pDX, IDC_UPPER7_MAP, m_CwndSample[14]);
	DDX_Control(pDX, IDC_UPPER8_MAP, m_CwndSample[15]);
}

BOOL CKeyMapper::OnInitDialog()
{
	int bank=0;
	int samplenumber;
	CRect KeyRect;
	CDialog::OnInitDialog();

	/* Create the piano Keyboard */
	m_Keys.Initialize(CMIDIKeyboard::LOW_NOTE, CMIDIKeyboard::HIGH_NOTE);

	GetSampleParameters();

	/* Setup the key mappings from the mirage */
	m_mapping_sample = -1;
	for (int c=0 ; c< 16 ; c++)
	{
		if ( c < 8 )
		{
			bank = 0;
			samplenumber = c;
		} else {
			bank = 1;
			samplenumber = c - 8;
		}

		m_lastkey[c] = 1+ProgramDumpTable[bank].WaveSampleControlBlock[samplenumber].TopKey;
		KeyRect = m_Keys.GetKeyRect(m_lastkey[c]);
		m_LastKeyRect[c].bottom = KeyRect.bottom;
		m_LastKeyRect[c].top = KeyRect.top;
		m_LastKeyRect[c].left = KeyRect.left;
		m_LastKeyRect[c].right = KeyRect.right;
	}
	/* Attach ourselves to the piano control so we will be notified */
	m_Keys.AttachListener(*this);

	if(midi::CMIDIInDevice::GetNumDevs() > 0)
	{
		m_InDevice.SetReceiver(*this);
        if(m_InDevice.Open(theApp.GetProfileIntA("Settings","InPort",0)-1))
		{
	        // Start receiving MIDI events
		    m_InDevice.StartRecording();
		} else {
			return false;
		}
	}
	return true;
}

void CKeyMapper::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC		pDC;
	CFont	font;
	char	szString[30];
	char	*noteName;
	int		octave=-1;
	CPen	Pen;
	CRect	Rect;
	CRect	TopKeyRect;
	CRect	KeyRect;
	CRect	OverLapKeyRect;
	CBrush	Brush;
	CBrush	OL_Brush;
	int		c=0;
	COLORREF	bgColor;

	noteName=(char *) malloc(6);
	font.CreateFontA(12,
					5,
					0,
					0,
					FW_NORMAL,
					FALSE,
					FALSE,
					FALSE,
					DEFAULT_CHARSET,
					OUT_TT_ONLY_PRECIS,
					CLIP_DEFAULT_PRECIS,
					PROOF_QUALITY,
					DEFAULT_PITCH,
					"Arial");
	
	Pen.CreatePen(PS_SOLID,1,RGB(0,0,0));

	pDC.Attach(lpDrawItemStruct->hDC);

	pDC.SetMapMode(MM_ANISOTROPIC);
	GetClientRect(&Rect);
   
	for (c=0; c < 16;c++)
	{
		if (c < 8 && m_CwndSample[c].GetSafeHwnd() == lpDrawItemStruct->hwndItem )
		{
			if ( m_lastkey[c] > 0 )
			{
				noteName=NoteName(m_lastkey[c]-1,&octave);
				sprintf_s(szString,sizeof(szString),"Lower %i - Topkey: %s %i", c+1,noteName,octave);
			} else {
				sprintf_s(szString,sizeof(szString),"Lower %i", c+1);
			}
			break;
		}
		if (c > 7 && m_CwndSample[c].GetSafeHwnd() == lpDrawItemStruct->hwndItem )
		{
			if ( m_lastkey[c] > 0 )
			{
				noteName=NoteName(m_lastkey[c],&octave);
				sprintf_s(szString,sizeof(szString),"Upper %i - Topkey: %s %i", c-7,noteName,octave);
			} else {
				sprintf_s(szString,sizeof(szString),"Upper %i", c-7);
			}
			break;
		}
	}

	/* Get the position of the selectect note */
	KeyRect = Rect;
	TopKeyRect=m_Keys.GetKeyRect(m_lastkey[c]-1);
	if ( m_lastkey[c] < 61 )
	{
		KeyRect.right = TopKeyRect.right-(TopKeyRect.Width()/2);
	} else {
		KeyRect.right = TopKeyRect.right;
	}

	if ( c == 0 )
		m_TopKey = 1;
	if ( m_TopKey > 1 )
		KeyRect.left = m_Keys.GetKeyRect(m_TopKey - 1).left + (m_Keys.GetKeyRect(m_TopKey - 1).Width()/2);

	/* Set the color of the brush */
	Brush.Detach();
	bgColor = pDC.GetBkColor();

	if (c == m_mapping_sample )
	{
		Brush.CreateSolidBrush(RGB(0,0x80,0));
		OL_Brush.CreateSolidBrush(RGB(0x40,0xC0,0x40));
		pDC.SetBkColor(bgColor+RGB(0x0f,0x0f,0x0f));
	} else {
		Brush.CreateSolidBrush(RGB(0,0x69,0xFA));
		OL_Brush.CreateSolidBrush(RGB(0x40,0xA9,0xFA));
	}

	pDC.FillSolidRect(Rect, pDC.GetBkColor());

	if ( c == 11 )
		Sleep(1);
	OverLapKeyRect=Rect;
	if ( KeyRect.right > KeyRect.left && KeyRect.right != (KeyRect.left + 1) && m_TopKey < 61 )
	{
		OverLapKeyRect.right = KeyRect.left;
	} else {
		OverLapKeyRect.right = KeyRect.right;
	}

	if ( m_lastkey[c] >= 61 )
	{
		KeyRect.right = Rect.right;
	}

	if ( KeyRect.right < KeyRect.left )
	{
		KeyRect.right = 0;
		KeyRect.left = 0;
	}
	pDC.FillRect(KeyRect,&Brush);
	pDC.FillRect(OverLapKeyRect,&OL_Brush);

	pDC.SetBkColor(bgColor);
	pDC.SelectObject(&font);
	pDC.SelectObject(&Pen);
	pDC.SetBkMode(TRANSPARENT);
	pDC.TextOut(20,0,szString,int(strlen(szString)));

	pDC.Detach();

	if ( m_TopKey < m_lastkey[c])
		m_TopKey = m_lastkey[c];
}

void CKeyMapper::OnBnClickedOk()
{
	int bank;
	unsigned char samplenumber;

	if(m_InDevice.IsOpen())
    {
        if(m_InDevice.IsRecording())
        {
            m_InDevice.StopRecording();
        }   

        m_InDevice.Close();
    }

	if (!GetSampleParameters())
	{
		OnOK();
		return;
	}

	for (int c=0 ; c< 16 ; c++)
	{
		if ( c < 8 )
		{
			bank = 0;
			samplenumber = c;
		} else {
			bank = 1;
			/* Construct the select sample front pannel command */
			samplenumber = c - 8;
		}
		if ( m_lastkey[c] == 1+ProgramDumpTable[bank].WaveSampleControlBlock[samplenumber].TopKey)
			continue;
		if ( bank == 0 )
		{
			unsigned char SelectSample[]={7,
											MirID[0],
											MirID[1],
											MirID[2],
											0x01, // Commando Code
											0x15, // Lower Sample Select
											0x7F,
											0xF7}; // Lower sample select
			if (!DoSampleSelect(SelectSample,samplenumber))
			{
				OnOK();
				return;
			}
		} else {
			unsigned char SelectSample[]={7,
											MirID[0],
											MirID[1],
											MirID[2],
											0x01, // Commando Code
											0x14, // Upper Sample Select
											0x7F,
											0xF7}; // Upper Sample Select
			if(!DoSampleSelect(SelectSample,samplenumber))
			{
				OnOK();
				return;
			}
		}
		ChangeParameter("Sample Top Key",72,m_lastkey[c]-1);
	}
	OnOK();
}
BEGIN_MESSAGE_MAP(CKeyMapper, CDialog)
	ON_WM_DRAWITEM()
	ON_STN_CLICKED(IDC_LOWER1_MAP, &CKeyMapper::OnStnClickedLower1Map)
	ON_STN_CLICKED(IDC_LOWER2_MAP, &CKeyMapper::OnStnClickedLower2Map)
	ON_STN_CLICKED(IDC_LOWER3_MAP, &CKeyMapper::OnStnClickedLower3Map)
	ON_STN_CLICKED(IDC_LOWER4_MAP, &CKeyMapper::OnStnClickedLower4Map)
	ON_STN_CLICKED(IDC_LOWER5_MAP, &CKeyMapper::OnStnClickedLower5Map)
	ON_STN_CLICKED(IDC_LOWER6_MAP, &CKeyMapper::OnStnClickedLower6Map)
	ON_STN_CLICKED(IDC_LOWER7_MAP, &CKeyMapper::OnStnClickedLower7Map)
	ON_STN_CLICKED(IDC_LOWER8_MAP, &CKeyMapper::OnStnClickedLower8Map)
	ON_STN_CLICKED(IDC_UPPER1_MAP, &CKeyMapper::OnStnClickedUpper1Map)
	ON_STN_CLICKED(IDC_UPPER2_MAP, &CKeyMapper::OnStnClickedUpper2Map)
	ON_STN_CLICKED(IDC_UPPER3_MAP, &CKeyMapper::OnStnClickedUpper3Map)
	ON_STN_CLICKED(IDC_UPPER4_MAP, &CKeyMapper::OnStnClickedUpper4Map)
	ON_STN_CLICKED(IDC_UPPER5_MAP, &CKeyMapper::OnStnClickedUpper5Map)
	ON_STN_CLICKED(IDC_UPPER6_MAP, &CKeyMapper::OnStnClickedUpper6Map)
	ON_STN_CLICKED(IDC_UPPER7_MAP, &CKeyMapper::OnStnClickedUpper7Map)
	ON_STN_CLICKED(IDC_UPPER8_MAP, &CKeyMapper::OnStnClickedUpper8Map)
	ON_BN_CLICKED(IDOK, &CKeyMapper::OnBnClickedOk)
END_MESSAGE_MAP()

void CKeyMapper::OnStnClickedLower1Map()
{
	m_mapping_sample = 0;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedLower2Map()
{
	m_mapping_sample = 1;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedLower3Map()
{
	m_mapping_sample = 2;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedLower4Map()
{
	m_mapping_sample = 3;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedLower5Map()
{
	m_mapping_sample = 4;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedLower6Map()
{
	m_mapping_sample = 5;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedLower7Map()
{
	m_mapping_sample = 6;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedLower8Map()
{
	m_mapping_sample = 7;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedUpper1Map()
{
	m_mapping_sample = 8;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedUpper2Map()
{
	m_mapping_sample = 9;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedUpper3Map()
{
	m_mapping_sample = 10;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedUpper4Map()
{
	m_mapping_sample = 11;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedUpper5Map()
{
	m_mapping_sample = 12;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedUpper6Map()
{
	m_mapping_sample = 13;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedUpper7Map()
{
	m_mapping_sample = 14;
	Invalidate(FALSE);
}

void CKeyMapper::OnStnClickedUpper8Map()
{
	m_mapping_sample = 15;
	Invalidate(FALSE);
}
