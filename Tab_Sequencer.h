#pragma once


// CTabSequencer dialog

class CTabSequencer : public CDialog
{
	DECLARE_DYNAMIC(CTabSequencer)

public:
	CTabSequencer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabSequencer();
	virtual BOOL OnInitDialog();
	void OnDialogOk();

	afx_msg void CTabSequencer::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void OnBnClickedMidiPedal();
	afx_msg void OnKillFocusClockRateEdit();
	void MidiPedalButton();

	// Configuration
	CButton m_seq_clock;
	CButton m_clock_jack;
	CButton m_seq_loop_switch;
	CButton m_midi_pedal;
	CSliderCtrl m_clock_rate_slider;
	CEdit m_clock_rate_edit;

// Dialog Data
	enum { IDD = IDD_TAB_CONFIGURATION };

private:
	_config_dump_table_	m_config;
	CString	m_str;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
