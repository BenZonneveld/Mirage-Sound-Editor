// Mirage EditorView.cpp : implementation of the CMirageEditorView class
//
#include "stdafx.h"
#include "d3dx9.h"
#include "d3d9.h"
#include "d3dx9core.h"
#include "D3DX9Mesh.h"

#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "Mirage EditorDoc.h"
#include "CntrItem.h"
#include "Wavesamples.h"
#include "Mirage EditorView.h"
#include "MirageSysex.h"
#include "LoopDialog.h"
#include "float_cast.h"
#include "Mirage Helpers.h"
#include "TransmitSamples.h"
#include "Resample_Dialog.h"
#include "samplerate.h" // Libsamplerate
#include "Resource.h"
#include "Wavapi.h"

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
	ON_COMMAND(ID_MIRAGE_SENDSAMPLE, OnMirageSendsample)
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_TOOLS_LOOPWINDOW, OnToolsLoopwindow)
	ON_COMMAND(ID_TOOLS_REVERSESAMPLE, OnToolsReversesample)
	ON_COMMAND(ID_TOOLS_RESAMPLE, OnToolsResample)
	ON_COMMAND(ID_TOOLS_NORMALIZE, OnToolsNormalize)
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
	return FALSE;
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
	KillD3DWindow(GetDocument());
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
	double ratio_old;
	double ratio_fix;

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
			Invalidate();
			//UpdateWindow();
		}
	}

	switch(nChar)
	{
		case 'w':
		case 'W':
			pDoc->DisplayTypeWavedraw();
			Invalidate(FALSE);
			break;
		case 'a':
		case 'A':
			SetScrollSizes(MM_TEXT,CSize(0,0));
			Invalidate(FALSE);
			pDoc->DisplayType3DTypeA();
			CreateD3DWindow(pDoc);
			break;
		case 'b':
		case 'B':
			pDoc->DisplayType3DTypeB();
			Invalidate(FALSE);
			break;
		case 'n':
			OnToolsNormalize();
			break;
		case 'l':
			OnToolsLoopwindow();
			break;
		case 'r':
			OnToolsReversesample();
			break;
		case 's':
			OnToolsResample();
			break;
		case 'f':
		case 'F':
			_WaveSample_ *pWav;
			MWAV hWAV = pDoc->GetMWAV();
			if (hWAV == NULL)
			{
				return;
			}

			LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
			pWav = (_WaveSample_ *)lpWAV;
			::GlobalUnlock((HGLOBAL) hWAV);

			ratio_old=pDoc->GetRatio();
			ratio_fix=256.0/(double)AverageSamplesPeriod(pWav,pDoc->SelectionStart,pDoc->SelectionEnd);
			if ( ratio_fix < 0.5 )
			{
				while ( ratio_fix < 0.5 )
				{
					ratio_fix=2.0*ratio_fix;
				}
			}
			if ( ratio_fix > 2 )
			{
				while ( ratio_fix > 2)
				{
					ratio_fix=ratio_fix / 2.0;
				}
			}
			pDoc->SetRatio(ratio_fix);
			Resample();
			pDoc->SetRatio(ratio_old);
			Invalidate(FALSE);
			break;
	}
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
/*		switch(GetDocument()->DisplayType())
		{
			case 'W':
				OnUpdateDisplaytypeWavedraw();
				break;
			case 'A':
				OnUpdateDisplaytype3dtypea();
				break;
			case 'B':
				OnUpdateDisplaytype3dtypeb();
				break;
	//		default:
	//			Mode_Wavedraw(pDC);
		}*/

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

	if ( nFlags == 0 )
		return;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

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
	WaveCSize.cx = lrint(Rect.right /pDoc->ZoomLevel());
	int x_scale=(10*((10*Rect.right)/(MIRAGE_PAGESIZE))/pDoc->ZoomLevel());

	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(x_scale * (pWav->data_header.dataSIZE + EXTEND),Y_SCALE*320);
	pDC->SetWindowOrg(0, 0);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left, Rect.top);
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( WaveCSize.cx, Rect.bottom);

	pDC->OffsetViewportOrg(-(GetScrollPosition().x),Rect.top);
	pDC->DPtoLP(&point);

	point.x = point.x/x_scale;
	Xpos = point.x & 0xFF00;

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
		//UpdateWindow();
	}

	/* The selection */
	if ( nFlags == MK_LBUTTON || nFlags == (MK_LBUTTON|MK_SHIFT) )
	{
		int SelectionStart = pDoc->SelectionStart;
		int SelectionEnd = pDoc->SelectionEnd;

		if ( (nFlags == (MK_LBUTTON|MK_SHIFT) || SelectionEnd == -1 ) && 
			point.x < long(pWav->data_header.dataSIZE) ||
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

		CString Message;
		Message.Format("Delta: %i, Ratio: %f", pDoc->SelectionEnd-pDoc->SelectionStart, (256.0/(double)(pDoc->SelectionEnd-pDoc->SelectionStart)));
		theApp.GetMainFrame()->SetGenericMessage(Message);

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

void CMirageEditorView::OnToolsLoopwindow()
{
	CLoopDialog LoopEdit;
	CMirageEditorDoc* pDoc = GetDocument();

	theApp.m_CurrentDoc = GetDocument();
	pDoc = theApp.m_CurrentDoc;
	
	LoopEdit.DoModal();
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	Invalidate();
	//UpdateWindow();
}

void CMirageEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
//	InvalidateRect(NULL,FALSE);
//	Invalidate(FALSE);
	UpdateWindow();
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

void CMirageEditorView::OnToolsReversesample()
{
	_WaveSample_ *pWav;
	DWORD	samplesize;
	DWORD	counter = 0;
	unsigned char	samplevalue;

	CMirageEditorDoc* pDoc = GetDocument();
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
	
	samplesize = pWav->data_header.dataSIZE - 16;

	while ( (samplesize - counter) >= counter )
	{
		samplevalue = pWav->SampleData[(samplesize - counter)];
		pWav->SampleData[(samplesize - counter)] = pWav->SampleData[counter];
		pWav->SampleData[counter] = samplevalue;
		counter++;
	}

	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	Invalidate();
	//UpdateWindow();
}

void CMirageEditorView::OnToolsResample()
{
	_WaveSample_ *pWav;
	DWORD	samplesize;
	DWORD	counter = 0;

	CResample_Dialog ResampleDlg;
	// For Resampling:
	SRC_STATE	*src_state ;		// The sample rate converter object
	SRC_DATA	src_data;			// struct to pass audio and control data into the sample rate converter
	int channels = MAX_WAVCHANNELS;	// the number of interleaved channels that the sample rate converter is being asked to process
	int			srcErrorCode;
	int			newRate;
	double		srcRatio = 1.0;
	float		*lpFloatDataIn;		// Float buffer for sample rate conversion
	float		*lpFloatDataOut;
	double		gain = 1.0;
	double		max = 0.0 ;

	CMirageEditorDoc* pDoc = GetDocument();
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
	
	// Allocate memory
	lpFloatDataIn = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,(DWORD)pWav->data_header.dataSIZE*sizeof(float));
	lpFloatDataOut = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(pWav->SampleData)*sizeof(float)); 
	src_data.data_in = lpFloatDataIn;
	src_data.data_out = lpFloatDataOut;
	src_data.input_frames = pWav->data_header.dataSIZE;
	src_data.input_frames_used = 0;
	src_data.output_frames_gen = 0;
	src_data.end_of_input = 1;
	ResampleDlg.m_lpSrcData = (LPSTR *)&src_data;
	ResampleDlg.m_currentpages = GetNumberOfPages(pWav);

	ResampleDlg.DoModal();

	if (ResampleDlg.m_resample_cancel_ok == false )
		return;

	samplesize = 0xFF + (ResampleDlg.m_targetpages * MIRAGE_PAGESIZE);
	srcRatio = samplesize / ( 1.0 * pWav->data_header.dataSIZE );
	src_data.src_ratio = srcRatio;
	src_data.output_frames = samplesize;
	newRate = (int)floor(srcRatio * pWav->waveFormat.fmtFORMAT.nSamplesPerSec);

	// Create ProgressBar Window
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Resampling Progress");

resample:
	src_unchar_to_float_array(pWav->SampleData, lpFloatDataIn, (int)pWav->data_header.dataSIZE);
	if(src_is_valid_ratio(src_data.src_ratio) == 0)
	{
		MessageBox("Sample rate change out of valid range", NULL, MB_ICONERROR | MB_OK);
		goto src_out2;
	}
	/* Initialize the sample rate converter. */
	if ((src_state = src_new (theApp.GetProfileIntA("Settings","SampleRateConverter",0), channels, &srcErrorCode)) == NULL)
		goto src_out;
	if ((srcErrorCode=src_process(src_state, &src_data)))
	{
src_out:
		MessageBox(src_strerror(srcErrorCode), NULL, MB_ICONERROR | MB_OK);
src_out2:
		src_state = src_delete (src_state) ;
		return;
	}
maxgain:
	max = 0.0;
	max = apply_gain(src_data.data_out, src_data.output_frames_gen, channels, max, gain);
	// Redo resample if the gain is too large
	if (max > 1.0)
	{	
		gain = 1.0 / max;
		progress.SetWindowTextA("Changing gain and redoing Resampling");
		goto resample;
	}
	if ( max < 1.0 )
	{
		gain = 1.0 / max;
		goto maxgain;
	}
	src_state = src_delete (src_state);
	src_float_to_unchar_array (lpFloatDataOut, (unsigned char *)pWav->SampleData, src_data.output_frames_gen);
	/* int src_simple (SRC_DATA *data, int converter_type, int channels) ; */
//	dwDataSize = sizeof(sWav.SampleData);
	pWav->waveFormat.fmtFORMAT.nSamplesPerSec = newRate;
	pWav->waveFormat.fmtFORMAT.nAvgBytesPerSec = newRate;
	pWav->data_header.dataSIZE = src_data.output_frames;
	// Also update the Riff Header!
	pWav->riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_) + src_data.output_frames;
	pWav->sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)pWav->waveFormat.fmtFORMAT.nSamplesPerSec);
	pWav->sampler.Loops.dwStart = lrint(pWav->sampler.Loops.dwStart * srcRatio)& 0xFF00;
	pWav->sampler.Loops.dwEnd = lrint(pWav->sampler.Loops.dwEnd * srcRatio);

