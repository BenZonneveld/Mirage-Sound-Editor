// Dialog_Program.cpp : implementation file
//

#include "stdafx.h"
#include "../Mirage Editor.h"
#include "Dialog_Program.h"
#include "../Midi/MirageSysex.h"
#include "../macros.h"


// CMirProgram dialog

IMPLEMENT_DYNAMIC(CMirProgram, CDialog)

BEGIN_MESSAGE_MAP(CMirProgram, CDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	// Keyboard
	ON_EN_KILLFOCUS(IDC_LFO_FREQ_EDIT, &CMirProgram::OnKillFocusLfoFreqEdit)
	ON_EN_KILLFOCUS(IDC_LFO_DEPTH_EDIT, &CMirProgram::OnKillFocusLfoDepthEdit)
	ON_EN_KILLFOCUS(IDC_CUTOFF_EDIT, &CMirProgram::OnKillFocusCutoffEdit)
	ON_EN_KILLFOCUS(IDC_RESO_EDIT, &CMirProgram::OnKillFocusResoEdit)
	// Filter Env part 1
	ON_EN_KILLFOCUS(IDC_FILT_ATT_EDIT, &CMirProgram::OnKillFocusFAttEdit)
	ON_EN_KILLFOCUS(IDC_FILT_PEAK_EDIT, &CMirProgram::OnKillFocusFPeakEdit)
	ON_EN_KILLFOCUS(IDC_FILT_DECAY_EDIT, &CMirProgram::OnKillFocusFDecEdit)
	ON_EN_KILLFOCUS(IDC_FILT_SUS_EDIT, &CMirProgram::OnKillFocusFSusEdit)
	ON_EN_KILLFOCUS(IDC_FILT_REL_EDIT, &CMirProgram::OnKillFocusFRelEdit)
	// Filter Env Part 2
	ON_EN_KILLFOCUS(IDC_FILT_ATT_MOD_EDIT, &CMirProgram::OnKillFocusFMAttEdit)
	ON_EN_KILLFOCUS(IDC_FILT_PEAK_MOD_EDIT, &CMirProgram::OnKillFocusFMPeakEdit)
	ON_EN_KILLFOCUS(IDC_FILT_DECAY_MOD_EDIT, &CMirProgram::OnKillFocusFMDecEdit)
	ON_EN_KILLFOCUS(IDC_FILT_SUS_MOD_EDIT, &CMirProgram::OnKillFocusFMSusEdit)
	ON_EN_KILLFOCUS(IDC_FILT_REL_MOD_EDIT, &CMirProgram::OnKillFocusFMRelEdit)
	// Amplitude Env Part 1
	ON_EN_KILLFOCUS(IDC_AMP_ATT_EDIT, &CMirProgram::OnKillFocusAAttEdit)
	ON_EN_KILLFOCUS(IDC_AMP_PEAK_EDIT, &CMirProgram::OnKillFocusAPeakEdit)
	ON_EN_KILLFOCUS(IDC_AMP_DECAY_EDIT, &CMirProgram::OnKillFocusADecEdit)
	ON_EN_KILLFOCUS(IDC_AMP_SUS_EDIT, &CMirProgram::OnKillFocusASusEdit)
	ON_EN_KILLFOCUS(IDC_AMP_REL_EDIT, &CMirProgram::OnKillFocusARelEdit)
	// Amplitude Env Part 2
	ON_EN_KILLFOCUS(IDC_AMP_ATT_MOD_EDIT, &CMirProgram::OnKillFocusAMAttEdit)
	ON_EN_KILLFOCUS(IDC_AMP_PEAK_MOD_EDIT, &CMirProgram::OnKillFocusAMPeakEdit)
	ON_EN_KILLFOCUS(IDC_AMP_DECAY_MOD_EDIT, &CMirProgram::OnKillFocusAMDecEdit)
	ON_EN_KILLFOCUS(IDC_AMP_SUS_MOD_EDIT, &CMirProgram::OnKillFocusAMSusEdit)
	ON_EN_KILLFOCUS(IDC_AMP_REL_MOD_EDIT, &CMirProgram::OnKillFocusAMRelEdit)
	// WaveSample
	ON_EN_KILLFOCUS(IDC_INIT_WAVE_EDIT, &CMirProgram::OnKillFocusInitWaveEdit)
	ON_EN_KILLFOCUS(IDC_OSC_DET_EDIT, &CMirProgram::OnKillFocusOscDetEdit)
	ON_EN_KILLFOCUS(IDC_OSC_MIX_EDIT, &CMirProgram::OnKillFocusOscMixEdit)
	ON_EN_KILLFOCUS(IDC_OSC_VEL_EDIT, &CMirProgram::OnKillFocusOscVelEdit)

	ON_BN_CLICKED(IDC_UPPER_LOWER, &CMirProgram::OnBnClickedUpperLower)
END_MESSAGE_MAP()

CMirProgram::CMirProgram(CWnd* pParent /*=NULL*/)
	: CDialog(CMirProgram::IDD, pParent)
	, m_MixMode(FALSE)
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
	DDX_Check(pDX, IDC_MIXMODE_CHECK, m_MixMode);
	DDX_Control(pDX, IDC_INIT_WAVE_SLIDER, m_InitWave_slider);
	DDX_Control(pDX, IDC_INIT_WAVE_EDIT, m_InitWave_edit);
	DDX_Control(pDX, IDC_OSC_DET_SLIDER, m_OscDet_slider);
	DDX_Control(pDX, IDC_OSC_MIX_SLIDER, m_OscMix_slider);
	DDX_Control(pDX, IDC_OSC_VELO_SLIDER, m_OscVel_slider);
	DDX_Control(pDX, IDC_OSC_DET_EDIT, m_OscDet_edit);
	DDX_Control(pDX, IDC_OSC_MIX_EDIT, m_OscMix_edit);
	DDX_Control(pDX, IDC_OSC_VEL_EDIT, m_OscVel_edit);
	DDX_Control(pDX, IDC_PGM_SLIDER, m_pgm_select);
	DDX_Control(pDX, IDC_UPPER_LOWER, m_upper_lower);
	DDX_Control(pDX, IDC_MIXMODE_CHECK, m_mixmode_check);
}

