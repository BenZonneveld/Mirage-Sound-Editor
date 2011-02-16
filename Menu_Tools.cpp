#include "stdafx.h"
#include "Mirage Editor.h" // For definition of "theApp"
#include "Mirage EditorDoc.h" 
#include "Mirage EditorView.h"
#include "Resource.h"
#include "Dialog_LoopEdit.h"
#include "Dialog_Resample.h"
#include "Wavesamples.h"
#ifndef NOFFTW3
#include "Fourier.h"
#endif
#include "Mirage Helpers.h"
#include "float_cast.h"

//#include "Globals.h"
#include "Dialog_Resynthesize.h"
#include "sndobj_dsp.h"

//IMPLEMENT_DYNCREATE(CMirageEditorView, CScrollView)

//BEGIN_MESSAGE_MAP(CMirageEditorView, CScrollView)

//END_MESSAGE_MAP()

void CMirageEditorView::OnToolsLoopwindow()
{
	CLoopDialog LoopEdit;
	CMirageEditorDoc* pDoc = GetDocument();

	theApp.m_CurrentDoc = GetDocument();
	pDoc = theApp.m_CurrentDoc;
	
	LoopEdit.DoModal();
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	Invalidate(FALSE);
}

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
	Invalidate(FALSE);
}

void CMirageEditorView::OnToolsResample()
{
	_WaveSample_ *pWav;
	DWORD	samplesize;
	DWORD	LoopSize;
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

	samplesize = MIRAGE_PAGESIZE + (ResampleDlg.m_targetpages * MIRAGE_PAGESIZE);
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
	pWav->riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_) + sizeof(_instrument_) + src_data.output_frames;
	pWav->sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)pWav->waveFormat.fmtFORMAT.nSamplesPerSec);

	// Fix the loop
	LoopSize = pWav->sampler.Loops.dwEnd-pWav->sampler.Loops.dwStart;
	pWav->sampler.Loops.dwStart = lrint(pWav->sampler.Loops.dwStart * srcRatio)& 0xFF00;
//	pWav->sampler.Loops.dwEnd = lrint(pWav->sampler.Loops.dwEnd * srcRatio);
	pWav->sampler.Loops.dwEnd = pWav->sampler.Loops.dwStart + lrint(LoopSize * srcRatio);

//	RemoveZeroSamples(pWav);

	progress.DestroyWindow();
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	pDoc->NotFromMirage();
	Invalidate(FALSE);
}

void CMirageEditorView::OnToolsNormalize()
{
	_WaveSample_ *pWav;
	float		*lpFloatData;
	double		gain = 1.0;
	double		max = 0.0 ;
  int     counter;

	CMirageEditorDoc* pDoc = GetDocument();

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

  for ( counter = 0 ; counter < (int)pWav->data_header.dataSIZE; counter++)
  {
    if (pDoc->FromMirage() == true && counter >= (int)(pWav->data_header.dataSIZE - 16))
    {
      break;
    }
    if ( pWav->SampleData[counter] == 0 || pWav->SampleData[counter] == 255 )
      return;
  }

	lpFloatData = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(pWav->SampleData)*sizeof(float)); 

Normalize:
	src_unchar_to_float_array(pWav->SampleData, lpFloatData, (int)pWav->data_header.dataSIZE);
	max = 0.0;
	max = apply_gain(lpFloatData, (long)pWav->data_header.dataSIZE, 1, max, gain);
	// Redo normalize if the gain is too large
	if ( gain == 1.0 )
	{
		if (max > 1.0)
		{	
			gain = 1.0 / max;
			goto Normalize;
		}
		if ( max < 0.99 )
		{
			gain = 1.0 / max;
			goto Normalize;
		}
	}

	src_float_to_unchar_array (lpFloatData, (unsigned char *)&pWav->SampleData, (int)pWav->data_header.dataSIZE);
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	Invalidate(FALSE);
}

void CMirageEditorView::Resample()
{
	_WaveSample_ *pWav;
	DWORD	counter = 0;
	DWORD LoopSize = 0;

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
	pWav->riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_) + sizeof(_instrument_) + src_data.output_frames;
	pWav->sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)pWav->waveFormat.fmtFORMAT.nSamplesPerSec);

	// Fix the loop
	LoopSize = pWav->sampler.Loops.dwEnd-pWav->sampler.Loops.dwStart;
	pWav->sampler.Loops.dwStart = lrint(pWav->sampler.Loops.dwStart * pDoc->GetRatio())& 0xFF00;
//	pWav->sampler.Loops.dwEnd = lrint(pWav->sampler.Loops.dwEnd * srcRatio);
	pWav->sampler.Loops.dwEnd = pWav->sampler.Loops.dwStart + lrint(LoopSize * pDoc->GetRatio());

	RemoveZeroSamples(pWav);

	progress.DestroyWindow();
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	pDoc->NotFromMirage();
	Invalidate(FALSE);
}

