// ProgressBar.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "ProgressBar.h"

#define BAR_MAX 1000000

CProgressDialog::CProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
	//}}AFX_DATA_INIT
}


void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialog)
	DDX_Control(pDX, IDC_PROGRESS, Bar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers

void CProgressDialog::progress (unsigned int progressValue)
{
	UpdateData();
//	float value = getCropedValue (progressValue);
	Bar.SetPos(progressValue);

	UpdateData (FALSE);
}

BOOL CProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	Bar.SetRange32 (0, BAR_MAX);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
