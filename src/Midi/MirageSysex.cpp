/* $Id: MirageSysex.cpp,v 1.18 2008/02/23 23:58:51 root Exp $ */

#include "stdafx.h"
#include "../Globals.h"

#include "../Mirage Editor.h"

#include "Nybble.h"
#include "../WaveApi/Wave Doc.h"
#include "../WaveApi/Wave View.h"
#include "SendSysex.h"
#include "../WaveApi/wavesamples.h"
#include "../Dsp/float_cast.h"
#include "Mirage Helpers.h"
#include "../Dsp/Tuning.h"
#include "../Dialogs/Dialog_OrigKey.h"
#include "../Dialogs/Dialog_TxSamParms.h"
#include "MirageSysex.h"
#include "Mirage Sysex_Strings.h"
#include "MidiWrapper/LongMsg.h"

midi::CLongMsg LongMsg;

char	DataDumped = 0;
unsigned char	lower_upper_select = 0;
unsigned char	ProgramStatus = 0xFF;
unsigned char	WavesampleStatus = 0xFF;
unsigned char	WavesampleStore = 0;

/* For setting the original key */
unsigned char LastMidiKey;

#pragma pack (1)
struct _program_dump_table_ ProgramDumpTable[2];

#pragma pack(1)
struct _config_dump_table_ ConfigDump;

struct _WaSaRe_ WaveSampleReceive;
int MirageOS;

BOOL GetAvailableSamples(void)
{
	DWORD wait_state;
	progress.SetWindowTextA("Getting Available Lower Samples");
	progress.Bar.SetRange32(0,1255);
	progress.ShowWindow(SW_SHOW);

	SendData(ProgramDumpReqLower);
	WaitForSysex();

	progress.progress(0);
	progress.ShowWindow(SW_HIDE);

	// Should be aprox 625 bytes of Program Dump Data
	progress.SetWindowTextA("Getting Available Upper Samples");
	progress.Bar.SetRange32(0,1255);
	progress.ShowWindow(SW_SHOW);

	SendData(ProgramDumpReqUpper);

	WaitForSysex();
	progress.progress(0);
	progress.ShowWindow(SW_HIDE);

	return true;
}

BOOL GetSampleParameters(void)
{
	/*
	 * Get the sample parameters for the lower programs
	 */
	SendData(ProgramDumpReqLower);
	WaitForSysex();
	/*
	 * Get the sample parameters for the upper programs
	 */
	SendData(ProgramDumpReqUpper);
	WaitForSysex();

	return true;
}

BOOL DoSampleSelect(unsigned char *SampleSelect,unsigned char SampleNumber)
{
	unsigned char ExpectedWavesample;
	unsigned char SelectedWavesample;
	unsigned char ul_Wavesample;
	unsigned char * pSample = NULL;
	int trycount=0;
	char errorstring[50];
	DWORD wait_state;

	unsigned char SampleNumberSelect[]={7,
									MirID[0],
									MirID[1],
									MirID[2],
									0x01, // Commando Code
									(SampleNumber+1),
									0x7F,
									0xF7}; // Select Sample

	// Start Recording
	ProgramStatus = 0xFF;
	WavesampleStatus = 0xFF;

	SendData(SampleSelect);
	/*
	/* Now check the response from the Mirage
	/* Get Program Status Message (That's what the MASOS documentation calls it.
	/* This seems like a bug in the manual because the Mirage actualy returns
	/* The PREVIOUSLY selected wavesample
	/* Value is put into "ProgramStatus" 
	*/
	WaitForSysex();

	Sleep(10);
	SendData(SampleNumberSelect);
	/*
	/* Get Wavesample Status Message
	/* This is the response from the Mirage with the current selected wavesample
	/* We check if this is the correct sample we are expecting
	/* Value is put into "WavesampleStatus"
	*/
	WaitForSysex();

	/*
	/* Determine lower or upper sample expected
	/* And the expected sample number
	*/
	
	WavesampleStatus = theApp.m_WavesampleStatus;
	ExpectedWavesample = SampleNumber;
	ul_Wavesample = (WavesampleStatus & 0xF0) >> 1;
	SelectedWavesample = WavesampleStatus & 0x0F;
	WavesampleStore = SelectedWavesample; // + ul_Wavesample; // Where to store the received sample

	switch(SampleSelect[5])
	{
	case 0x15: // Lower Sample
		{
			if ( ul_Wavesample != 0 )
			{
				MessageBox(NULL,"Selected Lower Sample, but got Upper Sample?!","Error", MB_ICONERROR);
				return false;
			}
			break;
		}
	case 0x14: // Upper Sample
		{
			if ( ul_Wavesample != 8 )
			{
				MessageBox(NULL,"Selected Upper Sample, but got Lower Sample?!","Error", MB_ICONERROR);
				return false;
			}
			break;
		}
	}
	if ( ExpectedWavesample != SelectedWavesample )
	{
		sprintf_s(errorstring,sizeof(errorstring),"Selected sample %d but got %d from Mirage.",SelectedWavesample,ExpectedWavesample);
		MessageBox(NULL,(CString)errorstring, "Error", MB_ICONERROR);
		return false;
	}
	return true;
}


