// Dialog_ConfigParams.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Dialog_ConfigParams.h"
#include "MirageSysex.h"
#include "macros.h"


// CConfigParams dialog

IMPLEMENT_DYNAMIC(CConfigParams, CDialog)

CConfigParams::CConfigParams(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigParams::IDD, pParent)
{

}

CConfigParams::~CConfigParams()
{
}

void CConfigParams::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MASTER_TUNE_SLIDER, m_mastertune_slider);
	DDX_Control(pDX, IDC_MASTER_TUNE_EDIT, m_mastertune_edit);
	DDX_Control(pDX, IDC_PITCHBEND_RANGE_SLIDER, m_PitchBend_slider);
	DDX_Control(pDX, IDC_PITCHBEND_RANGE_EDIT, m_PitchBend_edit);
	DDX_Control(pDX, IDC_VELO_SENS_SLIDER, m_velosens_slider);
	DDX_Control(pDX, IDC_VELO_SENS_EDIT, m_velosens_edit);
	DDX_Control(pDX, IDC_UPPERLOWER_BALANCE_SLIDER, m_balance_slider);
	DDX_Control(pDX, IDC_UPPERLOWER_BALANCE_EDIT, m_balance_edit);
	DDX_Control(pDX, IDC_PROGRAM_LINK, m_programlink_check);
	DDX_Control(pDX, IDC_SAMPLE_TIME_SLIDER, m_sample_time_slider);
	DDX_Control(pDX, IDC_SAMPLE_TIME_EDIT, m_sample_time_edit);
	DDX_Control(pDX, IDC_FILTER_FREQ_SLIDER, m_filter_freq_slider);
	DDX_Control(pDX, IDC_FILTER_FREQ_EDIT, m_filter_freq_edit);
	DDX_Control(pDX, IDC_THRESHOLD_EDIT, m_threshold_edit);
	DDX_Control(pDX, IDC_THRESHOLD_SLIDER, m_threshold_slider);
	DDX_Control(pDX, IDC_USER_MULTISAMPLING, m_user_multisampling_check);
	DDX_Control(pDX, IDC_SAMPLING_LEVEL, m_sampling_level_button);
	DDX_Control(pDX, IDC_MIDI_OMNI, m_midi_omni);
	DDX_Control(pDX, IDC_MIDI_THRU, m_midi_thru);
	DDX_Control(pDX, IDC_MIDI_CHANNEL, m_midi_channel);
	DDX_Control(pDX, IDC_SEQ_CLOCK, m_seq_clock);
	DDX_Control(pDX, IDC_CLOCK_JACK, m_clock_jack);
	DDX_Control(pDX, IDC_SEQ_LOOP_SWITCH, m_seq_loop_switch);
	DDX_Control(pDX, IDC_MIDI_PEDAL, m_midi_pedal);
	DDX_Control(pDX, IDC_CLOCK_RATE_SLIDER, m_clock_rate_slider);
	DDX_Control(pDX, IDC_CLOCK_RATE_EDIT, m_clock_rate_edit);
	DDX_Control(pDX, IDC_LOCAL_MODE, m_local_mode);
}

BOOL CConfigParams::OnInitDialog()
{
	CDialog::OnInitDialog();
	memcpy(&m_config,&ConfigDump,sizeof(ConfigDump));

	// Generic Keyboard
	InitSlider(m_mastertune_slider,m_mastertune_edit,0,99,10,10);
	InitSlider(m_PitchBend_slider,m_PitchBend_edit,0,34,10,10);
	InitSlider(m_velosens_slider,m_velosens_edit,0,63,10,10);
	InitSlider(m_balance_slider,m_balance_edit,0,63,10,10);
	SetSliderValue(m_mastertune_slider,m_mastertune_edit,m_config.master_tune);
	SetSliderValue(m_PitchBend_slider,m_PitchBend_edit,m_config.pitch_bend_range);
	SetSliderValue(m_velosens_slider,m_velosens_edit,m_config.velocity_sensitivity);
	SetSliderValue(m_balance_slider,m_balance_edit,m_config.up_low_balance/2);
	m_programlink_check.SetCheck(m_config.program_link_switch);

	// Sampling Config
	InitSlider(m_sample_time_slider,m_sample_time_edit,20,99,10,10);
	InitSlider(m_filter_freq_slider,m_filter_freq_edit,0,99,10,10);
	InitSlider(m_threshold_slider,m_threshold_edit,0,63,10,10);
	SetSliderValue(m_sample_time_slider,m_sample_time_edit,m_config.sample_time_adjust);
	SetSliderValue(m_filter_freq_slider,m_filter_freq_edit,m_config.input_filter_freq/2);
	SetSliderValue(m_threshold_slider,m_threshold_edit,m_config.sample_treshold/2);
	m_user_multisampling_check.SetCheck(m_config.multisample_switch);
	SamplingLevelButton();

	// Configuration
	MidiOmniButton();
	MidiThruButton();
	MidiPedalButton();
	LocalModeButton();
	m_seq_clock.SetCheck(m_config.source_start_msb);
	m_clock_jack.SetCheck(m_config.source_start_lsb);
	m_seq_loop_switch.SetCheck(m_config.source_end_lsb);
	InitSlider(m_clock_rate_slider,m_clock_rate_edit,0,99,10,10);
	SetSliderValue(m_clock_rate_slider,m_clock_rate_edit,m_config.source_end_msb);
	return TRUE;
}

