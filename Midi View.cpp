// SimpleWordPadView.cpp : implementation of the CMidiView class
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Midi Doc.h"
#include "Midi View.h"
#include "ThreadNames.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMidiView

IMPLEMENT_DYNCREATE(CMidiView, CScrollView)

BEGIN_MESSAGE_MAP(CMidiView, CScrollView)
	//{{AFX_MSG_MAP(CMidiView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMidiView construction/destruction

CMidiView::CMidiView()
{
	// TODO: add construction code here

}

CMidiView::~CMidiView()
{
}

BOOL CMidiView::Create(LPCTSTR lpszClassName,LPCTSTR szTitle, DWORD style, const RECT& rect , CWnd* pParent, UINT nID, CCreateContext* pContext)
{
	// Setup the shared menu
	//if (menu.m_hMenu == NULL)
	//	menu.LoadMenu(IDR_MirageSampDumpTYPE);
	//m_hMenuShared = menu.m_hMenu;

	if(!CScrollView::Create(lpszClassName, szTitle, style, rect, pParent, nID, pContext))
		return FALSE;

//#pragma warning(push)
//#pragma warning(disable:6014)
//	theApp.m_pMidiMonThread = new CMidiMonThread(m_hWnd);
//#pragma warning(pop)
//	theApp.midi_monitor_started = CreateEvent(	NULL,               // default security attributes
//																			TRUE,               // manual-reset event
//																			FALSE,              // initial state is nonsignaled
//																			FALSE);
//
//	theApp.m_pMidiMonThread->CreateThread();
//	SetThreadName(theApp.m_pMidiMonThread->m_nThreadID, "MIDI Monitor");

	return TRUE;
}

BOOL CMidiView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CScrollView::PreCreateWindow(cs);
}

int CMidiView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CClientDC dc(this);
	m_ftTimes.CreatePointFont(110,"Courier New",&dc);
	
	CFont *pOldFont;
	pOldFont = dc.SelectObject(&m_ftTimes);

	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	m_nLineHt = tm.tmHeight + tm.tmExternalLeading;

	dc.SelectObject(pOldFont);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMidiView drawing

void CMidiView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;

	sizeTotal.cx = 50;
	sizeTotal.cy = 2 * m_nLineHt;

	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CMidiView::OnDraw(CDC* pDC)
{
	CMidiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSize sizeTotal;
	POINT scrollPos;

	sizeTotal.cx = 50;
	sizeTotal.cy = m_nLineHt * pDoc->GetSize();

	scrollPos.x = 0;
	scrollPos.y = pDoc->GetSize();

	SetScrollSizes(MM_TEXT, sizeTotal);

//	ScrollToPosition(scrollPos);

	CFont *pOldFont;
	
	int ct;

	pDC->SetBkMode(TRANSPARENT);
	
	int start = GetDeviceScrollPosition().y /m_nLineHt -1;
	if (start < 0) start =0;

	CRect rcClient;

	GetClientRect(&rcClient);

	ct = start + rcClient.Height()/m_nLineHt + 3;

	if( ct > pDoc->GetSize() ) ct=pDoc->GetSize();
	
	pOldFont = pDC->SelectObject(&m_ftTimes);
	
	pDC->SetTextColor(RGB(127,0,0));
	
	for(int i=start;i<ct;i++)
//	for(int i=ct-1;i>=start;i--)
	{
		if ( pDoc->GetIO(i) )
		{
			pDC->SetTextColor(RGB(127,0,0));
		} else {
			pDC->SetTextColor(RGB(0,127,0));
		}
		pDC->TextOut(0,i * m_nLineHt,&pDoc->GetData(i)[0]);
	}
	
	pDC->SelectObject(pOldFont);

}




/////////////////////////////////////////////////////////////////////////////
// CMidiView diagnostics

#ifdef _DEBUG
void CMidiView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMidiView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

CMidiDoc* CMidiView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMidiDoc)));
	return (CMidiDoc*)m_pDocument;
}

CMidiView * CMidiView::GetView()
{
  CMDIChildWnd * pChild =
      ((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();

  if ( !pChild )
      return NULL;

  CView * pView = pChild->GetActiveView();

  if ( !pView )
     return NULL;

  // Fail if view is of wrong kind
  if ( ! pView->IsKindOf( RUNTIME_CLASS(CMidiView) ) )
     return NULL;

  return (CMidiView *) pView;
}


/////////////////////////////////////////////////////////////////////////////
// CMidiView message handlers
