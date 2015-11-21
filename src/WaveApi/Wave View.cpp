// Mirage EditorView.cpp : implementation of the CMirageEditorView class
//
#include "stdafx.h"
#include "d3dx9.h"
#include "d3d9.h"
#include "d3dx9core.h"
#include "D3DX9Mesh.h"

#include "../Globals.h"

#include "../Mirage Editor.h"

#include "Wave Doc.h"
#include "Wave View.h"

#include "../Gui/CntrItem.h"
#include "Wavesamples.h"
#include "../Dialogs/Dialog_ReceiveSamples.h"

#include "../Midi/MirageSysex.h"

#include "../Dsp/float_cast.h"
#include "../Midi/Mirage Helpers.h"
#include "../Dialogs/Dialog_TransmitSamples.h"
#include "../Dialogs/Dialog_Resample.h"
#include "../Resource.h"
#include "Wavapi.h"
#include "../Dialogs/Dialog_KeyMapper.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMirageEditorView

IMPLEMENT_DYNCREATE(CMirageEditorView, CScrollView)

BEGIN_MESSAGE_MAP(CMirageEditorView, CScrollView)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_TOOLS_LOOPWINDOW, OnToolsLoopwindow)
	ON_COMMAND(ID_TOOLS_REVERSESAMPLE, OnToolsReversesample)
	ON_COMMAND(ID_TOOLS_RESAMPLE, OnToolsResample)
	ON_COMMAND(ID_TOOLS_NORMALIZE, OnToolsNormalize)
#ifdef _DEBUG
	ON_COMMAND(ID_TOOLS_RESYNTHESIZE, &CMirageEditorView::OnToolsResynthesize)
#endif
	ON_COMMAND(ID_TOOLS_DETECTPITCH, &CMirageEditorView::OnToolsDetectpitch)
	ON_COMMAND(ID_TOOLS_ALLIGNTOPAGES, &CMirageEditorView::OnToolsAllignToPages)
	ON_COMMAND(ID_MIRAGE_SENDSAMPLE, OnMirageSendsample)
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_DISPLAYTYPE_WAVEDRAW, OnUpdateDisplaytype)
	ON_COMMAND(ID_DISPLAYTYPE_WAVEDRAW, TypeWaveDraw)
	ON_UPDATE_COMMAND_UI(ID_DISPLAYTYPE_WAVEDRAW_OLD, OnUpdateDisplaytype)
	ON_UPDATE_COMMAND_UI(ID_DISPLAYTYPE_3DTYPEA, OnUpdateDisplaytype)
	ON_COMMAND(ID_DISPLAYTYPE_3DTYPEA, Type3D_A)
	ON_UPDATE_COMMAND_UI(ID_DISPLAYTYPE_3DTYPEB, OnUpdateDisplaytype)
	ON_COMMAND(ID_DISPLAYTYPE_3DTYPEB, Type3D_B)
	ON_UPDATE_COMMAND_UI(ID_PLAYSND, OnUpdatePlayButton)
	ON_UPDATE_COMMAND_UI(ID_LOOP, OnUpdateLoopButton)
	ON_COMMAND(ID_LOOP, &CMirageEditorView::LoopToggle)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditMenu)
	ON_COMMAND(ID_PASTESPECIAL_APPEND, OnPastespecialAppend)
	ON_COMMAND(ID_PASTESPECIAL_PREPEND, OnPastespecialPrepend)
	ON_COMMAND(ID_PASTESPECIAL_MULTIPLECOPIES, OnPastespecialMultiplecopies)
	ON_UPDATE_COMMAND_UI(ID_PASTESPECIAL_APPEND, OnUpdatePastespecialAppend)
	ON_UPDATE_COMMAND_UI(ID_PASTESPECIAL_PREPEND, OnUpdatePastespecialPrepend)
	ON_UPDATE_COMMAND_UI(ID_PASTESPECIAL_MULTIPLECOPIES, OnUpdatePastespecialMultiplecopies)
	ON_COMMAND(ID_MIRAGE_SENDLOOPPOINTS, &CMirageEditorView::OnMirageSendlooppoints)
	ON_UPDATE_COMMAND_UI(ID_TRUNCATE_AFTERLOOP, &CMirageEditorView::OnUpdateTruncateAfterloop)
	ON_UPDATE_COMMAND_UI(ID_TRUNCATE_BEFORELOOP, &CMirageEditorView::OnUpdateTruncateBeforeloop)
	ON_UPDATE_COMMAND_UI(ID_TRUNCATE_ONLYKEEPLOOP, &CMirageEditorView::OnUpdateTruncateKeepLoop)
	ON_COMMAND(ID_TRUNCATE_AFTERLOOP, &CMirageEditorView::OnTruncateAfterloop)
	ON_COMMAND(ID_TRUNCATE_BEFORELOOP, &CMirageEditorView::OnTruncateBeforeloop)
	ON_COMMAND(ID_TRUNCATE_ONLYKEEPLOOP, &CMirageEditorView::OnTruncateOnlykeeploop)
	ON_COMMAND(ID_DISPLAYTYPE_WAVEDRAW, &CMirageEditorView::TypeWaveDraw)
	ON_COMMAND(ID_DISPLAYTYPE_3DTYPEA, &CMirageEditorView::Type3D_A)
	ON_COMMAND(ID_DISPLAYTYPE_3DTYPEB, &CMirageEditorView::Type3D_B)
