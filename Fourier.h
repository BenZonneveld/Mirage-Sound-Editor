#pragma once
#pragma comment (lib, "libfftw3.ming/libfftw3f-3.lib")
#include "afxwin.h"
//#include <complex>
#include "fftw3.h"


class CFourier :
  public CDialog
{
public:
	CFourier(void);
	~CFourier(void);
	double DetectPitch(struct _WaveSample_ *pWav);
	void	Resynthesize(struct _WaveSample_ *pWav);
private:
	typedef struct{
		int							N;				// Number of time data points
		float 					*twiddle;	// Twiddle Factor
		fftwf_complex		*fft_in;	// fft workspace, input
		fftwf_complex		*fft_out; // fft workspace, output
		fftwf_plan			fft_plan;	// fft configuration
	} mdctf_plan;
	mdctf_plan *m_plan;

	BOOL mdctf_init(int N);
	void mdctf_free(/*mdctf_plan* m_plan*/);
	void mdctf(float* mdcf_line, float* time_signal/* ,mdctf_plan* m_plan*/);
	void imdctf(float* time_signal, float* mdct_line/* ,mfctf_plan* m_plan*/);

};
