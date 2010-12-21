#pragma once


// CMirProgram dialog

class CMirProgram : public CDialog
{
	DECLARE_DYNAMIC(CMirProgram)

public:
	CMirProgram(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMirProgram();

// Dialog Data
	enum { IDD = IDD_PROGRAM_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