BOOL GetSample(unsigned char *SampleSelect, unsigned char SampleNumber)
{
	short pages;
	int	pagecount=0;
//	BOOL LoopSwitch;
	DWORD wait_state;
//	byte trycount=0;

	if (DoSampleSelect(SampleSelect,SampleNumber) == false)
		return false;
	unsigned char bank = (0x15-SampleSelect[5]);
	WaveSampleReceive.ul_Wavesample = (WavesampleStatus & 0xF0) >> 1;

	pages = 1 + (ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleEnd - ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleStart);

	/* If there is a loop enabled disable this before receiving the sample */
	if (ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopSwitch == 1 )
	{
		WaveSampleReceive.LoopSwitch = true;
		SendData(LoopOff);
	} else {
		WaveSampleReceive.LoopSwitch = false;
		SendData(LoopOff); // For midi receive consistency, now we don't have to work around this case
	}

	WaitForSysex();

	/* Now Request the selected sample from the Mirage */
	progress.SetWindowTextA("Getting Sample Data");
	progress.Bar.SetRange32(0, 8+(pages*MIRAGE_PAGESIZE*2));
	// Reset the progress
	progress.progress(0);
	progress.ShowWindow(SW_SHOW);

	SendData(WaveDumpReq);
	WaitForSysex();

	return true;
}

BOOL GotSample(void)
{
	progress.ShowWindow(SW_HIDE);
	if ( WaveSample.samplepages == 0 )
		return false;
	if(WaveSample.checksum != GetChecksum(&WaveSample))
	{
		SendData(WavesampleNack);
		
		MessageBox(NULL,"Sample checksum not correct.","ERROR",MB_ICONERROR);
		return false;
	} else {
		SendData(WavesampleAck);
	}
	
	/* Remember to switch the loop back on */
	if ( WaveSampleReceive.LoopSwitch != FALSE )
	{
		SendData(LoopOn);
	}

	CreateRiffWave(WavesampleStore, (WaveSampleReceive.ul_Wavesample >> 4), WaveSampleReceive.LoopSwitch );
	CreateFromMirage(WavesampleStore,WaveSampleReceive.ul_Wavesample);

	return true;
}

