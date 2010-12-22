// Dialog_Program.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Dialog_Program.h"


// CMirProgram dialog

IMPLEMENT_DYNAMIC(CMirProgram, CDialog)

BEGIN_MESSAGE_MAP(CMirProgram, CDialog)
	ON_WM_VSCROLL()
	ON_EN_KILLFOCUS(IDC_LFO_FREQ_EDIT, &CMirProgram::OnKillFocusLfoFreqEdit)
	ON_EN_KILLFOCUS(IDC_LFO_DEPTH_EDIT, &CMirProgram::OnKillFocusLfoDepthEdit)
	ON_EN_KILLFOCUS(IDC_CUTOFF_EDIT, &CMirProgram::OnKillFocusCutoffEdit)
	ON_EN_KILLFOCUS(IDC_RESO_EDIT, &CMirProgram::OnKillFocusResoEdit)
	ON_EN_KILLFOCUS(IDC_FILT_ATT_EDIT, &CMirProgram::OnKillFocusFAttEdit)
	ON_EN_KILLFOCUS(IDC_FILT_PEAK_EDIT, &CMirProgram::OnKillFocusFPeakEdit)
	ON_EN_KILLFOCUS(IDC_FILT_DECAY_EDIT, &CMirProgram::OnKillFocusFDecEdit)
	ON_EN_KILLFOCUS(IDC_FILT_SUS_EDIT, &CMirProgram::OnKillFocusFSusEdit)
	ON_EN_KILLFOCUS(IDC_FILT_REL_EDIT, &CMirProgram::OnKillFocusFRelEdit)

	ON_EN_KILLFOCUS(IDC_FILT_ATT_MOD_EDIT, &CMirProgram::OnKillFocusFMAttEdit)
	ON_EN_KILLFOCUS(IDC_FILT_PEAK_MOD_EDIT, &CMirProgram::OnKillFocusFMPeakEdit)
	ON_EN_KILLFOCUS(IDC_FILT_DECAY_MOD_EDIT, &CMirProgram::OnKillFocusFMDecEdit)
	ON_EN_KILLFOCUS(IDC_FILT_SUS_MOD_EDIT, &CMirProgram::OnKillFocusFMSusEdit)
	ON_EN_KILLFOCUS(IDC_FILT_REL_MOD_EDIT, &CMirProgram::OnKillFocusFMRelEdit)

	ON_EN_KILLFOCUS(IDC_AMP_ATT_EDIT, &CMirProgram::OnKillFocusAAttEdit)
	ON_EN_KILLFOCUS(IDC_AMP_PEAK_EDIT, &CMirProgram::OnKillFocusAPeakEdit)
	ON_EN_KILLFOCUS(IDC_AMP_DECAY_EDIT, &CMirProgram::OnKillFocusADecEdit)
	ON_EN_KILLFOCUS(IDC_AMP_SUS_EDIT, &CMirProgram::OnKillFocusASusEdit)
	ON_EN_KILLFOCUS(IDC_AMP_REL_EDIT, &CMirProgram::OnKillFocusARelEdit)

	ON_EN_KILLFOCUS(IDC_AMP_ATT_MOD_EDIT, &CMirProgram::OnKillFocusAMAttEdit)
	ON_EN_KILLFOCUS(IDC_AMP_PEAK_MOD_EDIT, &CMirProgram::OnKillFocusAMPeakEdit)
	ON_EN_KILLFOCUS(IDC_AMP_DECAY_MOD_EDIT, &CMirProgram::OnKillFocusAMDecEdit)
	ON_EN_KILLFOCUS(IDC_AMP_SUS_MOD_EDIT, &CMirProgram::OnKillFocusAMSusEdit)
	ON_EN_KILLFOCUS(IDC_AMP_REL_MOD_EDIT, &CMirProgram::OnKillFocusAMRelEdit)

END_MESSAGE_MAP()

CMirProgram::CMirProgram(CWnd* pParent /*=NULL*/)
	: CDialog(CMirProgram::IDD, pParent)
{
}

CMirProgram::~CMirProgram()
{
}

