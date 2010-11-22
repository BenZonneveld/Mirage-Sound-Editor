#include "stdafx.h"
#include "globals.h"
#include "Mirage Editor.h"
#include "Mirage EditorDoc.h" 
#include "Mirage EditorView.h"
#include "Wavesamples.h"
#include "CPasteMulti.h"

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
	pDoc->ReleaseMesh();

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

	memcpy(buffer,pWav, headersize/* + sizeof(pWav->data_header)*/);
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
		void * MoveFrom = 0;
		void * MoveTo = 0;
		DWORD MoveSize = 0;
		DWORD NewSize = 0;
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

/*		if ( clip->waveFormat.fmtFORMAT.nSamplesPerSec != pWav->waveFormat.fmtFORMAT.nAvgBytesPerSec )
		{
			// TODO: Maybe resample the clipboard wave to the destination rate
			::GlobalUnlock((HGLOBAL) hWAV);
			return;
		}
*/
		if ( (pWav->data_header.dataSIZE + clip->data_header.dataSIZE) > MAX_WAVESIZE )
		{
			::GlobalUnlock((HGLOBAL) hWAV);
			return;
		}

		if ( (GetDocument()->SelectionEnd -
					GetDocument()->SelectionStart) 
					!= clip->data_header.dataSIZE )
		{
			MoveTo = pWav->SampleData +
								GetDocument()->SelectionStart +
								clip->data_header.dataSIZE;
			MoveFrom = pWav->SampleData +
								GetDocument()->SelectionEnd;

			MoveSize = pWav->data_header.dataSIZE -
								GetDocument()->SelectionEnd; // - 

			NewSize = pWav->data_header.dataSIZE -
								(GetDocument()->SelectionEnd - GetDocument()->SelectionStart) +
								clip->data_header.dataSIZE;

			// Selection is larger then clipboard size
			if ( (GetDocument()->SelectionEnd -
						GetDocument()->SelectionStart) > clip->data_header.dataSIZE )
			{
				// Move remaining data to the front
			}

			// Selection is less than clipboard size
			if ( (GetDocument()->SelectionEnd -
						GetDocument()->SelectionStart) < clip->data_header.dataSIZE )
			{
				// Move remaining data to the back
			}

			// Move the data so the paste replaces the selection
			memcpy(MoveTo, // Move to selection start
						MoveFrom, 
						MoveSize);

			pWav->data_header.dataSIZE = NewSize;

			pWav->riff_header.riffSIZE = NewSize;
		}

		memcpy((pWav->SampleData + GetDocument()->SelectionStart),
				clip->SampleData,
				clip->data_header.dataSIZE);

//		pWav->data_header.dataSIZE = pWav->data_header.dataSIZE + clip->data_header.dataSIZE - (GetDocument()->SelectionEnd - GetDocument()->SelectionStart);
//		pWav->riff_header.riffSIZE = pWav->riff_header.riffSIZE + clip->data_header.dataSIZE - (GetDocument()->SelectionEnd - GetDocument()->SelectionStart);

		::GlobalUnlock((HGLOBAL) hWAV);
		GetDocument()->CheckPoint(); // Save state for undo
		Invalidate(FALSE);
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
		Invalidate(FALSE);
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
		Invalidate(FALSE);
	}
}