BOOL PutSample(unsigned char *SampleSelect,unsigned char SampleNumber, bool LoopOnly)
{
	COrigKey		GetOriginalKey;
	CTxSamParms TxSamParams;
	_WaveSample_ *pWav;
	unsigned char TransmitSamplePages;
	DWORD		DataSize;
//	unsigned char *TransmitSample;
	unsigned char LsNybble;
	unsigned char MsNybble;
	unsigned int counter;
	unsigned int counter2;
	unsigned char bank;
	unsigned int samplerate;
	unsigned char tuning_fine;
	unsigned char tuning_course;
	unsigned char OriginKey;
	unsigned char LastKey;
	unsigned char TargetLoopStart;
	unsigned char TargetLoopEnd;
	unsigned char TargetLoopFine;
	_program_dump_table_ PrgDump;

	/* Do The sample select */
	switch (SampleSelect[5])
	{
	case 0x15:
		bank = 0;
		break;
	case 0x14:
		bank = 1;
		break;
	}

	if (DoSampleSelect(SampleSelect,SampleNumber) == false)
		return false;
	
	/* Get the document */
	MWAV hWAV = theApp.m_CurrentDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return false;
	}

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	/* Get the number of pages to transmit */
	TransmitSamplePages = GetNumberOfPages(pWav);

	if ( ! LoopOnly )
	{
		TxSamParams.DoModal();
		TxSamParams.DestroyWindow();
	}

	/* Get the original key */
	if ( theApp.GetProfileIntA("Settings","TxSampleParams",true) == 1 && ! LoopOnly )
	{
		GetOriginalKey.DoModal();
		GetOriginalKey.DestroyWindow();

		LastKey = theApp.m_LastNote;
	}

	if ( LoopOnly )
		goto LoopOnly;

	DataSize=(((TransmitSamplePages+1) * MIRAGE_PAGESIZE) * 2) + 8;

	for ( counter = 0 ; counter < 4; counter++)
	{
		TransmitSample[counter] = WaveDumpData[counter];
	}

	/* Construct Pagecount */
	TransmitSample[4] = TransmitSamplePages & 0x0F;
	TransmitSample[5] = (TransmitSamplePages & 0xF0) >> 4;

	/* Construct Pages */
	counter2 = 6;
	int temp=0xFF + (TransmitSamplePages * MIRAGE_PAGESIZE);

	for(counter = 0 ; counter <= unsigned int(0xFF + (TransmitSamplePages * MIRAGE_PAGESIZE)); counter++)
	{
		if ( counter < pWav->data_header.dataSIZE )
		{
			if (pWav->SampleData[counter] > 0 )
			{
				LsNybble = pWav->SampleData[counter] & 0x0F;
				MsNybble = (pWav->SampleData[counter] & 0xF0) >> 4;
			} else {
				LsNybble = 1;
				MsNybble = 0;
			}
		} else {
			LsNybble = 0;
			MsNybble = 8;
			pWav->SampleData[counter] = 0x80;
		}
		TransmitSample[counter2] = LsNybble;
		TransmitSample[(counter2+1)] = MsNybble;
		counter2 += 2;
	}

	pWav->samplepages = TransmitSamplePages;
	/* Get Checksum */
	TransmitSample[counter2] = GetChecksum(pWav);
	counter2++;
	TransmitSample[counter2] = 0xF7;

	/* Now Transmit the sample */
	progress.SetWindowTextA("Transmitting Sample");
	progress.Bar.SetRange32(0,(counter2+1));

	SendLongData(TransmitSample, counter2+1);


	WaitForSysex();
	/* Get the OS version again to confirm sample is transmitted
	 * actually a workaround for some midi interfaces which
	 * return immediately while data is still being transmitted
	 */
	GetConfigParms();
	WaitForSysex();

	if ( theApp.GetProfileIntA("Settings","TxSampleParams",true) == false && ! LoopOnly )
		return true;
