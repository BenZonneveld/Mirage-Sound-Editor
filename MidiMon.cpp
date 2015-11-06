#include "stdafx.h"
#include "Globals.h"
#include "Resource.h"
#include "Midi Doc.h"
#include "MidiMon.h"
#include "MidiMonThread.h"
#include "ThreadNames.h"
#include "Mirage Editor.h"

IMPLEMENT_DYNCREATE(CMidiMonChildWnd, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CMidiMonChildWnd, CMDIChildWnd)
	ON_MESSAGE(WM_MM_PUTDATA ,OnPutData)
//	ON_WM_CREATE()
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

		WaitForSingleObject(theApp.midi_monitor_started, INFINITE);

/*		if (theApp.m_pMidiMonThread->m_pMainWnd->GetExStyle() & WS_EX_CLIENTEDGE)
		{
			// remove the 3d style from the frame, since the view is
			//  providing it.
			// make sure to recalc the non-client area
			ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
		}*/
		return theApp.m_pMidiMonThread->m_pMainWnd;
	}
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


BOOL CMidiMonChildWnd::DestroyWindow()
{
	OnPrepareToClose();

	return CMDIChildWnd::DestroyWindow();
}

LRESULT CMidiMonChildWnd::OnPrepareToClose(WPARAM, LPARAM)
{
//	CWnd* pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
	
	return 0;
}

LRESULT CMidiMonChildWnd::OnPutData(WPARAM wParam, LPARAM lParam)
{
	//m_pMidiMonWnd = (CMidiMonWnd*)GetDlgItem(IDC_MIDIMON_WND);
	//if (m_pMidiMonWnd == NULL)
	//	return FALSE; // child CBounceWnd not created yet.

	//return (BOOL)m_pMidiMonWnd->SendMessage(WM_MM_PUTDATA, wParam, lParam);
	return TRUE;
}