void CMirProgram::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LFO_FREQ_SLID, m_lfo_freq_slider);
	DDX_Control(pDX, IDC_LFO_FREQ_EDIT, m_lfo_freq_edit);
	DDX_Control(pDX, IDC_LFO_DEPTH, m_lfo_depth_slider);
	DDX_Control(pDX, IDC_LFO_DEPTH_EDIT, m_lfo_depth_edit);
	DDX_Control(pDX, IDC_FILTER_CUTOFF, m_cutoff_slider);
	DDX_Control(pDX, IDC_CUTOFF_EDIT, m_cutoff_edit);
	DDX_Control(pDX, IDC_FILTER_RESONANCE, m_reso_slider);
	DDX_Control(pDX, IDC_RESO_EDIT, m_reso_edit);
	DDX_Control(pDX, IDC_FILTER_KEYB_TRACK, m_filter_tracking_slider);
	DDX_Control(pDX, IDC_MONOMODE, m_mono_mode);
	// Filter Envelope
	DDX_Control(pDX, IDC_FILTER_ENV_ATTACK, m_f_env_att_slider);
	DDX_Control(pDX, IDC_FILTER_ENV_PEAK, m_f_env_peak_slider);
	DDX_Control(pDX, IDC_FILTER_ENV_DECAY, m_f_env_dec_slider);
	DDX_Control(pDX, IDC_FILTER_ENV_SUSTAIN, m_f_env_sus_slider);
	DDX_Control(pDX, IDC_FILTER_ENV_RELEASE, m_f_env_rel_slider);
	DDX_Control(pDX, IDC_FILT_ATT_EDIT, m_f_env_att_edit);
	DDX_Control(pDX, IDC_FILT_PEAK_EDIT, m_f_env_peak_edit);
	DDX_Control(pDX, IDC_FILT_DECAY_EDIT, m_f_env_dec_edit);
	DDX_Control(pDX, IDC_FILT_SUS_EDIT, m_f_env_sus_edit);
	DDX_Control(pDX, IDC_FILT_REL_EDIT, m_f_env_rel_edit);
	DDX_Control(pDX, IDC_FILTER_ATT_MOD, m_f_mod_att_slider);
	DDX_Control(pDX, IDC_FILTER_PEAK_MOD, m_f_mod_peak_slider);
	DDX_Control(pDX, IDC_FILTER_DEC_MOD, m_f_mod_dec_slider);
	DDX_Control(pDX, IDC_FILTER_SUS_MOD, m_f_mod_sus_slider);
	DDX_Control(pDX, IDC_FILTER_REL_MOD, m_f_mod_rel_slider);
	DDX_Control(pDX, IDC_FILT_ATT_MOD_EDIT, m_f_mod_att_edit);
	DDX_Control(pDX, IDC_FILT_PEAK_MOD_EDIT, m_f_mod_peak_edit);
	DDX_Control(pDX, IDC_FILT_DECAY_MOD_EDIT, m_f_mod_dec_edit);
	DDX_Control(pDX, IDC_FILT_SUS_MOD_EDIT, m_f_mod_sus_edit);
	DDX_Control(pDX, IDC_FILT_REL_MOD_EDIT, m_f_mod_rel_edit);
	// Amplitude Envelope
	DDX_Control(pDX, IDC_AMP_ENV_ATTACK, m_a_env_att_slider);
	DDX_Control(pDX, IDC_AMP_ENV_PEAK, m_a_env_peak_slider);
	DDX_Control(pDX, IDC_AMP_ENV_DECAY, m_a_env_dec_slider);
	DDX_Control(pDX, IDC_AMP_ENV_SUSTAIN, m_a_env_sus_slider);
	DDX_Control(pDX, IDC_AMP_ENV_RELEASE, m_a_env_rel_slider);
	DDX_Control(pDX, IDC_AMP_ATT_EDIT, m_a_env_att_edit);
	DDX_Control(pDX, IDC_AMP_PEAK_EDIT, m_a_env_peak_edit);
	DDX_Control(pDX, IDC_AMP_DECAY_EDIT, m_a_env_dec_edit);
	DDX_Control(pDX, IDC_AMP_SUS_EDIT, m_a_env_sus_edit);
	DDX_Control(pDX, IDC_AMP_REL_EDIT, m_a_env_rel_edit);
	DDX_Control(pDX, IDC_AMP_ATT_MOD, m_a_mod_att_slider);
	DDX_Control(pDX, IDC_AMP_PEAK_MOD, m_a_mod_peak_slider);
	DDX_Control(pDX, IDC_AMP_DEC_MOD, m_a_mod_dec_slider);
	DDX_Control(pDX, IDC_AMP_SUS_MOD, m_a_mod_sus_slider);
	DDX_Control(pDX, IDC_AMP_REL_MOD, m_a_mod_rel_slider);
	DDX_Control(pDX, IDC_AMP_ATT_MOD_EDIT, m_a_mod_att_edit);
	DDX_Control(pDX, IDC_AMP_PEAK_MOD_EDIT, m_a_mod_peak_edit);
	DDX_Control(pDX, IDC_AMP_DECAY_MOD_EDIT, m_a_mod_dec_edit);
	DDX_Control(pDX, IDC_AMP_SUS_MOD_EDIT, m_a_mod_sus_edit);
	DDX_Control(pDX, IDC_AMP_REL_MOD_EDIT, m_a_mod_rel_edit);
}

