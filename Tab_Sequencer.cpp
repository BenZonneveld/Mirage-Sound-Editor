// Tab_Sequencer.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "MirageSysex.h"
#include "macros.h"
#include "Tab_Sequencer.h"


// CTabSequencer dialog

IMPLEMENT_DYNAMIC(CTabSequencer, CDialog)

CTabSequencer::CTabSequencer(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSequencer::IDD, pParent)
{

}

CTabSequencer::~CTabSequencer()
{
}

void CTabSequencer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEQ_CLOCK, m_seq_clock);
	DDX_Control(pDX, IDC_CLOCK_JACK, m_clock_jack);
	DDX_Control(pDX, IDC_SEQ_LOOP_SWITCH, m_seq_loop_switch);
	DDX_Control(pDX, IDC_MIDI_PEDAL, m_midi_pedal);
	DDX_Control(pDX, IDC_CLOCK_RATE_SLIDER, m_clock_rate_slider);
	DDX_Control(pDX, IDC_CLOCK_RATE_EDIT, m_clock_rate_edit);
}

BOOL CTabSequencer::OnInitDialog()
{
	CDialog::OnInitDialog();
	memcpy(&m_config,&ConfigDump,sizeof(ConfigDump));

	// Configuration
	m_seq_clock.SetCheck(m_config.source_start_msb);
	m_clock_jack.SetCheck(m_config.source_start_lsb);
	m_seq_loop_switch.SetCheck(m_config.dest_lsb);
	InitSlider(m_clock_rate_slider,m_clock_rate_edit,0,99,10,10);
	SetSliderValue(m_clock_rate_slider,m_clock_rate_edit,m_config.source_end_msb);
	MidiPedalButton();
	return TRUE;
}
void CTabSequencer::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	CDialog::OnHScroll(SBCode, nPos, pScrollBar);
	SetSlider(m_clock_rate_slider,m_clock_rate_edit);
}

void CTabSequencer::OnKillFocusClockRateEdit()
{
	SetSliderFromEdit(m_clock_rate_slider,m_clock_rate_edit);
}

void CTabSequencer::OnBnClickedMidiPedal()
{
	m_config.dest_lsb=!m_config.dest_lsb;
	MidiPedalButton();
}

void CTabSequencer::MidiPedalButton()
{
	switch(m_config.dest_lsb)
	{
		case false:
			m_midi_pedal.SetWindowTextA("Foot Switch Off");
			m_midi_pedal.UpdateWindow();
			break;
		case true:
			m_midi_pedal.SetWindowTextA("Foot Switch On");
			m_midi_pedal.UpdateWindow();
			break;
	}
}

void CTabSequencer::OnDialogOk()
{
	ConfigDump.source_start_msb = m_seq_clock.GetCheck();
	ConfigDump.source_start_lsb = m_clock_jack.GetCheck();
	ConfigDump.dest_lsb = m_seq_loop_switch.GetCheck();
	ConfigDump.source_end_msb = m_clock_rate_slider.GetRangeMax()-m_clock_rate_slider.GetPos();
	ConfigDump.dest_lsb = m_config.dest_lsb;

	CWnd::DestroyWindow();
}

BEGIN_MESSAGE_MAP(CTabSequencer, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_MIDI_PEDAL, &CTabSequencer::OnBnClickedMidiPedal)
	ON_EN_KILLFOCUS(IDC_CLOCK_RATE_EDIT, &CTabSequencer::OnKillFocusClockRateEdit)
END_MESSAGE_MAP()


// CTabSequencer message handlers
