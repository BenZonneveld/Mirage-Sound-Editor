#include "stdafx.h"
#include "../Mirage Editor.h" // For definition of "theApp"
#include "../WaveApi/Wave Doc.h" 
#include "../WaveApi/Wave View.h"
#include "../res/Resource.h"
#include "../Dialogs/Dialog_LoopEdit.h"
#include "../Dialogs/Dialog_Resample.h"
#include "../WaveApi/Wavesamples.h"

#include "../Midi/Mirage Helpers.h"
#include "../Dsp/float_cast.h"

//#include "Globals.h"
#include "../Dialogs/Dialog_Resynthesize.h"
#include "../Dsp/sndobj_dsp.h"

//IMPLEMENT_DYNCREATE(CMirageEditorView, CScrollView)

//BEGIN_MESSAGE_MAP(CMirageEditorView, CScrollView)

//END_MESSAGE_MAP()
using namespace std;

void CMirageEditorView::OnToolsLoopwindow()
{
	CLoopDialog LoopEdit;
	CMirageEditorDoc* pDoc = GetDocument();

	theApp.m_CurrentDoc = GetDocument();
	
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
	lpFloatDataOut = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(pWav->SampleData)*sizeof(float));  //-V531
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
	progress.SetWindowTextA("Resampling Progress");
	progress.ShowWindow(SW_SHOW);

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

	progress.ShowWindow(SW_HIDE);
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

	lpFloatData = (float *)::GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizeof(pWav->SampleData)*sizeof(float));  //-V531

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
	progress.SetWindowTextA("Resampling Progress");
	progress.ShowWindow(SW_SHOW);
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

	progress.ShowWindow(SW_HIDE);
	pDoc->CheckPoint(); // Save state for undo
	pDoc->SetModifiedFlag(true);
	pDoc->NotFromMirage();
	Invalidate(FALSE);
}

// ===================================================================
//  EstimatePeriod
//
//  Returns best estimate of period.
// ===================================================================
double CMirageEditorView::EstimatePeriod(
    const unsigned char    *x,         //  Sample data.
    const int       n,          //  Number of samples.  Should be at least 2 x maxP
    const int       minP,       //  Minimum period of interest
    const int       maxP,       //  Maximum period
    double&         q )         //  Quality (1= perfectly periodic)
{
 /*   assert( minP > 1 );
    assert( maxP > minP );
    assert( n >= 2*maxP );
    assert( x != NULL );
*/     
    q = 0;
     
    //  --------------------------------
    //  Compute the normalized autocorrelation (NAC).  The normalization is such that
    //  if the signal is perfectly periodic with (integer) period p, the NAC will be
    //  exactly 1.0.  (Bonus: NAC is also exactly 1.0 for periodic signal
    //  with exponential decay or increase in magnitude).
     
    vector<double> nac(maxP+2);
     
    for ( int p =  minP-1; p <= maxP+1; p++ )
    {
        double ac = 0.0;        // Standard auto-correlation
        double sumSqBeg = 0.0;  // Sum of squares of beginning part
        double sumSqEnd = 0.0;  // Sum of squares of ending part
         
        for ( int i = 0; i < n-p; i++ )
        {
            ac += (double)x[i]*(double)x[i+p];
            sumSqBeg += (double)x[i]*(double)x[i];
            sumSqEnd += (double)x[i+p]*(double)x[i+p];
        }
        nac[p] = ac / sqrt( sumSqBeg * sumSqEnd );
    }
     
    //  ---------------------------------------
    //  Find the highest peak in the range of interest.
     
    //  Get the highest value
    int bestP = minP;
    for ( int p = minP; p <= maxP; p++ )
        if ( nac[p] > nac[bestP] )
            bestP = p;
     
    //  Give up if it's highest value, but not actually a peak.
    //  This can happen if the period is outside the range [minP, maxP]
    if ( nac[bestP] < nac[bestP-1]
      && nac[bestP] < nac[bestP+1] )
    {
        return 0.0;
    }
     
    //  "Quality" of periodicity is the normalized autocorrelation
    //  at the best period (which may be a multiple of the actual
    //  period).
    q = nac[bestP];
     
 
    //  --------------------------------------
    //  Interpolate based on neighboring values
    //  E.g. if value to right is bigger than value to the left,
    //  real peak is a bit to the right of discretized peak.
    //  if left  == right, real peak = mid;
    //  if left  == mid,   real peak = mid-0.5
    //  if right == mid,   real peak = mid+0.5
     
    double mid   = nac[bestP];
    double left  = nac[bestP-1];
    double right = nac[bestP+1];
     
  //  assert( 2*mid - left - right > 0.0 );
 
    double shift = 0.5*(right-left) / ( 2*mid - left - right );
         
    double pEst = bestP + shift;
     
    //  -----------------------------------------------
    //  If the range of pitches being searched is greater
    //  than one octave, the basic algo above may make "octave"
    //  errors, in which the period identified is actually some
    //  integer multiple of the real period.  (Makes sense, as
    //  a signal that's periodic with period p is technically
    //  also period with period 2p).
    //
    //  Algorithm is pretty simple: we hypothesize that the real
    //  period is some "submultiple" of the "bestP" above.  To
    //  check it, we see whether the NAC is strong at each of the
    //  hypothetical subpeak positions.  E.g. if we think the real
    //  period is at 1/3 our initial estimate, we check whether the
    //  NAC is strong at 1/3 and 2/3 of the original period estimate.
     
    const double k_subMulThreshold = 0.90;  //  If strength at all submultiple of peak pos are
                                            //  this strong relative to the peak, assume the
                                            //  submultiple is the real period.
                                             
    //  For each possible multiple error (starting with the biggest)
    int maxMul = bestP / minP;
    bool found = false;
    for ( int mul = maxMul; !found && mul >= 1; mul-- )
    {
        //  Check whether all "submultiples" of original
        //  peak are nearly as strong.
        bool subsAllStrong = true;
         
        //  For each submultiple
        for ( int k = 1; k < mul; k++ )
        {
            int subMulP = int(k*pEst/mul+0.5);
            //  If it's not strong relative to the peak NAC, then
            //  not all submultiples are strong, so we haven't found
            //  the correct submultiple.
            if ( nac[subMulP] < k_subMulThreshold * nac[bestP] )
                subsAllStrong = false;
                 
            //  TODO: Use spline interpolation to get better estimates of nac
            //  magnitudes for non-integer periods in the above comparison
        }
 
        //  If yes, then we're done.   New estimate of
        //  period is "submultiple" of original period.
        if ( subsAllStrong == true )
        {
            found = true;
            pEst = pEst / mul;
        }
    }

		nac.clear();

    return pEst;
}

