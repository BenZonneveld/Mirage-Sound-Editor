// Tab_GeneralKeyboard.cpp : implementation file
//

#include "stdafx.h"
#include "../Mirage Editor.h"
#include "../macros.h"
#include "../Midi/MirageSysex.h"
#include "Tab_GeneralKeyboard.h"


// CTabGeneralKeyboard dialog

IMPLEMENT_DYNAMIC(CTabGeneralKeyboard, CDialog)

CTabGeneralKeyboard::CTabGeneralKeyboard(CWnd* pParent /*=NULL*/)
	: CDialog(CTabGeneralKeyboard::IDD, pParent)
{

}

CTabGeneralKeyboard::~CTabGeneralKeyboard()
{
}

void CTabGeneralKeyboard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MASTER_TUNE_SLIDER, m_mastertune_slider);
	DDX_Control(pDX, IDC_MASTER_TUNE_EDIT, m_mastertune_edit);
	DDX_Control(pDX, IDC_PITCHBEND_RANGE_SLIDER, m_PitchBend_slider);
	DDX_Control(pDX, IDC_PITCHBEND_RANGE_EDIT, m_PitchBend_edit);
	DDX_Control(pDX, IDC_VELO_SENS_SLIDER, m_velosens_slider);
	DDX_Control(pDX, IDC_VELO_SENS_EDIT, m_velosens_edit);
	DDX_Control(pDX, IDC_UPPERLOWER_BALANCE_SLIDER, m_balance_slider);
	DDX_Control(pDX, IDC_UPPERLOWER_BALANCE_EDIT, m_balance_edit);
	DDX_Control(pDX, IDC_PROGRAM_LINK, m_programlink_check);
}

BOOL CTabGeneralKeyboard::OnInitDialog()
{
	CDialog::OnInitDialog();
	memcpy(&m_config,&ConfigDump,sizeof(ConfigDump));

	// Generic Keyboard
	InitSlider(m_mastertune_slider,m_mastertune_edit,0,99,10,10);
	InitSlider(m_PitchBend_slider,m_PitchBend_edit,0,34,10,10);
	InitSlider(m_velosens_slider,m_velosens_edit,0,63,10,10);
	InitSlider(m_balance_slider,m_balance_edit,0,63,10,10);
	SetSliderValue(m_mastertune_slider,m_mastertune_edit,m_config.master_tune);
	SetSliderValue(m_PitchBend_slider,m_PitchBend_edit,m_config.pitch_bend_range);
	SetSliderValue(m_velosens_slider,m_velosens_edit,m_config.velocity_sensitivity);
	SetSliderValue(m_balance_slider,m_balance_edit,m_config.up_low_balance/2);
	m_programlink_check.SetCheck(m_config.program_link_switch);

	return TRUE;
}

void CTabGeneralKeyboard::OnVScroll(UINT SBCode, UINT nPos, CScrollBar *pScrollBar)
{
	CDialog::OnVScroll(SBCode, nPos, pScrollBar);

	// Generic Keyboard
	SetSlider(m_mastertune_slider,m_mastertune_edit);
	SetSlider(m_PitchBend_slider,m_PitchBend_edit);
	SetSlider(m_velosens_slider,m_velosens_edit);
	SetSlider(m_balance_slider,m_balance_edit);
}

void CTabGeneralKeyboard::OnKillFocusMasterTuneEdit()
{
	SetSliderFromEdit(m_mastertune_slider,m_mastertune_edit);
}

void CTabGeneralKeyboard::OnKillFocusPitchbendRangeEdit()
{
	SetSliderFromEdit(m_PitchBend_slider,m_PitchBend_edit);
}

void CTabGeneralKeyboard::OnKillFocusVeloSensEdit()
{
	SetSliderFromEdit(m_velosens_slider,m_velosens_edit);
}

void CTabGeneralKeyboard::OnKillFocusBalanceEdit()
{
	SetSliderFromEdit(m_balance_slider,m_balance_edit);
}

void CTabGeneralKeyboard::OnDialogOk()
{
	ConfigDump.master_tune = m_mastertune_slider.GetRangeMax()-m_mastertune_slider.GetPos();
	ConfigDump.pitch_bend_range = m_PitchBend_slider.GetRangeMax()-m_PitchBend_slider.GetPos();
	ConfigDump.velocity_sensitivity = m_velosens_slider.GetRangeMax()-m_velosens_slider.GetPos();
	ConfigDump.up_low_balance = 2 * (m_balance_slider.GetRangeMax()-m_balance_slider.GetPos());
	ConfigDump.program_link_switch = m_programlink_check.GetCheck();

	CWnd::DestroyWindow();
}

BEGIN_MESSAGE_MAP(CTabGeneralKeyboard, CDialog)
		ON_WM_VSCROLL()
		ON_EN_KILLFOCUS(IDC_MASTER_TUNE_EDIT, &CTabGeneralKeyboard::OnKillFocusMasterTuneEdit)
	ON_EN_KILLFOCUS(IDC_PITCHBEND_RANGE_EDIT, &CTabGeneralKeyboard::OnKillFocusPitchbendRangeEdit)
	ON_EN_KILLFOCUS(IDC_VELO_SENS_EDIT, &CTabGeneralKeyboard::OnKillFocusVeloSensEdit)
	ON_EN_KILLFOCUS(IDC_UPPERLOWER_BALANCE_EDIT, &CTabGeneralKeyboard::OnKillFocusBalanceEdit)
END_MESSAGE_MAP()


// CTabGeneralKeyboard message handlers
