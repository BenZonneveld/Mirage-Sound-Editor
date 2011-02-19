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
	int	m_fftsize;
	int	m_hopsize;
	int	m_iterations;
	int	m_convolve;
	bool		m_resynth_ok;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CSpinButtonCtrl m_spin_fftsize;
	CSpinButtonCtrl m_spin_hopsize;
	CSpinButtonCtrl m_spin_iterations;
	CSpinButtonCtrl m_spin_convolve;
};