END_MESSAGE_MAP()

// CMirageEditorView construction/destruction

CMirageEditorView::CMirageEditorView()
{
	m_pSelection = NULL;
	// TODO: add construction code here

}

CMirageEditorView::~CMirageEditorView()
{
}

BOOL CMirageEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CScrollView::PreCreateWindow(cs);
}

// CMirageEditorView drawing

void CMirageEditorView::OnDraw(CDC* pDC)
{
	if (!pDC)
		return;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	switch(pDoc->DisplayType())
	{
		case 'W':
		case 'w':
			Mode_Wavedraw(pDC);
			break;
		case 'A':
		case 'a':
			Mode_3dTypeA(pDC);
			break;
		case 'B':
		case 'b':
			Mode_3dTypeB(pDC);
			break;
	}
}

BOOL CMirageEditorView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

void CMirageEditorView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CDC* pDC;

	pDC=GetDC();
	m_pSelection = NULL;    // initialize selection
	ASSERT(GetDocument() != NULL);
	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
}

void CMirageEditorView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
	{
		pActiveItem->Deactivate();
		ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	}
	GetDocument()->KillD3DWindow();
	CScrollView::OnDestroy();

}

// Moving the Mousewheel
BOOL CMirageEditorView::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return false;

	if(zDelta > 0 )
	{
		if ( fFlags & MK_CONTROL && pDoc->DisplayType() != 'W' )
		{
			pDoc->RatioDec();
			Resample();
			//Zita_Resampler();
		} else {
			if ( fFlags & MK_SHIFT )
			{
				pDoc->ZoomDecTen();
			} else {
				pDoc->ZoomDec(); 
			}
		}
	} else {
		if ( fFlags & MK_CONTROL && pDoc->DisplayType() != 'W')
		{
			pDoc->RatioInc();
			Resample();
		} else {
			if ( fFlags & MK_SHIFT )
			{
				pDoc->ZoomIncTen();
			} else {
				pDoc->ZoomInc();
			}
		}
	}

	if ( pDoc->DisplayType() == 'A' )
	{
		Mode_3dTypeA(GetDC());
	} else {
		Invalidate(FALSE);
	}

	return CView::OnMouseWheel(fFlags, zDelta, point);
}

void CMirageEditorView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRect WindowRect;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;


	if ( nChar > 0x30 && nChar < 0x39 )
	{
		nChar = nChar - 0x30;
		pDoc->SetPageMultiplier(nChar);
		pDoc->SetMesh(NULL);
		if ( pDoc->DisplayType() == 'A' )
		{
			Mode_3dTypeA(GetDC());
		}
		if ( pDoc->DisplayType() == 'B' )
		{
			Invalidate(FALSE);
		}
	}

	switch(nChar)
	{
    case 0x7F:  // Shift + Delete
      KeepOnlySelection();
      Invalidate(FALSE);
      break;
		case 'p':
			if (progress.GetSafeHwnd() == NULL)
			{
				progress.Create(CProgressDialog::IDD, NULL);
				progress.SetWindowTextA("Testing Progress Updates");
				progress.Bar.SetRange32(0,40);
			}
			::PostMessage(progress.GetSafeHwnd(),WM_PROGRESS,0,progress.Bar.GetPos()+1);
		//	::PostMessage(progress.GetSafeHwnd(),WM_PROGRESS,0,progress.Bar.GetPos()+1);
			break;
	}
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}
// OLE Client support and commands

BOOL CMirageEditorView::IsSelected(const CObject* pDocItem) const
{
	// The implementation below is adequate if your selection consists of
	//  only CMirageEditorCntrItem objects.  To handle different selection
	//  mechanisms, the implementation here should be replaced

	// TODO: implement this function that tests for a selected OLE client item

	return pDocItem == m_pSelection;
}