BOOL CMirProgram::OnInitDialog()
{
	CString str;
	CDialog::OnInitDialog();

	/* Init Sliders for LFO */
	InitSlider(m_lfo_freq_slider,m_lfo_freq_edit,0,99,10,10);
	InitSlider(m_lfo_depth_slider,m_lfo_depth_edit,0,99,10,10);
	SetSliderValue(m_lfo_freq_slider,m_lfo_freq_edit,0);
	SetSliderValue(m_lfo_depth_slider,m_lfo_depth_edit,0);

	/* Init Sliders for filter */
	InitSlider(m_cutoff_slider,m_cutoff_edit,0,99,10,10);
	InitSlider(m_reso_slider,m_reso_edit,0,40,10,10);
	SetSliderValue(m_cutoff_slider,m_cutoff_edit,0);
	SetSliderValue(m_reso_slider,m_reso_edit,0);

	m_filter_tracking_slider.SetRange(0,4);
	m_filter_tracking_slider.SetTicFreq(1);
	m_filter_tracking_slider.SetPageSize(1);
	m_filter_tracking_slider.SetPos(2);

	/* Init Slider for Filter envelope part 1 */
	InitSlider(m_f_env_att_slider,m_f_env_att_edit,0,31,5,5);
	InitSlider(m_f_env_peak_slider,m_f_env_peak_edit,0,31,5,5);
	InitSlider(m_f_env_dec_slider,m_f_env_dec_edit,0,31,5,5);
	InitSlider(m_f_env_sus_slider,m_f_env_sus_edit,0,31,5,5);
	InitSlider(m_f_env_rel_slider,m_f_env_rel_edit,0,31,5,5);
	SetSliderValue(m_f_env_att_slider,m_f_env_att_edit,0);
	SetSliderValue(m_f_env_peak_slider,m_f_env_peak_edit,0);
	SetSliderValue(m_f_env_dec_slider,m_f_env_dec_edit,0);
	SetSliderValue(m_f_env_sus_slider,m_f_env_sus_edit,0);
	SetSliderValue(m_f_env_rel_slider,m_f_env_rel_edit,0);

	/* Init Slider for Filter envelope part 2 */
	InitSlider(m_f_mod_att_slider,m_f_mod_att_edit,0,31,5,5);
	InitSlider(m_f_mod_peak_slider,m_f_mod_peak_edit,0,31,5,5);
	InitSlider(m_f_mod_dec_slider,m_f_mod_dec_edit,0,31,5,5);
	InitSlider(m_f_mod_sus_slider,m_f_mod_sus_edit,0,31,5,5);
	InitSlider(m_f_mod_rel_slider,m_f_mod_rel_edit,0,31,5,5);
	SetSliderValue(m_f_mod_att_slider,m_f_mod_att_edit,0);
	SetSliderValue(m_f_mod_peak_slider,m_f_mod_peak_edit,0);
	SetSliderValue(m_f_mod_dec_slider,m_f_mod_dec_edit,0);
	SetSliderValue(m_f_mod_sus_slider,m_f_mod_sus_edit,0);
	SetSliderValue(m_f_mod_rel_slider,m_f_mod_rel_edit,0);

	/* Init Slider for Amplitude envelope part 1 */
	InitSlider(m_a_env_att_slider,m_a_env_att_edit,0,31,5,5);
	InitSlider(m_a_env_peak_slider,m_a_env_peak_edit,0,31,5,5);
	InitSlider(m_a_env_dec_slider,m_a_env_dec_edit,0,31,5,5);
	InitSlider(m_a_env_sus_slider,m_a_env_sus_edit,0,31,5,5);
	InitSlider(m_a_env_rel_slider,m_a_env_rel_edit,0,31,5,5);
	SetSliderValue(m_a_env_att_slider,m_a_env_att_edit,0);
	SetSliderValue(m_a_env_peak_slider,m_a_env_peak_edit,0);
	SetSliderValue(m_a_env_dec_slider,m_a_env_dec_edit,0);
	SetSliderValue(m_a_env_sus_slider,m_a_env_sus_edit,0);
	SetSliderValue(m_a_env_rel_slider,m_a_env_rel_edit,0);

	/* Init Slider for Amplitude envelope part 2 */
	InitSlider(m_a_mod_att_slider,m_a_mod_att_edit,0,31,5,5);
	InitSlider(m_a_mod_peak_slider,m_a_mod_peak_edit,0,31,5,5);
	InitSlider(m_a_mod_dec_slider,m_a_mod_dec_edit,0,31,5,5);
	InitSlider(m_a_mod_sus_slider,m_a_mod_sus_edit,0,31,5,5);
	InitSlider(m_a_mod_rel_slider,m_a_mod_rel_edit,0,31,5,5);
	SetSliderValue(m_a_mod_att_slider,m_a_mod_att_edit,0);
	SetSliderValue(m_a_mod_peak_slider,m_a_mod_peak_edit,0);
	SetSliderValue(m_a_mod_dec_slider,m_a_mod_dec_edit,0);
	SetSliderValue(m_a_mod_sus_slider,m_a_mod_sus_edit,0);
	SetSliderValue(m_a_mod_rel_slider,m_a_mod_rel_edit,0);

	return TRUE;
}