//	RemoveZeroSamples(pWav);

	progress.DestroyWindow();
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	pDoc->NotFromMirage();
	Invalidate();
	//UpdateWindow();
}


void CMirageEditorView::Resample()
{
	_WaveSample_ *pWav;
	DWORD	counter = 0;

	CResample_Dialog ResampleDlg;
	// For Resampling:
	SRC_STATE	*src_state ;		// The sample rate converter object
	SRC_DATA	src_data;			// struct to pass audio and control data into the sample rate converter
	int channels = MAX_WAVCHANNELS;	// the number of interleaved channels that the sample rate converter is being asked to process
	int			srcErrorCode;
	int			newRate;
	double		srcRatio = 1.0;
	float		*lpFloatDataIn;		// Float buffer for sample rate conversion
	float		*lpFloatDataOut;
	double		gain = 1.0;
	double		max = 0.0 ;

	CMirageEditorDoc* pDoc = GetDocument();
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
	
	// Allocate memory
	lpFloatDataIn = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,(DWORD)pWav->data_header.dataSIZE*sizeof(float));
	lpFloatDataOut = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (DWORD)pWav->data_header.dataSIZE*pDoc->GetRatio()*sizeof(float)); 
	src_data.data_in = lpFloatDataIn;
	src_data.data_out = lpFloatDataOut;
	src_data.input_frames = pWav->data_header.dataSIZE;
	src_data.input_frames_used = 0;
	src_data.output_frames_gen = 0;
	src_data.end_of_input = 1;

	src_data.src_ratio = pDoc->GetRatio();

	src_data.output_frames = src_data.src_ratio*pWav->data_header.dataSIZE;
	newRate = (int)floor(pDoc->GetRatio() * pWav->waveFormat.fmtFORMAT.nSamplesPerSec);

	// Create ProgressBar Window
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA("Resampling Progress");

resample:
	src_unchar_to_float_array(pWav->SampleData, lpFloatDataIn, (int)pWav->data_header.dataSIZE);
	if(src_is_valid_ratio(src_data.src_ratio) == 0)
	{
		MessageBox("Sample rate change out of valid range", NULL, MB_ICONERROR | MB_OK);
		goto src_out2;
	}
	/* Initialize the sample rate converter. */
	if ((src_state = src_new (SRC_LINEAR /*Fast Resample */, channels, &srcErrorCode)) == NULL)
		goto src_out;
	if ((srcErrorCode=src_process(src_state, &src_data)))
	{
src_out:
		MessageBox(src_strerror(srcErrorCode), NULL, MB_ICONERROR | MB_OK);
src_out2:
		src_state = src_delete (src_state) ;
		return;
	}
maxgain:
	max = 0.0;
	max = apply_gain(src_data.data_out, src_data.output_frames_gen, channels, max, gain);
	// Redo resample if the gain is too large
	if (max > 1.0)
	{	
		gain = 1.0 / max;
		progress.SetWindowTextA("Changing gain and redoing Resampling");
		goto resample;
	}
	if ( max < 1.0 )
	{
		gain = 1.0 / max;
		goto maxgain;
	}
	src_state = src_delete (src_state);
	src_float_to_unchar_array (lpFloatDataOut, (unsigned char *)pWav->SampleData, src_data.output_frames_gen);
	/* int src_simple (SRC_DATA *data, int converter_type, int channels) ; */
