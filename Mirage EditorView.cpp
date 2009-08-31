// Mirage EditorView.cpp : implementation of the CMirageEditorView class
//

#include "stdafx.h"
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
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

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
			break;
		case 'A':
			Mode_3dTypeB(pDC);
			break;
		case 'B':
			Mode_3dTypeB(pDC);
			break;
		default:
			Mode_Wavedraw(pDC);
	}
}

void CMirageEditorView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: remove this code when final selection model code is written
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
		} else {
			if ( fFlags & MK_SHIFT )
			{
				pDoc->ZoomIncTen();
			} else {
				pDoc->ZoomInc();
			}
		}
	}
	Invalidate();
	UpdateWindow();

	return CView::OnMouseWheel(fFlags, zDelta, point);
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

	if ( nFlags != MK_LBUTTON )
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

	if ( Xpos == dwStart || Xpos == dwStart - MIRAGE_PAGESIZE )
	{
		SetCursor(LoadCursor(NULL,IDC_SIZEWE));
		pDoc->m_startpoint_selected = true;
	} else
		if ( (point.x <= long(dwEnd + MIRAGE_PAGESIZE) ) && (point.x >= long(dwEnd - MIRAGE_PAGESIZE)) && point.x < long(pWav->data_header.dataSIZE) )
	{
		SetCursor(LoadCursor(NULL,IDC_SIZEWE));
		pDoc->m_endpoint_selected = true;
	} else {
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	}
	if ( Xpos <= dwStart + 256 )
		pDoc->m_startpoint_selected = true;

	if ( !nFlags )
		return;

	/* Set the Loop Startpoint */
	if ( nFlags == MK_LBUTTON &&
		Xpos < (dwEnd - MIRAGE_PAGESIZE) &&
		Xpos < pWav->data_header.dataSIZE &&
		pDoc->m_startpoint_selected == true )
	{
		/* Check if the startpoint is before the end of the loop */
		if ( Xpos < dwEnd  || pWav->sampler.Loops.dwPlayCount == 1)
		{
			pWav->sampler.Loops.dwStart = Xpos;
			if (pWav->sampler.Loops.dwPlayCount == 1 )
			{
				pWav->sampler.Loops.dwPlayCount = 0;
				pWav->sampler.cSampleLoops = 1;
				if ( pWav->sampler.Loops.dwEnd < pWav->sampler.Loops.dwStart )
					pWav->sampler.Loops.dwEnd = (pWav->data_header.dataSIZE - 16);
			}
		}
	}

	/* Set the Loop Endpoint */
	if ( nFlags == MK_LBUTTON &&
		point.x > long( dwStart + MIRAGE_PAGESIZE ) &&
		point.x < long(pWav->data_header.dataSIZE) &&
		pDoc->m_endpoint_selected == true )
	{
		if ( point.x > long(dwStart + MIRAGE_PAGESIZE) || pWav->sampler.Loops.dwPlayCount == 1)
		{
			pWav->sampler.Loops.dwEnd = point.x;
			if (pWav->sampler.Loops.dwPlayCount == 1 )
			{
				pWav->sampler.Loops.dwPlayCount = 0;
				pWav->sampler.cSampleLoops = 1;
			}
		}
	}

	CRect LoopRect;
	CRect OldLoop;

	OldLoop.left = dwStart;
	OldLoop.top = -140;
	OldLoop.right = dwEnd;
	OldLoop.bottom = 159;

	LoopRect.left = pWav->sampler.Loops.dwStart;
	LoopRect.top = -140;
	LoopRect.right = pWav->sampler.Loops.dwEnd;
	LoopRect.bottom = 159;

	if ( dwStart != LoopRect.left || dwEnd != LoopRect.right )
	{
		int Rop2 = pDC->GetROP2();
		pDC->SetROP2(R2_XORPEN);
		pDC->Rectangle(OldLoop);
		pDC->Rectangle(LoopRect);
		pDC->SetROP2(Rop2);
	}
}

