#pragma once


// CTabSamplingConfig dialog

class CTabSamplingConfig : public CDialog
{
	DECLARE_DYNAMIC(CTabSamplingConfig)

public:
	CTabSamplingConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabSamplingConfig();
	virtual BOOL OnInitDialog();
	afx_msg void CTabSamplingConfig::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);

	// Sampling Config
	CSliderCtrl m_sample_time_slider;
	CSliderCtrl m_filter_freq_slider;
	CSliderCtrl m_threshold_slider;
	CEdit m_sample_time_edit;
	CEdit m_filter_freq_edit;
	CEdit m_threshold_edit;
	CButton m_user_multisampling_check;
	CButton m_sampling_level_button;
	void SamplingLevelButton();
	afx_msg void OnBnClickedSamplingLevel();
	afx_msg void OnKillFocusSampleTimeEdit();
	afx_msg void OnKillFocusFilterInputEdit();
	afx_msg void OnKillFocusThresholdEdit();

// Dialog Data
	enum { IDD = IDD_TAB_SAMPLING_CONFIG };

private:
	_config_dump_table_	m_config;
	CString	m_str;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
