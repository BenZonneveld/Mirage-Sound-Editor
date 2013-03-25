// RotaryFaderMFCDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "RotaryFaderMFC.h"
#include "RotaryFaderMFCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotaryFaderMFCDlg Dialogfeld

CRotaryFaderMFCDlg::CRotaryFaderMFCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRotaryFaderMFCDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRotaryFaderMFCDlg)
		// HINWEIS: Der Klassenassistent fügt hier Member-Initialisierung ein
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRotaryFaderMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotaryFaderMFCDlg)
	DDX_Control(pDX, IDC_SLIDER1, m_Rotary1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRotaryFaderMFCDlg, CDialog)
	//{{AFX_MSG_MAP(CRotaryFaderMFCDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_FLIPSCALE, OnBtnFlipScale)
	ON_BN_CLICKED(IDC_BTN_FLIPKNOB, OnBtnFlipKnob)
	ON_BN_CLICKED(IDC_BTN_FLIPDOT, OnBtnFlipDot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotaryFaderMFCDlg Nachrichten-Handler

BOOL CRotaryFaderMFCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{	
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// load all bitmaps

	TCHAR* pszBitmapFile = _T("background4a151x.bmp");
	m_hBitmap[Background] = (HBITMAP)::LoadImage(NULL, pszBitmapFile, IMAGE_BITMAP, 0,0,LR_LOADFROMFILE);

	pszBitmapFile = _T("rotaryscale22a151x.bmp");
	m_hBitmap[Scale1] = (HBITMAP)::LoadImage(NULL, pszBitmapFile, IMAGE_BITMAP, 0,0,LR_LOADFROMFILE);

	pszBitmapFile = _T("rotaryknob22a77x.bmp");
	m_hBitmap[Knob1] = (HBITMAP)::LoadImage(NULL, pszBitmapFile, IMAGE_BITMAP, 0,0,LR_LOADFROMFILE);

	pszBitmapFile = _T("rotarydot22a15x.bmp");
	m_hBitmap[Dot1] = (HBITMAP)::LoadImage(NULL, pszBitmapFile, IMAGE_BITMAP, 0,0,LR_LOADFROMFILE);

	pszBitmapFile = _T("rotaryscale23a151x.bmp");
	m_hBitmap[Scale2] = (HBITMAP)::LoadImage(NULL, pszBitmapFile, IMAGE_BITMAP, 0,0,LR_LOADFROMFILE);

	pszBitmapFile = _T("rotaryknob23a77x.bmp");
	m_hBitmap[Knob2] = (HBITMAP)::LoadImage(NULL, pszBitmapFile, IMAGE_BITMAP, 0,0,LR_LOADFROMFILE);

	pszBitmapFile = _T("rotarydot23a15x.bmp");
	m_hBitmap[Dot2] = (HBITMAP)::LoadImage(NULL, pszBitmapFile, IMAGE_BITMAP, 0,0,LR_LOADFROMFILE);




	m_Background.Attach(m_hBitmap[Background]);
	m_Rotary1.SetBitmap(m_hBitmap[Scale1], CVMRotaryFaderCtrl::Scale);
	m_Rotary1.SetBitmap(m_hBitmap[Knob1], CVMRotaryFaderCtrl::Knob);
	m_Rotary1.SetBitmap(m_hBitmap[Dot1], CVMRotaryFaderCtrl::Dot);
	


	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CRotaryFaderMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CRotaryFaderMFCDlg::OnPaint() 
{
	CRect rect;
	GetClientRect(&rect);
	CPaintDC dc(this); // Gerätekontext für Zeichnen


	if (IsIconic())
	{

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		int w = rect.Width();
		int h = rect.Height();
		m_Background.DrawTiles(dc.m_hDC,w,h);

		//CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CRotaryFaderMFCDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRotaryFaderMFCDlg::OnBtnFlipScale() 
{
	HBITMAP hbmpOld = m_Rotary1.GetBitmap(CVMRotaryFaderCtrl::Scale);

	if(hbmpOld == m_hBitmap[Scale1]) {
		m_Rotary1.SetBitmap(m_hBitmap[Scale2], CVMRotaryFaderCtrl::Scale);
	}
	else {
		m_Rotary1.SetBitmap(m_hBitmap[Scale1], CVMRotaryFaderCtrl::Scale);
	}
}

void CRotaryFaderMFCDlg::OnBtnFlipKnob() 
{
	HBITMAP hbmpOld = m_Rotary1.GetBitmap(CVMRotaryFaderCtrl::Knob);

	if(hbmpOld == m_hBitmap[Knob1]) {
		m_Rotary1.SetBitmap(m_hBitmap[Knob2], CVMRotaryFaderCtrl::Knob);
		m_Rotary1.SetKnobRadius(20); 

	}
	else {
		m_Rotary1.SetBitmap(m_hBitmap[Knob1], CVMRotaryFaderCtrl::Knob);
		m_Rotary1.SetKnobRadius(30); 
	}
}


void CRotaryFaderMFCDlg::OnBtnFlipDot() 
{
	HBITMAP hbmpOld = m_Rotary1.GetBitmap(CVMRotaryFaderCtrl::Dot);
	HBITMAP hbmpKnob = m_Rotary1.GetBitmap(CVMRotaryFaderCtrl::Knob);

	if(hbmpOld == m_hBitmap[Dot1]) {
		m_Rotary1.SetBitmap(m_hBitmap[Dot2], CVMRotaryFaderCtrl::Dot);
		m_Rotary1.SetKnobRadius(20); 
	}
	else {
		m_Rotary1.SetBitmap(m_hBitmap[Dot1], CVMRotaryFaderCtrl::Dot);
	}

	if(hbmpKnob == m_hBitmap[Knob1]) 
		m_Rotary1.SetKnobRadius(30); 
	else
		m_Rotary1.SetKnobRadius(20); 

}

