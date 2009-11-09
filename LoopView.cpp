// SampleView.cpp : implementation of the CSampleView class
// $Id: LoopView.cpp,v 1.12 2008/02/23 23:58:51 root Exp $

#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

//#include "MirageSysex.h" // For MIRAGE_PAGESIZE
#include "Mirage EditorDoc.h"
#include "LoopDialog.h"
#include "LoopDoc.h"
#include "LoopView.h"


/////////////////////////////////////////////////////////////////////////////
// CSampleView

IMPLEMENT_DYNCREATE(CLoopView, CView)

BEGIN_MESSAGE_MAP(CLoopView, CView)
	//{{AFX_MSG_MAP(CLoopView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

_WaveSample_ CLoopView::m_sWav;
DWORD CLoopView::m_LoopEnd;
DWORD CLoopView::m_LoopStart;

/////////////////////////////////////////////////////////////////////////////
// CSampleView construction/destruction

CLoopView::CLoopView()
{
	// TODO: add construction code here
	m_LoopStart = 0;
	m_LoopEnd = 0;
}

CLoopView::~CLoopView()
{
}

BOOL CLoopView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

/////////////////////////////////////////////////////////////////////////////
// CSampleView drawing

void CLoopView::OnDraw(CDC* pDC)
{
	if (!pDC)
		return;
	
	char szString[30];
	DWORD LoopEnd; 

	CRect Rect;
	// Set pen colors
	pDC->SaveDC();
	CPen BlackPen(PS_SOLID, 1, RGB(0,0,0) );
	CPen GreyPen(PS_SOLID, 1, RGB(150,150,150));
	CPen StartRedPen(PS_SOLID,1, RGB(255,0,0));
	CPen LStartRedPen(PS_SOLID, 1, RGB(255,170,170));
	CPen EndBluePen(PS_SOLID,1,RGB(0,0,255));
	CPen LEndBluePen(PS_SOLID,1,RGB(170,170,255));
	CPen GreenPen(PS_SOLID,1,RGB(0,255,0));
	GetClientRect(&Rect);

	// Setup the window
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(512,300);
	pDC->SetWindowOrg(0, 0);
	pDC->SetViewportOrg(Rect.left, (Rect.bottom / 2));
	pDC->SetViewportExt( Rect.right , Rect.bottom);

//	LOGFONT LogFont;

	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &m_sWav.SampleData );
	for( DWORD p = 0; p < 512; p++ )
	{
		if ( p > MIRAGE_PAGESIZE )
		{
			/* Draw waveform after loop End */
			pDC->SelectObject(&LEndBluePen);
			if ( ((m_LoopEnd - MIRAGE_PAGESIZE) + p) <= m_sWav.data_header.dataSIZE )
			{
				if ( p == 0 )
				{
					pDC->MoveTo(p, 128 - (buffer[ (m_LoopEnd  - MIRAGE_PAGESIZE) + p ]));
				} else {
					pDC->MoveTo(p -1,  128 - (buffer[ (m_LoopEnd - MIRAGE_PAGESIZE) + (p -1 ) ]));
				}
				pDC->LineTo(p, 128 - (buffer[ (m_LoopEnd - MIRAGE_PAGESIZE) + p ]));
			}
			/* Draw waveform after Loop Start */
			pDC->SelectObject(&StartRedPen);
			if ( p == 0 )
			{
				pDC->MoveTo(p, 128 - (buffer[ (p - MIRAGE_PAGESIZE) + m_LoopStart  ]));
			} else {
				pDC->MoveTo(p -1,  128 - (buffer[ (p - 257) + m_LoopStart ]));
			}

			pDC->LineTo(p, 128 - (buffer[ (p - MIRAGE_PAGESIZE) + m_LoopStart  ]));
		}
		if ( p <= MIRAGE_PAGESIZE )
		{
			/* Draw Waveform before Loop Start */
			if ( m_LoopStart >= MIRAGE_PAGESIZE )
			{
				pDC->SelectObject(&LStartRedPen);
				pDC->MoveTo(p, 128 - (buffer[ (p - 257) + m_LoopStart ]));
				pDC->LineTo(p, 128 - (buffer[ (p - MIRAGE_PAGESIZE) + m_LoopStart ]));
			}
			/* Draw waveform before Loop End */
			pDC->SelectObject(&EndBluePen);
			pDC->MoveTo(p, 128 - (buffer[ (m_LoopEnd - 257) + p ]));
			pDC->LineTo(p, 128 - (buffer[ (m_LoopEnd - MIRAGE_PAGESIZE) + p ]));
		}
	}

	// Draw Background
	pDC->SelectObject(&GreyPen);
	if ( (m_LoopEnd & 0xFF) > 0 )
	{
		LoopEnd = (m_LoopEnd + MIRAGE_PAGESIZE) & 0xFF00;
	} else {
		LoopEnd = m_LoopEnd;
	}

	/* Set the font */
	CFont font;

	font.CreateFontA(14,
		0,
		0,
		0,
		FW_LIGHT,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		"Arial"); //CreatePointFont(60, "Arial");
	CFont *pFont = pDC->SelectObject(&font);

	/* Loop End */
	sprintf_s(szString, sizeof(szString),
		"Loop End: %04X",
		m_LoopEnd);
	pDC->TextOutA(4,
					-150,
					szString,
					int(strlen(szString)));

	/* Loop Start */
	sprintf_s(szString, sizeof(szString),
		"Loop Start: %02X",
		m_LoopStart >> 8);
	pDC->TextOutA(260,
					-150,
					szString,
					int(strlen(szString)));

	/* Number of Pages in the Loop */