BOOL CMirProgram::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bLowerUpperPgm=false;
	m_Program=0;

	memcpy(&ProgramParmBlock,&ProgramDumpTable->ProgramParameterBlock[m_Program],sizeof(ProgramParmBlock));

	/* Program Selection Slider */
	m_pgm_select.SetRange(1,4);
	m_pgm_select.SetTicFreq(1);
	m_pgm_select.SetPageSize(1);

	m_mono_mode.SetCheck(ProgramParmBlock.MonoModeSwitch);

	/* Init Sliders for LFO */
	InitSlider(m_lfo_freq_slider,m_lfo_freq_edit,0,99,10,10);
	InitSlider(m_lfo_depth_slider,m_lfo_depth_edit,0,99,10,10);
	SetSliderValue(m_lfo_freq_slider,m_lfo_freq_edit,ProgramParmBlock.LFO_Freq);
	SetSliderValue(m_lfo_depth_slider,m_lfo_depth_edit,ProgramParmBlock.LFO_Depth);

	/* Init Sliders for filter */
	InitSlider(m_cutoff_slider,m_cutoff_edit,0,99,10,10);
	InitSlider(m_reso_slider,m_reso_edit,0,40,10,10);
	SetSliderValue(m_cutoff_slider,m_cutoff_edit,ProgramParmBlock.FilterCutOff/2);
	SetSliderValue(m_reso_slider,m_reso_edit,ProgramParmBlock.Resonance/4);

	m_filter_tracking_slider.SetRange(0,4);
	m_filter_tracking_slider.SetTicFreq(1);
	m_filter_tracking_slider.SetPageSize(1);
	m_filter_tracking_slider.SetPos(ProgramParmBlock.FilterKeyboardTracking);

	/* Init Slider for Filter envelope part 1 */
	InitSlider(m_f_env_att_slider,m_f_env_att_edit,0,31,5,5);
	InitSlider(m_f_env_peak_slider,m_f_env_peak_edit,0,31,5,5);
	InitSlider(m_f_env_dec_slider,m_f_env_dec_edit,0,31,5,5);
	InitSlider(m_f_env_sus_slider,m_f_env_sus_edit,0,31,5,5);
	InitSlider(m_f_env_rel_slider,m_f_env_rel_edit,0,31,5,5);
	SetSliderValue(m_f_env_att_slider,m_f_env_att_edit,ProgramParmBlock.FiltEnvAttack);
	SetSliderValue(m_f_env_peak_slider,m_f_env_peak_edit,ProgramParmBlock.FiltEnvPeak);
	SetSliderValue(m_f_env_dec_slider,m_f_env_dec_edit,ProgramParmBlock.FiltEnvDecay);
	SetSliderValue(m_f_env_sus_slider,m_f_env_sus_edit,ProgramParmBlock.FiltEnvSustain);
	SetSliderValue(m_f_env_rel_slider,m_f_env_rel_edit,ProgramParmBlock.FiltEnvRelease);

	/* Init Slider for Filter envelope part 2 */
	InitSlider(m_f_mod_att_slider,m_f_mod_att_edit,0,31,5,5);
	InitSlider(m_f_mod_peak_slider,m_f_mod_peak_edit,0,31,5,5);
	InitSlider(m_f_mod_dec_slider,m_f_mod_dec_edit,0,31,5,5);
	InitSlider(m_f_mod_sus_slider,m_f_mod_sus_edit,0,31,5,5);
	InitSlider(m_f_mod_rel_slider,m_f_mod_rel_edit,0,31,5,5);
	SetSliderValue(m_f_mod_att_slider,m_f_mod_att_edit,ProgramParmBlock.FiltAttackVelo);
	SetSliderValue(m_f_mod_peak_slider,m_f_mod_peak_edit,ProgramParmBlock.FiltPeakVelo);
	SetSliderValue(m_f_mod_dec_slider,m_f_mod_dec_edit,ProgramParmBlock.FiltDecayKeyScale);
	SetSliderValue(m_f_mod_sus_slider,m_f_mod_sus_edit,ProgramParmBlock.FiltSustainVelo);
	SetSliderValue(m_f_mod_rel_slider,m_f_mod_rel_edit,ProgramParmBlock.FiltReleaseVelo);

	/* Init Slider for Amplitude envelope part 1 */
	InitSlider(m_a_env_att_slider,m_a_env_att_edit,0,31,5,5);
	InitSlider(m_a_env_peak_slider,m_a_env_peak_edit,0,31,5,5);
	InitSlider(m_a_env_dec_slider,m_a_env_dec_edit,0,31,5,5);
	InitSlider(m_a_env_sus_slider,m_a_env_sus_edit,0,31,5,5);
	InitSlider(m_a_env_rel_slider,m_a_env_rel_edit,0,31,5,5);
	SetSliderValue(m_a_env_att_slider,m_a_env_att_edit,ProgramParmBlock.AmpEnvAttack);
	SetSliderValue(m_a_env_peak_slider,m_a_env_peak_edit,ProgramParmBlock.AmpEnvPeak);
	SetSliderValue(m_a_env_dec_slider,m_a_env_dec_edit,ProgramParmBlock.AmpEnvDecay);
	SetSliderValue(m_a_env_sus_slider,m_a_env_sus_edit,ProgramParmBlock.AmpEnvSustain);
	SetSliderValue(m_a_env_rel_slider,m_a_env_rel_edit,ProgramParmBlock.AmpEnvRelease);

	/* Init Slider for Amplitude envelope part 2 */
	InitSlider(m_a_mod_att_slider,m_a_mod_att_edit,0,31,5,5);
	InitSlider(m_a_mod_peak_slider,m_a_mod_peak_edit,0,31,5,5);
	InitSlider(m_a_mod_dec_slider,m_a_mod_dec_edit,0,31,5,5);
	InitSlider(m_a_mod_sus_slider,m_a_mod_sus_edit,0,31,5,5);
	InitSlider(m_a_mod_rel_slider,m_a_mod_rel_edit,0,31,5,5);
	SetSliderValue(m_a_mod_att_slider,m_a_mod_att_edit,ProgramParmBlock.AmpAttackVelo);
	SetSliderValue(m_a_mod_peak_slider,m_a_mod_peak_edit,ProgramParmBlock.AmpPeakVelo);
	SetSliderValue(m_a_mod_dec_slider,m_a_mod_dec_edit,ProgramParmBlock.AmpDecayKeyScale);
	SetSliderValue(m_a_mod_sus_slider,m_a_mod_sus_edit,ProgramParmBlock.AmpSustainVelo);
	SetSliderValue(m_a_mod_rel_slider,m_a_mod_rel_edit,ProgramParmBlock.AmpReleaseVelo);

	/* Init Sliders for wavesample */
	InitSlider(m_InitWave_slider,m_InitWave_edit,1,8,1,1);
	InitSlider(m_OscDet_slider,m_OscDet_edit,0,99,10,10);
	InitSlider(m_OscMix_slider,m_OscMix_edit,0,63,10,5);
	InitSlider(m_OscVel_slider,m_OscVel_edit,0,31,5,5);
	SetSliderValue(m_InitWave_slider,m_InitWave_edit,ProgramParmBlock.InitialWavesample);
	SetSliderValue(m_OscDet_slider,m_OscDet_edit,ProgramParmBlock.Osc_Detune);
	SetSliderValue(m_OscMix_slider,m_OscMix_edit,ProgramParmBlock.Osc_Mix);
	SetSliderValue(m_OscVel_slider,m_OscVel_edit,ProgramParmBlock.MixVelSens);
	m_mixmode_check.SetCheck(ProgramParmBlock.MixModeSwitch);
	return TRUE;
}

