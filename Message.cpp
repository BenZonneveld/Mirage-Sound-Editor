// Message.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Message.h"


// CMessage dialog

//IMPLEMENT_DYNAMIC(CMessage, CDialog)

CMessage::CMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CMessage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessage)
	//}}AFX_DATA_INIT

}

CMessage::~CMessage()
{
}

void CMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CMessage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CMessage, CDialog)
	//{{AFX_MSG_MAP(CMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CMessage message handlers
