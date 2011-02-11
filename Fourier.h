#pragma once
#pragma comment (lib, "libfftw3.ming/libfftw3f-3.lib")
#include "afxwin.h"
#include "fftw3.h"


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