void CMirProgram::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
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
	/* WaveSample Sliders */
	SetSlider(m_InitWave_slider,m_InitWave_edit);
	SetSlider(m_OscDet_slider,m_OscDet_edit);
	SetSlider(m_OscMix_slider,m_OscMix_edit);
	SetSlider(m_OscVel_slider,m_OscVel_edit);
	CDialog::OnVScroll(SBCode, nPos, pScrollBar);
}

void CMirProgram::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	m_Program = m_pgm_select.GetPos()-1;

	memcpy(&ProgramParmBlock,
					&ProgramDumpTable->ProgramParameterBlock[m_Program],
					sizeof(ProgramParmBlock));

	m_mono_mode.SetCheck(ProgramParmBlock.MonoModeSwitch);

	// LFO
	SetSliderValue(m_lfo_freq_slider,m_lfo_freq_edit,ProgramParmBlock.LFO_Freq);
	SetSliderValue(m_lfo_depth_slider,m_lfo_depth_edit,ProgramParmBlock.LFO_Depth);
	// Filter Settings
	SetSliderValue(m_cutoff_slider,m_cutoff_edit,ProgramParmBlock.FilterCutOff/2);
	SetSliderValue(m_reso_slider,m_reso_edit,ProgramParmBlock.Resonance/4);
	m_filter_tracking_slider.SetPos(ProgramParmBlock.FilterKeyboardTracking);
	// Filter Envelope
	SetSliderValue(m_f_env_att_slider,m_f_env_att_edit,ProgramParmBlock.FiltEnvAttack);
	SetSliderValue(m_f_env_peak_slider,m_f_env_peak_edit,ProgramParmBlock.FiltEnvPeak);
	SetSliderValue(m_f_env_dec_slider,m_f_env_dec_edit,ProgramParmBlock.FiltEnvDecay);
	SetSliderValue(m_f_env_sus_slider,m_f_env_sus_edit,ProgramParmBlock.FiltEnvSustain);
	SetSliderValue(m_f_env_rel_slider,m_f_env_rel_edit,ProgramParmBlock.FiltEnvRelease);
	SetSliderValue(m_f_mod_att_slider,m_f_mod_att_edit,ProgramParmBlock.FiltAttackVelo);
	SetSliderValue(m_f_mod_peak_slider,m_f_mod_peak_edit,ProgramParmBlock.FiltPeakVelo);
	SetSliderValue(m_f_mod_dec_slider,m_f_mod_dec_edit,ProgramParmBlock.FiltDecayKeyScale);
	SetSliderValue(m_f_mod_sus_slider,m_f_mod_sus_edit,ProgramParmBlock.FiltSustainVelo);
	SetSliderValue(m_f_mod_rel_slider,m_f_mod_rel_edit,ProgramParmBlock.FiltReleaseVelo);
	// Amplitude Envelope
	SetSliderValue(m_a_env_att_slider,m_a_env_att_edit,ProgramParmBlock.AmpEnvAttack);
	SetSliderValue(m_a_env_peak_slider,m_a_env_peak_edit,ProgramParmBlock.AmpEnvPeak);
	SetSliderValue(m_a_env_dec_slider,m_a_env_dec_edit,ProgramParmBlock.AmpEnvDecay);
	SetSliderValue(m_a_env_sus_slider,m_a_env_sus_edit,ProgramParmBlock.AmpEnvSustain);
	SetSliderValue(m_a_env_rel_slider,m_a_env_rel_edit,ProgramParmBlock.AmpEnvRelease);
	SetSliderValue(m_a_mod_att_slider,m_a_mod_att_edit,ProgramParmBlock.AmpAttackVelo);
	SetSliderValue(m_a_mod_peak_slider,m_a_mod_peak_edit,ProgramParmBlock.AmpPeakVelo);
	SetSliderValue(m_a_mod_dec_slider,m_a_mod_dec_edit,ProgramParmBlock.AmpDecayKeyScale);
	SetSliderValue(m_a_mod_sus_slider,m_a_mod_sus_edit,ProgramParmBlock.AmpSustainVelo);
	SetSliderValue(m_a_mod_rel_slider,m_a_mod_rel_edit,ProgramParmBlock.AmpReleaseVelo);
	// Sliders for Wavesample
	SetSliderValue(m_InitWave_slider,m_InitWave_edit,ProgramParmBlock.InitialWavesample);
	SetSliderValue(m_OscDet_slider,m_OscDet_edit,ProgramParmBlock.Osc_Detune);
	SetSliderValue(m_OscMix_slider,m_OscMix_edit,ProgramParmBlock.Osc_Mix);
	SetSliderValue(m_OscVel_slider,m_OscVel_edit,ProgramParmBlock.MixVelSens);
	m_mixmode_check.SetCheck(ProgramParmBlock.MixModeSwitch);
	CDialog::OnHScroll(SBCode, nPos, pScrollBar);
}