//	dwDataSize = sizeof(sWav.SampleData);
	pWav->waveFormat.fmtFORMAT.nSamplesPerSec = newRate;
	pWav->waveFormat.fmtFORMAT.nAvgBytesPerSec = newRate;
	pWav->data_header.dataSIZE = src_data.output_frames;
	// Also update the Riff Header!
	pWav->riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_) + src_data.output_frames;
	pWav->sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)pWav->waveFormat.fmtFORMAT.nSamplesPerSec);
	pWav->sampler.Loops.dwStart = lrint(pWav->sampler.Loops.dwStart * srcRatio)& 0xFF00;
	pWav->sampler.Loops.dwEnd = lrint(pWav->sampler.Loops.dwEnd * srcRatio);

	RemoveZeroSamples(pWav);

	progress.DestroyWindow();
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	pDoc->NotFromMirage();
	Invalidate();
	//UpdateWindow();
}

void CMirageEditorView::OnMirageSendsample()
{
	theApp.m_CurrentDoc = GetDocument();

	if(GetMirageOs())
	{
		if(GetAvailableSamples())
		{
			CTransmitSamples TransmitDlg;
			TransmitDlg.DoModal();
		}
	}
}

void CMirageEditorView::OnEditUndo()
{
	CMirageEditorDoc* pDoc = GetDocument();
	pDoc->Undo();
	pDoc->UpdateAllViews(NULL);
}

void CMirageEditorView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	CMirageEditorDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->CanUndo());
}

void CMirageEditorView::OnToolsNormalize()
{
	_WaveSample_ *pWav;
	float		*lpFloatData;
	double		gain = 1.0;
	double		max = 0.0 ;

	CMirageEditorDoc* pDoc = GetDocument();

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	lpFloatData = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(pWav->SampleData)*sizeof(float)); 

ReNormalize:
	src_unchar_to_float_array(pWav->SampleData, lpFloatData, (int)pWav->data_header.dataSIZE);
Normalize:
	max = 0.0;
	max = apply_gain(lpFloatData, (int)pWav->data_header.dataSIZE, 1, max, gain);
	// Redo resample if the gain is too large
	if (max > 1.0)
	{	
		gain = 1.0 / max;
		goto ReNormalize;
	}
	if ( max < 1.0 )
	{
		gain = 1.0 / max;
		goto Normalize;
	}
	src_float_to_unchar_array (lpFloatData, (unsigned char *)&pWav->SampleData, (int)pWav->data_header.dataSIZE);
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	Invalidate();
	//UpdateWindow();
}

void CMirageEditorView::OnEditRedo()
{
	CMirageEditorDoc* pDoc = GetDocument();
	pDoc->Redo();
	pDoc->UpdateAllViews(NULL);
}

void CMirageEditorView::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	CMirageEditorDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->CanRedo());
}

void CMirageEditorView::OnUpdateDisplaytype(CCmdUI *pCmdUI)
{
	CMirageEditorDoc* pDoc = GetDocument();

	CWnd * pMainWindow = AfxGetMainWnd();
	CMenu * pTopLevelMenu = pMainWindow->GetMenu();

	CMenu * pType = pTopLevelMenu->GetSubMenu(4);

	pType->EnableMenuItem(ID_DISPLAYTYPE_WAVEDRAW,MF_ENABLED);	
	pType->CheckMenuItem(ID_DISPLAYTYPE_WAVEDRAW,(pDoc->DisplayType() == 'W' ) ? MF_CHECKED:MF_UNCHECKED);
	pType->EnableMenuItem(ID_DISPLAYTYPE_WAVEDRAW_OLD,MF_ENABLED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_WAVEDRAW_OLD,(pDoc->DisplayType() == 'w' ) ? MF_CHECKED:MF_UNCHECKED);
	pType->EnableMenuItem(ID_DISPLAYTYPE_3DTYPEA,MF_ENABLED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEA,(pDoc->DisplayType() == 'A' ) ? MF_CHECKED:MF_UNCHECKED);
	pType->EnableMenuItem(ID_DISPLAYTYPE_3DTYPEB,MF_ENABLED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEB,(pDoc->DisplayType() == 'B' ) ? MF_CHECKED:MF_UNCHECKED);
}

void CMirageEditorView::TypeWaveDraw()
{
	CMirageEditorDoc* pDoc = GetDocument();

	if ( pDoc->GetMesh() != NULL )
		pDoc->GetMesh()->Release();
	if (pDoc->DisplayType() == 'A')
		KillD3DWindow(pDoc);
	pDoc->DisplayTypeWavedraw();

	Invalidate(FALSE);
}

void CMirageEditorView::Type3D_A()
{
	CMirageEditorDoc* pDoc = GetDocument();

	pDoc->DisplayType3DTypeA();

	/* This creates a memory leak I'm aware of,
	 * but at this moment I do not know how to free the mesh memory
	 */
	if ( pDoc->GetMesh() != NULL )
	{
//			LPDIRECT3DVERTEXBUFFER9 pVB9;
//			pDoc->GetMesh()->GetVertexBuffer(&pVB9);
			
//			pVB9->Release();
//			free(pVB9);
//			D3DVERTEXBUFFER_DESC pDesc;
//			pVB9->GetDesc(&pDesc);

//			LPD3DXMESH pMesh;
//			pMesh = pDoc->GetMesh()->GetVertexBuffer(&ppVB);
//			pMesh->Release;
		pDoc->SetMesh(NULL);
	}

	CreateD3DWindow(pDoc);

}
void CMirageEditorView::Type3D_B()
{
	CMirageEditorDoc* pDoc = GetDocument();

	if ( pDoc->GetMesh() != NULL )
		pDoc->GetMesh()->Release();
	if (pDoc->DisplayType() == 'A')
		KillD3DWindow(pDoc);
	pDoc->DisplayType3DTypeB();

	Invalidate(FALSE);
}

void CMirageEditorView::Mode_Wavedraw(CDC* pDC)
{
	if (!pDC)
		return;

	CRect Rect;

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

	double ZoomLevel = pDoc->ZoomLevel();// * ((sWav.data_header.dataSIZE / 256) / 16) ;
	WaveCSize = pDoc->GetDocSize();

	x_scale=(10*((10*Rect.right)/(MIRAGE_PAGESIZE))/ZoomLevel);

	pDC->SaveDC();

	pDC->SetMapMode(MM_ANISOTROPIC);
	GetClientRect(&Rect);
	CRect rc_source;

	WaveCSize.cx = lrint(Rect.right /ZoomLevel);
	SetScrollSizes(MM_TEXT, WaveCSize);

	// Sets the x- and y-extents of the window associated with the device context.
	pDC->SetWindowExt(x_scale * (pWav->data_header.dataSIZE + EXTEND),Y_SCALE*320);
	pDC->SetWindowOrg(0, 0);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left, Rect.top);
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( WaveCSize.cx, Rect.bottom);

	pDC->OffsetViewportOrg(-(GetScrollPosition().x),Rect.top);

	rc_source.top = 0;
	rc_source.left = 0;
	rc_source.bottom= Y_SCALE*320;
	rc_source.right = x_scale * (pWav->data_header.dataSIZE + EXTEND);
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
	RulerRect.right = x_scale * (pWav->data_header.dataSIZE + EXTEND);
	dcMemory.FillRect(RulerRect, &GreyBrush);
	
	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );
//	for( DWORD p = 0; p < pWav->data_header.dataSIZE + EXTEND ; p++ ) 
	for( DWORD p = 0; p < pWav->data_header.dataSIZE + EXTEND ; p++ ) 
	{
		if ( p < pWav->data_header.dataSIZE )
		{
			if ( ( p % 256) == 0 || p == pWav->data_header.dataSIZE-1 )
			{
				if ( p > 0 )
					MiragePages++;
				//if ( GetNumberOfPages(pWav) < 0x20 || MiragePages % ZoomFactor == 0 ) 
				{
					dcMemory.SelectObject(&GreenPen);
					dcMemory.MoveTo(p,0);
					dcMemory.LineTo(p,2);
					dcMemory.SelectObject(&Pen);
					dcMemory.SetBkMode(TRANSPARENT);
					sprintf_s(szString,sizeof(szString),"%02X",MiragePages);
					dcMemory.TextOut(x_pos,-156,szString,int(strlen(szString)));
				}
			}
			if ( p == 0 )
			{
				dcMemory.MoveTo(0,(300 -(buffer[ p ]))*Y_SCALE);
			} else {
				dcMemory.MoveTo((x_pos-x_scale), (300 - (buffer[ p -1 ]))*Y_SCALE);
			}
			/* Draw waveform line */
			if ( buffer[ p ] > 0 ) 
				dcMemory.LineTo(x_pos, (300 - (buffer[ p ]))*Y_SCALE);
			x_pos += (x_scale);
		}
	}

	theApp.GetMainFrame()->SetPages(GetNumberOfPages(pWav));
	theApp.GetMainFrame()->SetSampleRate(pWav->waveFormat.fmtFORMAT.nSamplesPerSec);

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
	dcMemory.MoveTo(x_scale*(pWav->sampler.Loops.dwStart+1),20*Y_SCALE);
	dcMemory.LineTo(x_scale*(pWav->sampler.Loops.dwStart+1),320*Y_SCALE);

	/* Loop End */
	dcMemory.MoveTo(x_scale*(pWav->sampler.Loops.dwEnd),20*Y_SCALE);
	dcMemory.LineTo(x_scale*(pWav->sampler.Loops.dwEnd),320*Y_SCALE);

	/* Draw the selection */
	CRect LoopRect;
	Pen.Detach();
	Pen.CreatePen(PS_SOLID,1,RGB(0,0,0)); // Red Pen For Loop Markers
	dcMemory.SelectObject(&Pen);

	LoopRect.left = x_scale*pDoc->SelectionStart;
	LoopRect.top = 20*Y_SCALE;
	LoopRect.right = x_scale*pDoc->SelectionEnd;
	LoopRect.bottom = 320*Y_SCALE;
	dcMemory.SetROP2(R2_XORPEN);
	dcMemory.Rectangle(LoopRect);
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

//	InitD3D(pDoc);

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

//	CSize	WaveCSize;
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
			unsigned char PageSkip = pDoc->PageSkip();
//			WaveCSize = pDoc->GetDocSize();

			int z_factor = (Multiplier*MIRAGE_PAGESIZE) / GetNumberOfPages(pWav);

			struct mesh_vertex{
				    D3DXVECTOR3 p;
					D3DXVECTOR3 n;
					DWORD color;
			};

			int		nWidth			= (int)ceil((float)(GetNumberOfPages(pWav)/Multiplier*PageSkip)); 
			int		nHeight			= (MIRAGE_PAGESIZE*Multiplier);
			int		nNumStrips      = nWidth-1;
		    int		nQuadsPerStrip  = nHeight-1;
			DWORD	m_dwNumFaces    = nNumStrips * nQuadsPerStrip * 2;
			DWORD	m_dwNumVertices = nWidth * nHeight;

			hr = D3DXCreateMeshFVF(m_dwNumFaces,
									m_dwNumVertices,
									D3DXMESH_MANAGED|D3DXMESH_32BIT/*|D3DXMESH_WRITEONLY*/,
									D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE,
									pDoc->GetpD3DDevice(),
									&ppMesh);

			DWORD* pIndexBuffer = NULL;
			ppMesh->LockIndexBuffer(0,(void**) &pIndexBuffer );

			int iStrip, iQuad;

			for( iStrip = 0; iStrip<nNumStrips; iStrip++ )
			{
				WORD nCurRow1 = (iStrip+0)*nHeight;
				WORD nCurRow2 = (iStrip+1)*nHeight;

				for( iQuad = 0; iQuad<nQuadsPerStrip; iQuad++ )
				{
					// first tri 
					*pIndexBuffer++ = nCurRow1;
					*pIndexBuffer++ = nCurRow1 + 1;
					*pIndexBuffer++ = nCurRow2;

					// second tri 
					*pIndexBuffer++ = nCurRow1 + 1;
					*pIndexBuffer++ = nCurRow2 + 1;
					*pIndexBuffer++ = nCurRow2;

					nCurRow1++;
					nCurRow2++;
				}
			}
	
			hr = ppMesh->UnlockIndexBuffer();

			mesh_vertex*	pVertexBuffer = NULL;
			hr = ppMesh->LockVertexBuffer( 0, (void**) &pVertexBuffer );
	
			LPDIRECT3DVERTEXBUFFER9 pVB9;
			ppMesh->GetVertexBuffer(&pVB9);
			
			D3DVERTEXBUFFER_DESC pDesc;
			pVB9->GetDesc(&pDesc);

			DWORD VertexBufferStart=(DWORD)pVertexBuffer;

			const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );

			for( DWORD p = 0; p < pWav->data_header.dataSIZE; p++ ) 
			{
				if ( p < pWav->data_header.dataSIZE)
				{
					if ( (p % (MIRAGE_PAGESIZE*Multiplier)) == 0 )
					{
						if ( p > 0 )
						{						
							MiragePages++;
							z_pos = z_pos+ z_increment;
							x_pos = 0;
						}
					} else {
						x_pos++;
					}
/*					if ( (p/(MIRAGE_PAGESIZE*Multiplier)) % PageSkip != 0 && 
						(GetNumberOfPages(pWav) -1) != (p/(MIRAGE_PAGESIZE*Multiplier)) )
						continue;*/
					/* Only Draw values larger than zero */
					if ( buffer[ p ] > 0 ) 
					{
						pVertexBuffer->p=D3DXVECTOR3(static_cast<float>(x_pos/(Multiplier+0.0f)),
													GetWaveValue(pDoc,pWav,x_pos,z_pos),
													static_cast<float>(z_factor*z_pos));
						// Compute the normal by hand
						D3DXVECTOR3 vecN;
						D3DXVECTOR3 vPt = D3DXVECTOR3(static_cast<float>(x_pos/(Multiplier+0.0f)),
													GetWaveValue(pDoc,pWav,x_pos,z_pos),
													static_cast<float>(z_factor*z_pos));
						D3DXVECTOR3 vN = D3DXVECTOR3(static_cast<float>(x_pos/(Multiplier+0.0f)),
													GetWaveValue(pDoc,pWav,x_pos,z_pos+1),
													static_cast<float>((z_factor*z_pos)+1.0f ));
						D3DXVECTOR3 vE = D3DXVECTOR3( static_cast<float>(x_pos/(Multiplier+0.0f))+1.0f,
													GetWaveValue(pDoc,pWav,x_pos+1,z_pos),
													static_cast<float>((z_factor*z_pos) ));					D3DXVECTOR3 v1 = vN - vPt;
						D3DXVECTOR3 v2 = vE - vPt;
						D3DXVec3Cross( &vecN, &v1, &v2 );
						D3DXVec3Normalize(&vecN, &vecN);

						pVertexBuffer->n = vecN;
						if ( p < (pWav->sampler.Loops.dwStart+1) || p > pWav->sampler.Loops.dwEnd )
						{
							pVertexBuffer->color = D3DCOLOR_ARGB(255,0,255,0);
						} else {
							pVertexBuffer->color = D3DCOLOR_ARGB(255,255,0,0);
						}
					}
					if ( (DWORD)pVertexBuffer - VertexBufferStart > pDesc.Size )
						break;
					pVertexBuffer++;
				}
			}
			ppMesh->UnlockVertexBuffer();

			DWORD* pdwAdjacency  = NULL;
			pdwAdjacency = new DWORD[ 3 * ppMesh->GetNumFaces() ];

			hr = ppMesh->GenerateAdjacency(0.0f, pdwAdjacency);
	
			DWORD* pdwAdjacencyOut = NULL;
			DWORD dwFlags = D3DXMESHOPT_VERTEXCACHE; // Was D3DXMESHOPT_VERTEXCACHE
			dwFlags |= D3DXMESHOPT_COMPACT;
			hr = ppMesh->OptimizeInplace( dwFlags, pdwAdjacency, pdwAdjacencyOut, NULL, NULL );

		/*	D3DXBUFFER	pAdjacencyBuffer = NULL;
			hr = D3DXWeldVertices(ppMesh,
									D3DXWELDEPSILONS_WELDALL,
									NULL,
									(DWORD*)pAdjacencyBuffer->GetBufferPointer(),
									(DWORD*)pAdjacencyBuffer->GetBufferPointer(),
									pdwAdjacencyOut,
*/
			//ID3DXMesh* pMesh;
			//D3DXATTRIBUTEWEIGHTS d3daw;
			//ZeroMemory( &d3daw, sizeof(D3DXATTRIBUTEWEIGHTS));
			//d3daw.Position	= 1.0f;
			//d3daw.Boundary	= 10000.0f;
			//d3daw.Normal	= 1.0f;

			//hr = D3DXSimplifyMesh(	ppMesh,
			//						pdwAdjacency,
			//						&d3daw,
			//						NULL,
			//						1,
			//						D3DXMESHSIMP_FACE,
			//						&pMesh);
			//SAFE_RELEASE( ppMesh );
			//ppMesh = pMesh;

			SAFE_DELETE_ARRAY( pdwAdjacency );

			NormalizeMesh(ppMesh, Rect.right * 1.0f, TRUE);

			hr = D3DXSaveMeshToX("C:/Mirage.x",
									ppMesh,
									NULL,
									NULL,
									NULL,
									0,
									D3DXF_FILEFORMAT_TEXT);

			pDoc->SetMesh(ppMesh);
		} else {
			ppMesh = pDoc->GetMesh();
		}

		// Setup a material for the teapot
		ZeroMemory( &Mtrl, sizeof(D3DMATERIAL9) );
		Mtrl.Diffuse.r = 0.4f;
		Mtrl.Diffuse.g = 0.4f;
		Mtrl.Diffuse.b = 0.4f;
		Mtrl.Diffuse.a = 1.0f;