//	pDC->SelectObject(pFont);
	pDC->SetTextColor(RGB(255, 25, 2));
	sprintf_s(szString,
				sizeof(szString),
				"Pages: %02X (%d)",
				(m_LoopEnd - m_LoopStart) >> 8,
				(m_LoopEnd - m_LoopStart) >> 8);
	pDC->TextOut(220,
					130,
					szString,
					int(strlen(szString)));

	/* Draw Axes */
	pDC->MoveTo(0,0);
	pDC->LineTo(512,0);
	pDC->MoveTo(256,-150);
	pDC->LineTo(256,150);
}

void CLoopView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	if (!theApp.m_CurrentDoc)
		return;

	MWAV hWAV = theApp.m_CurrentDoc->GetMWAV();
	if (hWAV != NULL)
	{
		LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
		memcpy((unsigned char *)&m_sWav,lpWAV,sizeof(m_sWav.riff_header));
		memcpy((unsigned char *)&m_sWav,lpWAV,m_sWav.riff_header.riffSIZE+8);
		m_LoopStart = m_sWav.sampler.Loops.dwStart;
		m_LoopEnd = m_sWav.sampler.Loops.dwEnd;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSampleView diagnostics

#ifdef _DEBUG
void CLoopView::AssertValid() const
{
	CView::AssertValid();
}

void CLoopView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMirageEditorDoc* CLoopView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLoopDoc)));
	return (CMirageEditorDoc*)theApp.m_CurrentDoc;
//	return (CLoopDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSampleView message handlers

void CLoopView::LoopEndF()
{
	if ( (m_LoopEnd - MIRAGE_PAGESIZE) <= m_LoopStart )
		return;
	m_LoopEnd = m_LoopEnd - MIRAGE_PAGESIZE;
}

void CLoopView::LoopEndB()
{
	if( (m_LoopEnd + MIRAGE_PAGESIZE ) >= m_sWav.data_header.dataSIZE)
		return;
	m_LoopEnd = m_LoopEnd + MIRAGE_PAGESIZE;
}

void CLoopView::LoopEndFineF()
{
	if( (m_LoopEnd - 1 ) <= m_LoopStart)
		return;
	m_LoopEnd--;

}

void CLoopView::LoopEndFineB()
{
	if( (m_LoopEnd + 1 ) >= m_sWav.data_header.dataSIZE)
		return;
	m_LoopEnd++;
}

void CLoopView::LoopStartF()
{
	if ( m_LoopStart == 0 )
		return;
	m_LoopStart = m_LoopStart - MIRAGE_PAGESIZE;
}

void CLoopView::LoopStartB()
{
	if ( (m_LoopStart + MIRAGE_PAGESIZE ) >= m_LoopEnd )
		return;
	m_LoopStart = m_LoopStart + MIRAGE_PAGESIZE;
}

void CLoopView::LoopEndPage()
{
	if ( (m_LoopEnd & 0xFF) >= 0x80 )
	{
		m_LoopEnd = (m_LoopEnd + MIRAGE_PAGESIZE) & 0xFF00;
	} else {
		m_LoopEnd = m_LoopEnd & 0xFF00;
	}
	m_LoopEnd--;
}

bool CLoopView::ToggleLoop()
{
	m_sWav.sampler.Loops.dwPlayCount = !m_sWav.sampler.Loops.dwPlayCount; // toggle loop
	return m_sWav.sampler.Loops.dwPlayCount ? false:true;
}

void CLoopView::AlternateLoop()
{
	DWORD counter = 0;
	DWORD LoopSize = m_LoopEnd - m_LoopStart;
	unsigned short samplevalue_end;
	unsigned short samplevalue_start;

	while ( (LoopSize - counter ) > counter )
	{
		samplevalue_end = m_sWav.SampleData[(m_LoopStart+(LoopSize - counter))];
		samplevalue_start = m_sWav.SampleData[(counter+m_LoopStart)];
		m_sWav.SampleData[(m_LoopStart+(LoopSize - counter))] = (samplevalue_end + (255 - m_sWav.SampleData[(counter+m_LoopStart)])) / 2;
		m_sWav.SampleData[(counter+m_LoopStart)] = (samplevalue_start + (255 - samplevalue_end)) / 2;
		counter++;
	}

}

bool CLoopView::LoopStatus()
{
	switch (m_sWav.sampler.Loops.dwPlayCount)
	{
	case 0:
		return true;
		break;
	default:
		return false;
		break;
	}
}

void CLoopView::UseLoop()
{
	m_sWav.sampler.Loops.dwStart = m_LoopStart;
	m_sWav.sampler.Loops.dwEnd = m_LoopEnd;

	MWAV hWAV = theApp.m_CurrentDoc->GetMWAV();
	if (hWAV != NULL)
	{
		LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
//		if (m_sWav.sampler.Loops.dwPlayCount == 1 )
//		{
//			m_sWav.sampler.Loops.dwPlayCount = 0;
			m_sWav.sampler.cSampleLoops = 1;
//		}
		memcpy(lpWAV,(unsigned char *)&m_sWav,m_sWav.riff_header.riffSIZE+8);
	}
}