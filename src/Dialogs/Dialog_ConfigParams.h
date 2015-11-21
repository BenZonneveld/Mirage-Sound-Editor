#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "../Gui/Tabby.h"


// CConfigParams dialog

class CConfigParams : public CDialog
{
	DECLARE_DYNAMIC(CConfigParams)

public:
	CConfigParams(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigParams();

// Dialog Data
	enum { IDD = IDD_CONFIGURATION_PARAMETERS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	CTabby	m_TabCtrl;
};
