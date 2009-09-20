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
#include "samplerate.h"
#include "Resource.h"

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
//	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_MIRAGE_SENDSAMPLE, &CMirageEditorView::OnMirageSendsample)
	ON_COMMAND(ID_OLE_INSERT_NEW, &CMirageEditorView::OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, &CMirageEditorView::OnCancelEditCntr)
	ON_COMMAND(ID_EDIT_UNDO, &CMirageEditorView::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CMirageEditorView::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CMirageEditorView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CMirageEditorView::OnUpdateEditRedo)
	ON_COMMAND(ID_TOOLS_LOOPWINDOW, &CMirageEditorView::OnToolsLoopwindow)
	ON_COMMAND(ID_TOOLS_REVERSESAMPLE, &CMirageEditorView::OnToolsReversesample)
	ON_COMMAND(ID_TOOLS_RESAMPLE, &CMirageEditorView::OnToolsResample)
	ON_COMMAND(ID_TOOLS_NORMALIZE, &CMirageEditorView::OnToolsNormalize)
	ON_COMMAND(ID_DISPLAYTYPE_WAVEDRAW, &CMirageEditorView::OnUpdateDisplaytypeWavedraw)
	ON_COMMAND(ID_DISPLAYTYPE_3DTYPEA, &CMirageEditorView::OnUpdateDisplaytype3dtypea)
	ON_COMMAND(ID_DISPLAYTYPE_3DTYPEB, &CMirageEditorView::OnUpdateDisplaytype3dtypeb)
	ON_UPDATE_COMMAND_UI(ID_PLAYSND, &CMirageEditorView::OnUpdatePlayButton)
	ON_UPDATE_COMMAND_UI(ID_LOOP, &CMirageEditorView::OnUpdateLoopButton)
	ON_COMMAND(ID_LOOP, &CMirageEditorView::LoopToggle)
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
			Mode_Wavedraw(pDC);
			//Mode_D3DWavedraw(pDC);
			break;
		case 'A':
			Mode_3dTypeA(pDC);
			break;
		case 'B':
			Mode_3dTypeB(pDC);
			break;
//		default:
//			Mode_Wavedraw(pDC);
	}
}

void CMirageEditorView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CDC* pDC;

	pDC=GetDC();
	m_pSelection = NULL;    // initialize selection
	ASSERT(GetDocument() != NULL);
	SetScrollSizes(MM_TEXT, GetDocument()->GetDocSize());
	CRect Rect;
	GetClientRect(&Rect);
	CreateD3DWindow(GetDocument(),Rect,pDC);
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
		Invalidate();
		UpdateWindow();
	}

	return CView::OnMouseWheel(fFlags, zDelta, point);
}