void CMirageEditorView::DetectPitch(bool DoResample)
{
	DWORD SelectStart;
	DWORD SelectEnd;
	DWORD optimal_rate;
	DWORD rate;
	double ratio_old;
	double ratio_fix;
	int fix=128;
  const double minF = 27.5;       //  Lowest pitch of interest (27.5 = A0, lowest note on piano.)
  const double maxF = 4186.0;     //  Highest pitch of interest(4186 = C8, highest note on piano.)
     
	double q;

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

	rate = SelectionWav->waveFormat.fmtFORMAT.nSamplesPerSec;

	// Start the autocorrelation
  const int minP = int(rate/maxF-1);    //  Minimum period
  const int maxP = int(rate/minF+1);    //  Maximum period

	double pEst = EstimatePeriod( SelectionWav->SampleData, // Sampledata
																SelectEnd - SelectStart, // Number of samples
																minP, // Minimum period of interest
																maxP, // Maximum period of interest
																q ); //Quality (1= perfectly periodic)
     
    //  Compute the fundamental frequency (reciprocal of period)
  double fEst = 0;
  if ( pEst > 0 )
		fEst = rate/pEst;

	pDoc->SetEstQ(q);
	pDoc->SetFreqEst(fEst);

	free(SelectionWav);
	
	if ( DoResample == true )
	{
		optimal_rate = fix*fEst;
		while ( optimal_rate > (1.9*rate) )
		{
			fix=fix/2;
			optimal_rate=fix*fEst;
		}

		ratio_fix = (double)optimal_rate / (double)rate;

		ratio_old=pDoc->GetRatio();
		pDoc->SetRatio(ratio_fix);
		Resample();
		pDoc->SetRatio(ratio_old);
	}
	pDoc->SetPitch(fEst);
	SetCursor(LoadCursor(NULL,IDC_ARROW));
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
	ResynthOpt.m_fftsize = pDoc->GetResynthFFT_Size();
	ResynthOpt.m_hopsize = pDoc->GetResynthHopSize();
	ResynthOpt.m_iterations = pDoc->GetResynthIterations();
	ResynthOpt.m_convolve = pDoc->GetResynthConvolution();

	ResynthOpt.DoModal();

	if ( ResynthOpt.m_resynth_ok == true )
	{

		pDoc->SetResynthFFT_Size(ResynthOpt.m_fftsize);
		pDoc->SetResynthHopSize(ResynthOpt.m_hopsize);
		pDoc->SetResynthIterations(ResynthOpt.m_iterations);
		pDoc->SetResynthConvolution(ResynthOpt.m_convolve);

//		for(int iterate=0; iterate < ResynthOpt.m_iterations; iterate++)
//		{
		Reverse(pWav->SampleData, samplesize);
		resynthesize(pDoc->GetPathName(),
									pWav->SampleData,
									samplesize,
									(float)pWav->waveFormat.fmtFORMAT.nSamplesPerSec,
									8,
									ResynthOpt.m_fftsize,
									ResynthOpt.m_hopsize,
									ResynthOpt.m_convolve);
		Reverse(pWav->SampleData,samplesize);
			//resynthesize(pDoc->GetPathName(),
			//						pWav->SampleData,
			//						samplesize,
			//						(float)pWav->waveFormat.fmtFORMAT.nSamplesPerSec,
			//						8,
			//						ResynthOpt.m_fftsize,
			//						ResynthOpt.m_hopsize,
			//						ResynthOpt.m_convolve);
			//Reverse(pWav->SampleData,samplesize);
//		}

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
	DetectPitch(false);
}

void CMirageEditorView::OnToolsAllignToPages()
{
	DetectPitch(true);
	GetDocument()->ReleaseMesh();
	Invalidate(FALSE);
}
void CMirageEditorView::Reverse(char unsigned * wavedata,int samplesize)
{
	int counter = 0;
	char unsigned samplevalue;

	while ( (samplesize - counter) >= counter )
	{
		samplevalue = wavedata[(samplesize - counter)];
		wavedata[(samplesize - counter)] = wavedata[counter];
		wavedata[counter] = samplevalue;
		counter++;
	}
}