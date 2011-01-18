// MirageMemLayout.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Dialog_MemLayout.h"
#include "MirageSysex.h"


// CMirageMemLayout dialog

IMPLEMENT_DYNAMIC(CMirageMemLayout, CDialog)

CMirageMemLayout::CMirageMemLayout(CWnd* pParent /*=NULL*/)
	: CDialog(CMirageMemLayout::IDD, pParent)
{

}

CMirageMemLayout::~CMirageMemLayout()
{
}

void CMirageMemLayout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAP_LWAVE1, m_CwndSample[0]);
	DDX_Control(pDX, IDC_MAP_LWAVE2, m_CwndSample[1]);
	DDX_Control(pDX, IDC_MAP_LWAVE3, m_CwndSample[2]);
	DDX_Control(pDX, IDC_MAP_LWAVE4, m_CwndSample[3]);
	DDX_Control(pDX, IDC_MAP_LWAVE5, m_CwndSample[4]);
	DDX_Control(pDX, IDC_MAP_LWAVE6, m_CwndSample[5]);
	DDX_Control(pDX, IDC_MAP_LWAVE7, m_CwndSample[6]);
	DDX_Control(pDX, IDC_MAP_LWAVE8, m_CwndSample[7]);
	DDX_Control(pDX, IDC_MAP_UWAVE1, m_CwndSample[8]);
	DDX_Control(pDX, IDC_MAP_UWAVE2, m_CwndSample[9]);
	DDX_Control(pDX, IDC_MAP_UWAVE3, m_CwndSample[10]);
	DDX_Control(pDX, IDC_MAP_UWAVE4, m_CwndSample[11]);
	DDX_Control(pDX, IDC_MAP_UWAVE5, m_CwndSample[12]);
	DDX_Control(pDX, IDC_MAP_UWAVE6, m_CwndSample[13]);
	DDX_Control(pDX, IDC_MAP_UWAVE7, m_CwndSample[14]);
	DDX_Control(pDX, IDC_MAP_UWAVE8, m_CwndSample[15]);
}


BEGIN_MESSAGE_MAP(CMirageMemLayout, CDialog)
	ON_WM_DRAWITEM()
	ON_STN_CLICKED(IDC_MAP_LWAVE1, &CMirageMemLayout::OnStnClickedMapLwave1)
	ON_STN_CLICKED(IDC_MAP_LWAVE2, &CMirageMemLayout::OnStnClickedMapLwave2)
	ON_STN_CLICKED(IDC_MAP_LWAVE3, &CMirageMemLayout::OnStnClickedMapLwave3)
	ON_STN_CLICKED(IDC_MAP_LWAVE4, &CMirageMemLayout::OnStnClickedMapLwave4)
	ON_STN_CLICKED(IDC_MAP_LWAVE5, &CMirageMemLayout::OnStnClickedMapLwave5)
	ON_STN_CLICKED(IDC_MAP_LWAVE6, &CMirageMemLayout::OnStnClickedMapLwave6)
	ON_STN_CLICKED(IDC_MAP_LWAVE7, &CMirageMemLayout::OnStnClickedMapLwave7)
	ON_STN_CLICKED(IDC_MAP_LWAVE8, &CMirageMemLayout::OnStnClickedMapLwave8)
	// Upper Waves
	ON_STN_CLICKED(IDC_MAP_UWAVE1, &CMirageMemLayout::OnStnClickedMapUwave1)
	ON_STN_CLICKED(IDC_MAP_UWAVE2, &CMirageMemLayout::OnStnClickedMapUwave2)
	ON_STN_CLICKED(IDC_MAP_UWAVE3, &CMirageMemLayout::OnStnClickedMapUwave3)
	ON_STN_CLICKED(IDC_MAP_UWAVE4, &CMirageMemLayout::OnStnClickedMapUwave4)
	ON_STN_CLICKED(IDC_MAP_UWAVE5, &CMirageMemLayout::OnStnClickedMapUwave5)
	ON_STN_CLICKED(IDC_MAP_UWAVE6, &CMirageMemLayout::OnStnClickedMapUwave6)
	ON_STN_CLICKED(IDC_MAP_UWAVE7, &CMirageMemLayout::OnStnClickedMapUwave7)
	ON_STN_CLICKED(IDC_MAP_UWAVE8, &CMirageMemLayout::OnStnClickedMapUwave8)
END_MESSAGE_MAP()

BOOL CMirageMemLayout::OnInitDialog()
{
	CDialog::OnInitDialog();
	Invalidate(FALSE);
	return TRUE;
}

void CMirageMemLayout::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC		pDC;
	CBrush	Brush;
	CBrush	OL_Brush;
	char unsigned bank;
	char unsigned sample=0;
	COLORREF	bgColor;
	CRect	Rect;
	CFont	font;
	char	szString[30];
	CPen Pen;
	double fiddle;

	pDC.Attach(lpDrawItemStruct->hDC);

	pDC.SetMapMode(MM_ANISOTROPIC);
	GetClientRect(&Rect);

	font.CreateFontA(12,
					5,
					0,
					0,
					FW_NORMAL,
					FALSE,
					FALSE,
					FALSE,
					DEFAULT_CHARSET,
					OUT_TT_ONLY_PRECIS,
					CLIP_DEFAULT_PRECIS,
					PROOF_QUALITY,
					DEFAULT_PITCH,
					"Arial");
	/* Set the color of the brush */
	Brush.Detach();
	bgColor = pDC.GetBkColor();

	pDC.FillSolidRect(Rect, pDC.GetBkColor());

	bank=0;
	sample = nIDCtl-17;
	if ( (nIDCtl-16) > 8 )
	{
		bank=1;
		sample=sample-8;
	}

	sprintf_s(szString,
						sizeof(szString),
						"%s %i %02X - %02X  TopKey: %i",
						bank ? "Upper":"Lower",
						(sample+1),
						ProgramDumpTable[bank].WaveSampleControlBlock[sample].SampleStart,
						ProgramDumpTable[bank].WaveSampleControlBlock[sample].SampleEnd,
						ProgramDumpTable[bank].WaveSampleControlBlock[sample].TopKey+1);

	fiddle = (Rect.right - Rect.left) / 256.0;
	Rect.right = (ProgramDumpTable[bank].WaveSampleControlBlock[sample].SampleEnd+1) * fiddle;
	Rect.left = ProgramDumpTable[bank].WaveSampleControlBlock[sample].SampleStart * fiddle;
//	Rect.right = 256;
//	Rect.left = 128;

	Brush.CreateSolidBrush(RGB(0,0x80,0));
//	OL_Brush.CreateSolidBrush(RGB(0x40,0xC0,0x40));
	pDC.SetBkColor(bgColor+RGB(0x0f,0x0f,0x0f));

	pDC.FillRect(Rect, &Brush);

	Pen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	pDC.SetBkColor(bgColor);
	pDC.SelectObject(&font);
	pDC.SelectObject(&Pen);
	pDC.SetBkMode(TRANSPARENT);
	pDC.TextOut(20,0,szString,int(strlen(szString)));

	pDC.Detach();
}

// CMirageMemLayout message handlers

void CMirageMemLayout::OnStnClickedMapLwave1()
{
	Invalidate(FALSE);
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapLwave2()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapLwave3()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapLwave4()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapLwave5()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapLwave6()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapLwave7()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapLwave8()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapUwave1()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapUwave2()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapUwave3()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapUwave4()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapUwave5()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapUwave6()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapUwave7()
{
	// TODO: Add your control notification handler code here
}

void CMirageMemLayout::OnStnClickedMapUwave8()
{
	// TODO: Add your control notification handler code here
}
