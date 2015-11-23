#include "StdAfx.h"
#define _USE_MATH_DEFINES 
#include <cmath>
#include "../globals.h"
#include "../WaveApi/wavesamples.h"
#include "samplerate.h"
#ifndef NOFFTW3
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
#endif