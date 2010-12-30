#pragma once


// CTabGeneralKeyboard dialog

class CTabGeneralKeyboard : public CDialog
{
	DECLARE_DYNAMIC(CTabGeneralKeyboard)
private:
	_config_dump_table_	m_config;
	CString	m_str;

public:
	CTabGeneralKeyboard(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTabGeneralKeyboard();
	virtual BOOL OnInitDialog();
	afx_msg void CTabGeneralKeyboard::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar);
	CSliderCtrl m_mastertune_slider;
	CSliderCtrl m_PitchBend_slider;
	CSliderCtrl m_velosens_slider;
	CSliderCtrl m_balance_slider;
	CEdit m_mastertune_edit;
	CEdit m_PitchBend_edit;
	CEdit m_velosens_edit;
	CEdit m_balance_edit;
	CButton m_programlink_check;

	// Generic Keyboard
	afx_msg void OnKillFocusMasterTuneEdit();
	afx_msg void OnKillFocusPitchbendRangeEdit();
	afx_msg void OnKillFocusVeloSensEdit();
	afx_msg void OnKillFocusBalanceEdit();


// Dialog Data
	enum { IDD = IDD_TAB_GENERAL_KEYBOARD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