void CMirageEditorView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
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
			Invalidate(TRUE);
			UpdateWindow();
		}
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
		switch(GetDocument()->DisplayType())
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
		}

		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // don't call the base class
			return;
		}
	}


	CScrollView::OnSetFocus(pOldWnd);
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

	if ( pDoc->DisplayType() != 'W' )
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

	CDC *pDC = GetDC();
	CRect Rect;

	pDC->SetMapMode( MM_ANISOTROPIC );
	this->GetClientRect(&Rect);

	// Sets the x- and y-extents of the window associated with the device context.
		pDC->SetWindowExt(lrint((pWav->data_header.dataSIZE + EXTEND) * pDoc->ZoomLevel()), 
						320);
	pDC->SetWindowOrg(0,0);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left - GetScrollPosition().x, (Rect.bottom/2));
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( Rect.right , Rect.bottom);

	pDC->DPtoLP(&point);

	Xpos = point.x & 0xFF00;

	if ( nFlags == MK_RBUTTON || nFlags == (MK_CONTROL|MK_RBUTTON)  )
	{
		if ( (Xpos == dwStart || Xpos == dwStart - MIRAGE_PAGESIZE ) 
			&& nFlags == MK_RBUTTON )
		{
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			pDoc->m_startpoint_selected = true;
		}
		if ( (point.x <= long(dwEnd + MIRAGE_PAGESIZE) ) 
				&& (point.x >= long(dwEnd - MIRAGE_PAGESIZE)) 
				&& point.x < long(pWav->data_header.dataSIZE) 
				&& nFlags == (MK_CONTROL|MK_RBUTTON) )
		{
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			pDoc->m_endpoint_selected = true;
		} 
		if ( Xpos <= dwStart + 256 && nFlags == MK_RBUTTON )
		{
			SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			pDoc->m_startpoint_selected = true;
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
//				if (pWav->sampler.Loops.dwPlayCount == 1 )
//				{
					//pWav->sampler.Loops.dwPlayCount = 0;
					pWav->sampler.cSampleLoops = 1;
					if ( pWav->sampler.Loops.dwEnd < pWav->sampler.Loops.dwStart )
						pWav->sampler.Loops.dwEnd = (pWav->data_header.dataSIZE - 16);
//				}
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
		Invalidate(true);
		UpdateWindow();
	}

	if ( nFlags == MK_LBUTTON || nFlags == (MK_LBUTTON|MK_SHIFT))
	{
		int SelectionStart = pDoc->SelectionStart;
		int SelectionEnd = pDoc->SelectionEnd;

		CRect LoopRect;
		CRect OldLoop;

		if (nFlags == MK_LBUTTON &&
			//point.x > long( pDoc->SelectionStart ) &&
			point.x < long(pWav->data_header.dataSIZE))
		{
			SelectionStart = point.x;
		}
		
		if (nFlags == (MK_LBUTTON|MK_SHIFT) &&
			point.x < long(pWav->data_header.dataSIZE))
		{
			SelectionEnd = point.x;
		}
		OldLoop.left = pDoc->SelectionStart;
		OldLoop.top = -140;
		OldLoop.right = pDoc->SelectionEnd;
		OldLoop.bottom = 159;


		LoopRect.left = SelectionStart;
		LoopRect.top = -140;
		LoopRect.right = SelectionEnd;
		LoopRect.bottom = 159;

		if ( pDoc->SelectionStart != SelectionStart || pDoc->SelectionEnd != SelectionEnd )
		{
			int Rop2 = pDC->GetROP2();
			pDC->SetROP2(R2_XORPEN);
			pDC->Rectangle(OldLoop);
			pDC->Rectangle(LoopRect);
			pDC->SetROP2(Rop2);
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
	Invalidate(true);
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
	Invalidate(true);
	UpdateWindow();
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

	RemoveZeroSamples(pWav);

	progress.DestroyWindow();
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	pDoc->NotFromMirage();
	Invalidate(true);
	UpdateWindow();
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
	lpFloatDataOut = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(pWav->SampleData)*sizeof(float)); 
	src_data.data_in = lpFloatDataIn;
	src_data.data_out = lpFloatDataOut;
	src_data.input_frames = pWav->data_header.dataSIZE;
	src_data.input_frames_used = 0;
	src_data.output_frames_gen = 0;
	src_data.end_of_input = 1;

	src_data.src_ratio = pDoc->GetRatio();
	src_data.output_frames = src_data.src_ratio*pWav->data_header.dataSIZE;
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
//	Invalidate(true);
//	UpdateWindow();
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
	Invalidate(true);
	UpdateWindow();
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

void CMirageEditorView::OnUpdateDisplaytypeWavedraw()
{
	CMirageEditorDoc* pDoc = GetDocument();
	CWnd * pMainWindow = AfxGetMainWnd();
	CMenu * pTopLevelMenu = pMainWindow->GetMenu();

	CMenu * pType = pTopLevelMenu->GetSubMenu(4);

	pType->CheckMenuItem(ID_DISPLAYTYPE_WAVEDRAW,MF_CHECKED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEA,MF_UNCHECKED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEB,MF_UNCHECKED);
	pDoc->DisplayTypeWavedraw();

	Invalidate();
	UpdateWindow();
}

void CMirageEditorView::OnUpdateDisplaytype3dtypea()
{
	CMirageEditorDoc* pDoc = GetDocument();
	CWnd * pMainWindow = AfxGetMainWnd();
	CMenu * pTopLevelMenu = pMainWindow->GetMenu();

	CMenu * pType = pTopLevelMenu->GetSubMenu(4);

	pType->CheckMenuItem(ID_DISPLAYTYPE_WAVEDRAW,MF_UNCHECKED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEA,MF_CHECKED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEB,MF_UNCHECKED);
	pDoc->DisplayType3DTypeA();

	Invalidate();
	UpdateWindow();
}

void CMirageEditorView::OnUpdateDisplaytype3dtypeb()
{
	CMirageEditorDoc* pDoc = GetDocument();
	CWnd * pMainWindow = AfxGetMainWnd();
	CMenu * pTopLevelMenu = pMainWindow->GetMenu();

	CMenu * pType = pTopLevelMenu->GetSubMenu(4);

	pType->CheckMenuItem(ID_DISPLAYTYPE_WAVEDRAW,MF_UNCHECKED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEA,MF_UNCHECKED);
	pType->CheckMenuItem(ID_DISPLAYTYPE_3DTYPEB,MF_CHECKED);
	pDoc->DisplayType3DTypeB();

	Invalidate();
	UpdateWindow();
}

void CMirageEditorView::Mode_Wavedraw(CDC* pDC)
{
	if (!pDC)
		return;

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
	char szString[30];
	
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	double ZoomLevel = pDoc->ZoomLevel();// * ((sWav.data_header.dataSIZE / 256) / 16) ;
	WaveCSize = pDoc->GetDocSize();

	pDC->SaveDC();
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
	CPen Pen(PS_SOLID, 1, RGB(0,0,0) );
	CPen LoopMarker(PS_SOLID,1,RGB(255,0,0));
	CPen RedPen(PS_SOLID,1, RGB(255,170,170));
	CPen GreenPen(PS_SOLID,1,RGB(240,240,240));
	CPen GreyPen(PS_SOLID,1,PALETTEINDEX(7));
	CBrush GreyBrush(PALETTEINDEX(7));

	/* Load the Bitmaps */
/*	CBitmap bmLoopStart;
	CBitmap bmLoopEnd;
	CDC dcMemory;
	bmLoopStart.LoadBitmap(IDB_LOOPSTART);
	bmLoopEnd.LoadBitmap(IDB_LOOPEND);
	dcMemory.CreateCompatibleDC(pDC);
*/

	pDC->SelectObject(&Pen);
	GetClientRect(&Rect);
	WaveCSize.cx = lrint(Rect.right /ZoomLevel);
	pDC->SetMapMode(MM_ANISOTROPIC);
	SetScrollSizes(MM_TEXT, WaveCSize);

	// Sets the x- and y-extents of the window associated with the device context.
	pDC->SetWindowExt(lrint((pWav->data_header.dataSIZE + EXTEND) * ZoomLevel), 
		320);
	pDC->SetWindowOrg(0, 0);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left - GetScrollPosition().x, (Rect.bottom/2));
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( Rect.right , Rect.bottom);

	/* Create the Ruler */
	RulerRect.left = 0;
	RulerRect.bottom = -140;
	RulerRect.top = -160;
	RulerRect.right = lrint((pWav->data_header.dataSIZE + EXTEND) * ZoomLevel);
	pDC->FillRect(RulerRect, &GreyBrush);

	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );
	for( DWORD p = 0; p < pWav->data_header.dataSIZE + EXTEND ; p++ ) 
	{
		if ( p < pWav->data_header.dataSIZE )
		{
			if ( ( p % 256) == 0 || p == pWav->data_header.dataSIZE-1 )
			{
				if ( p > 0 )
					MiragePages++;
				int ZoomFactor = 1+lrint(1/ZoomLevel);
				if ( GetNumberOfPages(pWav) < 0x20 || MiragePages % ZoomFactor == 0 ) 
				{
					pDC->SelectObject(&GreenPen);
					pDC->MoveTo(p,-160);
					pDC->LineTo(p,-140);
					pDC->SelectObject(&Pen);
					pDC->SetBkMode(TRANSPARENT);
					sprintf_s(szString,sizeof(szString),"%02X",MiragePages);
					pDC->TextOut(p,-156,szString,int(strlen(szString)));
				}
			}
			if ( p == 0 )
			{
				pDC->MoveTo(0,128 -(buffer[ p ]));
			} else {
				pDC->MoveTo(p-1 , 128 - (buffer[ p -1 ]));
			}
			/* Draw waveform line */
			if ( buffer[ p ] > 0 ) 
				pDC->LineTo(p, 128 - (buffer[ p ]));
		}
	}

	/* Display the samplesize in Mirage Pages */
	sprintf_s(szString,
				sizeof(szString),
				"Size: %d (%02X) Sample Pages",
				GetNumberOfPages(pWav),
				GetNumberOfPages(pWav));
	TextOut(pDC->operator HDC( ),
			lrint(20*ZoomLevel),
			145,
			szString,
			int(strlen(szString)));

	/* Draw the 0 point */
	pDC->SelectObject(&RedPen);
	pDC->MoveTo(0,0);
	pDC->LineTo(pWav->data_header.dataSIZE,0);

	/* Loop Selection rectangle */
	CRect LoopRect;
	pDC->SelectObject(&LoopMarker);
/*	LoopRect.left = pWav->sampler.Loops.dwStart+1;
	LoopRect.top = -140;*/
	/* Loop Start */
	pDC->MoveTo(pWav->sampler.Loops.dwStart+1,-140);
	pDC->LineTo(pWav->sampler.Loops.dwStart+1,159);

	pDC->MoveTo(pWav->sampler.Loops.dwEnd,-140);
	pDC->LineTo(pWav->sampler.Loops.dwEnd,159);
/*
	LoopRect.right = pWav->sampler.Loops.dwEnd;
	LoopRect.bottom = 159;
	int Rop2 = pDC->GetROP2();
	pDC->SetROP2(R2_XORPEN);
	pDC->Rectangle(LoopRect);
	pDC->SetROP2(Rop2);
*/
	pDC->RestoreDC(-1);
}