/*		Mtrl.Specular.r = 0.4f;
		Mtrl.Specular.g = 0.4f;
		Mtrl.Specular.b = 0.4f;
		Mtrl.Specular.a = 1.0f;
*/
		Mtrl.Ambient.r = 0.1f;
		Mtrl.Ambient.g = 0.1f;
		Mtrl.Ambient.b = 0.1f;
		Mtrl.Ambient.a = 1.0f;
		
		Mtrl.Power = 0.0f;

		pDoc->GetpD3DDevice()->SetMaterial( &Mtrl );

		pDoc->GetpD3DDevice()->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID|D3DFILL_WIREFRAME);
		
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

	for( DWORD p = 0; p < pWav->data_header.dataSIZE + EXTEND ; p++ ) 
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
					Pen.CreatePen(PS_SOLID, 1, RGB(7*(PenColor/8),7*(PenColor/8),7*(PenColor/8)) );
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

void CMirageEditorView::ReSizeD3DScene(CMirageEditorDoc* pDoc,int width, int height)
{
   if (height==0)				// Prevent A Divide By Zero By
   {
	height=1;				// Making Height Equal One
   }

   D3DXMATRIXA16 matProjection;		// Create A Projection Matrix

   // Calculate The Aspect Ratio Of The Window
   D3DXMatrixPerspectiveFovLH(&matProjection, D3DXToRadian( 45.0f ), static_cast<float>(width/height), 0.1f, 10000.0f);

   pDoc->GetpD3DDevice()->SetTransform( D3DTS_PROJECTION, &matProjection );
   D3DXMatrixIdentity(&matProjection);	// Reset The Projection Matrix
}

int CMirageEditorView::InitD3D(CMirageEditorDoc* pDoc)				// Setup For D3D Goes Here	
{
   int i;
   D3DLIGHT9	pLight[6];
   D3DXVECTOR3	vecDir[6];

   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_ZENABLE,  TRUE ); // Z-Buffer (Depth Buffer)
   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // Disable Backface Culling
   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_LIGHTING, TRUE); // Enable Light
//   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_SPECULARENABLE, TRUE); // Enable specular lighting 
   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(255,255,255)); // Enable ambient light

   ZeroMemory(&pLight,sizeof(D3DLIGHT9)*6);

   // Create a direction for out light - it must be normalized
   vecDir[0] = D3DXVECTOR3(0.0f,-1.0f,0.0f);
   vecDir[1] = D3DXVECTOR3(0.0f,1.0f,0.0f);
   vecDir[2] = D3DXVECTOR3(1.0f,0.0f,0.0f);
   vecDir[3] = D3DXVECTOR3(-1.0f,0.0f,0.0f);
   vecDir[4] = D3DXVECTOR3(0.0f,0.0f,1.0f);
   vecDir[5] = D3DXVECTOR3(0.0f,0.0f,-1.0f);

   for ( i=0 ; i< 2 ; i++ )
   {
		pLight[i].Type = D3DLIGHT_DIRECTIONAL;
		D3DXVec3Normalize( (D3DXVECTOR3*)&pLight[i].Direction, &vecDir[i]);

		pLight[i].Diffuse.r=1.0f;
		pLight[i].Diffuse.g=1.0f;
		pLight[i].Diffuse.b=1.0f;
		pLight[i].Diffuse.a=1.0f;
		pLight[i].Range = 10000.0f;

		pLight[i].Ambient.r=1.0f;
		pLight[i].Ambient.g=1.0f;
		pLight[i].Ambient.b=1.0f;
		pLight[i].Ambient.a=1.0f;

		pDoc->GetpD3DDevice()->SetLight(i,&pLight[i]);
		pDoc->GetpD3DDevice()->LightEnable(i,TRUE);
   }
   return TRUE;				// Initialization Went OK
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

