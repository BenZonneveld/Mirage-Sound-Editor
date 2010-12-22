#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CMirProgram dialog

#define InitSlider(slider_ctrl,edit_ctrl,min,max,tick_freq,pagesize) \
	slider_ctrl.SetRange(min,max); \
	slider_ctrl.SetTicFreq(tick_freq); \
	slider_ctrl.SetPageSize(pagesize);
#define SetSlider(slider_ctrl,edit_ctrl) \
	str.Format(_T("%d"),slider_ctrl.GetRangeMax()-slider_ctrl.GetPos()); \
	edit_ctrl.SetWindowTextA(str);
#define SetSliderValue(slider_ctrl,edit_ctrl,value) \
	slider_ctrl.SetPos(slider_ctrl.GetRangeMax()-value); \
	SetSlider(slider_ctrl,edit_ctrl);
#define SetSliderFromEdit(slider_ctrl,edit_ctrl) \
	edit_ctrl.GetWindowTextA(str); \
	slider_ctrl.SetPos(slider_ctrl.GetRangeMax()-atoi(str)); \
	SetSlider(slider_ctrl,edit_ctrl);

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

public:
	afx_msg void CMirProgram::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);

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
};