void CMirageEditorView::DetectPitchAndResample(bool DoResample)
{
#ifndef NOFFTW3
	CFourier fftw;
	double pitch;
	DWORD optimal_rate;
	DWORD rate;
	double ratio_old;
	double ratio_fix;
	int fix=128;
	DWORD SelectStart;
	DWORD SelectEnd;

	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	SetCursor(LoadCursor(NULL,IDC_WAIT));

	_WaveSample_ *pWav;
	_WaveSample_ *SelectionWav;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	SelectStart=pDoc->SelectionStart;
	SelectEnd=pDoc->SelectionEnd;

	if ( SelectStart < 0 || SelectEnd > pWav->data_header.dataSIZE )
	{
		SelectStart=pWav->sampler.Loops.dwStart;
		SelectEnd=pWav->sampler.Loops.dwEnd;

		if ( (SelectStart > SelectEnd ) ||
			SelectEnd-SelectStart < 0xFF )
			return;
	}

	SelectionWav=(_WaveSample_ *)malloc(sizeof(_WaveSample_));

	memcpy(SelectionWav,pWav,sizeof(_WaveSample_));

	memcpy(SelectionWav->SampleData,
					pWav->SampleData + SelectStart,
					SelectEnd - SelectStart);

	if ( DoResample == true )
		ResizeRiff(SelectionWav,SelectEnd - SelectStart);

	rate = SelectionWav->waveFormat.fmtFORMAT.nSamplesPerSec;
	pitch = fftw.DetectPitch(SelectionWav);
	free(SelectionWav);
	
	if ( DoResample == true )
	{
		optimal_rate = fix*pitch;
		while ( optimal_rate > (1.9*rate) )
		{
			fix=fix/2;
			optimal_rate=fix*pitch;
		}

		ratio_fix = (double)optimal_rate / (double)rate;

		ratio_old=pDoc->GetRatio();
		pDoc->SetRatio(ratio_fix);
		Resample();
		pDoc->SetRatio(ratio_old);
	}
	pDoc->SetPitch(pitch);
	SetCursor(LoadCursor(NULL,IDC_ARROW));
#endif
}

void CMirageEditorView::OnToolsResynthesize()
{
	CMirageEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CResynthesize ResynthOpt;

//	double **sound;
	float * sound;
	double val;

	// Wavefile
	_WaveSample_ *pWav;
	_WaveSample_ *SelectionWav;

	MWAV hWAV = pDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;

	signed long int samplerate = pWav->waveFormat.fmtFORMAT.nSamplesPerSec;
	signed long int samplesize = pWav->data_header.dataSIZE;

	/* Setting the defaults for the analysis */
	ResynthOpt.m_maxfreq_range = samplerate;
	ResynthOpt.m_maxfreq = samplerate/2;

	ResynthOpt.m_BandsPerOctave = pDoc->GetResynthBPO();
	ResynthOpt.m_PixPerSec = pDoc->GetResynthPixPerSecond();
	ResynthOpt.m_synth_mode = pDoc->GetResynthMode();
	ResynthOpt.m_convolution_mode = pDoc->GetResynthConvolution();

	ResynthOpt.DoModal();

	if ( ResynthOpt.m_resynth_ok == true )
	{

		pDoc->SetResynthBPO((double)ResynthOpt.m_BandsPerOctave);
		pDoc->SetResynthPixPerSecond((double)ResynthOpt.m_PixPerSec);
		pDoc->SetResynthMode(ResynthOpt.m_synth_mode);
		pDoc->SetResynthConvolution(ResynthOpt.m_convolution_mode);

/*		sound = (float *)malloc (samplesize *sizeof(float)); // allocate sound

		for(int i=0; i<samplesize; i++)
		{
			sound[i]=(float)pWav->SampleData[i]/128.0 - 1.0;
		}
*/

		resynthesize(pDoc->GetPathName(),pWav->SampleData,samplesize,(float)pWav->waveFormat.fmtFORMAT.nSamplesPerSec,8);

		::GlobalUnlock((HGLOBAL) hWAV);
		pDoc->CheckPoint(); // Save state for undo
		pDoc->SetModifiedFlag(true);
		Invalidate(FALSE);
	} else {
		::GlobalUnlock((HGLOBAL) hWAV);
	}
}

void CMirageEditorView::OnToolsDetectpitch()
{
	DetectPitchAndResample(false);
}

void CMirageEditorView::OnToolsAllignToPages()
{
	DetectPitchAndResample(true);
	GetDocument()->ReleaseMesh();
	Invalidate(FALSE);
}