void CMirageEditorView::KillD3DWindow(CMirageEditorDoc* pDoc)
{
	if ( pDoc->GetpD3DDevice() != NULL )
		pDoc->FreeD3DDevice(); // Release D3D Device

	if ( pDoc->GetpD3D() != NULL )
		pDoc->FreepD3D(); // Release D3D Interface

	if ( pDoc->GetMesh() != NULL )
		pDoc->FreeMesh();
}

bool CMirageEditorView::CreateD3DWindow(CMirageEditorDoc* pDoc/*, CRect WindowRect/*, CDC* pDC*/)
{
	LPDIRECT3D9			pD3D=NULL;
	LPDIRECT3DDEVICE9	pD3DDevice=NULL;
    // First some standard Win32 window creating
	CDC* pDC;
	pDC=GetDC();

	CRect WindowRect;
	GetClientRect(&WindowRect);

	HDC hDC=pDC->GetSafeHdc();
	HWND hWnd;

	hWnd=WindowFromDC(hDC);
	
    // Did We Get A Device Context?
    if (!(hDC))	
    {
		KillD3DWindow(pDoc);		// Reset The Display
		MessageBox("Can't Create A Device Context.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
    }

    // Check For The Correct DirectX 3D version
    pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if ( pD3D == NULL )
    {
		KillD3DWindow(pDoc);		// Reset The Display
		MessageBox("Can't find D3D SDK Version 9.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
    }
	
	pDoc->SetpD3D(pD3D);

	// get the display mode
	D3DDISPLAYMODE d3ddm;
	pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

    // Tell the window how we want things to be..
    D3DPRESENT_PARAMETERS d3dpp=
    {
		4*MIRAGE_PAGESIZE/*WindowRect.right*/,			// Back Buffer Width
		2*WindowRect.bottom,			// Back Buffer Height
		d3ddm.Format,		// Back Buffer Format (Color Depth)
		1,			// Back Buffer Count (Double Buffer)
		D3DMULTISAMPLE_NONE,	// No Multi Sample Type
		0,			// No Multi Sample Quality
		D3DSWAPEFFECT_DISCARD,	// Swap Effect (Fast)
		NULL,			// The Window Handle (Use Focus window)
		TRUE,		// Windowed
		TRUE,			// Enable Auto Depth Stencil  
		D3DFMT_D16,		// 16Bit Z-Buffer (Depth Buffer)
		0,			// No Flags
		D3DPRESENT_RATE_DEFAULT,   // Default Refresh Rate
		D3DPRESENT_INTERVAL_DEFAULT	// Presentation Interval (vertical sync)
    };

    // Check The Wanted Surface Format
    if ( FAILED( pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			d3dpp.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat ) ) )
    {
		KillD3DWindow(pDoc);		// Reset The Display
		MessageBox("Can't Find Surface Format.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
    }

    // Create The DirectX 3D Device 
	if(FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					 &d3dpp, &pD3DDevice ) ) )
    {
		KillD3DWindow(pDoc);		// Reset The Display
		MessageBox("Can't Create DirectX 3D Device.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
    }

	pDoc->SetpD3DDevice(pD3DDevice);

 	ReSizeD3DScene(pDoc,WindowRect.right, WindowRect.bottom);	// Set Up Our Perspective D3D Screen

    // Initialize Our Newly Created D3D Window
    if (!InitD3D(pDoc))
    {
		KillD3DWindow(pDoc);		// Reset The Display
		MessageBox("Initialization Failed.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
    }

    return TRUE;			// Success
}

float  CMirageEditorView::GetWaveValue(CMirageEditorDoc* pDoc,_WaveSample_ *pWav,int x, int z)
{	
	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );
	
	float WaveValue;

	int samplepos;
	int maxsize=pWav->data_header.dataSIZE;
	
	samplepos=x+(pDoc->GetPageMultiplier()*MIRAGE_PAGESIZE*z);
	if ( samplepos < maxsize )
	{
		if ( buffer[samplepos] > 1 )
		{
			WaveValue=static_cast<float>(buffer[samplepos]/2);
		} else {
			WaveValue=64.0f;
		}

		return (WaveValue);
	}
	return -1;
}

HRESULT CMirageEditorView::CalcBounds(ID3DXMesh *pMesh, D3DXVECTOR3 *vCenter, float *radius)
{
	BYTE *ptr=NULL;
	HRESULT hr;

	// return failure if no mesh pointer provided
	if (!pMesh)
		return D3DERR_INVALIDCALL;

	// get the face count
	DWORD numVerts=pMesh->GetNumVertices();

	// get the FVF flags
	DWORD fvfSize=D3DXGetFVFVertexSize(pMesh->GetFVF());  // See DX8 Version

	// lock the vertex buffer
	if (FAILED(hr=pMesh->LockVertexBuffer(0,(LPVOID *)&ptr)))

		// return on failure
		return hr;

	// compute bounding sphere
	if (FAILED(hr=D3DXComputeBoundingSphere((D3DXVECTOR3 *) ptr, 
						numVerts, 
						fvfSize,   // See DX8 Version
						vCenter, radius )))
		// return on failure
		return hr;

	// unlock the vertex buffer
	if (FAILED(hr=pMesh->UnlockVertexBuffer()))

		// return on failure
		return hr;
		
	// return success to caller
	return S_OK;
}

HRESULT CMirageEditorView::NormalizeMesh(ID3DXMesh *pMesh, float scaleTo=1.0f, BOOL bCenter=TRUE)
{
	D3DXVECTOR3 vCenter;
	float radius;
	HRESULT hr;

	// calculate bounds of mesh
	if (FAILED(hr=CalcBounds(pMesh,&vCenter,&radius)))
		return hr;

	// calculate scaling factor
	float scale=scaleTo/radius;

	// calculate offset if centering requested
	D3DXVECTOR3 vOff;
	if (bCenter) 
		vOff=-vCenter;
	else
		vOff=D3DXVECTOR3(0.0f,0.0f,0.0f);

	// scale and offset mesh
	return ScaleMesh(pMesh,scale,&vOff);
}

HRESULT CMirageEditorView::ScaleMesh(ID3DXMesh *pMesh, float scale, D3DXVECTOR3 *offset=NULL)
{
	BYTE *ptr=NULL;
	HRESULT hr;
	D3DXVECTOR3 vOff;

	// return failure if no mesh pointer set
	if (!pMesh)
		return D3DERR_INVALIDCALL;

	// select default or specified offset vector
	if (offset)
		vOff=*offset;
	else
		vOff=D3DXVECTOR3(0.0f,0.0f,0.0f);

	// get the face count
	DWORD numVerts=pMesh->GetNumVertices();

	// get the FVF flags
	DWORD fvf=pMesh->GetFVF();

	// calculate vertex size
	DWORD vertSize=D3DXGetFVFVertexSize(fvf);

	// lock the vertex buffer
	if (FAILED(hr=pMesh->LockVertexBuffer(0,(LPVOID *)&ptr)))
	
		// return on failure
		return hr;

	// loop through the vertices
	for (DWORD i=0;i<numVerts;i++) {

		// get pointer to location
		D3DXVECTOR3 *vPtr=(D3DXVECTOR3 *) ptr;

		// scale the vertex
		*vPtr+=vOff;
		vPtr->x*=scale;
		vPtr->y*=scale;
		vPtr->z*=scale;

		// increment pointer to next vertex
		ptr+=vertSize;
	}

	// unlock the vertex buffer
	if (FAILED(hr=pMesh->UnlockVertexBuffer()))

		// return on failure
		return hr;
		
	// return success to caller
	return S_OK;
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

void CMirageEditorView::OnEditCut()
{
	DWORD	LoopEnd;
	DWORD	LoopStart;
	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	COleDataSource* pData = new COleDataSource;

	_WaveSample_ *pWav;

	int clipboardsize = 0;
	int headersize = 0;
	HGLOBAL clipbuffer;
	_WaveSample_ * buffer;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	
	clipboardsize = (pDoc->SelectionEnd - pDoc->SelectionStart);
	headersize = sizeof(pWav->riff_header) +
				sizeof(pWav->waveFormat) +
				sizeof(pWav->sampler) +
				sizeof(pWav->instrument);

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, (headersize + clipboardsize + 9));
	buffer = (_WaveSample_ *)GlobalLock(clipbuffer);

	memcpy(buffer,pWav, headersize + sizeof(pWav->data_header));
	buffer->riff_header.riffSIZE = headersize + clipboardsize;
	buffer->data_header.dataSIZE = clipboardsize;

	memcpy(buffer->SampleData,
			pWav->SampleData + pDoc->SelectionStart,
			clipboardsize);

	GlobalUnlock(clipbuffer);
	pData->CacheGlobalData(CF_RIFF,clipbuffer);
	pData->SetClipboard();

	memcpy( pWav->SampleData + pDoc->SelectionStart,
			pWav->SampleData + pDoc->SelectionEnd,
			pWav->data_header.dataSIZE - pDoc->SelectionEnd);

	pWav->riff_header.riffSIZE = pWav->riff_header.riffSIZE - clipboardsize;
	pWav->data_header.dataSIZE = pWav->data_header.dataSIZE - clipboardsize;

	LoopStart = pWav->sampler.Loops.dwStart;
	LoopEnd	= pWav->sampler.Loops.dwEnd;

	/* Fix the LoopPoints */
	if (pDoc->SelectionEnd > pWav->sampler.Loops.dwEnd &&
		pDoc->SelectionStart < pWav->sampler.Loops.dwEnd)
	{
		/* Move the Loop End Pointer */
		LoopEnd = pWav->sampler.Loops.dwEnd - (pWav->sampler.Loops.dwEnd - pDoc->SelectionStart);
	}

	if ( pDoc->SelectionEnd < pWav->sampler.Loops.dwEnd )
	{
		LoopEnd = pWav->sampler.Loops.dwEnd - clipboardsize;
	}

	if ( pDoc->SelectionEnd < pWav->sampler.Loops.dwStart )
	{
		LoopStart = pWav->sampler.Loops.dwStart - clipboardsize;
		LoopEnd = pWav->sampler.Loops.dwEnd - clipboardsize;
	}

	if (pDoc->SelectionStart < pWav->sampler.Loops.dwStart &&
		pDoc->SelectionEnd > pWav->sampler.Loops.dwStart )
	{
		LoopStart = pWav->sampler.Loops.dwStart - (pWav->sampler.Loops.dwStart - pDoc->SelectionStart);
	}

	pWav->sampler.Loops.dwStart = LoopStart;
	pWav->sampler.Loops.dwEnd = LoopEnd;

	/* Clear the selection */
	pDoc->SelectionStart = -1;
	pDoc->SelectionEnd = -1;

	::GlobalUnlock((HGLOBAL) hWAV);
	pDoc->CheckPoint(); // Save state for undo
	Invalidate(FALSE);
}

void CMirageEditorView::OnEditCopy()
{
	COleDataSource* pData = new COleDataSource;

	_WaveSample_ *pWav;

	int clipboardsize = 0;
	int headersize = 0;
	HGLOBAL clipbuffer;
	_WaveSample_ * buffer;

	MWAV hWAV = GetDocument()->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;

	::GlobalUnlock((HGLOBAL) hWAV);
		
	clipboardsize = (GetDocument()->SelectionEnd - GetDocument()->SelectionStart);
	headersize = sizeof(pWav->riff_header) +
				sizeof(pWav->waveFormat) +
				sizeof(pWav->sampler) +
				sizeof(pWav->instrument);

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, (headersize + clipboardsize + 9));
	buffer = (_WaveSample_ *)GlobalLock(clipbuffer);

	memcpy(buffer,pWav, headersize + sizeof(pWav->data_header));
	buffer->riff_header.riffSIZE = headersize + clipboardsize;
	buffer->data_header.dataSIZE = clipboardsize;

	memcpy(buffer->SampleData,
			pWav->SampleData + GetDocument()->SelectionStart,
			clipboardsize);

	GlobalUnlock(clipbuffer);
	pData->CacheGlobalData(CF_RIFF,clipbuffer);
	pData->SetClipboard();
}