void CMirageEditorView::Mode_D3DWavedraw(CDC* pDC)
{
	if (!pDC)
		return;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	pDC->SaveDC();

	CRect Rect;
	CRect RulerRect;
	CSize NewSize;
	_WaveSample_ *pWav;
	CSize	WaveCSize;
	unsigned char MiragePages = 0;
	char szString[30];
	
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	double ZoomLevel = pDoc->ZoomLevel();// * ((sWav.data_header.dataSIZE / 256) / 16) ;
	WaveCSize = pDoc->GetDocSize();

	GetClientRect(&Rect);

	BeginD3DScene(pDoc);
	{
		int WaveSizeExtend = pWav->data_header.dataSIZE + EXTEND;
		struct point_vertex{
							float x, y, z, rhw;  // The transformed(screen space) position for the vertex.
							DWORD colour;        // The vertex colour.
							};

		const DWORD point_fvf=D3DFVF_XYZRHW|D3DFVF_DIFFUSE;

		point_vertex	*wave_data;
		wave_data = (point_vertex*) ::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, WaveSizeExtend * sizeof(point_vertex));
//		point_vertex wave_data[65535]; //Enough data to plot from one edge to the other

		pDoc->GetpD3DDevice()->SetFVF(point_fvf);

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
//		CFont *pFont2 = pDC->SelectObject(&font);

		/* Set the Pen colors */
		CPen Pen(PS_SOLID, 1, RGB(0,0,0) );
		CPen LoopMarker(PS_SOLID,1,RGB(255,0,0));
		CPen RedPen(PS_SOLID,1, RGB(255,170,170));
		CPen GreenPen(PS_SOLID,1,RGB(240,240,240));
		CPen GreyPen(PS_SOLID,1,PALETTEINDEX(7));
		CBrush GreyBrush(PALETTEINDEX(7));

//		pDC->SelectObject(&Pen);
		WaveCSize.cx = lrint(Rect.right /ZoomLevel);
//		pDC->SetMapMode(MM_ANISOTROPIC);
		SetScrollSizes(MM_TEXT, WaveCSize);

		// Sets the x- and y-extents of the window associated with the device context.
//		pDC->SetWindowExt(lrint((pWav->data_header.dataSIZE + EXTEND) * ZoomLevel), 
//			320);
//		pDC->SetWindowOrg(0, 0);

		// Sets the viewport origin of the device context
//		pDC->SetViewportOrg( Rect.left - GetScrollPosition().x, (Rect.bottom/2));
		// Sets the x- and y-extents of the viewport of the device context.
//		pDC->SetViewportExt( Rect.right , Rect.bottom);

		/* Create the Ruler */
		RulerRect.left = 0;
		RulerRect.bottom = -140;
		RulerRect.top = -160;
		RulerRect.right = lrint((pWav->data_header.dataSIZE + EXTEND) * ZoomLevel);
//		pDC->FillRect(RulerRect, &GreyBrush);

		const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );
		for( DWORD p = 0; p < pWav->data_header.dataSIZE + EXTEND ; p++ ) 
		{
			if ( p < pWav->data_header.dataSIZE )
			{
				if ( ( p % 256) == 0 || p == pWav->data_header.dataSIZE-1 )
				{
					if ( p > 0 )
						MiragePages++;
					int ZoomFactor = 1+lrint(1/ZoomLevel);
					/* Draw The Ruler */
/*					if ( GetNumberOfPages(pWav) < 0x20 || MiragePages % ZoomFactor == 0 ) 
					{
						pDC->SelectObject(&GreenPen);
						pDC->MoveTo(p,-160);
						pDC->LineTo(p,-140);
						pDC->SelectObject(&Pen);
						pDC->SetBkMode(TRANSPARENT);
						sprintf_s(szString,sizeof(szString),"%02X",MiragePages);
						pDC->TextOut(p,-156,szString,int(strlen(szString)));
					}
*/				}
				if ( p == 0 )
				{
					wave_data[p].x = p;
					wave_data[p].x = static_cast<float>((128 - buffer[ p ])/255);
					wave_data[p].z=-1.0f;
					wave_data[p].rhw=1.0f;
					wave_data[p].colour = D3DCOLOR_XRGB(255,255,255);
//					pDC->MoveTo(0,128 -(buffer[ p ]));
				}
				/* Draw waveform line */
				if ( buffer[ p ] > 0 )
					wave_data[p].x = p;
					wave_data[p].y = static_cast<float>((128 - buffer[ p ])/255);
					wave_data[p].z=-1.0f;
					wave_data[p].rhw=1.0f;
					wave_data[p].colour = D3DCOLOR_XRGB(255,255,255);

//					pDC->LineTo(p, 128 - (buffer[ p ]));
			}
		}

		pDoc->GetpD3DDevice()->DrawPrimitiveUP(D3DPT_LINESTRIP,        //PrimitiveType
			                                 WaveSizeExtend-1,                //PrimitiveCount
						                     wave_data,                   //pVertexStreamZeroData
									         sizeof(point_vertex));  //VertexStreamZeroStride


		/* Display the samplesize in Mirage Pages */
		sprintf_s(szString,
					sizeof(szString),
					"Size: %d (%02X) Sample Pages",
					GetNumberOfPages(pWav),
					GetNumberOfPages(pWav));