void CMirProgram::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	CString str;
	/* LFO Sliders */
	SetSlider(m_lfo_freq_slider,m_lfo_freq_edit);
	SetSlider(m_lfo_depth_slider,m_lfo_depth_edit);

	/* Filter Sliders */
	SetSlider(m_cutoff_slider,m_cutoff_edit);
	SetSlider(m_reso_slider,m_reso_edit);

	/* Filter Envelope Sliders part 1 */
	SetSlider(m_f_env_att_slider,m_f_env_att_edit);
	SetSlider(m_f_env_peak_slider,m_f_env_peak_edit);
	SetSlider(m_f_env_dec_slider,m_f_env_dec_edit);
	SetSlider(m_f_env_sus_slider,m_f_env_sus_edit);
	SetSlider(m_f_env_rel_slider,m_f_env_rel_edit);
	/* Filter Envelope Sliders part 2 */
	SetSlider(m_f_mod_att_slider,m_f_mod_att_edit);
	SetSlider(m_f_mod_peak_slider,m_f_mod_peak_edit);
	SetSlider(m_f_mod_dec_slider,m_f_mod_dec_edit);
	SetSlider(m_f_mod_sus_slider,m_f_mod_sus_edit);
	SetSlider(m_f_mod_rel_slider,m_f_mod_rel_edit);

	/* Amplitude Envelope Sliders part 1 */
	SetSlider(m_a_env_att_slider,m_a_env_att_edit);
	SetSlider(m_a_env_peak_slider,m_a_env_peak_edit);
	SetSlider(m_a_env_dec_slider,m_a_env_dec_edit);
	SetSlider(m_a_env_sus_slider,m_a_env_sus_edit);
	SetSlider(m_a_env_rel_slider,m_a_env_rel_edit);
	/* Amplitude Envelope Sliders part 2 */
	SetSlider(m_a_mod_att_slider,m_a_mod_att_edit);
	SetSlider(m_a_mod_peak_slider,m_a_mod_peak_edit);
	SetSlider(m_a_mod_dec_slider,m_a_mod_dec_edit);
	SetSlider(m_a_mod_sus_slider,m_a_mod_sus_edit);
	SetSlider(m_a_mod_rel_slider,m_a_mod_rel_edit);
	CDialog::OnVScroll(SBCode, nPos, pScrollBar);
}

