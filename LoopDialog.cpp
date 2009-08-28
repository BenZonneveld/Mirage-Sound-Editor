// LoopDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Mirage EditorDoc.h"

// For LoopView
#include "LoopDoc.h"
#include "LoopFrm.h"
#include "LoopDialog.h"
#include "LoopView.h"
#include "DFV/DFVCtrl.h"
#include "RepeatButton.h"

// CLoopDialog dialog

IMPLEMENT_DYNAMIC(CLoopDialog, CDialog)

CLoopDialog::CLoopDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLoopDialog::IDD, pParent)
	//, PictureControl(0)
{
//	m_pView = NULL;
//	m_pPrevParent = NULL;
}

CLoopDialog::~CLoopDialog()
{
}

void CLoopDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOOP_TOGGLE, m_Loop_Toggle);
	DDX_Control(pDX, IDC_END_FINE_F, EndFine_Forward);
	DDX_Control(pDX, IDC_END_FINE_B, EndFine_Back);
}


BEGIN_MESSAGE_MAP(CLoopDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CLoopDialog::OnLoopClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLoopDialog::OnLoopClickedCancel)
	ON_BN_CLICKED(IDC_END_F, &CLoopDialog::OnBnClickedEndF)
	ON_BN_CLICKED(IDC_END_B, &CLoopDialog::OnBnClickedEndB)
	ON_BN_CLICKED(IDC_END_FINE_F, &CLoopDialog::OnBnClickedEndFineF)
	ON_BN_CLICKED(IDC_END_FINE_B, &CLoopDialog::OnBnClickedEndFineB)
	ON_BN_CLICKED(IDC_START_F, &CLoopDialog::OnBnClickedStartF)
	ON_BN_CLICKED(IDC_START_B, &CLoopDialog::OnBnClickedStartB)
	ON_BN_CLICKED(IDC_PAGE_RESET, &CLoopDialog::OnBnClickedPageReset)
	ON_BN_CLICKED(IDC_LOOP_TOGGLE, &CLoopDialog::OnBnClickedLoopToggle)
	ON_BN_CLICKED(IDC_FORWARD_BACKWARD_LOOP, &CLoopDialog::OnBnClickedForwardBackwardLoop)
END_MESSAGE_MAP()


// CLoopDialog message handlers
BOOL CLoopDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	switch (CLoopView::LoopStatus())
	{
	case true:
		m_Loop_Toggle.SetWindowTextA("Loop Off");
		break;
	case false:
		m_Loop_Toggle.SetWindowTextA("Loop On");
		break;
	}

	m_DFVCtrl1.Create(this, IDC_IMAGE,
        IDR_MAINFRAME,
        RUNTIME_CLASS(CLoopDoc),
        RUNTIME_CLASS(CLoopFrame),       // main SDI frame window
        RUNTIME_CLASS(CLoopView),
        WS_CHILD | WS_BORDER | WS_VISIBLE, 0L);

	return TRUE;
}


void CLoopDialog::OnLoopClickedOk()
{
	OnOK();
	CLoopView::UseLoop();
}

void CLoopDialog::OnLoopClickedCancel()
{
	OnCancel();
}

void CLoopDialog::OnBnClickedEndF()
{
	Invalidate(true);
	CLoopView::LoopEndF();
	UpdateWindow();
}

void CLoopDialog::OnBnClickedEndB()
{
	Invalidate(true);
	CLoopView::LoopEndB();
	UpdateWindow();
}


void CLoopDialog::OnBnClickedEndFineF()
{
	Invalidate(true);
	CLoopView::LoopEndFineF();
	UpdateWindow();
}

void CLoopDialog::OnBnClickedEndFineB()
{
	Invalidate(true);
	CLoopView::LoopEndFineB();
	UpdateWindow();
}

void CLoopDialog::OnBnClickedStartF()
{
	Invalidate(true);
	CLoopView::LoopStartF();
	UpdateWindow();
}

void CLoopDialog::OnBnClickedStartB()
{
	Invalidate(true);
	CLoopView::LoopStartB();
	UpdateWindow();
}

void CLoopDialog::OnBnClickedPageReset()
{
	Invalidate(true);
	CLoopView::LoopEndPage();
	UpdateWindow();
}

void CLoopDialog::OnBnClickedLoopToggle()
{
	switch (CLoopView::ToggleLoop())
	{
	case true:
		m_Loop_Toggle.SetWindowTextA("Loop Off");
		break;
	case false:
		m_Loop_Toggle.SetWindowTextA("Loop On");
		break;
	}
}

void CLoopDialog::OnBnClickedForwardBackwardLoop()
{
	Invalidate(true);
	CLoopView::AlternateLoop();
	UpdateWindow();
}