LoopOnly:

	GetSampleParameters();
	memcpy(&PrgDump,&ProgramDumpTable[bank],sizeof(ProgramDumpTable[bank]));

	unsigned char CurSampleEnd=PrgDump.WaveSampleControlBlock[SampleNumber].SampleEnd;
	unsigned char CurSampleStart=PrgDump.WaveSampleControlBlock[SampleNumber].SampleStart;
	
	TargetLoopStart = unsigned char(pWav->sampler.Loops.dwStart >> 8);
	TargetLoopEnd = unsigned char((pWav->sampler.Loops.dwEnd & 0xFF00) >> 8);
	TargetLoopFine = unsigned char(pWav->sampler.Loops.dwEnd & 0x00FF);

	/* Set sample endpoint */
	if ( (CurSampleEnd - CurSampleStart) > TransmitSamplePages )
	{
		/* It's not possible to move the sample end point before the loop end, so change the loop first */
		/* Set Loop Start Point */
		ChangeParameter("Setting Loop Startpoint", 62, CurSampleStart);

		/* Set Loop End Point */
		ChangeParameter("Setting Loop Endpoint",63, CurSampleStart+1);
	}


	ChangeParameter("Setting Sample Endpoint", 61, CurSampleStart+TransmitSamplePages);

 	SendData(LoopOn);
	WaitForSysex(); 
	
	SendData(LoopOff);
	WaitForSysex();
	
	/* Next check if we have to set the looppoints */
	if ( pWav->sampler.Loops.dwPlayCount == 0 ) /* Check if Loop is enabled */
	{
		///* Set Loop Start Point */
		ChangeParameter("Setting Loop Startpoint", 62, CurSampleStart+TargetLoopStart);

		///* Set Loop End Point */
		ChangeParameter("Setting Loop Endpoint",63, CurSampleStart+TargetLoopEnd);

		///* Set Loop End Fine */
		ChangeParameter("Setting Loop End Fine point",64, CurSampleStart+TargetLoopFine);
		SendData(LoopOn);
	} // Loop on/off Detect

	/* If we received the sample from the Mirage we do not change the tuning parameters */
	if ( theApp.m_CurrentDoc->FromMirage() != true )
	{	
		samplerate = pWav->waveFormat.fmtFORMAT.nSamplesPerSec;
		tuning_course = 255;

		if ( samplerate >= 5000 && samplerate < 10000 && tuning_course == 255 )
		{
			tuning_course = 0;
			samplerate = samplerate * 4;
		} else 
		if ( samplerate >= 10000 && samplerate < 20000 && tuning_course == 255 )
		{
			tuning_course = 1;
			samplerate = samplerate * 2;
		}
		else 
		if ( samplerate >= 20000 && samplerate < 40000 && tuning_course == 255 )
		{
			tuning_course = 2;
		}
		else 
		if ( samplerate >= 40000 && samplerate < 80000 && tuning_course == 255 )
		{
			tuning_course = 3;
			samplerate = samplerate / 2;
		}
		else 
		if ( samplerate >= 80000 && samplerate < 160000 && tuning_course == 255 )
		{
			tuning_course = 4;
			samplerate = samplerate / 4;
		}
		else 
		if ( samplerate >= 160000 && samplerate < 320000 && tuning_course == 255 )
		{
			tuning_course = 5;
			samplerate = samplerate / 8;
		}

		for (tuning_fine = 0 ; tuning_fine <= 255 ; tuning_fine++)
		{
			if ( (Tuning[tuning_fine] >= samplerate) )
			{
				if (Tuning[tuning_fine] == samplerate )
				{
					break;
				} else {
					if ( samplerate < ((Tuning[tuning_fine]+Tuning[tuning_fine-1]) / 2) ) // Underrun is possible!!!
						tuning_fine--;
					break;
				}
			}
		}
		
		if ( LastKey < 0x45 )
		{
			OriginKey = 0x45 - LastKey;

OctaveDown:
			if (OriginKey < 12 )
			{
				if ( ((short)tuning_fine + (OriginKey * KEY_TRANSPOSE )) > 255 )
					tuning_course++;
				tuning_fine = tuning_fine + (unsigned char)(OriginKey * KEY_TRANSPOSE );
			} else {
				tuning_course = tuning_course + (OriginKey / 12);
				OriginKey = OriginKey - ((OriginKey / 12) * 12);
				goto OctaveDown;
			}
		}
		if ( LastKey > 0x45 )
		{
			OriginKey = LastKey - 0x45;
OctaveUp:
			if (OriginKey < 12 )
			{	
				if ( ((short)tuning_fine - (OriginKey * KEY_TRANSPOSE )) < 0 )
					tuning_course--;
				tuning_fine = tuning_fine - (unsigned char)(OriginKey * KEY_TRANSPOSE );
			} else {
				tuning_course = tuning_course - (OriginKey / 12);
				OriginKey = OriginKey - ((OriginKey / 12) * 12);
				goto OctaveUp;
			}
		}
		unsigned char Current_Fine_Tune = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].FineTune;
		unsigned char Current_Tune_Course = ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].CoarseTune;


		/* Course Tuning */
		if ( tuning_course > 0 && tuning_course <= 7 )
		{
			ChangeParameter("Setting Course Tuning",67,tuning_course);
		}

		/* Fine Tuning */
		ChangeParameter("Setting Fine Tuning",68,tuning_fine);
	}

	progress.ShowWindow(SW_HIDE);

	return true;
}

