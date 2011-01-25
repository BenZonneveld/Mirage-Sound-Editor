// Dialog_Resynthesize.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
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
	DDX_Control(pDX, IDC_SPIN_MAXFREQ, m_spin_maxfreq);
	DDX_Control(pDX, IDC_SPIN_BANDSPEROCTAVE, m_spin_bpo);
	DDX_Control(pDX, IDC_SPIN_PIXPERSEC, m_spin_pps);
	DDX_Control(pDX, IDC_DSP_SINE, m_synth);
	DDX_Control(pDX, IDC_CONVOLUTION, m_convolution);
}


BEGIN_MESSAGE_MAP(CResynthesize, CDialog)
	ON_BN_CLICKED(IDOK, &CResynthesize::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CResynthesize::OnBnClickedCancel)
//	ON_EN_CHANGE(IDC_DSP_LOGBASE, &CResynthesize::OnEnChangeDspLogbase)
END_MESSAGE_MAP()

BOOL CResynthesize::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_spin_maxfreq.SetRange32(0,m_maxfreq_range);
	m_spin_maxfreq.SetPos32(m_maxfreq);

	m_spin_bpo.SetRange32(1,120);
	m_spin_bpo.SetPos32(m_BandsPerOctave);

	m_spin_pps.SetRange32(30,300);
	m_spin_pps.SetPos32(m_PixPerSec);

	m_synth.SetCheck(m_synth_mode);

	m_convolution.SetCheck(m_convolution_mode);

	return TRUE;
}
// CResynthesize message handlers

void CResynthesize::OnBnClickedOk()
{
	m_maxfreq = (double)m_spin_maxfreq.GetPos32();
	if ( m_maxfreq > m_maxfreq_range/2 )
	{
		m_maxfreq = m_maxfreq_range/2;
	}
	if ( m_maxfreq < m_maxfreq_range/8)
	{
		m_maxfreq = m_maxfreq_range/8;
	}

	m_BandsPerOctave = (double)m_spin_bpo.GetPos32();
	if ( m_BandsPerOctave < 6 )
	{
		m_BandsPerOctave = 6;
	}
	
	m_PixPerSec = (double)m_spin_pps.GetPos();
	if ( m_PixPerSec < 5 )
	{
		m_PixPerSec = 5;
	}

	m_synth_mode = m_synth.GetCheck();

	m_convolution_mode = m_convolution.GetCheck();
	OnOK();
	
	m_resynth_ok = true;
}

void CResynthesize::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
	m_resynth_ok = false;
}

/*void CResynthesize::OnEnChangeDspLogbase()
{
	if ( m_spin_logbase.GetPos() != 2 )
	{
		m_spin_bpo.EnableWindow(FALSE);
	} else {
		m_spin_bpo.EnableWindow(TRUE);
	}
}*/
