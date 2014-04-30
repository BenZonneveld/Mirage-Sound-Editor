// TransmitSampleParams.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Dialog_TxSamParms.h"


// CTxSamParms dialog

IMPLEMENT_DYNAMIC(CTxSamParms, CDialog)

CTxSamParms::CTxSamParms(CWnd* pParent /*=NULL*/)
	: CDialog(CTxSamParms::IDD, pParent)
{

}

CTxSamParms::~CTxSamParms()
{
}

void CTxSamParms::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAMPLE_PARAMS, mSampleParamsBool);
}


BEGIN_MESSAGE_MAP(CTxSamParms, CDialog)
	ON_BN_CLICKED(IDC_SAMPLE_PARMS_OK, &CTxSamParms::OnBnClickedSampleParmsOk)
END_MESSAGE_MAP()

BOOL CTxSamParms::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CButton *SendParams = &mSampleParamsBool;

	SendParams->SetCheck(theApp.GetProfileIntA("Settings","TxSampleParams",true));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// CTxSamParms message handlers

void CTxSamParms::OnBnClickedSampleParmsOk()
{
	CButton *SendParams = &mSampleParamsBool;

	theApp.WriteProfileInt("Settings","TxSampleParams", SendParams->GetCheck());

	CDialog::OnOK();
}