void CMirProgram::OnKillFocusLfoFreqEdit()
{
	SetSliderFromEdit(m_lfo_freq_slider,m_lfo_freq_edit);
}

void CMirProgram::OnKillFocusLfoDepthEdit()
{
	SetSliderFromEdit(m_lfo_depth_slider,m_lfo_depth_edit);
}

void CMirProgram::OnKillFocusCutoffEdit()
{
	SetSliderFromEdit(m_cutoff_slider,m_cutoff_edit);
}

void CMirProgram::OnKillFocusResoEdit()
{
	SetSliderFromEdit(m_reso_slider,m_reso_edit);
}

void CMirProgram::OnKillFocusFAttEdit()
{
	SetSliderFromEdit(m_f_env_att_slider,m_f_env_att_edit);
}

void CMirProgram::OnKillFocusFPeakEdit()
{
	SetSliderFromEdit(m_f_env_peak_slider,m_f_env_peak_edit);
}

void CMirProgram::OnKillFocusFDecEdit()
{
	SetSliderFromEdit(m_f_env_dec_slider,m_f_env_dec_edit);
}

void CMirProgram::OnKillFocusFSusEdit()
{
	SetSliderFromEdit(m_f_env_sus_slider,m_f_env_sus_edit);
}

void CMirProgram::OnKillFocusFRelEdit()
{
	SetSliderFromEdit(m_f_env_rel_slider,m_f_env_rel_edit);
}

