#pragma once
#ifndef NOFFTW3
#pragma comment (lib, "libfftw3.ming/libfftw3f-3.lib")
#include "afxwin.h"
#include "..\..\libfftw3\fftw3.h"


class CFourier :
  public CDialog
{
public:
	CFourier(void);
	~CFourier(void);
	double DetectPitch(struct _WaveSample_ *pWav);
	double DetectTopHarmonic(struct _WaveSample_ *pWav);
private:

};
#endif