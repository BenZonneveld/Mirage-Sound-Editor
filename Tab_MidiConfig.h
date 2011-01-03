#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CTabMidiConfig dialog

class CTabMidiConfig : public CDialog
{
	DECLARE_DYNAMIC(CTabMidiConfig)

public:
	CTabMidiConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabMidiConfig();
	void OnDialogOk();

	afx_msg void CTabMidiConfig::OnHScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);
	afx_msg void OnBnClickedMidiOmni();
	afx_msg void OnBnClickedMidiThru();
	afx_msg void OnBnClickedLocalMode();
	void MidiOmniButton();
	void MidiThruButton();
	void LocalModeButton();

	CButton m_midi_omni;
	CButton m_midi_thru;
	CComboBox m_midi_channel;
	CButton m_local_mode;

// Dialog Data
	enum { IDD = IDD_TAB_MIDI_CONFIGURATION };

private:
	_config_dump_table_	m_config;
	CString	m_str;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_lfo_mod;
	CComboBox m_mix_mod;
	CEdit m_after_depth_edit;
	CSliderCtrl m_after_depth_slider;
	CComboBox m_midi_function;
};
