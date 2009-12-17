// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "Mirage Editor.h"

#include "LoopFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CLoopFrame, CDFVFrameWnd)

BEGIN_MESSAGE_MAP(CLoopFrame, CDFVFrameWnd)
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CLoopFrame::CLoopFrame()
{
	// TODO: add member initialization code here
}

CLoopFrame::~CLoopFrame()
{
}


BOOL CLoopFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CDFVFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


// CChildFrame diagnostics

#ifdef _DEBUG
void CLoopFrame::AssertValid() const
{
	CDFVFrameWnd::AssertValid();
}

void CLoopFrame::Dump(CDumpContext& dc) const
{
	CDFVFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CChildFrame message handlers