void CConfigParams::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	// Generic Keyboard
	SetSlider(m_mastertune_slider,m_mastertune_edit);
	SetSlider(m_PitchBend_slider,m_PitchBend_edit);
	SetSlider(m_velosens_slider,m_velosens_edit);
	SetSlider(m_balance_slider,m_balance_edit);

	// Sampling Config
	SetSlider(m_sample_time_slider,m_sample_time_edit);
	SetSlider(m_filter_freq_slider,m_filter_freq_edit);
	SetSlider(m_threshold_slider,m_threshold_edit);
	CDialog::OnVScroll(SBCode, nPos, pScrollBar);
}

void CConfigParams::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	CDialog::OnHScroll(SBCode, nPos, pScrollBar);
	SetSlider(m_clock_rate_slider,m_clock_rate_edit);
}

void CConfigParams::OnKillFocusMasterTuneEdit()
{
	SetSliderFromEdit(m_mastertune_slider,m_mastertune_edit);
}

void CConfigParams::OnKillFocusPitchbendRangeEdit()
{
	SetSliderFromEdit(m_PitchBend_slider,m_PitchBend_edit);
}

void CConfigParams::OnKillFocusVeloSensEdit()
{
	SetSliderFromEdit(m_velosens_slider,m_velosens_edit);
}

void CConfigParams::OnKillFocusBalanceEdit()
{
	SetSliderFromEdit(m_balance_slider,m_balance_edit);
}

void CConfigParams::OnKillFocusSampleTimeEdit()
{
	SetSliderFromEdit(m_sample_time_slider,m_sample_time_edit);
}

void CConfigParams::OnKillFocusFilterInputEdit()
{
	SetSliderFromEdit(m_filter_freq_slider,m_filter_freq_edit);
}

void CConfigParams::OnKillFocusThresholdEdit()
{
	SetSliderFromEdit(m_threshold_slider,m_threshold_edit);
}

void CConfigParams::OnKillFocusClockRateEdit()
{
	SetSliderFromEdit(m_clock_rate_slider,m_clock_rate_edit);
}

BEGIN_MESSAGE_MAP(CConfigParams, CDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_MASTER_TUNE_EDIT, &CConfigParams::OnKillFocusMasterTuneEdit)
	ON_BN_CLICKED(IDC_SAMPLING_LEVEL, &CConfigParams::OnBnClickedSamplingLevel)
	ON_BN_CLICKED(IDC_MIDI_OMNI, &CConfigParams::OnBnClickedMidiOmni)
	ON_BN_CLICKED(IDC_MIDI_THRU, &CConfigParams::OnBnClickedMidiThru)
	ON_BN_CLICKED(IDC_MIDI_PEDAL, &CConfigParams::OnBnClickedMidiPedal)
	ON_BN_CLICKED(IDC_LOCAL_MODE, &CConfigParams::OnBnClickedLocalMode)

	ON_EN_KILLFOCUS(IDC_MASTER_TUNE_EDIT, &CConfigParams::OnKillFocusMasterTuneEdit)
	ON_EN_KILLFOCUS(IDC_PITCHBEND_RANGE_EDIT, &CConfigParams::OnKillFocusPitchbendRangeEdit)
	ON_EN_KILLFOCUS(IDC_VELO_SENS_EDIT, &CConfigParams::OnKillFocusVeloSensEdit)
	ON_EN_KILLFOCUS(IDC_UPPERLOWER_BALANCE_EDIT, &CConfigParams::OnKillFocusBalanceEdit)
	ON_EN_KILLFOCUS(IDC_SAMPLE_TIME_EDIT, &CConfigParams::OnKillFocusSampleTimeEdit)
	ON_EN_KILLFOCUS(IDC_FILTER_FREQ_EDIT, &CConfigParams::OnKillFocusFilterInputEdit)
	ON_EN_KILLFOCUS(IDC_THRESHOLD_EDIT, &CConfigParams::OnKillFocusThresholdEdit)
	ON_EN_KILLFOCUS(IDC_CLOCK_RATE_EDIT, &CConfigParams::OnKillFocusClockRateEdit)
END_MESSAGE_MAP()

void CConfigParams::OnBnClickedSamplingLevel()
{
	m_config.mic_line_switch=!m_config.mic_line_switch;
	SamplingLevelButton();
}

void CConfigParams::SamplingLevelButton()
{
	switch(m_config.mic_line_switch)
	{
		case false:
			m_sampling_level_button.SetWindowTextA("Mic");
			m_sampling_level_button.UpdateWindow();
			break;
		case true:
			m_sampling_level_button.SetWindowTextA("Line");
			m_sampling_level_button.UpdateWindow();
			break;
	}
}

void CConfigParams::OnBnClickedMidiOmni()
{
	m_config.source_start_msb=!m_config.source_start_msb;
	MidiOmniButton();
}

void CConfigParams::MidiOmniButton()
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

void CConfigParams::OnBnClickedMidiThru()
{
	m_config.thru_mode=!m_config.thru_mode;
	MidiThruButton();
}

void CConfigParams::MidiThruButton()
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

void CConfigParams::OnBnClickedMidiPedal()
{
	m_config.dest_msb=!m_config.dest_msb;
	MidiPedalButton();
}

void CConfigParams::MidiPedalButton()
{
	switch(m_config.dest_msb)
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

void CConfigParams::OnBnClickedLocalMode()
{
	m_config.omni_mode=!m_config.omni_mode;
	LocalModeButton();
}

void CConfigParams::LocalModeButton()
{	
	switch(m_config.omni_mode)
	{
		case false:
			m_local_mode.SetWindowTextA("Local Mode Off");
			m_local_mode.UpdateWindow();
			break;
		case true:
			m_local_mode.SetWindowTextA("Local Mode On");
			m_local_mode.UpdateWindow();
			break;
	}
}