BOOL GetConfigParms()
{
	SendData(ConfigParmsDumpReq);
	return WaitForSysex();
}

BOOL WaitForSysex()
{
	MSG uMsg;
	while (true)
	{
		while (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			switch (uMsg.message)
			{
			case WM_SYSEX_DONE:
				return TRUE;
				break;
			//case WM_PROGRESS:
			//	progress.Bar.StepIt();
			//	progress.UpdateWindow();
			//	break;
			default:
				TranslateMessage(&uMsg);
				DispatchMessage(&uMsg);
			}
		}
	}
}
BOOL SendConfigParms()
{
	unsigned char LsNybble;
	unsigned char MsNybble;
	unsigned char TXByteCount=0;
	unsigned char ConfigParmsTX[64];
	unsigned char *configdump;
	int c=0;

	configdump=(unsigned char*)malloc(sizeof(ConfigDump));
	memcpy(configdump,&ConfigDump,sizeof(ConfigDump));
	memcpy(ConfigParmsTX+1,ConfigParmsDump,4);

	for(c=0; c < sizeof(ConfigDump);c++)
	{
		LsNybble = configdump[c] & 0x0F;
		MsNybble = (configdump[c] & 0xF0) >> 4;
		ConfigParmsTX[5+TXByteCount]=LsNybble;
		ConfigParmsTX[6+TXByteCount]=MsNybble;
		TXByteCount += 2;
	}

	TXByteCount += 4;
	ConfigParmsTX[TXByteCount]=0xF7;
	ConfigParmsTX[0]=TXByteCount;
	
	SendLongData(ConfigParmsTX+1,ConfigParmsTX[0]);

	return TRUE;
}

void ChangeParameter(const char * Name, unsigned char Parameter, unsigned char Value)
{
	unsigned char ParmDecimal;
	unsigned char ParmDigit;
	int no_parms;
	DWORD wait_state;
	bool progress_val_set = false;
	int progress_value;
	int maxval;

	progress.ShowWindow(SW_SHOW);
	progress.SetWindowTextA(Name);

	// First select the parameter number we are going to change
	ParmDecimal = Parameter/10;
	ParmDigit = Parameter-(10*ParmDecimal);
	ParmChange[6] = ParmDecimal;
	ParmChange[7] = ParmDigit;

	SendData(ParmChange);
//	WaitForSysex();

	ReceivedParmNumber = 0xff;
GetInitialValue:
	SendData(GetCurrentValue);
	WaitForSysex();

ParmChangeLoop:
	ReceivedParmNumber = 0xff;
	SendData(GetCurrentValue);
	WaitForSysex();
	Sleep(5);
	if (ReceivedParmNumber != Parameter )
	{
		goto ParmChangeLoop;
	}

	// Update the progressbar
	if (progress_val_set == false )
	{
		if ( ReceivedParmValue[Parameter] > Value )
		{
			maxval = ReceivedParmValue[Parameter] - Value;
		} else {
			maxval = Value - ReceivedParmValue[Parameter];
		}
		progress.Bar.SetRange32(0,maxval);
		progress_val_set = true;
		progress_value = 0;
	} else {
		progress_value++;
		progress.progress(progress_value);
	}

	if ( ReceivedParmValue[Parameter] > Value )
	{
		SendData(ValueDown);
		WaitForSysex();
		Sleep(10);
//		WaitForSysex();
		goto ParmChangeLoop;
	}
	if ( ReceivedParmValue[Parameter] < Value )
	{
		SendData(ValueUp);
		WaitForSysex();
		Sleep(10);
//		WaitForSysex();
		goto ParmChangeLoop;
	}
//	Sleep();
	progress.ShowWindow(SW_HIDE);
}