void CMirProgram::OnKillFocusAAttEdit()
{
	SetSliderFromEdit(m_a_env_att_slider,m_a_env_att_edit);
}

void CMirProgram::OnKillFocusAPeakEdit()
{
	SetSliderFromEdit(m_a_env_peak_slider,m_a_env_peak_edit);
}

void CMirProgram::OnKillFocusADecEdit()
{
	SetSliderFromEdit(m_a_env_dec_slider,m_a_env_dec_edit);
}

void CMirProgram::OnKillFocusASusEdit()
{
	SetSliderFromEdit(m_a_env_sus_slider,m_a_env_sus_edit);
}

void CMirProgram::OnKillFocusARelEdit()
{
	SetSliderFromEdit(m_a_env_rel_slider,m_a_env_rel_edit);
}

void CMirProgram::OnKillFocusFMAttEdit()
{
	SetSliderFromEdit(m_f_mod_att_slider,m_f_mod_att_edit);
}

void CMirProgram::OnKillFocusFMPeakEdit()
{
	SetSliderFromEdit(m_f_mod_peak_slider,m_f_mod_peak_edit);
}

void CMirProgram::OnKillFocusFMDecEdit()
{
	SetSliderFromEdit(m_f_mod_dec_slider,m_f_mod_dec_edit);
}

