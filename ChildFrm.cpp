// ChildFrm.cpp : implementation of the CChildFrame class
//
#include "stdafx.h"
#include "Mirage Editor.h"

#include "ruler.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

//Create the ruler

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, 
                                CCreateContext* pContext)
{
    if (!m_Rulers.CreateRulers(this, pContext)) {
        TRACE("Error creation of rulers\n");
        return CMDIChildWnd::OnCreateClient(lpcs, pContext);
    }
    return TRUE;
}

//Toggle the ruler
void CChildFrame::ShowRulers(BOOL bShow)
{
    m_Rulers.ShowRulers(bShow);

}

//Update the ruler

void CChildFrame::UpdateRulersInfo(stRULER_INFO stRulerInfo)
{
    m_Rulers.UpdateRulersInfo(stRulerInfo);
}

// CChildFrame message handlers