void CMirageEditorView::OnPastespecialMultiplecopies()
{
	COleDataObject  dataObj;

	dataObj.AttachClipboard();

	if ( dataObj.IsDataAvailable(CF_RIFF))
	{
		_WaveSample_ *pWav;
		_WaveSample_ *clip;

		int c = 0;

		HGLOBAL clipbuffer;

		CPasteMulti	MultiRepeatDlg;

		MultiRepeatDlg.DoModal();

		if ( theApp.RepeatCount == 0 )
		{
			return;
		}

		//TODO: Repeat the selection based on the count
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

		for(c = 0 ; c < theApp.RepeatCount ; c++ )
		{
			memcpy((pWav->SampleData + (c * clip->data_header.dataSIZE)),
						clip->SampleData,
						clip->data_header.dataSIZE);
		}

		pWav->data_header.dataSIZE = pWav->data_header.dataSIZE - 
																	clip->data_header.dataSIZE + 
																	((theApp.RepeatCount ) * clip->data_header.dataSIZE);
		pWav->riff_header.riffSIZE = pWav->riff_header.riffSIZE -
																	clip->data_header.dataSIZE +
																	((theApp.RepeatCount ) * clip->data_header.dataSIZE);

		::GlobalUnlock((HGLOBAL) hWAV);
		GetDocument()->CheckPoint(); // Save state for undo
		Invalidate(FALSE);
	}

	Invalidate(FALSE);
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

void CMirageEditorView::OnUpdateTruncateAfterloop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMirageEditorView::OnUpdateTruncateBeforeloop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMirageEditorView::OnUpdateTruncateKeepLoop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMirageEditorView::OnTruncateAfterloop()
{
	_WaveSample_ *pWav;
	DWORD LoopEnd;

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

	if ( (pWav->sampler.Loops.dwEnd > (pWav->sampler.Loops.dwEnd & 0xFF00) ) && (pWav->sampler.Loops.dwEnd & 0xFF00) < 0xFF00 )
	{
		LoopEnd = (pWav->sampler.Loops.dwEnd + 0x0100) & 0xFF00;
	} else {
		LoopEnd = pWav->sampler.Loops.dwEnd & 0xFF00;
	}

	if ( LoopEnd >= pWav->data_header.dataSIZE )
		return;

	ResizeRiff(pWav, LoopEnd);

	::GlobalUnlock((HGLOBAL) hWAV);

	Invalidate(FALSE);
	UpdateWindow();
}

void CMirageEditorView::OnTruncateBeforeloop()
{
	DWORD	LoopEnd;
	DWORD	LoopStart;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	_WaveSample_ *pWav;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;

 	LoopStart = pWav->sampler.Loops.dwStart;
  LoopEnd = pWav->sampler.Loops.dwEnd;

	memcpy( pWav->SampleData,
			    pWav->SampleData + LoopStart,
			    (LoopEnd-LoopStart));

 	ResizeRiff(pWav, LoopEnd - LoopStart);

	pWav->sampler.Loops.dwStart = 0;
	pWav->sampler.Loops.dwEnd = pWav->sampler.Loops.dwEnd - LoopStart;

	/* Clear the selection */
	pDoc->SelectionStart = -1;
	pDoc->SelectionEnd = -1;

	::GlobalUnlock((HGLOBAL) hWAV);
	pDoc->CheckPoint(); // Save state for undo
	Invalidate(FALSE);
}

void CMirageEditorView::OnTruncateOnlykeeploop()
{
	DWORD	LoopEnd;
	DWORD	LoopStart;
  DWORD RemovedCount;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	_WaveSample_ *pWav;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;

 	LoopStart = pWav->sampler.Loops.dwStart;

 	if ( (pWav->sampler.Loops.dwEnd > (pWav->sampler.Loops.dwEnd & 0xFF00) ) && (pWav->sampler.Loops.dwEnd & 0xFF00) < 0xFF00 )
	{
		LoopEnd = ((pWav->sampler.Loops.dwEnd - LoopStart)+ 0x0100) & 0xFF00;
	} else {
		LoopEnd = (pWav->sampler.Loops.dwEnd - LoopStart) & 0xFF00;
	}

	memcpy( pWav->SampleData,
			    pWav->SampleData + LoopStart,
			    (LoopEnd));

 	ResizeRiff(pWav, LoopEnd);

	pWav->sampler.Loops.dwStart = 0;
	pWav->sampler.Loops.dwEnd = pWav->sampler.Loops.dwEnd - LoopStart;

	/* Clear the selection */
	pDoc->SelectionStart = -1;
	pDoc->SelectionEnd = -1;

	::GlobalUnlock((HGLOBAL) hWAV);
	pDoc->CheckPoint(); // Save state for undo
	Invalidate(FALSE);
}

void  CMirageEditorView::KeepOnlySelection()
{
	DWORD	LoopEnd;
	DWORD	LoopStart;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	_WaveSample_ *pWav;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;

  if ( pDoc->SelectionStart < 0 || pDoc->SelectionEnd > pWav->data_header.dataSIZE )
  {
  	::GlobalUnlock((HGLOBAL) hWAV);
    return;
  }

	memcpy( pWav->SampleData,
			    pWav->SampleData + pDoc->SelectionStart,
			    pDoc->SelectionEnd - pDoc->SelectionStart);

  ResizeRiff(pWav,pDoc->SelectionEnd - pDoc->SelectionStart);

	LoopStart = pWav->sampler.Loops.dwStart;
	LoopEnd	= pWav->sampler.Loops.dwEnd;

	/* Fix the LoopPoints */
  LoopStart = pWav->sampler.Loops.dwStart - pDoc->SelectionStart;
	LoopEnd = pWav->sampler.Loops.dwEnd - pDoc->SelectionStart;

	pWav->sampler.Loops.dwStart = LoopStart;
	pWav->sampler.Loops.dwEnd = LoopEnd;

	/* Clear the selection */
	pDoc->SelectionStart = -1;
	pDoc->SelectionEnd = -1;

	::GlobalUnlock((HGLOBAL) hWAV);
	pDoc->CheckPoint(); // Save state for undo
	Invalidate(FALSE);
  }

