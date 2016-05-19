// SimpleWordPadView.cpp : implementation of the CMidiView class
//

#include "stdafx.h"
#include "../Mirage Editor.h"
#include "Midi Doc.h"
#include "Midi View.h"
#include "../ThreadNames.h"

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
	ON_MESSAGE( WM_MM_PUTDATA, OnPutData )
	ON_MESSAGE( WM_PARSESYSEX, OnParseSysex )
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

int CMidiView::OnCreate(LPCREATESTRUCT lpCreateStruct) // This should be called from the Thread
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

	CScrollView::OnInitialUpdate();

	CSize sizeTotal;

	sizeTotal.cx = 50;
	sizeTotal.cy = 2 * m_nLineHt;

	SetScrollSizes(MM_TEXT, sizeTotal);

	dc.SelectObject(pOldFont);

	return 0;
}

BOOL CMidiView::Create(LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName, DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext)
{
	// can't use for desktop or pop-up windows (use CreateEx instead)
	ASSERT(pParentWnd != NULL);
	ASSERT((dwStyle & WS_POPUP) == 0);

/*****************************************************/
	ASSERT(lpszClassName == NULL || AfxIsValidString(lpszClassName) || 
		AfxIsValidAtom(lpszClassName));
	ENSURE_ARG(lpszWindowName == NULL || AfxIsValidString(lpszWindowName));
	
	// allow modification of several common create parameters
	CREATESTRUCT cs;
	cs.dwExStyle = 0;
	cs.lpszClass = lpszClassName;
	cs.lpszName = lpszWindowName;
	cs.style = dwStyle | WS_CHILD;
	cs.x = rect.left;
	cs.y = rect.top;
	cs.cx = rect.right - rect.left;
	cs.cy = rect.bottom - rect.top;
	cs.hwndParent = pParentWnd->GetSafeHwnd();
	cs.hMenu = (HMENU)(UINT_PTR)nID;
	cs.hInstance = AfxGetInstanceHandle();
	cs.lpCreateParams = (LPVOID)pContext;

	if (!PreCreateWindow(cs))
	{
		PostNcDestroy();
		return FALSE;
	}

	AfxHookWindowCreate(this);
	HWND hWnd = ::CreateWindowEx(cs.dwExStyle, cs.lpszClass,
			cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy,
			cs.hwndParent, cs.hMenu, cs.hInstance, cs.lpCreateParams);

#ifdef _DEBUG
	if (hWnd == NULL)
	{
		TRACE(traceAppMsg, 0, "Warning: Window creation failed: GetLastError returns 0x%8.8X\n",
			GetLastError());
	}
#endif

	if (!AfxUnhookWindowCreate())
		PostNcDestroy();        // cleanup if CreateWindowEx fails too soon

	if (hWnd == NULL)
		return FALSE;
	ASSERT(hWnd == m_hWnd); // should have been set in send msg hook
	return TRUE;

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
LRESULT CMidiView::OnPutData(WPARAM wParam, LPARAM lParam)
{
	CMidiDoc* pMidiDoc = GetDocument();

	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
	pMidiDoc->PutData(mydata, pcds->dwData);

	return 0;
}

LRESULT CMidiView::OnParseSysex(WPARAM wParam, LPARAM lParam)
{
	CMidiDoc* pMidiDoc = GetDocument();
	unsigned char * ptr;
	DWORD BytesRecorded;
	BOOL io_dir;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	ptr = (unsigned char *)(pcds->lpData);
	io_dir = (BOOL)(pcds->dwData);
	BytesRecorded = pcds->cbData;

	char SEMessage[64];
	std::string LogMessage;

	unsigned char MessageID;
	int i;

	switch (*(ptr+3))
	{
		case CONFIG_PARM_REQ:
			LogMessage += "Config Parameters Request";
			break;
		case COMMAND_CODE:
			LogMessage += "Command Code";
			switch ( *(ptr+4))
			{
				case SELECT_LOWER:
					LogMessage += "Select Lower Wavesample";
					break;
				case SELECT_UPPER:
					LogMessage += "Select Upper Wavesample";
					break;
			}
			break;
		case CONFIG_PARM_DUMP:
			LogMessage += "Config Parameters Dump Data";
			break;
		case LOWER_PRG_DUMP_REQ:
			LogMessage += "Lower Program Dump Request";
			break;
		case UPPER_PRG_DUMP_REQ:
			LogMessage += "Upper Program Dump Request";
			break;
		case WAVE_DUMP_REQ:
			LogMessage += "Wave Dump Request";
			break;
		case PRG_DUMP_LOWER:
			LogMessage += "Lower Program Dump Data";
			break;
		case PRG_DUMP_UPPER:
			LogMessage += "Upper Program Dump Data";
			break;
		case WAVE_DUMP_DATA:
			LogMessage += "Wave Dump Data";
			break;
		case PRG_STATUS_MSG:
			LogMessage += "Program Status Message";
			break;
		case WAVE_STATUS_MSG:
			LogMessage += "Wavesample Status Message";
			break;
		case WAVE_ACK:
			LogMessage += "Wavesample acknowledge";
			break;
		case WAVE_NACK:
			LogMessage += "Wavesample NOT acknowleged";
			break;
		case WAVEDUMPABSREQ:
			LogMessage += "Wavesample Dump Absolute Request";
			break;
		case WAVEDUMPABSDATA:
			LogMessage += "Wavesample Dump Absolute Data";
			break;
		case PRG_PARM_MSG:
			LogMessage += "Program Parameter Message";
			break;
		case SMP_PARM_MSG:
			LogMessage += "Wavesample Parameter Message";
			break;
		case WAVEMANIPCMD:
			LogMessage += "Wavesample Manipulation Function Command";
			break;
		default:
			LogMessage += "Unknown Mirage Sysex";
			sprintf(SEMessage," Message ID: %02X ",*(ptr+3));
			LogMessage += SEMessage;
	}
	if ( *(ptr) != 0xF0 && *(ptr+(BytesRecorded-1)) != 0xF7 )
	{
		return 0;
	}
	pMidiDoc->PutData(LogMessage, io_dir);
	LogMessage.clear();

	if ( BytesRecorded > 47 ) BytesRecorded = 47;

	for(i=0 ; i <= BytesRecorded; i++)
	{
		sprintf(SEMessage,"%02X ",*(ptr+i));
		LogMessage += SEMessage;
		if ( (i+1) % 16 == 0 )
		{
			pMidiDoc->PutData(LogMessage, io_dir);
			LogMessage.clear();
		}
	}
	if ( i % 16 != 0 )
	{
		pMidiDoc->PutData(LogMessage, io_dir);
	}
	return 0;
}
