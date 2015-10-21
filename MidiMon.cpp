#include "stdafx.h"
#include "Globals.h"
#include "Resource.h"
#include "Midi Doc.h"
#include "MidiMon.h"
#include "MidiMonThread.h"


BEGIN_MESSAGE_MAP(CMidiMonChildWnd, CMDIChildWnd)
END_MESSAGE_MAP()

CMidiMonChildWnd::CMidiMonChildWnd()
{
}

BOOL CMidiMonChildWnd::Create(LPCTSTR szTitle, LONG style, const RECT& rect, CMDIFrameWnd *pParent)
{
		// Setup the shared menu
	if (menu.m_hMenu == NULL)
		menu.LoadMenu(IDR_MirageSampDumpTYPE);
	m_hMenuShared = menu.m_hMenu;

//	if(!CMDIChildWnd::Create(NULL, szTitle, style, rect, pParent))
//		return FALSE;

#pragma warning(push)
#pragma warning(disable:6014)
	CMidiMonThread* pMidiMonThread = new CMidiMonThread(m_hWnd);
#pragma warning(pop)

	pMidiMonThread->CreateThread();

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


	CWnd* pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
	if (pMidiMonWnd == NULL)
		return FALSE; // child CBounceWnd not created yet.

	// It is safe to create the COnCmdMsg on the stack because
	// it will not be deleted until the SendMessage call has
	// synchronously executed.

	COnCmdMsg oncmdmsg;
	oncmdmsg.m_nID =    nID;
	oncmdmsg.m_nCode = nCode;
	oncmdmsg.m_pExtra = pExtra;
	oncmdmsg.m_pHandlerInfo = pHandlerInfo;

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

	return (BOOL)pMidiMonWnd->SendMessage(WM_USER_ONCMDMSG,
		GetCurrentMessage()->wParam, (LPARAM)&oncmdmsg);
}

BOOL CMidiMonChildWnd::DestroyWindow()
{
	OnPrepareToClose();

	return CMDIChildWnd::DestroyWindow();
}

LRESULT CMidiMonChildWnd::OnPrepareToClose(WPARAM, LPARAM)
{
	CWnd* pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
	
	return 0;
}

BEGIN_MESSAGE_MAP(CMidiMonWnd, CWnd)
	//{{AFX_MSG_MAP(CMidiMonWnd)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MM_PUTDATA , CMidiMonWnd::OnPutData)
	ON_MESSAGE(WM_USER_ONCMDMSG, OnDelegatedCmdMsg)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMidiMonWnd, CWnd)

BOOL CMidiMonWnd::Create(LPCTSTR szTitle, LONG style, const RECT &rect, CWnd *pParent)
{
	LPCTSTR lpszMidiMonClass = 
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
		LoadCursor(NULL, IDC_UPARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL);

	return CWnd::Create(lpszMidiMonClass, szTitle, style, rect, pParent, IDC_MIDIMON_WND);
}

CMidiMonWnd::CMidiMonWnd()
{
	m_pMidiDoc = new CMidiDoc();
}

LRESULT CMidiMonWnd::OnDelegatedCmdMsg(WPARAM, LPARAM lParam)
{
	COnCmdMsg* pOnCmdMsg = (COnCmdMsg*)lParam;
	return CWnd::OnCmdMsg(pOnCmdMsg->m_nID, pOnCmdMsg->m_nCode, pOnCmdMsg->m_pExtra,
			pOnCmdMsg->m_pHandlerInfo);
}

LRESULT CMidiMonWnd::OnPutData(WPARAM wParam, LPARAM lParam)
{
//	MSG msg;

//	PeekMessage(&msg, NULL, WM_MIDIMONITOR,WM_MIDIMONITOR, PM_REMOVE);

	string mydata;
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	mydata=(LPCTSTR)(pcds->lpData);
//	theApp.m_pMidiDoc->PutData(mydata, pcds->dwData);
	return true;
}