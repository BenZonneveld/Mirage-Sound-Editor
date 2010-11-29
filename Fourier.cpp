#include "StdAfx.h"
#define _USE_MATH_DEFINES 
#include <cmath>
#include "globals.h"
#include "wavesamples.h"
#include "samplerate.h"
#include "Fourier.h"

CFourier::CFourier(void)
{
}

CFourier::~CFourier(void)
{
}

double CFourier::DetectPitch(struct _WaveSample_ *pWav)
{
	char NewValue;
	int v=0;
	int m;
	double cc;
	double absval;
	int Harmonic;
	double LoudestHarmonic;
	double *fftw_i = NULL;
	double *fftw_r = NULL;
	fftw_plan plan = NULL;

	int n = pWav->data_header.dataSIZE;
	int samplerate = pWav->waveFormat.fmtFORMAT.nSamplesPerSec;
	double bandwidth = (double)samplerate / 2.0;
	double correction = (double)samplerate / (double)n;

	fftw_i = (double *) fftw_malloc(sizeof(double) * n);
	fftw_r = (double *) fftw_malloc(sizeof(double) * n);
	fftw_set_timelimit(10.0);
	plan = fftw_plan_r2r_1d(n, fftw_r, fftw_i, FFTW_R2HC , FFTW_MEASURE);

	// We have no imaginary data, so clear m_fftw_i
	memset((void *)fftw_i,0,n* sizeof(float));

	// Fill rdata with actual data
	for (v=0;v<n;v++)
	{
		fftw_r[v] = pWav->SampleData[v];
	}

	fftw_execute(plan);

	// post-process FFT data: make absolute values, and calculate
	// real frequency of each power line in the spectrum
	m = 1;
	Harmonic=1;
	LoudestHarmonic=0.0;

	for (int i=1; i < (n-2);i++)
	{
		absval = sqrt(fftw_i[i] * fftw_i[i]);
		cc = (double)m*correction;
		if ( cc > bandwidth )
			break;
		if ( absval > (sqrt(fftw_i[Harmonic] * fftw_i[Harmonic])) )
		{
			Harmonic=i;
			LoudestHarmonic=cc;
		}
		m++;
	}

	fftw_destroy_plan(plan);
	fftw_free(fftw_i);
	fftw_free(fftw_r);
	
	return LoudestHarmonic;
}

double CFourier::DetectTopHarmonic(struct _WaveSample_ *pWav)
{
	char NewValue;
	int v=0;
	int m;
	double cc;
	double absval;
	int Harmonic;
	double TopHarmonic;
	double *fftw_i = NULL;
	double *fftw_r = NULL;
	fftw_plan plan = NULL;

	int n = pWav->data_header.dataSIZE;
	int samplerate = pWav->waveFormat.fmtFORMAT.nSamplesPerSec;
	double bandwidth = (double)samplerate / 2.0;
	double correction = (double)samplerate / (double)n;

	fftw_i = (double *) fftw_malloc(sizeof(double) * n);
	fftw_r = (double *) fftw_malloc(sizeof(double) * n);
	fftw_set_timelimit(10.0);
	plan = fftw_plan_r2r_1d(n, fftw_r, fftw_i, FFTW_R2HC , FFTW_MEASURE);

	// We have no imaginary data, so clear m_fftw_i
	memset((void *)fftw_i,0,n* sizeof(float));

	// Fill rdata with actual data
	for (v=0;v<n;v++)
	{
		fftw_r[v] = pWav->SampleData[v];
	}

	fftw_execute(plan);

	// post-process FFT data: make absolute values, and calculate
	// real frequency of each power line in the spectrum
	m = 1;
	Harmonic=1;
	TopHarmonic=0.0;

	for (int i=1; i < (n-2);i++)
	{
		absval = sqrt(fftw_i[i] * fftw_i[i]);
		cc = (double)m*correction;
		if ( cc > bandwidth )
			break;
		if ( absval > 10 )
		{
			Harmonic=i;
			TopHarmonic=cc;
		}
		m++;
	}

	fftw_destroy_plan(plan);
	fftw_free(fftw_i);
	fftw_free(fftw_r);
	
	return TopHarmonic;
}

/* MDCT and IMDCT use:
	1)	init (declare first "extern void* mdctf_init(int)"
			void * m_plan = mdctf_init(N);
	2)	run mdct/imdct as many times as you wish
			mdctf(freq,time,m_plan);
	3)	free
			mdctf_free(m_plan);
*/

