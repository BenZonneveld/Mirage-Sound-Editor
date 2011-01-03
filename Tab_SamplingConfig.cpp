// Tab_SamplingConfig.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "macros.h"
#include "MirageSysex.h"
#include "Tab_SamplingConfig.h"


// CTabSamplingConfig dialog

IMPLEMENT_DYNAMIC(CTabSamplingConfig, CDialog)

CTabSamplingConfig::CTabSamplingConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CTabSamplingConfig::IDD, pParent)
{

}

CTabSamplingConfig::~CTabSamplingConfig()
{
}

void CTabSamplingConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAMPLE_TIME_SLIDER, m_sample_time_slider);
	DDX_Control(pDX, IDC_SAMPLE_TIME_EDIT, m_sample_time_edit);
	DDX_Control(pDX, IDC_FILTER_FREQ_SLIDER, m_filter_freq_slider);
	DDX_Control(pDX, IDC_FILTER_FREQ_EDIT, m_filter_freq_edit);
	DDX_Control(pDX, IDC_THRESHOLD_EDIT, m_threshold_edit);
	DDX_Control(pDX, IDC_THRESHOLD_SLIDER, m_threshold_slider);
	DDX_Control(pDX, IDC_USER_MULTISAMPLING, m_user_multisampling_check);
	DDX_Control(pDX, IDC_SAMPLING_LEVEL, m_sampling_level_button);
}

BOOL CTabSamplingConfig::OnInitDialog()
{
	CDialog::OnInitDialog();
	memcpy(&m_config,&ConfigDump,sizeof(ConfigDump));

	// Sampling Config
	InitSlider(m_sample_time_slider,m_sample_time_edit,20,99,10,10);
	InitSlider(m_filter_freq_slider,m_filter_freq_edit,0,99,10,10);
	InitSlider(m_threshold_slider,m_threshold_edit,0,63,10,10);
	SetSliderValue(m_sample_time_slider,m_sample_time_edit,m_config.sample_time_adjust+20);
	SetSliderValue(m_filter_freq_slider,m_filter_freq_edit,m_config.input_filter_freq/2);
	SetSliderValue(m_threshold_slider,m_threshold_edit,m_config.sample_treshold/2);
	m_user_multisampling_check.SetCheck(m_config.multisample_switch);
	SamplingLevelButton();
	
	return TRUE;
}

void CTabSamplingConfig::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	// Sampling Config
	SetSlider(m_sample_time_slider,m_sample_time_edit);
	SetSlider(m_filter_freq_slider,m_filter_freq_edit);
	SetSlider(m_threshold_slider,m_threshold_edit);
	CDialog::OnVScroll(SBCode, nPos, pScrollBar);
}

void CTabSamplingConfig::OnKillFocusSampleTimeEdit()
{
	SetSliderFromEdit(m_sample_time_slider,m_sample_time_edit);
}

void CTabSamplingConfig::OnKillFocusFilterInputEdit()
{
	SetSliderFromEdit(m_filter_freq_slider,m_filter_freq_edit);
}

void CTabSamplingConfig::OnKillFocusThresholdEdit()
{
	SetSliderFromEdit(m_threshold_slider,m_threshold_edit);
}

void CTabSamplingConfig::OnBnClickedSamplingLevel()
{
	m_config.mic_line_switch=!m_config.mic_line_switch;
	SamplingLevelButton();
}

void CTabSamplingConfig::SamplingLevelButton()
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

void CTabSamplingConfig::OnDialogOk()
{
	char string[3];
	ConfigDump.mic_line_switch = m_config.mic_line_switch;
	m_sample_time_edit.GetLine(0,(LPTSTR)string,2);
	ConfigDump.sample_time_adjust = atoi(string);
	ConfigDump.input_filter_freq = 2 * (m_filter_freq_slider.GetRangeMax()-m_filter_freq_slider.GetPos());
	ConfigDump.sample_treshold = 2 * (m_threshold_slider.GetRangeMax()-m_threshold_slider.GetPos());
	ConfigDump.multisample_switch = m_user_multisampling_check.GetCheck();

	CWnd::DestroyWindow();
}

BEGIN_MESSAGE_MAP(CTabSamplingConfig, CDialog)
	ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_SAMPLING_LEVEL, &CTabSamplingConfig::OnBnClickedSamplingLevel)
	ON_EN_KILLFOCUS(IDC_SAMPLE_TIME_EDIT, &CTabSamplingConfig::OnKillFocusSampleTimeEdit)
	ON_EN_KILLFOCUS(IDC_FILTER_FREQ_EDIT, &CTabSamplingConfig::OnKillFocusFilterInputEdit)
	ON_EN_KILLFOCUS(IDC_THRESHOLD_EDIT, &CTabSamplingConfig::OnKillFocusThresholdEdit)


END_MESSAGE_MAP()


// CTabSamplingConfig message handlers