/*		TextOut(pDC->operator HDC( ),
				lrint(20*ZoomLevel),
				145,
				szString,
				int(strlen(szString)));
*/
		/* Draw the 0 point */
/*		pDC->SelectObject(&RedPen);
		pDC->MoveTo(0,0);
		pDC->LineTo(pWav->data_header.dataSIZE,0);
*/
		/* Loop Start */
//		pDC->MoveTo(pWav->sampler.Loops.dwStart+1,-140);
//		pDC->LineTo(pWav->sampler.Loops.dwStart+1,159);
		/* Loop End */
//		pDC->MoveTo(pWav->sampler.Loops.dwEnd,-140);
//		pDC->LineTo(pWav->sampler.Loops.dwEnd,159);
	}
	EndD3DScene(pDoc);
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

	CSize	WaveCSize;
	_WaveSample_ *pWav;
	unsigned char MiragePages = 0;
	int z_pos = 0;
	int z_increment = 1;
	int x_pos = 0;
	HRESULT hr;
	UINT	Multiplier = pDoc->GetPageMultiplier();

	BeginD3DScene(pDoc);
	{
		if ( pDoc->GetMesh() == NULL )
		{
			unsigned char PageSkip = pDoc->PageSkip();
			WaveCSize = pDoc->GetDocSize();
			LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
			pWav = (_WaveSample_ *)lpWAV;
			::GlobalUnlock((HGLOBAL) hWAV);

			int z_factor = (Multiplier*MIRAGE_PAGESIZE) / GetNumberOfPages(pWav);

			struct mesh_vertex{
				    D3DXVECTOR3 p;
					D3DXVECTOR3 n;
					DWORD color;
			};

			int		nWidth			= ceil((float)(GetNumberOfPages(pWav)/Multiplier*PageSkip)); 
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
			int iRow, iCol;
			float iX, iZ;

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
													GetWaveValue(pWav,x_pos,z_pos),
													static_cast<float>(z_factor*z_pos));
						// Compute the normal by hand
						D3DXVECTOR3 vecN;
						D3DXVECTOR3 vPt = D3DXVECTOR3(static_cast<float>(x_pos/(Multiplier+0.0f)),
													GetWaveValue(pWav,x_pos,z_pos),
													static_cast<float>(z_factor*z_pos));
						D3DXVECTOR3 vN = D3DXVECTOR3(static_cast<float>(x_pos/(Multiplier+0.0f)),
													GetWaveValue(pWav,x_pos,z_pos+1),
													static_cast<float>((z_factor*z_pos)+1.0f ));
						D3DXVECTOR3 vE = D3DXVECTOR3( static_cast<float>(x_pos/(Multiplier+0.0f))+1.0f,
													GetWaveValue(pWav,x_pos+1,z_pos),
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

		Mtrl.Ambient.r = 0.4f;
		Mtrl.Ambient.g = 0.4f;
		Mtrl.Ambient.b = 0.4f;
		Mtrl.Ambient.a = 1.0f;
*/		
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
	if (!pDC)
		return;

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
	UINT	Multiplier = pDoc->GetPageMultiplier();

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	unsigned char PageSkip = pDoc->PageSkip();
	WaveCSize = pDoc->GetDocSize();

	pDC->SaveDC();
	/* Set the Pen colors */
	CPen Pen(PS_SOLID, 1, RGB(0,0,0) );
	pDC->SelectObject(&Pen);

	CPen RedPen(PS_SOLID,1, RGB(255,170,170));

	GetClientRect(&Rect);
	WaveCSize.cx = 1;
	pDC->SetMapMode(MM_ANISOTROPIC);
	SetScrollSizes(MM_TEXT, WaveCSize);

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

	/* Set the font */
	CFont font;
	font.CreateFontA(14*y_scale,
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


	// Sets the x- and y-extents of the window associated with the device context.
	pDC->SetWindowExt(x_scale*MIRAGE_PAGESIZE*Multiplier, y_scale*windowheight);
	pDC->SetWindowOrg(0, windowheight);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left, Rect.top);
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( Rect.right , Rect.bottom);

	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );
	double SRRatio=pDoc->GetRatio();

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
					pDC->SelectObject(&CPen(PS_SOLID,1, RGB(16*y_offset,16*y_offset,16*y_offset)));
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
				pDC->MoveTo(0,(Y_OFFSET+(0xff - buffer[ p ]) - y_offset)*y_scale);
			} else {
				pDC->MoveTo((x_pos-x_scale), (Y_OFFSET+(0xff- buffer[ p -1 ]) - y_offset)*y_scale);
			}
			/* Draw waveform line */
			if ( buffer[ p ] > 0 ) 
				pDC->LineTo(x_pos, (Y_OFFSET+(0xff - buffer[ p ]) - y_offset)*y_scale);
		}
	}

	/* Display the samplesize in Mirage Pages */
	sprintf_s(szString,
			sizeof(szString),
			"Size: %d (%02X) Sample Pages,Step %d, Y_OFFSET: %d, Ratio: %.3f",
			GetNumberOfPages(pWav),
			GetNumberOfPages(pWav),
			PageSkip,
			Y_OFFSET,
			pDoc->GetRatio());
	TextOut(pDC->operator HDC( ),
			0,
			-60,
			szString,
			int(strlen(szString)));

	pDC->RestoreDC(-1);
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
   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_ZENABLE,  TRUE ); // Z-Buffer (Depth Buffer)
   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // Disable Backface Culling
   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_LIGHTING, TRUE); // Enable Light