void CMirageEditorView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();
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
			if ( (p % 256) == 0 )
			{
				if ( p > 0 )
					MiragePages++;
				pDC->SelectObject(&GreenPen);
				pDC->MoveTo(p,-160);
				pDC->LineTo(p,-140);
				pDC->SelectObject(&Pen);
				pDC->SetBkMode(TRANSPARENT);
				sprintf_s(szString,sizeof(szString),"%02X",MiragePages);
				pDC->TextOut(p,-156,szString,int(strlen(szString)));
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
	pDC->SelectObject(&Pen);
	LoopRect.left = pWav->sampler.Loops.dwStart+1;
	LoopRect.top = -140;
	LoopRect.right = pWav->sampler.Loops.dwEnd;
	LoopRect.bottom = 159;
	int Rop2 = pDC->GetROP2();
	pDC->SetROP2(R2_XORPEN);
	pDC->Rectangle(LoopRect);
	pDC->SetROP2(Rop2);

	pDC->RestoreDC(-1);
}

void CMirageEditorView::Mode_3dTypeA(CDC* pDC)
{
	if (!pDC)
		return;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	/* SetWorldTransform */
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
	int y_increment = 2;
	int x_pos = 0;
	long x_scale;
	long y_scale;

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

	int windowheight = 256+(y_increment*GetNumberOfPages(pWav));
	int Y_OFFSET=windowheight-256;
//	Y_OFFSET=Y_OFFSET*GetNumberOfPages(pWav);
	x_scale=(1000*(Rect.right/MIRAGE_PAGESIZE));
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
	pDC->SetWindowExt(x_scale*MIRAGE_PAGESIZE, y_scale*windowheight);
	pDC->SetWindowOrg(0, windowheight);

	// Sets the viewport origin of the device context
	pDC->SetViewportOrg( Rect.left, Rect.top);
	// Sets the x- and y-extents of the viewport of the device context.
	pDC->SetViewportExt( Rect.right , Rect.bottom);

	if (pDoc->DisplayType() == 'A' )
	{
		pDC->SetGraphicsMode(GM_ADVANCED);
		XFORM Xform;
		//float angle=0.0; //(-0.523598776/4);
		float angle= -(30/(180/3.14));

		Xform.eM11=(FLOAT) cos(angle); // (30 degrees)
		Xform.eM12=(FLOAT) -(sin(angle));
		Xform.eM21=(FLOAT) sin(angle)+pDoc->ZoomLevel();
		Xform.eM22=(FLOAT) cos(angle);
		Xform.eDx=(FLOAT) 50.0;
		Xform.eDy=(FLOAT) 0.0;
		pDC->SetWorldTransform(&Xform);
	}
	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );
	double SRRatio=pDoc->GetRatio();

	for( DWORD p = 0; p < pWav->data_header.dataSIZE + EXTEND ; p++ ) 
	{
		if ( p < pWav->data_header.dataSIZE )
		{
			if ( (p % MIRAGE_PAGESIZE) == 0 )
			{
				if ( p > 0 )
				{
					MiragePages++;
					y_offset = y_offset + y_increment;
					x_pos = 0;
				}
			} else {
				x_pos=x_pos+x_scale;
			}
			if ( (p/MIRAGE_PAGESIZE) % PageSkip != 0 && (GetNumberOfPages(pWav) -1) != (p/MIRAGE_PAGESIZE) )
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

	if (pDoc->DisplayType() == 'A' )
	{
		pDC->SetGraphicsMode(GM_ADVANCED);
		XFORM Xform;
		float angle=(0);

		Xform.eM11=(FLOAT) cos(angle); // (30 degrees)
		Xform.eM12=(FLOAT) -sin(angle);
		Xform.eM21=(FLOAT) sin(angle);
		Xform.eM22=(FLOAT) cos(angle);
		Xform.eDx=(FLOAT) 0.0;
		Xform.eDy=(FLOAT) 0.0;
		pDC->SetWorldTransform(&Xform);
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