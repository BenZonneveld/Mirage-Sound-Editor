#include "stdafx.h"
#include "samplerate.h"
#include <SndObj/AudioDefs.h>
#include "SndMem.h"

void resynthesize(CString Pathname,char unsigned * wavedata, long samplesize, float sr, unsigned char bit_depth)
{
	float *lpFloatOut;
	short *short_array;
	lpFloatOut=(float *)malloc(samplesize*sizeof(float));
	short_array=(short * )malloc(samplesize*sizeof(short));


	src_unchar_to_float_array(wavedata, lpFloatOut, (int)samplesize);
	src_float_to_short_array(lpFloatOut, short_array, (int)samplesize);

	int encoding=BYTESAM;
/*	char * path=Pathname.GetBuffer(Pathname.GetLength());
	SndWave input(path, READ,1,8,0,0,DEF_VECSIZE,sr);

	sr=input.GetSr();

	if ( input.GetSize() == 8 )
	{
		encoding = BYTESAM;
	} else {
		encoding = SHORTSAM;
	}

	encoding = BYTESAM;*/
	int fftsize = 2048;
	int count=0;
	int maxcount=20;
	HammingTable hanning(1024,0.5f);

	SndMem input(short_array,samplesize,READ,1,16,0,DEF_VECSIZE,sr);
	SndMem sndout(short_array, samplesize, OVERWRITE,1,	16,0,DEF_VECSIZE,sr);

	SndRTIO output(2,SND_OUTPUT,DEF_BSIZE,DEF_PERIOD,SHORTSAM,0,DEF_VECSIZE,sr);
	SndIn  in(&input,1);//,DEF_VECSIZE,sr);
//	ButtHP highpass(2500.0f,&in,0,DEF_VECSIZE,sr);
	PVA anal(&hanning,&in,.5f,fftsize,DEF_VECSIZE,sr);
	PVBlur blur(&anal,0.05f,DEF_VECSIZE,fftsize,sr);
	PVS synth(&hanning,&blur,fftsize,DEF_VECSIZE,sr);
//	ButtLP synth(2000.f,&in,0,DEF_VECSIZE,sr);
//	Gain outgain(-1.0f,&synth,DEF_VECSIZE,sr);

	output.SetOutput(1,&synth);
	output.SetOutput(2,&synth);
	sndout.SetOutput(1,&synth);

	while(!input.Eof())
	{
		input.Read();
		in.DoProcess();
//		highpass.DoProcess();
		anal.DoProcess();
		blur.DoProcess();
		synth.DoProcess();
//		outgain.DoProcess();
		output.Write();	
		sndout.Write();
	}
	src_short_to_float_array(short_array,lpFloatOut, (int)samplesize);
	src_float_to_unchar_array(lpFloatOut,wavedata,(int)samplesize);
	free(short_array);
	free(lpFloatOut);
}