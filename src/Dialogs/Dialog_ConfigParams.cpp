// Dialog_ConfigParams.cpp : implementation file
//

#include "stdafx.h"
#include "../Mirage Editor.h"
#include "../Gui/Tab_GeneralKeyboard.h"
#include "../Gui/Tab_SamplingConfig.h"
#include "../Gui/Tab_Sequencer.h"
#include "../Gui/Tab_MidiConfig.h"
#include "Dialog_ConfigParams.h"
#include "../Gui/Tabby.h"
#include "../macros.h"
#include "../Midi/MirageSysex.h"
#include "../Midi/Mirage Sysex_Strings.h"

// CConfigParams dialog

IMPLEMENT_DYNAMIC(CConfigParams, CDialog)

CConfigParams::CConfigParams(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigParams::IDD, pParent)
{

}

CConfigParams::~CConfigParams()
{
}

void CConfigParams::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONFIG_TABS, m_TabCtrl);
}

BOOL CConfigParams::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CConfigParams, CDialog)
END_MESSAGE_MAP()


void CConfigParams::OnOK()
{
	m_TabCtrl.m_TabOneDialog.OnDialogOk();
	m_TabCtrl.m_TabTwoDialog.OnDialogOk();
	m_TabCtrl.m_TabThreeDialog.OnDialogOk();
	m_TabCtrl.m_TabFourDialog.OnDialogOk();

	SendConfigParms();
	CDialog::OnOK();
}

void CConfigParams::OnCancel()
{
	CDialog::OnCancel();
}