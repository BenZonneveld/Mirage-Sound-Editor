#include "stdafx.h"
#include "samplerate.h"
#include <SndObj/AudioDefs.h>
#include "SndMem.h"

void resynthesize(CString Pathname,char unsigned * wavedata, long samplesize, float sr, unsigned char bit_depth, int fftsize,int hopsize, int convolute)
{
	float *lpFloatOut;
	short *short_array;
	char unsigned *outwave;
	char unsigned *inwave;
//	int convolute = 3;
//	int fftsize = 1024;
	int offset=convolute*(fftsize/4);
	double gain=1.0;
	double max=0.0;

	inwave=(char unsigned*)malloc((samplesize+(2*offset))*sizeof(char unsigned));
	outwave=(char unsigned*)malloc((samplesize+(4*offset))*sizeof(char unsigned));

	memset(inwave,0x80,samplesize+(2*offset));
	memcpy(inwave+offset,wavedata,samplesize);

	// Detect amplitude
	lpFloatOut=new float[samplesize*2];//(float *)malloc(samplesize*sizeof(float));
	src_unchar_to_float_array(wavedata, lpFloatOut, (int)(samplesize));
	max=apply_gain(lpFloatOut, samplesize, 1, max, gain);
	gain=1.0/max;

	int encoding=BYTESAM;
	HammingTable hanning(1024,0.5f);
	
	SndMem input(inwave,samplesize+(2*offset),READ,1,8,0,DEF_VECSIZE,sr);
	SndMem sndout(outwave, samplesize+offset, OVERWRITE,1,8,0,DEF_VECSIZE,sr);

//	SndRTIO output(2,SND_OUTPUT,DEF_BSIZE,DEF_PERIOD,SHORTSAM,0,DEF_VECSIZE,sr);
	SndIn  in(&input,1);//,DEF_VECSIZE,sr);
	PVA anal(&hanning,&in,.75f,fftsize,hopsize,sr);
	PVConvol blur_pass1(&anal,(convolute+1)/2,hopsize,fftsize,sr);
	PVConvol blur_pass2(&blur_pass1,convolute,hopsize,fftsize,sr);
	PVS synth(&hanning,&blur_pass2,fftsize,hopsize,sr);
	Gain outgain(-0.5f,&synth,DEF_VECSIZE,sr);
//	output.SetOutput(1,&synth);
//	output.SetOutput(2,&synth);
	sndout.SetOutput(1,&outgain);

	while(!input.Eof())
	{
		input.Read();
		in.DoProcess();
		anal.DoProcess();
  	blur_pass1.DoProcess();
  	blur_pass2.DoProcess();
		synth.DoProcess();
		outgain.DoProcess();
		sndout.Write();
	}

	// Fix the output volume
	memcpy(wavedata,(outwave+2*offset),samplesize);
	src_unchar_to_float_array(wavedata, lpFloatOut, (int)(samplesize));
	max=0.0;
	max=apply_gain(lpFloatOut, samplesize, 1, max, gain);
	src_float_to_unchar_array(lpFloatOut,wavedata,samplesize);

	// Cleanup
	free(outwave);
	free(inwave);
	delete [] lpFloatOut;
}