void CMirageEditorView::OnInsertObject()
{
	// Invoke the standard Insert Object dialog box to obtain information
	//  for new CMirageEditorCntrItem object
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	BeginWaitCursor();

	CMirageEditorCntrItem* pItem = NULL;
	TRY
	{
		// Create new item connected to this document
		CMirageEditorDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CMirageEditorCntrItem(pDoc);
		ASSERT_VALID(pItem);

		// Initialize the item from the dialog data
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // any exception will do
		ASSERT_VALID(pItem);
		
        if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);
		// As an arbitrary user interface design, this sets the selection
		//  to the last item inserted

		// TODO: reimplement selection as appropriate for your application
		m_pSelection = pItem;   // set selection to last inserted item
		pDoc->UpdateAllViews(NULL);
	}
	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the container (not the server) causes the deactivation
void CMirageEditorView::OnCancelEditCntr()
{
	// Close any in-place active item on this view.
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place
void CMirageEditorView::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// need to set focus to this item if it is in the same view

		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // don't call the base class
			return;
		}
	}


	CScrollView::OnSetFocus(pOldWnd);
}

void CMirageEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || pDoc->GetSelection() == true )
		return;

	pDoc->SetSelection(true);
	if ( nFlags != (MK_LBUTTON|MK_SHIFT) )
	{
		pDoc->SelectionEnd = -1;
		pDoc->SelectionStart = -1;
	}
}

void CMirageEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || pDoc->GetSelection() == false )
		return;

	pDoc->SetSelection(false);
	pDoc->CheckPoint(); // Save state for undo
}

void CMirageEditorView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->CheckPoint(); // Save state for undo
}

