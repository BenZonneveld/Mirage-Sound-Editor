#include "stdafx.h"
#include <SndObj/AudioDefs.h>
#include "SndMem.h"

void resynthesize(CString Pathname,char unsigned * wavedata, long samplesize, float sr, unsigned char bit_depth)
{
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
	int count=0;
	int maxcount=20;
	HammingTable hanning(1024,0.5f);

	SndMem input(wavedata,samplesize,READ,1,8,0,DEF_VECSIZE,sr);
	SndRTIO output(2,SND_OUTPUT,DEF_BSIZE,DEF_PERIOD,encoding,0,DEF_VECSIZE,sr);
	SndIn  in(&input,1);//,DEF_VECSIZE,sr);
//	ButtHP highpass(2500.0f,&in,0,DEF_VECSIZE,sr);
	PVA anal(&hanning,&in,1.0f,DEF_FFTSIZE,DEF_VECSIZE,sr);
	PVBlur blur(&anal,0.05f,DEF_VECSIZE,DEF_FFTSIZE,sr);
	PVS synth(&hanning,&blur);//,DEF_FFTSIZE,DEF_VECSIZE,sr);
//	ButtLP synth(2000.f,&in,0,DEF_VECSIZE,sr);
//	Gain outgain(-1.0f,&synth,DEF_VECSIZE,sr);

	output.SetOutput(1,&synth);
	output.SetOutput(2,&synth);

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
	}
}