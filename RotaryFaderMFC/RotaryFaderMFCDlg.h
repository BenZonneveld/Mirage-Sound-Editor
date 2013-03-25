// RotaryFaderMFCDlg.h : Header-Datei
//

#if !defined(AFX_ROTARYFADERMFCDLG_H__23FC1156_13EC_4885_A1F9_1F8AF028C3BC__INCLUDED_)
#define AFX_ROTARYFADERMFCDLG_H__23FC1156_13EC_4885_A1F9_1F8AF028C3BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRotaryFaderMFCDlg Dialogfeld

class CRotaryFaderMFCDlg : public CDialog
{
// Konstruktion
public:
	CRotaryFaderMFCDlg(CWnd* pParent = NULL);	// Standard-Konstruktor


	enum eHBITMAP
	{
		Background,
		Scale1,
		Knob1,
		Dot1,

		Scale2,
		Knob2,
		Dot2,

		BitmapCount

	};

// Dialogfelddaten
	//{{AFX_DATA(CRotaryFaderMFCDlg)
	enum { IDD = IDD_ROTARYFADERMFC_DIALOG };
	CVMRotaryFaderCtrl	m_Rotary1;
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CRotaryFaderMFCDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;
	CVMBitmap	m_Background;
	HBITMAP		m_hBitmap[BitmapCount];


	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CRotaryFaderMFCDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnFlipScale();
	afx_msg void OnBtnFlipKnob();
	afx_msg void OnBtnFlipDot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_ROTARYFADERMFCDLG_H__23FC1156_13EC_4885_A1F9_1F8AF028C3BC__INCLUDED_)