void CMirageEditorView::OnMouseMove( UINT nFlags, CPoint point)
{
/* MK_CONTROL   Set if the CTRL key is down.
 * MK_LBUTTON   Set if the left mouse button is down.
 * MK_MBUTTON   Set if the middle mouse button is down.
 * MK_RBUTTON   Set if the right mouse button is down.
 * MK_SHIFT   Set if the SHIFT key is down.
 */
	DWORD	Xpos;
	DWORD	dwStart;
	DWORD	dwEnd;
	CRect	Rect;
	CString	GenericMessage;

  CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if ( nFlags == 0 )
	{
    pDoc->SetValPointer(false,-1);

    int TokenPos=0;
		CString Token;
		GenericMessage=theApp.GetMainFrame()->GetGenericMessage();
		if ( GenericMessage.GetLength() > 0 )
		{
			Token=GenericMessage.Tokenize(_T(" "),TokenPos);
			while (Token != _T("Samples"))
			{
				Token=GenericMessage.Tokenize(_T(" "),TokenPos);
			}
			if ( TokenPos < GenericMessage.GetLength() && TokenPos > 1 )
				GenericMessage.Truncate(TokenPos-1);
		}
		theApp.GetMainFrame()->SetGenericMessage(GenericMessage);
 		Invalidate(FALSE);
		return;
	}

	if ( pDoc->DisplayType() == 'A' )
	{
		if ( nFlags == MK_LBUTTON )
		{
			pDoc->SetPitchYaw(point);
			Mode_3dTypeA(GetDC());
		} else {
			pDoc->SetLastMouse(point);
		}
	}

	if ( pDoc->DisplayType() != 'w' )
		return;
	if ( nFlags != MK_RBUTTON )
	{
		pDoc->m_startpoint_selected = false;
		pDoc->m_endpoint_selected = false;
	}


	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	_WaveSample_ *pWav;

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);
	dwStart = pWav->sampler.Loops.dwStart;
	dwEnd = pWav->sampler.Loops.dwEnd;

	CDC* pDC;
	pDC = GetDC();
	GetClientRect(&Rect);

	CSize WaveCSize;

	double ZoomLevel = pDoc->ZoomLevel();
	WaveCSize = pDoc->GetDocSize();

	ZoomLevel = (WaveCSize.cx / 65535.0f) * ZoomLevel;

	int x_scale=(10*((10*Rect.right)/(MIRAGE_PAGESIZE)));
	WaveCSize.cx = lrint(Rect.right / ZoomLevel);

	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(x_scale * (65536 + EXTEND),Y_SCALE*320);
	pDC->SetWindowOrg(0, 0);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left, Rect.top);
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( WaveCSize.cx, Rect.bottom);

	pDC->OffsetViewportOrg(-(GetScrollPosition().x),Rect.top);
	pDC->DPtoLP(&point);

	point.x = point.x/x_scale;
	Xpos = point.x & 0xFF00;

  /* Get sample value at cursor position */
	if ( nFlags == MK_CONTROL )
	{
    pDoc->SetValPointer(true,point.x);
    Invalidate(FALSE);
    return;
	}

	// LoopPoints
	if ( nFlags == MK_RBUTTON || nFlags == (MK_SHIFT|MK_RBUTTON)  )
	{
		if ( /*(Xpos == dwStart || Xpos == dwStart - MIRAGE_PAGESIZE  ) 
			&&*/ nFlags == MK_RBUTTON )
		{
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			pDoc->m_startpoint_selected = true;
		}
		if ( /*(point.x <= long(dwEnd + MIRAGE_PAGESIZE) ) 
				&& (point.x >= long(dwEnd - MIRAGE_PAGESIZE)) 
				&& point.x < long(pWav->data_header.dataSIZE) 
				&&*/ nFlags == (MK_SHIFT|MK_RBUTTON) )
		{
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			pDoc->m_endpoint_selected = true;
		} 
		if ( !pDoc->m_startpoint_selected && !pDoc->m_endpoint_selected )
		{
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		}

		if ( !nFlags )
			return;

		/* Set the Loop Startpoint */
		if (Xpos < (dwEnd - MIRAGE_PAGESIZE) &&
			Xpos < pWav->data_header.dataSIZE &&
			pDoc->m_startpoint_selected == true )
		{
			/* Check if the startpoint is before the end of the loop */
			if ( Xpos < dwEnd  || pWav->sampler.Loops.dwPlayCount == 1)
			{
				pWav->sampler.Loops.dwStart = Xpos;
				pWav->sampler.cSampleLoops = 1;
				if ( pWav->sampler.Loops.dwEnd < pWav->sampler.Loops.dwStart )
					pWav->sampler.Loops.dwEnd = (pWav->data_header.dataSIZE - 16);
			}
		}

		/* Set the Loop Endpoint */
		if (point.x > long( dwStart + MIRAGE_PAGESIZE ) &&
			point.x < long(pWav->data_header.dataSIZE) &&
			pDoc->m_endpoint_selected == true )
		{
			if ( point.x >= long(dwStart + MIRAGE_PAGESIZE) || pWav->sampler.Loops.dwPlayCount == 1)
			{
				pWav->sampler.Loops.dwEnd = point.x;
//				if (pWav->sampler.Loops.dwPlayCount == 1 )
//				{
					//pWav->sampler.Loops.dwPlayCount = 0;
					pWav->sampler.cSampleLoops = 1;
//				}
			}
		}
		Invalidate(FALSE);
	}

	/* The selection */
	if ( nFlags == MK_LBUTTON || nFlags == (MK_LBUTTON|MK_SHIFT) )
	{
		int SelectionStart = pDoc->SelectionStart;
		int SelectionEnd = pDoc->SelectionEnd;

		if ( (nFlags == (MK_LBUTTON|MK_SHIFT) || SelectionEnd == -1 ) && 
			point.x <= long(pWav->data_header.dataSIZE) ||
			SelectionStart == -1 )
		{
			SelectionStart = point.x;
			if ( SelectionStart > SelectionEnd && SelectionEnd != -1 )
			{
				SelectionStart = pDoc->SelectionStart;
			}
			if ( SelectionEnd == -1 )
				SelectionEnd = SelectionStart;
		}
		
		if (point.x > long(SelectionStart) &&
			point.x < long(pWav->data_header.dataSIZE))
		{
			SelectionEnd = point.x;
			if ( SelectionEnd <= SelectionStart)
			{
				SelectionEnd = pDoc->SelectionEnd;
			}

		}

//		CString Message;
//		Message.Format("Delta: %i, Ratio: %f", pDoc->SelectionEnd-pDoc->SelectionStart, (256.0/(double)(pDoc->SelectionEnd-pDoc->SelectionStart)));
//		theApp.GetMainFrame()->SetGenericMessage(Message);

		if ( pDoc->SelectionStart != SelectionStart || pDoc->SelectionEnd != SelectionEnd )
		{
			Invalidate(FALSE);
			pDoc->SelectionStart = SelectionStart;
			pDoc->SelectionEnd = SelectionEnd;
		}
	}
}

void CMirageEditorView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();
	//ReSizeD3DScene(GetDocument(),cx, cy);	// Set Up Our Perspective D3D Screen
}

void CMirageEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
//	Invalidate(FALSE);
}

// CMirageEditorView diagnostics

#ifdef _DEBUG
void CMirageEditorView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMirageEditorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMirageEditorDoc* CMirageEditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMirageEditorDoc)));
	return (CMirageEditorDoc*)m_pDocument;
}
#endif //_DEBUG

void CMirageEditorView::OnMirageSendsample()
{
	theApp.m_CurrentDoc = GetDocument();

	if(GetConfigParms())
	{
		if(GetAvailableSamples())
		{
			CTransmitSamples TransmitDlg;
			TransmitDlg.DoModal();
		}
	}
}

void CMirageEditorView::OnMirageSendlooppoints()
{
	theApp.m_CurrentDoc = GetDocument();

	theApp.m_CurrentDoc->SetLoopOnly(true);
	if(GetConfigParms())
	{
		if(GetAvailableSamples())
		{

			CTransmitSamples TransmitDlg;
			TransmitDlg.DoModal();
		}
	}
	theApp.m_CurrentDoc->SetLoopOnly(false);
}

