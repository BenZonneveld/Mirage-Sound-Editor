#pragma once
#include "afxwin.h"


// CTxSamParms dialog

class CTxSamParms : public CDialog
{
	DECLARE_DYNAMIC(CTxSamParms)

public:
	CTxSamParms(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTxSamParms();

// Dialog Data
	enum { IDD = IDD_SAMPLE_PARMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CButton mSampleParamsBool;
public:
	afx_msg void OnBnClickedSampleParmsOk();
};
