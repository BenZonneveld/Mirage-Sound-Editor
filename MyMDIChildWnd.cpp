#include "stdafx.h"
#include "DocTemplateThread.h"
#include "resource.h"
#include "Mirage Editor.h"

// CMyMDIChildWnd

BEGIN_MESSAGE_MAP(CMyMDIChildWnd, CMDIChildWnd)
END_MESSAGE_MAP()


CMenu CMyMDIChildWnd::menu;

CMyMDIChildWnd::CMyMDIChildWnd()
{
}

BOOL CMyMDIChildWnd::Create(LPCTSTR szTitle, LONG style /* = 0 */,
	const RECT& rect /* = rectDefault */,
	CMDIFrameWnd* parent /* = NULL */)
{
	// Setup the shared menu
	if (menu.m_hMenu == NULL)
		menu.LoadMenu(IDR_MAINFRAME);
	m_hMenuShared = menu.m_hMenu;

	if (!CMDIChildWnd::Create(NULL, szTitle, style, rect, parent))
		return FALSE;

	// The default PostNcDestroy handler will delete this CBounceMDIChildWnd
	// object when destroyed.  When Windows destroys the CBounceMDIChildWnd
	// window, it will also destroy the CBounceWnd, which is the child
	// window of the MDI child window, executing in the separate thread.
	// Finally, when the child CBounceWnd window is destroyed, the
	// CWinThread object will be automatically destroyed, as explained
	// in the comment for CBounceThread::InitInstance in mtbounce.cpp.
//
#pragma warning(push)
#pragma warning(disable:6014)
	theApp.m_pMidMonThread = new CMultiDocTemplateThread(m_hWnd);
#pragma warning(pop)
	theApp.m_pMidMonThread->CreateThread();
//	theApp.m_pMidMonThread->SuspendThread();

	return TRUE;
}


// CMyWnd

BEGIN_MESSAGE_MAP(CMyWnd, CWnd)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CMyWnd, CWnd)

BOOL CMyWnd::Create(LPCTSTR szTitle, LONG style, const RECT &rect, CWnd *pParent)
{
		LPCTSTR lpszMyClassName =
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
			LoadCursor(NULL, IDC_UPARROW),
			(HBRUSH)(COLOR_WINDOW+1),
			NULL);

	return CWnd::Create(lpszMyClassName, szTitle, style, rect, pParent,
		IDC_MONITOR_WND);
}

CMyWnd::CMyWnd()
{
}