void CMirageEditorView::OnUpdateDisplaytype(CCmdUI *pCmdUI)
{
	CMirageEditorDoc* pDoc = GetDocument();

	CWnd * pMainWindow = AfxGetMainWnd();
	CMenu * pTopLevelMenu = pMainWindow->GetMenu();

	CMenu * pType = pTopLevelMenu->GetSubMenu(4);

	pType->EnableMenuItem(ID_DISPLAYTYPE_WAVEDRAW,MF_ENABLED);	
	pType->CheckMenuItem(ID_DISPLAYTYPE_WAVEDRAW,(pDoc->DisplayType() == 'w' ) ? MF_CHECKED:MF_UNCHECKED);
//	pType->EnableMenuItem(ID_DISPLAYTYPE_WAVEDRAW_OLD,MF_ENABLED);
//	pType->CheckMenuItem(ID_DISPLAYTYPE_WAVEDRAW_OLD,(pDoc->DisplayType() == 'w' ) ? MF_CHECKED:MF_UNCHECKED);
	pType->EnableMenuItem(ID_DISPLAYTYPE_3DTYPEA,MF_ENABLED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEA,(pDoc->DisplayType() == 'A' ) ? MF_CHECKED:MF_UNCHECKED);
	pType->EnableMenuItem(ID_DISPLAYTYPE_3DTYPEB,MF_ENABLED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEB,(pDoc->DisplayType() == 'B' ) ? MF_CHECKED:MF_UNCHECKED);
}

void CMirageEditorView::TypeWaveDraw()
{
	CMirageEditorDoc* pDoc = GetDocument();

	if (pDoc->DisplayType() == 'A' || pDoc->DisplayType() == 'B' )
		pDoc->KillD3DWindow();
	pDoc->DisplayTypeWavedraw();

	Invalidate(FALSE);
}

void CMirageEditorView::Type3D_A()
{
	CRect WindowRect;

	CMirageEditorDoc* pDoc = GetDocument();

	SetScrollSizes(MM_TEXT,CSize(0,0));
	Invalidate(FALSE);
	pDoc->DisplayType3DTypeA();
	GetClientRect(&WindowRect);
	pDoc->CreateD3DWindow(GetDC(),WindowRect);
}

void CMirageEditorView::Type3D_B()
{
	CMirageEditorDoc* pDoc = GetDocument();

	if (pDoc->DisplayType() == 'A')
		pDoc->KillD3DWindow();
	pDoc->DisplayType3DTypeB();

	Invalidate(FALSE);
}

void CMirageEditorView::Mode_Wavedraw(CDC* pDC)
{
	if (!pDC)
		return;

	CRect Rect;
	CRect	LPRect;
//	CRect	UpdateRect;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	CRect RulerRect;
	CSize NewSize;
	_WaveSample_ *pWav;
	CSize	WaveCSize;
	unsigned char MiragePages = 0;
	char szString[30];
	int x_scale = 0;
	int x_pos = 0;
	int x_prev = 0;
//	int windowheight = 256;

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	double ZoomLevel = pDoc->ZoomLevel();
	WaveCSize = pDoc->GetDocSize();

	ZoomLevel =  (WaveCSize.cx / 65535.0f) * ZoomLevel;

	pDC->SaveDC();

	pDC->SetMapMode(MM_ANISOTROPIC);
	GetClientRect(&Rect);
//	GetUpdateRect(&UpdateRect);
	CRect rc_source;

	x_scale=(10*((10*Rect.right)/(MIRAGE_PAGESIZE)));
	WaveCSize.cx = lrint(Rect.right / ZoomLevel);

	// Sets the x- and y-extents of the window associated with the device context.
	pDC->SetWindowExt(x_scale * (65536 + EXTEND),Y_SCALE*320);
	pDC->SetWindowOrg(0, 0);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left, Rect.top);
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( WaveCSize.cx, Rect.bottom);

	// Set scrollsizes
	LPRect.right=(pDoc->GetDocSize().cx * x_scale) * pDoc->ZoomLevel();
	pDC->LPtoDP(LPRect);
	if ( pWav->data_header.dataSIZE < 65535 )
	{
		SetScrollSizes(MM_TEXT, CSize(LPRect.right / pDoc->ZoomLevel(),1));
	} else {
		SetScrollSizes(MM_TEXT, WaveCSize);
	}
	
	pDC->OffsetViewportOrg(-(GetScrollPosition().x),Rect.top);

	rc_source.top = 0;
	rc_source.left = 0;
	rc_source.bottom= Y_SCALE*320;
	rc_source.right = x_scale * (65535 + EXTEND);
	CMemDC dcMemory(pDC, &Rect, &rc_source);

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
					"Arial");
	CFont *pFont2 = pDC->SelectObject(&font);

	/* Set the Pen colors */
	CPen Pen;
	CPen GreenPen(PS_SOLID,1,RGB(240,240,240));
	CPen GreyPen(PS_SOLID,1,PALETTEINDEX(7));
	CBrush GreyBrush(PALETTEINDEX(7));

	Pen.CreatePen(PS_SOLID, 1, RGB(0,0,0) );

	dcMemory.SelectObject(&Pen);

	/* Create the Ruler */
	RulerRect.left = 0;
	RulerRect.bottom = 20*Y_SCALE;
	RulerRect.top = 0;
	RulerRect.right = rc_source.right;
