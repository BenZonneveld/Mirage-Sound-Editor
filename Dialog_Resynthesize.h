#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CResynthesize dialog

class CResynthesize : public CDialog
{
	DECLARE_DYNAMIC(CResynthesize)

public:
	CResynthesize(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResynthesize();

// Dialog Data
	enum { IDD = IDD_RESYNTHESIS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double	m_maxfreq;
	double	m_maxfreq_range;
	double	m_BandsPerOctave;
	double	m_PixPerSec;
	double	m_logbase;
	bool		m_anal_mode;
	bool		m_synth_mode;
	bool		m_resynth_ok;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_spin_maxfreq;
	CSpinButtonCtrl m_spin_bpo;
	CSpinButtonCtrl m_spin_pps;
	CSpinButtonCtrl m_spin_logbase;
//	afx_msg void OnEnChangeDspLogbase();
	CButton m_linear;
	CButton m_synth;
};