//BOOL CFourier::mdctf_init(int N)
//{
//	double alpha, omiga, scale;
//	int n;
//
//	if ( 0x00 != ( N & 0x03 ))
//	{
//		return FALSE;
//	}
//
//	m_plan = (mdctf_plan*)malloc(sizeof(mdctf_plan));
//
//	m_plan->N = N;
//
//	m_plan->twiddle = (float*)malloc(sizeof(float) * N >> 1 );
//	alpha = 2.f * M_PI / (8.f * N);
//	omiga = 2.f * M_PI / N;
//	scale = sqrt(sqrt(2.f / N ));
//	for(n = 0 ; n < (N >> 2 ) ; n++)
//	{
//		m_plan->twiddle[2*n+0] = (float) (scale * cos(omiga * n + alpha ));
//		m_plan->twiddle[2*n+1] = (float) (scale * sin(omiga * n + alpha ));
//	}
//
//	m_plan->fft_in	 = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N >> 2);
//	m_plan->fft_out  = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * N >> 2);
//	m_plan->fft_plan = fftwf_plan_dft_1d(N >> 2,
//																				m_plan->fft_in,
//																				m_plan->fft_out,
//																				FFTW_FORWARD,
//																				FFTW_MEASURE);
//
//	return TRUE;
//}
//
//void CFourier::mdctf_free(/*mdctf_plan* m_plan*/)
//{
//	fftwf_destroy_plan(m_plan->fft_plan);
//	fftwf_free(m_plan->fft_in);
//	fftwf_free(m_plan->fft_out);
//	free(m_plan->twiddle);
//	free(m_plan);
//}
//
//void CFourier::mdctf(float* mdct_line, float* time_signal/* ,mdctf_plan* m_plan*/)
//{
//	float *xr, *xi, r0,i0;
//	float *cos_tw, *sin_tw, c, s;
//	int		N4, N2, N34, N54, n;
//
//	N4  = (m_plan->N) >> 2;
//	N2	= 2 * N4;
//	N34 = 3 * N4;
//	N54 = 5 * N4;
//
//	cos_tw = m_plan->twiddle;
//	sin_tw = cos_tw + 1;
//
//	/* odd/even folding and pre-twiddle */
//	xr = (float *) m_plan->fft_in;
//	xi = xr + 1;
//	for(n = 0 ; n < N4; n += 2)
//	{
//		r0 = time_signal[N34-1-n] + time_signal[N34+n];
//		i0 = time_signal[N4+n]		- time_signal[N4-1-n];
//
//		c = cos_tw[n];
//		s = sin_tw[n];
//
//		xr[n] = r0 * c + i0 * s;
//		xi[n] = i0 * c - r0 * s;
//	}
//
//	for(; n < N2; n += 2)
//	{
//		r0 = time_signal[N34-1-n] - time_signal[-N4+n];
//		i0 = time_signal[N4+n]		+ time_signal[N54-1-n];
//
//		c = cos_tw[n];
//		s = sin_tw[n];
//
//		xr[n]	= r0 * c + i0 * s;
//		xi[n] = i0 * c - r0 * s;
//	}
//
//	/* complex FFT of N/4 long */
//	fftwf_execute(m_plan->fft_plan);
//
//	/* post-twiddle */
//	xr = ( float*) m_plan->fft_out;
//	xi = xr + 1;
//	for(n = 0 ; n < N2; n += 2)
//	{
//		r0 = xr[n];
//		i0 = xi[n];
//
//		c = cos_tw[n];
//		s = sin_tw[n];
//
//		mdct_line[n]			= - r0 * c - i0 * s;
//		mdct_line[N2-1-n] = - r0 * s + i0 * c;
//	}
//}
//
//void CFourier::imdctf(float* time_signal, float* mdct_line/* ,mfctf_plan* m_plan*/)
//{
//	float	*xr, *xi, r0, i0, r1, i1;
//	float	*cos_tw, *sin_tw, c, s;
//	int		N4, N2, N34, N54, n;
//
//	N4	= (m_plan->N) >> 2;
//	N2	= 2 * N4;
//	N34	= 3 * N4;
//	N54	=	5 * N4;
//
//	cos_tw = m_plan->twiddle;
//	sin_tw = cos_tw + 1;
//
//	/* pre-twiddle */
//	xr = (float*) m_plan->fft_in;
//	xi = xr + 1;
//	for(n = 0 ; n < N2; n += 2)
//	{
//		r0 = mdct_line[n];
//		i0 = mdct_line[N2-1-n];
//
//		c = cos_tw[n];
//		s = sin_tw[n];
//
//		xr[n] = -2.f * (i0 * s + r0 * c);
//		xi[n] = -2.f * (i0 * c - r0 * s);
//	}
//
//	/* complex FFT of N/4 long */
//	fftwf_execute(m_plan->fft_plan);
//
//	/* odd/even expanding and post-twiddle */
//	xr = (float *) m_plan->fft_out;
//	xi = xr + 1;
//	for(n = 0 ; n < N4; n += 2)
//	{
//		r0 = xr[n];
//		i0 = xi[n];
//
//		c = cos_tw[n];
//		s = sin_tw[n];
//
//		r1 = r0 * c + i0 * s;
//		i1 = r0 * s - i0 * c;
//
//		time_signal[N34-1-n]	= r1;
//		time_signal[N34+n]		= r1;
//		time_signal[N4+n]			= i1;
//		time_signal[N4-1-n]		= -i1;
//	}
//
//	for(; n < N2; n += 2)
//	{
//		r0 = xr[n];
//		i0 = xi[n];
//
//		c = cos_tw[n];
//		s = sin_tw[n];
//
//		r1 = r0 * c + i0 * s;
//		i1 = r0 * s - i0 * c;
//
//		time_signal[N34-1-n]	= r1;
//		time_signal[-N4+n]		= -r1;
//		time_signal[N4+n]			= i1;
//		time_signal[N54-1-n]	= i1;
//	}
//}
