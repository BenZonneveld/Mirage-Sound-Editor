#pragma once
#include "Wave Doc.h"
#include "DFV/Ctrl_DFV.h"
#include "afxwin.h"
#include "Ctrl_RepeatButton.h"

// CLoopDialog dialog

class CLoopDialog : public CDialog
{
	DECLARE_DYNAMIC(CLoopDialog)

public:
	CLoopDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoopDialog();
	CMirageEditorDoc*	m_LoopDocDialog;
	CDFVCtrl			m_DFVCtrl1;
	CDC*				m_pDC;
	BOOL	m_bPlayLoop;
// Dialog Data
	enum { IDD = IDD_LOOPDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); 

	DECLARE_MESSAGE_MAP()

private:
	static _WaveSample_	m_sWav;
	CView* m_pView;
	CWnd* m_pPrevParent;
	CRepeatButton EndFine_Forward;
	CRepeatButton EndFine_Back;
	CButton m_Loop_Toggle;
	CButton m_PlayLoopStatus;
	static DWORD WINAPI PlayLoop(LPVOID param);

public:
	afx_msg void OnLoopClickedOk();
	afx_msg void OnLoopClickedCancel();
	afx_msg void OnBnClickedEndF();
	afx_msg void OnBnClickedEndB();
	afx_msg void OnBnClickedEndFineF();
	afx_msg void OnBnClickedEndFineB();
	afx_msg void OnBnClickedStartF();
	afx_msg void OnBnClickedStartB();
	afx_msg void OnBnClickedPageReset();
	afx_msg void OnBnClickedLoopToggle();
	afx_msg void OnBnClickedForwardBackwardLoop();
	afx_msg void OnBnClickedDlgPlayLoop();
};

extern HANDLE LoopDialogPlayLoop;