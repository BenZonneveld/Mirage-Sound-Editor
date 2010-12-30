// Dialog_ConfigParams.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Tab_GeneralKeyboard.h"
#include "Tab_SamplingConfig.h"
#include "Tab_Sequencer.h"
#include "Tab_MidiConfig.h"
#include "Dialog_ConfigParams.h"
#include "Tabby.h"
#include "MirageSysex.h"
#include "macros.h"


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

