#include "stdafx.h"
#include "../../Globals.h"
#include "../../res/Resource.h"
#include "Midi Doc.h"
#include "MidiMon.h"
#include "MidiMonThread.h"
#include "../../ThreadNames.h"
#include "../../Mirage Editor.h"

IMPLEMENT_DYNCREATE(CMidiMonChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CMidiMonChildWnd, CMDIChildWnd)
END_MESSAGE_MAP()

CMidiMonChildWnd::CMidiMonChildWnd()
{
}


CWnd* CMidiMonChildWnd::CreateView(CCreateContext* pContext, UINT nID)
{
	ASSERT(m_hWnd != NULL);
	ASSERT(::IsWindow(m_hWnd));
	ENSURE_ARG(pContext != NULL);
	ENSURE_ARG(pContext->m_pNewViewClass != NULL);

	m_nID = nID;

	BOOL mode = FALSE;
	if (mode)
	{
		CWnd* pParentWnd = (CWnd*)CWnd::FromHandle(this->GetSafeHwnd());

		// Note: can be a CWnd with PostNcDestroy self cleanup
		CWnd* pView = (CWnd*)pContext->m_pNewViewClass->CreateObject();
		if (pView == NULL)
		{
			TRACE(traceAppMsg, 0, "Warning: Dynamic create of view type %hs failed.\n",
				pContext->m_pNewViewClass->m_lpszClassName);
			return NULL;
		}
		ASSERT_KINDOF(CWnd, pView);

		// views are always created with a border!
		if (!pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
			CRect(0,0,0,0), pParentWnd, nID, pContext))
		{
			TRACE(traceAppMsg, 0, "Warning: could not create view for frame.\n");
			return NULL;        // can't continue without a view
		}

		if (pView->GetExStyle() & WS_EX_CLIENTEDGE)
		{
			// remove the 3d style from the frame, since the view is
			//  providing it.
			// make sure to recalc the non-client area
			ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
		}
		return pView;
	} else {
		// Start the thread 
#pragma warning(push)
#pragma warning(disable:6014)
		theApp.m_pMidiMonThread = new CMidiMonThread(this->GetSafeHwnd());
#pragma warning(pop)

		theApp.m_pMidiMonThread->CreateThread(CREATE_SUSPENDED);
		theApp.m_pMidiMonThread->SetCreateContext(pContext);
		theApp.m_pMidiMonThread->SetID(nID);
		SetThreadName(theApp.m_pMidiMonThread->m_nThreadID, "MIDI Monitor");
		theApp.m_pMidiMonThread->ResumeThread();

//		WaitForSingleObject(theApp.midi_monitor_started, 500);
	Sleep(500);
		if (theApp.m_pMidiMonThread->m_pMainWnd->GetExStyle() & WS_EX_CLIENTEDGE)
		{
			// remove the 3d style from the frame, since the view is
			//  providing it.
			// make sure to recalc the non-client area
			ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
		}

		CMidiDoc* pMidiDoc;

//		theApp.m_pMidiMonThread->m_pMainWnd->ShowWindow(SW_SHOW);

//	pContext->m_pNewDocTemplate->InitialUpdateFrame((CFrameWnd*)theApp.m_pMidiMonThread->m_pMainWnd	, pContext->m_pCurrentDoc);

		return theApp.m_pMidiMonThread->m_pMainWnd;
	}
}

void CMidiMonChildWnd::OnInitialUpdate()
{

}
BOOL CMidiMonChildWnd::OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext)
{
	// default create client will create a view if asked for it
	if (pContext != NULL && pContext->m_pNewViewClass != NULL)
	{
		if (CreateView(pContext, AFX_IDW_PANE_FIRST) == NULL)
			return FALSE;
	}
	return TRUE;
}

BOOL CMidiMonChildWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// The default CFrameWnd::OnCmdMsg function is responsible for
	// dispatching menu commands to ON_COMMAND handlers and for
	// initializing menus and toolbars via ON_UPDATE_COMMAND_UI
	// handlers.  For the MDI child window, all such dispatching
	// and command user interface initialization are done in the
	// main application thread.  The CBounceWnd object processes
	// user interface events in a separate thread.  This override,
	// CBounceMDIChildWnd::OnCmdMsg, delegates command handling to
	// the child CBounceWnd window.  It cannot do this delegation
	// by simply calling CBounceWnd::OnCmdMsg, because doing so
	// would defeat the design of handling CBounceWnd user interface
	// events in the separate thread.  If CBounceMDIChildWnd::OnCmdMsg
	// simply called CBounceWnd::OnCmdMsg, then the command handling
	// would be processed in the same main application thread as that
	// of the MDI child window.
	//
	// A common and safe way to make a call from one thread to another
	// is to call SendMessage for a window whose message pump runs in
	// a second thread.  That is how CBounceMDIChildWnd::OnCmdMsg is
	// implemented.  All of the OnCmdMsg parameters are gathered into
	// an application-defined (but re-usable) COnCmdMsg structure,
	// that is sent by reference to the CBounceWnd window via the
	// lParam of SendMessage.


	CWnd* pMidiViewWnd = (CMidiView*)GetDlgItem(m_nID);
	if (pMidiViewWnd == NULL)
		return FALSE; // child CBounceWnd not created yet.


	// In addition to passing all of the OnCmdMsg parameters to
	// the CBounceWnd window via the SendMessage lParam, it is also
	// necessary to forward the implicit wParam originally passed
	// in the WM_COMMAND message for the menu command.  The original
	// WM_COMMAND wParam is not a parameter of OnCmdMsg, but it can
	// be obtained by calling GetCurrentMessage.  The wParam is needed by
	// the CBounceWnd::OnColor command handler to distinguish which
	// menu item, i.e., which color, was selected from the Color menu.
	//
	// The original single thread MDI sample application, the
	// CBounceWnd::OnColor command handler calls GetCurrentMessage
	// to retrieve the wParam from the WM_COMMAND message.  To satisfy
	// that implementation of CBounceWnd::OnColor, we call
	// GetCurrentMessage() and forward the wParam in the user-defined
	// WM_USER_ONCMDMSG message.  Alternatively, we could have
	// called GetMessageMsg and passed the wParam as an additional
	// member of the COnCmdMsg structure.

	return (BOOL)pMidiViewWnd->SendMessage(nID,
		GetCurrentMessage()->wParam, GetCurrentMessage()->lParam);
}

BOOL CMidiMonChildWnd::DestroyWindow()
{
	theApp.PostThreadMessage(ID_WINDOW_MIDIMONITOR,NULL,NULL);

	return TRUE;
//	return CMDIChildWnd::DestroyWindow();
}

LRESULT CMidiMonChildWnd::OnPrepareToClose(WPARAM, LPARAM)
{
//	CWnd* pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
	
	return 0;
}


