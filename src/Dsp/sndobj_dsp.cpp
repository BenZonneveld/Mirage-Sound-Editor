#include "stdafx.h"
//#include <afx.h>
#include "samplerate.h"
#include <SndObj/AudioDefs.h>
#include "SndMem.h"

void resynthesize(CString Pathname,char unsigned * wavedata, long samplesize, float sr, unsigned char bit_depth, int fftsize,int hopsize, int convolute)
{
	float *lpFloatOut;

	float time;
	char unsigned *outwave;
	char unsigned *inwave;

	hopsize = fftsize / 4;
	
//	int offset=convolute*(fftsize/4);
	double gain=1.0;
	double max=0.0;

	inwave=(char unsigned*)malloc(samplesize*sizeof(char unsigned));
	outwave=(char unsigned*)malloc((samplesize)*sizeof(char unsigned));

	memset(inwave,0x80,samplesize);
	memcpy(inwave,wavedata,samplesize);
	memset(outwave, 0x80, samplesize);
	memcpy(outwave, wavedata, samplesize);

	// Detect amplitude
	lpFloatOut=new float[samplesize];//(float *)malloc(samplesize*sizeof(float));
	src_unchar_to_float_array(inwave, lpFloatOut, (int)(samplesize));
	max=apply_gain(lpFloatOut, samplesize, 1, max, gain);
	gain=1.0/max;
	src_float_to_unchar_array(lpFloatOut, inwave, samplesize);

	int encoding=BYTESAM;
	//HammingTable hanning(1024,0.5f);
	HammingTable window(fftsize, 0.54f);

	SndMem input(inwave,samplesize,READ,1,8,0,DEF_VECSIZE,sr);
	SndMem sndout(outwave, samplesize, OVERWRITE,1,8,0,DEF_VECSIZE,sr);

	//SndIn  in(&input,1);//,DEF_VECSIZE,sr);
	//PVA anal(&hanning,&in,.75f,fftsize,hopsize,sr);
	//PVConvol blur_pass1(&anal,(convolute+1)/2,hopsize,fftsize,sr);
	//PVConvol blur_pass2(&blur_pass1,convolute,hopsize,fftsize,sr);
	//PVS synth(&hanning,&blur_pass2,fftsize,hopsize,sr);
	//Gain outgain(-0.5f,&synth,DEF_VECSIZE,sr);
	SndIn  in(&input);
	PVA anal(&window, &in,1.0F, fftsize,hopsize,sr);
	time = 0.125F;
	PVBlur blur(&anal, time,256, fftsize,sr);
	PVS synth(&window, &blur, fftsize, hopsize,sr);

	sndout.SetOutput(1,&synth);

	while(!input.Eof())
	{
		input.Read();
		in.DoProcess();
		anal.DoProcess();
		blur.DoProcess();
		synth.DoProcess();
		sndout.Write();
	}

	// Fix the output volume
	src_unchar_to_float_array(outwave, lpFloatOut, (int)(samplesize));
	max=0.0;
	gain = 1.0;
	max=apply_gain(lpFloatOut, samplesize, 1, max, gain);
	src_float_to_unchar_array(lpFloatOut,wavedata,samplesize);

	// Cleanup
//	free(outwave);
	free(inwave);
//	delete lpFloatOut;
}