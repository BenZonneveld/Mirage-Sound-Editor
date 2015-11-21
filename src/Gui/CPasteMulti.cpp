// PasteMulti.cpp : implementation file
//

#include "stdafx.h"
#include "../Mirage Editor.h"
#include "CPasteMulti.h"


// PasteMulti dialog

IMPLEMENT_DYNAMIC(CPasteMulti, CDialog)

CPasteMulti::CPasteMulti(CWnd* pParent /*=NULL*/)
	: CDialog(CPasteMulti::IDD, pParent)
{
}

CPasteMulti::~CPasteMulti()
{
}

void CPasteMulti::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPEATCOUNT, RepeatValue);
	DDX_Control(pDX, IDC_REPEAT_SPIN, SpinButton);
}


BEGIN_MESSAGE_MAP(CPasteMulti, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REPEAT_SPIN, &CPasteMulti::OnDeltaposRepeatSpin)
	ON_BN_CLICKED(IDOK, &CPasteMulti::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPasteMulti::OnBnClickedCancel)
END_MESSAGE_MAP()

//BOOL CPasteMulti::OnInitDialog()
//{
////	CSpinButtonCtrl *Spin = &SpinButton;
////	SpinButton.SetRange(1,255);
//	return TRUE;
//}

// PasteMulti message handlers

void CPasteMulti::OnDeltaposRepeatSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	SpinButton.SetRange(1,255);
	CString	MyValue;
	int i;
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	if ( pNMUpDown->iPos == 0 )
	{
		SpinButton.SetPos(1);
		pNMUpDown->iPos = 1;
	}

	if ( RepeatValue.GetWindowTextLengthA() > 0 )
	{
		RepeatValue.GetWindowTextA(MyValue);
		SpinButton.SetPos(atoi(MyValue));
		pNMUpDown->iPos = atoi(MyValue);
	}
	i = pNMUpDown->iPos;

	if(pNMUpDown->iDelta > 0)
	{
		i++;
		// do your correction
	}
	else if (pNMUpDown->iDelta < 0 )
	{
		if ( i > 1 )
			i--;
		// do your correction
	}

	MyValue.Format("%i",i);
	RepeatValue.SetWindowTextA(MyValue);//SetDlgItemInt(IDC_REPEATCOUNT,i,FALSE);
	//RepeatCount->SetModify(TRUE);
	*pResult = 0;

//	this.RepeatValue=i;
}

void CPasteMulti::OnBnClickedOk()
{
	CString MyValue;
	RepeatValue.GetWindowTextA(MyValue);
	theApp.RepeatCount=atoi(MyValue);
	OnOK();
}

void CPasteMulti::OnBnClickedCancel()
{
	OnCancel();
	theApp.RepeatCount=0;
}