//	RulerRect.right = x_scale * (pWav->data_header.dataSIZE + EXTEND);
	dcMemory.FillRect(RulerRect, &GreyBrush);

	
	/* Draw the selection */
	CRect SelectRect;
	SelectRect.left = x_scale*pDoc->SelectionStart;
	SelectRect.top = 20*Y_SCALE;
	SelectRect.right = x_scale*pDoc->SelectionEnd;
	SelectRect.bottom = 320*Y_SCALE;
//	dcMemory.SetROP2(R2_XORPEN);
	dcMemory.FillSolidRect(SelectRect,RGB(113,206,234));

	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );
	int checktemp = (pWav->data_header.dataSIZE/(2*WaveCSize.cx));

	for( DWORD p = 0; p < pWav->data_header.dataSIZE + EXTEND ; p++ ) 
	{
		if ( p < pWav->data_header.dataSIZE )
		{
			if ( ( p % 256) == 0 || p == pWav->data_header.dataSIZE-1 )
			{
				if ( p > 0 )
					MiragePages++;
				{
					// The Ruler
					if ( p % MIRAGE_PAGESIZE == 0 )
					{
							// Draw page lines
							dcMemory.MoveTo(x_pos,18 * Y_SCALE);

							// Give the page number
							if ( p % ((1+checktemp)*MIRAGE_PAGESIZE) == 0 )
							{

								dcMemory.SelectObject(&Pen);
								dcMemory.SetBkMode(TRANSPARENT);
								sprintf_s(szString,sizeof(szString),"%02X",MiragePages);
								dcMemory.TextOut(x_pos,-156,szString,int(strlen(szString)));
								dcMemory.MoveTo(x_pos, 0 );
							}
							// Draw the page lines
							dcMemory.SelectObject(&GreenPen);
							dcMemory.LineTo(x_pos,320 * Y_SCALE);
							
							// Reset the pen to a black pen
							dcMemory.SelectObject(&Pen);

					}
				}
			}
			if ( p == 0 )
			{
				dcMemory.MoveTo(0,(300 -(buffer[ p ]))*Y_SCALE);
			} else {
				dcMemory.MoveTo((x_pos-x_scale), (300 - (buffer[ p -1 ]))*Y_SCALE);
			}
			/* Draw waveform line */
			//if ( buffer[ p ] > 0 ) 
				dcMemory.LineTo(x_pos, (300 - (buffer[ p ]))*Y_SCALE);
			x_pos += (x_scale);
		}
	}

	theApp.GetMainFrame()->SetPages(GetNumberOfPages(pWav));
	theApp.GetMainFrame()->SetSampleRate(pWav->waveFormat.fmtFORMAT.nSamplesPerSec);
	theApp.GetMainFrame()->SetPitch(pDoc->GetPitch());

	CString Message;

  /* Draw the value pointer */
  if ( pDoc->GetValPointer() == true )
  {
    Message.Format("%4X Samples Value at %4X = %i, div=%.2f",
								pWav->data_header.dataSIZE,
                pDoc->GetValPosition(),
                pWav->SampleData[pDoc->GetValPosition()],
								static_cast<float>(pWav->SampleData[pDoc->GetValPosition()]/2.0f));
    /* Value Pointer */
    Pen.Detach();
    Pen.CreatePen(PS_SOLID,1,RGB(0,255,0));
   	dcMemory.SelectObject(&Pen);
    dcMemory.MoveTo(x_scale*pDoc->GetValPosition(),20*Y_SCALE);
    dcMemory.LineTo(x_scale*pDoc->GetValPosition(),320*Y_SCALE);
  } else {
	  Message.Format("%4X Samples",pWav->data_header.dataSIZE);
  }
	theApp.GetMainFrame()->SetGenericMessage(Message);

	/* Draw the 0 point */
	Pen.Detach();
	Pen.CreatePen(PS_SOLID,1, RGB(255,170,170)); //Light Red Pen
	dcMemory.SelectObject(&Pen);
	dcMemory.MoveTo(0,172*Y_SCALE);
	dcMemory.LineTo(x_scale * (pWav->data_header.dataSIZE + EXTEND),172*Y_SCALE);

	/* Draw the loop Markers */
	Pen.Detach();
	Pen.CreatePen(PS_SOLID,1,RGB(255,0,0)); // Red Pen For Loop Markers
	dcMemory.SelectObject(&Pen);
	/* Loop Start */
	dcMemory.MoveTo(x_scale*pWav->sampler.Loops.dwStart,20*Y_SCALE);
	dcMemory.LineTo(x_scale*pWav->sampler.Loops.dwStart,320*Y_SCALE);

	/* Loop End */
	dcMemory.MoveTo(x_scale*pWav->sampler.Loops.dwEnd,20*Y_SCALE);
	dcMemory.LineTo(x_scale*pWav->sampler.Loops.dwEnd,320*Y_SCALE);
}