void CMirProgram::OnKillFocusLfoFreqEdit()
{
	CString str;
//	m_lfo_freq_edit.GetWindowTextA(str);
	SetSliderFromEdit(m_lfo_freq_slider,m_lfo_freq_edit);
}

void CMirProgram::OnKillFocusLfoDepthEdit()
{
	CString str;
	SetSliderFromEdit(m_lfo_depth_slider,m_lfo_depth_edit);
}

void CMirProgram::OnKillFocusCutoffEdit()
{
	CString str;
	SetSliderFromEdit(m_cutoff_slider,m_cutoff_edit);
}

void CMirProgram::OnKillFocusResoEdit()
{
	CString str;
	SetSliderFromEdit(m_reso_slider,m_reso_edit);
}

void CMirProgram::OnKillFocusFAttEdit()
{
	CString str;
	SetSliderFromEdit(m_f_env_att_slider,m_f_env_att_edit);
}

void CMirProgram::OnKillFocusFPeakEdit()
{
	CString str;
	SetSliderFromEdit(m_f_env_peak_slider,m_f_env_peak_edit);
}

void CMirProgram::OnKillFocusFDecEdit()
{
	CString str;
	SetSliderFromEdit(m_f_env_dec_slider,m_f_env_dec_edit);
}

void CMirProgram::OnKillFocusFSusEdit()
{
	CString str;
	SetSliderFromEdit(m_f_env_sus_slider,m_f_env_sus_edit);
}

void CMirProgram::OnKillFocusFRelEdit()
{
	CString str;
	SetSliderFromEdit(m_f_env_rel_slider,m_f_env_rel_edit);
}

void CMirProgram::OnKillFocusAAttEdit()
{
	CString str;
	SetSliderFromEdit(m_a_env_att_slider,m_a_env_att_edit);
}

void CMirProgram::OnKillFocusAPeakEdit()
{
	CString str;
	SetSliderFromEdit(m_a_env_peak_slider,m_a_env_peak_edit);
}

void CMirProgram::OnKillFocusADecEdit()
{
	CString str;
	SetSliderFromEdit(m_a_env_dec_slider,m_a_env_dec_edit);
}

void CMirProgram::OnKillFocusASusEdit()
{
	CString str;
	SetSliderFromEdit(m_a_env_sus_slider,m_a_env_sus_edit);
}

void CMirProgram::OnKillFocusARelEdit()
{
	CString str;
	SetSliderFromEdit(m_a_env_rel_slider,m_a_env_rel_edit);
}

void CMirProgram::OnKillFocusFMAttEdit()
{
	CString str;
	SetSliderFromEdit(m_f_mod_att_slider,m_f_mod_att_edit);
}

void CMirProgram::OnKillFocusFMPeakEdit()
{
	CString str;
	SetSliderFromEdit(m_f_mod_peak_slider,m_f_mod_peak_edit);
}

void CMirProgram::OnKillFocusFMDecEdit()
{
	CString str;
	SetSliderFromEdit(m_f_mod_dec_slider,m_f_mod_dec_edit);
}

void CMirProgram::OnKillFocusFMSusEdit()
{
	CString str;
	SetSliderFromEdit(m_f_mod_sus_slider,m_f_mod_sus_edit);
}

void CMirProgram::OnKillFocusFMRelEdit()
{
	CString str;
	SetSliderFromEdit(m_f_mod_rel_slider,m_f_mod_rel_edit);
}

void CMirProgram::OnKillFocusAMAttEdit()
{
	CString str;
	SetSliderFromEdit(m_a_mod_att_slider,m_a_mod_att_edit);
}

void CMirProgram::OnKillFocusAMPeakEdit()
{
	CString str;
	SetSliderFromEdit(m_a_mod_peak_slider,m_a_mod_peak_edit);
}

void CMirProgram::OnKillFocusAMDecEdit()
{
	CString str;
	SetSliderFromEdit(m_a_mod_dec_slider,m_a_mod_dec_edit);
}

void CMirProgram::OnKillFocusAMSusEdit()
{
	CString str;
	SetSliderFromEdit(m_a_mod_sus_slider,m_a_mod_sus_edit);
}

void CMirProgram::OnKillFocusAMRelEdit()
{
	CString str;
	SetSliderFromEdit(m_a_mod_rel_slider,m_a_mod_rel_edit);
}

