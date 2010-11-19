// BankSelect.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "DialogBankSelect.h"


// CBankSelect dialog

IMPLEMENT_DYNAMIC(CBankSelect, CDialog)

CBankSelect::CBankSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CBankSelect::IDD, pParent)
{

}

CBankSelect::~CBankSelect()
{
}

void CBankSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BANK1, m_Bank[0]);
	DDX_Control(pDX, IDC_BANK2, m_Bank[1]);
	DDX_Control(pDX, IDC_BANK3, m_Bank[2]);
	DDX_Control(pDX, IDC_BANK4, m_Bank[3]);
	DDX_Control(pDX, IDC_BANK5, m_Bank[4]);
	DDX_Control(pDX, IDC_BANK6, m_Bank[5]);
}


BEGIN_MESSAGE_MAP(CBankSelect, CDialog)
	ON_BN_CLICKED(IDOK, &CBankSelect::OnBnBankSelectOk)
END_MESSAGE_MAP()


// CBankSelect message handlers
BOOL CBankSelect::OnInitDialog()
{
	CDialog::OnInitDialog();
//	CButton LoadBank = &m_Bank;
	return TRUE;
}

void CBankSelect::OnBnBankSelectOk()
{
	int c=0;

	for(int n=0; n < 6 ; n++)
	{
		if (m_Bank[n].GetCheck() > 0 )
		{
			LoadBank.resize(LoadBank.size() + 1);
			LoadBank[c]=n;
			c++;
		}
	}

	CDialog::OnOK();
}