void CMirProgram::OnKillFocusFMSusEdit()
{
	SetSliderFromEdit(m_f_mod_sus_slider,m_f_mod_sus_edit);
}

void CMirProgram::OnKillFocusFMRelEdit()
{
	SetSliderFromEdit(m_f_mod_rel_slider,m_f_mod_rel_edit);
}

void CMirProgram::OnKillFocusAMAttEdit()
{
	SetSliderFromEdit(m_a_mod_att_slider,m_a_mod_att_edit);
}

void CMirProgram::OnKillFocusAMPeakEdit()
{
	SetSliderFromEdit(m_a_mod_peak_slider,m_a_mod_peak_edit);
}

void CMirProgram::OnKillFocusAMDecEdit()
{
	SetSliderFromEdit(m_a_mod_dec_slider,m_a_mod_dec_edit);
}

void CMirProgram::OnKillFocusAMSusEdit()
{
	SetSliderFromEdit(m_a_mod_sus_slider,m_a_mod_sus_edit);
}

void CMirProgram::OnKillFocusAMRelEdit()
{
	SetSliderFromEdit(m_a_mod_rel_slider,m_a_mod_rel_edit);
}

void CMirProgram::OnKillFocusInitWaveEdit()
{
	SetSliderFromEdit(m_InitWave_slider,m_InitWave_edit);
}

void CMirProgram::OnKillFocusOscDetEdit()
{
	SetSliderFromEdit(m_OscDet_slider,m_OscDet_edit);
}

void CMirProgram::OnKillFocusOscMixEdit()
{
	SetSliderFromEdit(m_OscMix_slider,m_OscMix_edit);
}

void CMirProgram::OnKillFocusOscVelEdit()
{
	SetSliderFromEdit(m_OscVel_slider,m_OscVel_slider);
}

void CMirProgram::OnBnClickedUpperLower()
{
	switch(m_bLowerUpperPgm)
	{
		case false:
			m_bLowerUpperPgm=!m_bLowerUpperPgm;
			m_upper_lower.SetWindowTextA("Lower Program");
			m_upper_lower.UpdateWindow();
			break;
		case true:
			m_bLowerUpperPgm=!m_bLowerUpperPgm;
			m_upper_lower.SetWindowTextA("Upper Program");
			m_upper_lower.UpdateWindow();
			break;
	}
}
