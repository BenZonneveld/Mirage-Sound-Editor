// Tab_MidiConfig.cpp : implementation file
//

#include "stdafx.h"
#include "../Mirage Editor.h"
#include "../Midi/MirageSysex.h"
#include "../macros.h"
#include "Tab_MidiConfig.h"


// CTabMidiConfig dialog

IMPLEMENT_DYNAMIC(CTabMidiConfig, CDialog)

CTabMidiConfig::CTabMidiConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CTabMidiConfig::IDD, pParent)
{

}

CTabMidiConfig::~CTabMidiConfig()
{
}

void CTabMidiConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MIDI_OMNI, m_midi_omni);
	DDX_Control(pDX, IDC_MIDI_THRU, m_midi_thru);
	DDX_Control(pDX, IDC_MIDI_CHANNEL, m_midi_channel);
	DDX_Control(pDX, IDC_LOCAL_MODE, m_local_mode);
	DDX_Control(pDX, IDC_COMBO_LFO_MOD, m_lfo_mod);
	DDX_Control(pDX, IDC_COMBO_MIX_MOD, m_mix_mod);
	DDX_Control(pDX, IDC_AFTER_DEPTH_EDIT, m_after_depth_edit);
	DDX_Control(pDX, IDC_AFTER_DEPTH_SLIDER, m_after_depth_slider);
	DDX_Control(pDX, IDC_COMBO_MIDI_FUNCTION, m_midi_function);
}

BOOL CTabMidiConfig::OnInitDialog()
{
	int c=0;
	char channel[3];
	char modsources[][13]={"None        ",
							"Mod Wheel   ",
							"Breath Ctrl ",
							"Foot Pedal  ",
							"Data Entry  ",
							"Volume Pedal",
							"Aftertouch  ",
							"PolyPressure"};
	char midi_function[][24]={"Key Only",
													"Key+Ctrl",
													"Key+Ctrl+Pgm",
													"Key+Ctrl+(Pgm after +1)"};

	CDialog::OnInitDialog();
	memcpy(&m_config,&ConfigDump,sizeof(ConfigDump));
	// Midi Configuration
	MidiOmniButton();
	MidiThruButton();
	for (c=0; c < 16 ; c++ )
	{
		sprintf_s(channel,3,"%d",c+1);
		m_midi_channel.AddString(channel);
		if ( m_config.midi_channel == c )
			m_midi_channel.SetCurSel(c);
	}
	LocalModeButton();

	for(c=0; c < 8; c++)
	{
		m_lfo_mod.AddString(modsources[c]);
		m_mix_mod.AddString(modsources[c]);
	}

	for(c=0; c < 4; c++)
	{
		m_midi_function.AddString(midi_function[c]);
	}
	return TRUE;
}

void CTabMidiConfig::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	SetSlider(m_after_depth_slider,m_after_depth_edit);
	CDialog::OnHScroll(SBCode, nPos, pScrollBar);
}

void CTabMidiConfig::OnBnClickedMidiOmni()
{
	m_config.source_start_msb=!m_config.source_start_msb;
	MidiOmniButton();
}

void CTabMidiConfig::MidiOmniButton()
{
	switch(m_config.source_start_msb)
	{
		case false:
			m_midi_omni.SetWindowTextA("Omni Off");
			m_midi_omni.UpdateWindow();
			break;
		case true:
			m_midi_omni.SetWindowTextA("Omni On");
			m_midi_omni.UpdateWindow();
			break;
	}
}

void CTabMidiConfig::OnBnClickedMidiThru()
{
	m_config.thru_mode=!m_config.thru_mode;
	MidiThruButton();
}

void CTabMidiConfig::MidiThruButton()
{
	switch(m_config.thru_mode)
	{
		case false:
			m_midi_thru.SetWindowTextA("Thru Off");
			m_midi_thru.UpdateWindow();
			break;
		case true:
			m_midi_thru.SetWindowTextA("Thru On");
			m_midi_thru.UpdateWindow();
			break;
	}
}

void CTabMidiConfig::OnBnClickedLocalMode()
{
	m_config.omni_mode=!m_config.omni_mode;
	LocalModeButton();
}

void CTabMidiConfig::LocalModeButton()
{	
	switch(m_config.omni_mode)
	{
		case false:
			m_local_mode.SetWindowTextA("Local Off");
			m_local_mode.UpdateWindow();
			break;
		case true:
			m_local_mode.SetWindowTextA("Local On");
			m_local_mode.UpdateWindow();
			break;
	}
}

void CTabMidiConfig::OnDialogOk()
{
	int CurSel = m_midi_channel.GetCurSel();
	ConfigDump.midi_channel = CurSel;
	ConfigDump.omni_mode = m_config.omni_mode; // Local Mode
	ConfigDump.thru_mode = m_config.thru_mode;
	ConfigDump.source_start_msb = m_config.source_start_msb; // Omni mode
	// LFO Mod Source
//	ConfigDump.lfo_mod_source 
	// Mix Mod Source
//	ConfigDump.mix_mod_source
	// Midi Function
//	ConfigDump.midi_function
	// Aftertouch Mod Depth
//	ConfigDump.aftertouch_mod_depth = m_after_depth_slider.GetRangeMax()-m_after_depth_slider.GetPos();

	CWnd::DestroyWindow();
}

BEGIN_MESSAGE_MAP(CTabMidiConfig, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_MIDI_OMNI, &CTabMidiConfig::OnBnClickedMidiOmni)
	ON_BN_CLICKED(IDC_MIDI_THRU, &CTabMidiConfig::OnBnClickedMidiThru)
	ON_BN_CLICKED(IDC_LOCAL_MODE, &CTabMidiConfig::OnBnClickedLocalMode)
END_MESSAGE_MAP()


// CTabMidiConfig message handlers
