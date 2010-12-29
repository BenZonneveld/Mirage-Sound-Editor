#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CMirProgram dialog

class CMirProgram : public CDialog
{
	DECLARE_DYNAMIC(CMirProgram)

public:
	CMirProgram(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMirProgram();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_PROGRAM_SETTINGS };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	CString m_str;
	bool	m_bLowerUpperPgm;
	char unsigned m_Program;
	_program_parameter_block_ ProgramParmBlock;

public:
	afx_msg void CMirProgram::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void CMirProgram::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);

	// Keyboard Params
	CSliderCtrl m_lfo_freq_slider;
	CSliderCtrl m_lfo_depth_slider;
	CSliderCtrl m_cutoff_slider;
	CSliderCtrl m_reso_slider;
	CSliderCtrl m_filter_tracking_slider;
	CEdit m_lfo_freq_edit;
	CEdit m_lfo_depth_edit;
	CEdit m_cutoff_edit;
	CEdit m_reso_edit;
	CButton m_mono_mode;

	afx_msg void OnKillFocusLfoFreqEdit();
	afx_msg void OnKillFocusLfoDepthEdit();
	afx_msg void OnKillFocusCutoffEdit();
	afx_msg void OnKillFocusResoEdit();

	// Filter Envelope
	CSliderCtrl m_f_env_att_slider;
	CSliderCtrl m_f_env_peak_slider;
	CSliderCtrl m_f_env_dec_slider;
	CSliderCtrl m_f_env_sus_slider;
	CSliderCtrl m_f_env_rel_slider;
	CEdit m_f_env_att_edit;
	CEdit m_f_env_peak_edit;
	CEdit m_f_env_dec_edit;
	CEdit m_f_env_sus_edit;
	CEdit m_f_env_rel_edit;
	CSliderCtrl m_f_mod_att_slider;
	CSliderCtrl m_f_mod_peak_slider;
	CSliderCtrl m_f_mod_dec_slider;
	CSliderCtrl m_f_mod_sus_slider;
	CSliderCtrl m_f_mod_rel_slider;
	CEdit m_f_mod_att_edit;
	CEdit m_f_mod_peak_edit;
	CEdit m_f_mod_dec_edit;
	CEdit m_f_mod_sus_edit;
	CEdit m_f_mod_rel_edit;
	afx_msg void OnKillFocusFAttEdit();
	afx_msg void OnKillFocusFPeakEdit();
	afx_msg void OnKillFocusFDecEdit();
	afx_msg void OnKillFocusFSusEdit();
	afx_msg void OnKillFocusFRelEdit();
	afx_msg void OnKillFocusFMAttEdit();
	afx_msg void OnKillFocusFMPeakEdit();
	afx_msg void OnKillFocusFMDecEdit();
	afx_msg void OnKillFocusFMSusEdit();
	afx_msg void OnKillFocusFMRelEdit();
	// Amplitude Envelope
	CSliderCtrl m_a_env_att_slider;
	CSliderCtrl m_a_env_peak_slider;
	CSliderCtrl m_a_env_dec_slider;
	CSliderCtrl m_a_env_sus_slider;
	CSliderCtrl m_a_env_rel_slider;
	CEdit m_a_env_att_edit;
	CEdit m_a_env_peak_edit;
	CEdit m_a_env_dec_edit;
	CEdit m_a_env_sus_edit;
	CEdit m_a_env_rel_edit;
	CSliderCtrl m_a_mod_att_slider;
	CSliderCtrl m_a_mod_peak_slider;
	CSliderCtrl m_a_mod_dec_slider;
	CSliderCtrl m_a_mod_sus_slider;
	CSliderCtrl m_a_mod_rel_slider;
	CEdit m_a_mod_att_edit;
	CEdit m_a_mod_peak_edit;
	CEdit m_a_mod_dec_edit;
	CEdit m_a_mod_sus_edit;
	CEdit m_a_mod_rel_edit;
	afx_msg void OnKillFocusAAttEdit();
	afx_msg void OnKillFocusAPeakEdit();
	afx_msg void OnKillFocusADecEdit();
	afx_msg void OnKillFocusASusEdit();
	afx_msg void OnKillFocusARelEdit();
	afx_msg void OnKillFocusAMAttEdit();
	afx_msg void OnKillFocusAMPeakEdit();
	afx_msg void OnKillFocusAMDecEdit();
	afx_msg void OnKillFocusAMSusEdit();
	afx_msg void OnKillFocusAMRelEdit();

	/* Wavesample */
	BOOL m_MixMode;
	CSliderCtrl m_InitWave_slider;
	CSliderCtrl m_OscDet_slider;
	CSliderCtrl m_OscMix_slider;
	CSliderCtrl m_OscVel_slider;
	CEdit m_InitWave_edit;
	CEdit m_OscDet_edit;
	CEdit m_OscMix_edit;
	CEdit m_OscVel_edit;
	afx_msg void OnKillFocusInitWaveEdit();
	afx_msg void OnKillFocusOscDetEdit();
	afx_msg void OnKillFocusOscMixEdit();
	afx_msg void OnKillFocusOscVelEdit();

	/* Program selection */
	CSliderCtrl m_pgm_select;
	CButton m_upper_lower;
	afx_msg void OnBnClickedUpperLower();
	CButton m_mixmode_check;
};
