#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CConfigParams dialog

class CConfigParams : public CDialog
{
	DECLARE_DYNAMIC(CConfigParams)

public:
	CConfigParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigParams();
	virtual BOOL OnInitDialog();
	afx_msg void CConfigParams::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void CConfigParams::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);

// Dialog Data
	enum { IDD = IDD_CONFIGURATION_PARAMETERS };

private:
	_config_dump_table_	m_config;
	CString	m_str;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// Generic Keyboard
	CSliderCtrl m_mastertune_slider;
	CSliderCtrl m_PitchBend_slider;
	CSliderCtrl m_velosens_slider;
	CSliderCtrl m_balance_slider;
	CEdit m_mastertune_edit;
	CEdit m_PitchBend_edit;
	CEdit m_velosens_edit;
	CEdit m_balance_edit;
	CButton m_programlink_check;
	afx_msg void OnKillFocusMasterTuneEdit();
	afx_msg void OnKillFocusPitchbendRangeEdit();
	afx_msg void OnKillFocusVeloSensEdit();
	afx_msg void OnKillFocusBalanceEdit();

	// Sampling Config
	CSliderCtrl m_sample_time_slider;
	CSliderCtrl m_filter_freq_slider;
	CSliderCtrl m_threshold_slider;
	CEdit m_sample_time_edit;
	CEdit m_filter_freq_edit;
	CEdit m_threshold_edit;
	CButton m_user_multisampling_check;
	CButton m_sampling_level_button;
	afx_msg void OnBnClickedSamplingLevel();
	void SamplingLevelButton();
	afx_msg void OnKillFocusSampleTimeEdit();
	afx_msg void OnKillFocusFilterInputEdit();
	afx_msg void OnKillFocusThresholdEdit();

	// Configuration
	CButton m_midi_omni;
	CButton m_midi_thru;
	CComboBox m_midi_channel;
	CButton m_seq_clock;
	CButton m_clock_jack;
	CButton m_seq_loop_switch;
	CButton m_midi_pedal;
	afx_msg void OnBnClickedMidiOmni();
	afx_msg void OnBnClickedMidiThru();
	afx_msg void OnBnClickedMidiPedal();
	void MidiOmniButton();
	void MidiThruButton();
	void MidiPedalButton();
	CSliderCtrl m_clock_rate_slider;
	CEdit m_clock_rate_edit;
	afx_msg void OnKillFocusClockRateEdit();
	CButton m_local_mode;
	afx_msg void OnBnClickedLocalMode();
	void LocalModeButton();
};
