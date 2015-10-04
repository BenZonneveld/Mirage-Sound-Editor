// CMidiView.cpp : implementation file
//

#include "stdafx.h"
#include "MidiDoc.h"
#include "Mirage Editor.h"
#include "MidiView.h"


// CMidiView

IMPLEMENT_DYNCREATE(CMidiView, CScrollView)

CMidiView::CMidiView()
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

CMidiView::~CMidiView()
{
}


BEGIN_MESSAGE_MAP(CMidiView, CScrollView)
END_MESSAGE_MAP()


// CMidiView drawing

void CMidiView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

BOOL CMidiView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CScrollView::PreCreateWindow(cs);
}

void CMidiView::OnDraw(CDC* pDC)
{
	CMidiDoc* pDoc = GetDocument();
	// TODO: add draw code here
}


// CMidiView diagnostics

#ifdef _DEBUG
void CMidiView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMidiView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMidiView message handlers