void CMirageEditorView::Mode_3dTypeA(CDC* pDC)
{
	if (!pDC)
		return;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	HDC hDC=pDC->GetSafeHdc();
	HWND hWnd;

	hWnd=WindowFromDC(hDC);

	CRect Rect;
	GetClientRect(&Rect);

	D3DMATERIAL9 Mtrl;
	D3DXMATRIXA16 matView;
  D3DXMATRIXA16 matWorld;
  D3DXMATRIXA16 matRotation;
  D3DXMATRIXA16 matTranslation;
	LPD3DXMESH ppMesh = NULL;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	_WaveSample_ *pWav;
	unsigned char MiragePages = 0;
	int z_pos = 0;
	int z_increment = 1;
	int x_pos = 0;
	HRESULT hr;
	UINT	Multiplier = pDoc->GetPageMultiplier();

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	theApp.GetMainFrame()->SetPages(GetNumberOfPages(pWav));
	theApp.GetMainFrame()->SetSampleRate(pWav->waveFormat.fmtFORMAT.nSamplesPerSec);

	BeginD3DScene(pDoc);
	{
		if ( pDoc->GetMesh() == NULL )
		{
			pDoc->Create3DMesh(Rect);
		}
		ppMesh = pDoc->GetMesh();
	

		// Setup a material for the teapot
		ZeroMemory( &Mtrl, sizeof(D3DMATERIAL9) );
		Mtrl.Diffuse.r = 0.4f;
		Mtrl.Diffuse.g = 0.4f;
		Mtrl.Diffuse.b = 0.4f;
		Mtrl.Diffuse.a = 1.0f;

		Mtrl.Specular.r = 0.4f;
		Mtrl.Specular.g = 0.4f;
		Mtrl.Specular.b = 0.4f;
		Mtrl.Specular.a = 1.0f;

/*		Mtrl.Ambient.r = 0.1f;
		Mtrl.Ambient.g = 0.1f;
		Mtrl.Ambient.b = 0.1f;
		Mtrl.Ambient.a = 1.0f;
*/		
		Mtrl.Power = 0.0f;

		pDoc->GetpD3DDevice()->SetMaterial( &Mtrl );
		
		D3DXMatrixIdentity( &matView );
	  pDoc->GetpD3DDevice()->SetTransform( D3DTS_VIEW, &matView );

		// ... and use the world matrix to spin and translate the waveform  
		// out where we can see it...
		D3DXMatrixRotationYawPitchRoll( &matRotation, D3DXToRadian(pDoc->GetPitchYaw().x), D3DXToRadian(pDoc->GetPitchYaw().y), 0.0f );
		D3DXMatrixTranslation( &matTranslation, 0.0f, 0.0f, 1100.0f + (pDoc->GetZ_Offset()) );
		matWorld = matRotation * matTranslation;
		pDoc->GetpD3DDevice()->SetTransform( D3DTS_WORLD, &matWorld );
		ppMesh->DrawSubset(0);
	}
	EndD3DScene(pDoc);

	theApp.GetMainFrame()->SetPages(GetNumberOfPages(pWav));
	theApp.GetMainFrame()->SetSampleRate(pWav->waveFormat.fmtFORMAT.nSamplesPerSec);
	CString Message;
	Message.Format("%4X Samples",pWav->data_header.dataSIZE);
	theApp.GetMainFrame()->SetGenericMessage(Message);
}