void CMirageEditorView::OnEditPaste()
{
	COleDataObject  dataObj;

	dataObj.AttachClipboard();

	if ( dataObj.IsDataAvailable(CF_RIFF))
	{
		_WaveSample_ *pWav;
		_WaveSample_ *clip;

		int clipboardsize = 0;
		int headersize = 0;
		HGLOBAL clipbuffer;
		_WaveSample_ * buffer;

		MWAV hWAV = GetDocument()->GetMWAV();
		if (hWAV == NULL)
		{
			return;
		}

		clipbuffer = dataObj.GetGlobalData(CF_RIFF);

		LPSTR lpCLIP = (LPSTR) ::GlobalLock((HGLOBAL) clipbuffer);
		clip = (_WaveSample_ *)lpCLIP;
		::GlobalUnlock((HGLOBAL) clipbuffer);

		LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
		
		pWav = (_WaveSample_ *)lpWAV;

		if ( clip->waveFormat.fmtFORMAT.nSamplesPerSec != pWav->waveFormat.fmtFORMAT.nAvgBytesPerSec )
		{
			// TODO: Maybe resample the clipboard wave to the destination rate
			::GlobalUnlock((HGLOBAL) hWAV);
			return;
		}

		if ( (pWav->data_header.dataSIZE + clip->data_header.dataSIZE) > MAX_WAVESIZE )
		{
			::GlobalUnlock((HGLOBAL) hWAV);
			return;
		}

		memcpy((pWav->SampleData + GetDocument()->SelectionEnd),
				(pWav->SampleData + GetDocument()->SelectionEnd + clip->data_header.dataSIZE),
				(pWav->data_header.dataSIZE - clip->data_header.dataSIZE));

		memcpy((pWav->SampleData + GetDocument()->SelectionStart),
				clip->SampleData,
				clip->data_header.dataSIZE);

		pWav->data_header.dataSIZE = pWav->data_header.dataSIZE + clip->data_header.dataSIZE - (GetDocument()->SelectionEnd - GetDocument()->SelectionStart);
		pWav->riff_header.riffSIZE = pWav->riff_header.riffSIZE + clip->data_header.dataSIZE - (GetDocument()->SelectionEnd - GetDocument()->SelectionStart);

		::GlobalUnlock((HGLOBAL) hWAV);
		GetDocument()->CheckPoint(); // Save state for undo
		Invalidate();
		//UpdateWindow();
	}
}

