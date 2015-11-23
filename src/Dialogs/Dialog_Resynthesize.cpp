// Dialog_Resynthesize.cpp : implementation file
//

#include "stdafx.h"
#include "../Mirage Editor.h"
#include "Dialog_Resynthesize.h"


// CResynthesize dialog

IMPLEMENT_DYNAMIC(CResynthesize, CDialog)

CResynthesize::CResynthesize(CWnd* pParent /*=NULL*/)
	: CDialog(CResynthesize::IDD, pParent)
{
}

CResynthesize::~CResynthesize()
{
}

void CResynthesize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_FFTSIZE, m_spin_fftsize);
	DDX_Control(pDX, IDC_SPIN_ITERATIONS, m_spin_iterations);
	DDX_Control(pDX, IDC_SPIN_HOPSIZE, m_spin_hopsize);
	DDX_Control(pDX, IDC_SPIN_CONVOLVE, m_spin_convolve);
}


BEGIN_MESSAGE_MAP(CResynthesize, CDialog)
	ON_BN_CLICKED(IDOK, &CResynthesize::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CResynthesize::OnBnClickedCancel)
//	ON_EN_CHANGE(IDC_DSP_LOGBASE, &CResynthesize::OnEnChangeDspLogbase)
END_MESSAGE_MAP()

BOOL CResynthesize::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_spin_fftsize.SetRange32(256,4096);
	m_spin_fftsize.SetPos32(m_fftsize);

	m_spin_hopsize.SetRange32(64,512);
	m_spin_hopsize.SetPos32(m_hopsize);

	m_spin_iterations.SetRange32(1,300);
	m_spin_iterations.SetPos32(m_iterations);

	m_spin_convolve.SetRange(3,15);
	m_spin_convolve.SetPos(m_convolve);

	return TRUE;
}
// CResynthesize message handlers

void CResynthesize::OnBnClickedOk()
{
	m_fftsize = m_spin_fftsize.GetPos32();
	if ( m_fftsize > 4096 )
		m_fftsize = 4096;
	if ( m_fftsize < 256)
		m_fftsize = 256;

	m_hopsize = m_spin_hopsize.GetPos32();
	if ( m_hopsize < 64 )
		m_hopsize = 64;
	if ( m_hopsize > 512)
		m_hopsize = 512;
	
	m_iterations = m_spin_iterations.GetPos();
	if ( m_iterations < 1 )
		m_iterations = 1;
	if ( m_iterations > 300 )
		m_iterations = 300;

	m_convolve = m_spin_convolve.GetPos();
	if ( m_convolve < 3 )
		m_convolve = 3;
	if ( m_convolve > 15 )
		m_convolve = 15;

	OnOK();
	
	m_resynth_ok = true;
}

void CResynthesize::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
	m_resynth_ok = false;
}