void CMirageEditorView::Mode_3dTypeB(CDC* pDC)
{
	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	CRect Rect;
	CRect RulerRect;
	CSize NewSize;
	_WaveSample_ *pWav;
	CSize	WaveCSize;
	unsigned char MiragePages = 0;
	char szString[256];
	int y_offset = 0;
	int y_increment = 5;
	int x_pos = 0;
	long x_scale;
	long y_scale;
	int PenColor = 0;
	UINT	Multiplier = pDoc->GetPageMultiplier();

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	unsigned char PageSkip = pDoc->PageSkip();
	WaveCSize = pDoc->GetDocSize();

	pDC->SaveDC();
	/* Set the Pen colors */
	CPen Pen;

	Pen.CreatePen(PS_SOLID, 1, RGB(0,0,0) );
	
	pDC->SelectObject(&Pen);

	GetClientRect(&Rect);
	WaveCSize.cx = 1;
	
	SetScrollSizes(MM_TEXT,WaveCSize);
	pDC->SetMapMode(MM_ANISOTROPIC);

	int windowheight = 256+(y_increment*(GetNumberOfPages(pWav)/Multiplier));
	int Y_OFFSET=windowheight-256;
//	Y_OFFSET=Y_OFFSET*GetNumberOfPages(pWav);
	x_scale=(100*((Rect.right*10)/(MIRAGE_PAGESIZE*Multiplier)));
	if (Rect.bottom > windowheight )
	{
		y_scale=(1000*(Rect.bottom/windowheight));
	} else {
		y_scale=1000;
	}

	// Sets the x- and y-extents of the window associated with the device context.
	pDC->SetWindowExt(x_scale*MIRAGE_PAGESIZE*Multiplier, y_scale*windowheight);
	pDC->SetWindowOrg(0, windowheight);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left, Rect.top);
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( Rect.right , Rect.bottom);

	CMemDC dcMemory(pDC,&Rect);

	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );

//	y_offset = Y_OFFSET;
	for( DWORD p = 0; p < pWav->data_header.dataSIZE + EXTEND ; p++ ) 
//	for( DWORD p = pWav->data_header.dataSIZE + EXTEND; p >= 0 ; p-- ) 
	{
		if ( p < pWav->data_header.dataSIZE )
		{
			if ( (p % (MIRAGE_PAGESIZE*Multiplier)) == 0 )
			{
				if ( p > 0 )
				{
					MiragePages++;
					y_offset = y_offset + y_increment;
					Pen.Detach();
					PenColor = PenColor + (0xFF / GetNumberOfPages(pWav));
					if ( p > (pWav->sampler.Loops.dwStart - 1 ) && p < (pWav->sampler.Loops.dwEnd + 1) )
					{
						Pen.CreatePen(PS_SOLID, 1, RGB(255,7*(PenColor/9),7*(PenColor/9)) );
					} else {
						Pen.CreatePen(PS_SOLID, 1, RGB(7*(PenColor/8),7*(PenColor/8),7*(PenColor/8)) );
					}
					dcMemory.SelectObject(&Pen);
					x_pos = 0;
				}
			} else {
				x_pos=x_pos+x_scale;
			}
			if ( (p/(MIRAGE_PAGESIZE*Multiplier)) % PageSkip != 0 && 
				(GetNumberOfPages(pWav) -1) != (p/(MIRAGE_PAGESIZE*Multiplier)) )
				continue;
			if ( p == 0 )
			{
				dcMemory.MoveTo(0,(Y_OFFSET+(0xff - buffer[ p ]) - y_offset)*y_scale);
			} else {
				dcMemory.MoveTo((x_pos-x_scale), (Y_OFFSET+(0xff- buffer[ p -1 ]) - y_offset)*y_scale);
			}
			/* Draw waveform line */
			if ( buffer[ p ] > 0 ) 
				dcMemory.LineTo(x_pos, (Y_OFFSET+(0xff - buffer[ p ]) - y_offset)*y_scale);
		}
	}

	theApp.GetMainFrame()->SetPages(GetNumberOfPages(pWav));
	theApp.GetMainFrame()->SetSampleRate(pWav->waveFormat.fmtFORMAT.nSamplesPerSec);
}

void CMirageEditorView::BeginD3DScene(CMirageEditorDoc* pDoc)
{
	// Clear screen and Depth buffer
	pDoc->GetpD3DDevice()->Clear(	0, NULL,
									D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
									D3DCOLOR_COLORVALUE(0.3f,0.3f,0.3f,1.0f),
									1.0f,0);
	pDoc->GetpD3DDevice()->BeginScene();
}

void CMirageEditorView::EndD3DScene(CMirageEditorDoc* pDoc)
{
	pDoc->GetpD3DDevice()->EndScene();
	pDoc->GetpD3DDevice()->Present(NULL, NULL, NULL, NULL); // Display Result
}

void CMirageEditorView::OnUpdatePlayButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMirageEditorView::OnUpdateLoopButton(CCmdUI* pCmdUI)
{
	_WaveSample_ *pWav;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	if( pWav->sampler.Loops.dwPlayCount == 0 )
	{
		pCmdUI->SetCheck(true);
	} else {
		pCmdUI->SetCheck(false);
	}
}

void CMirageEditorView::LoopToggle()
{
	_WaveSample_ *pWav;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	pWav->sampler.Loops.dwPlayCount = ! pWav->sampler.Loops.dwPlayCount;
}