void CMirageEditorView::OnPastespecialAppend()
{
	COleDataObject  dataObj;

	dataObj.AttachClipboard();

	if ( dataObj.IsDataAvailable(CF_RIFF))
	{
		_WaveSample_ *pWav;
		_WaveSample_ *clip;

		HGLOBAL clipbuffer;

		MWAV hWAV = GetDocument()->GetMWAV();
		if (hWAV == NULL)
		{
			return;
		}

		clipbuffer = dataObj.GetGlobalData(CF_RIFF);

		LPSTR lpCLIP = (LPSTR) ::GlobalLock((HGLOBAL) clipbuffer);
		clip = (_WaveSample_ *)lpCLIP;
		::GlobalUnlock((HGLOBAL) clipbuffer);

		LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
		
		pWav = (_WaveSample_ *)lpWAV;

		if ( clip->waveFormat.fmtFORMAT.nSamplesPerSec != pWav->waveFormat.fmtFORMAT.nAvgBytesPerSec )
		{
			// TODO: Maybe resample the clipboard wave to the destination rate
			::GlobalUnlock((HGLOBAL) hWAV);
			return;
		}

		if ( (pWav->data_header.dataSIZE + clip->data_header.dataSIZE) > MAX_WAVESIZE )
		{
			::GlobalUnlock((HGLOBAL) hWAV);
			return;
		}

		memcpy((pWav->SampleData + GetDocument()->SelectionEnd + clip->data_header.dataSIZE),
				(pWav->SampleData + GetDocument()->SelectionEnd),
				(pWav->data_header.dataSIZE - clip->data_header.dataSIZE));

		memcpy((pWav->SampleData + GetDocument()->SelectionEnd),
				clip->SampleData,
				clip->data_header.dataSIZE);

		pWav->data_header.dataSIZE = pWav->data_header.dataSIZE + clip->data_header.dataSIZE;
		pWav->riff_header.riffSIZE = pWav->riff_header.riffSIZE + clip->data_header.dataSIZE;

		::GlobalUnlock((HGLOBAL) hWAV);
		GetDocument()->CheckPoint(); // Save state for undo
		Invalidate();
		//UpdateWindow();
	}
}

void CMirageEditorView::OnPastespecialPrepend()
{
	COleDataObject  dataObj;

	dataObj.AttachClipboard();

	if ( dataObj.IsDataAvailable(CF_RIFF))
	{
		_WaveSample_ *pWav;
		_WaveSample_ *clip;

		HGLOBAL clipbuffer;

		MWAV hWAV = GetDocument()->GetMWAV();
		if (hWAV == NULL)
		{
			return;
		}

		clipbuffer = dataObj.GetGlobalData(CF_RIFF);

		LPSTR lpCLIP = (LPSTR) ::GlobalLock((HGLOBAL) clipbuffer);
		clip = (_WaveSample_ *)lpCLIP;
		::GlobalUnlock((HGLOBAL) clipbuffer);

		LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
		
		pWav = (_WaveSample_ *)lpWAV;

		if ( clip->waveFormat.fmtFORMAT.nSamplesPerSec != pWav->waveFormat.fmtFORMAT.nAvgBytesPerSec )
		{
			// TODO: Maybe resample the clipboard wave to the destination rate
			::GlobalUnlock((HGLOBAL) hWAV);
			return;
		}

		if ( (pWav->data_header.dataSIZE + clip->data_header.dataSIZE) > MAX_WAVESIZE )
		{
			::GlobalUnlock((HGLOBAL) hWAV);
			return;
		}

		memcpy((pWav->SampleData + GetDocument()->SelectionStart + clip->data_header.dataSIZE),
				(pWav->SampleData + GetDocument()->SelectionStart),
				(pWav->data_header.dataSIZE - clip->data_header.dataSIZE));

		memcpy((pWav->SampleData + GetDocument()->SelectionStart),
				clip->SampleData,
				clip->data_header.dataSIZE);

		pWav->data_header.dataSIZE = pWav->data_header.dataSIZE + clip->data_header.dataSIZE;
		pWav->riff_header.riffSIZE = pWav->riff_header.riffSIZE + clip->data_header.dataSIZE;

		::GlobalUnlock((HGLOBAL) hWAV);
		GetDocument()->CheckPoint(); // Save state for undo
		Invalidate();
		//UpdateWindow();
	}
}

void CMirageEditorView::OnPastespecialMultiplecopies()
{
	Invalidate();
	//UpdateWindow();
}

void CMirageEditorView::OnUpdateEditMenu(CCmdUI* pCmdUI)
{
	// most Edit menu commands are enabled only if we have a selection
	//  and there are no in-place activations for this view
	pCmdUI->Enable(	GetDocument()->SelectionStart > -1 &&
					GetDocument()->SelectionEnd > -1 &&
					GetDocument()->GetMWAV() != NULL );
}

void CMirageEditorView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	// determine if private or standard OLE formats are on the clipboard
	COleDataObject dataObj;
	BOOL bEnable = dataObj.AttachClipboard() &&
		(dataObj.IsDataAvailable(CF_RIFF) ||
		 COleClientItem::CanCreateFromData(&dataObj));

	// enable command based on availability
	pCmdUI->Enable(bEnable);
}

void CMirageEditorView::OnUpdatePastespecialAppend(CCmdUI *pCmdUI)
{
	// determine if private or standard OLE formats are on the clipboard
	COleDataObject dataObj;
	BOOL bEnable = dataObj.AttachClipboard() &&
		(dataObj.IsDataAvailable(CF_RIFF) ||
		 COleClientItem::CanCreateFromData(&dataObj));

	// enable command based on availability
	pCmdUI->Enable(bEnable);
}

void CMirageEditorView::OnUpdatePastespecialPrepend(CCmdUI *pCmdUI)
{
	// determine if private or standard OLE formats are on the clipboard
	COleDataObject dataObj;
	BOOL bEnable = dataObj.AttachClipboard() &&
		(dataObj.IsDataAvailable(CF_RIFF) ||
		 COleClientItem::CanCreateFromData(&dataObj));

	// enable command based on availability
	pCmdUI->Enable(bEnable);
}

void CMirageEditorView::OnUpdatePastespecialMultiplecopies(CCmdUI *pCmdUI)
{
	// determine if private or standard OLE formats are on the clipboard
	COleDataObject dataObj;
	BOOL bEnable = dataObj.AttachClipboard() &&
		(dataObj.IsDataAvailable(CF_RIFF) ||
		 COleClientItem::CanCreateFromData(&dataObj));

	// enable command based on availability
	pCmdUI->Enable(bEnable);
}