//   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_SPECULARENABLE, TRUE); // Enable specular lighting 
   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(255,255,255)); // Enable ambient light

   D3DLIGHT9	pLightTop;
   D3DLIGHT9	pLightBot;

   ZeroMemory(&pLightTop, sizeof(D3DLIGHT9));
   ZeroMemory(&pLightBot, sizeof(D3DLIGHT9));
   pLightTop.Type = D3DLIGHT_DIRECTIONAL;
   pLightBot.Type = D3DLIGHT_DIRECTIONAL;

   // Create a direction for out light - it must be normalized
   D3DXVECTOR3	vecDir;
   vecDir = D3DXVECTOR3(0.0f,-1.0f,0.0f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&pLightTop.Direction, &vecDir);

   vecDir = D3DXVECTOR3(0.0f,1.0f,0.0f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&pLightBot.Direction, &vecDir);

   pLightTop.Diffuse.r=1.0f;
   pLightTop.Diffuse.g=1.0f;
   pLightTop.Diffuse.b=1.0f;
   pLightTop.Diffuse.a=1.0f;
   pLightTop.Range = 10000.0f;

   pLightBot.Diffuse.r=1.0f;
   pLightBot.Diffuse.g=1.0f;
   pLightBot.Diffuse.b=1.0f;
   pLightBot.Diffuse.a=1.0f;
   pLightBot.Range = 10000.0f;

   pLightTop.Ambient.r=1.0f;
   pLightTop.Ambient.g=1.0f;
   pLightTop.Ambient.b=1.0f;
   pLightTop.Ambient.a=1.0f;

   pLightBot.Ambient.r=1.0f;
   pLightBot.Ambient.g=1.0f;
   pLightBot.Ambient.b=1.0f;
   pLightBot.Ambient.a=1.0f;

   pDoc->GetpD3DDevice()->SetLight(0,&pLightTop);
   pDoc->GetpD3DDevice()->LightEnable(0,TRUE);
   pDoc->GetpD3DDevice()->SetLight(1,&pLightBot);
   pDoc->GetpD3DDevice()->LightEnable(0,TRUE);

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
		pDoc->GetpD3DDevice()->Release(); // Release D3D Device

	if ( pDoc->GetpD3D() != NULL )
		pDoc->GetpD3D()->Release(); // Release D3D Interface
}

bool CMirageEditorView::CreateD3DWindow(CMirageEditorDoc* pDoc, CRect WindowRect, CDC* pDC)
{
	LPDIRECT3D9			pD3D=NULL;
	LPDIRECT3DDEVICE9	pD3DDevice=NULL;
    // First some standard Win32 window creating

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

float  CMirageEditorView::GetWaveValue(_WaveSample_ *pWav,int x, int z)
{
	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return -1;
	
	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );

	int samplepos;
	int maxsize=pWav->data_header.dataSIZE;
	
	samplepos=x+(pDoc->GetPageMultiplier()*MIRAGE_PAGESIZE*z);
	if ( samplepos < maxsize )
		return (static_cast<float>(buffer[samplepos]/2));

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

	if( pWav->sampler.Loops.dwPlayCount == 1 )
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