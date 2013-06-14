#include "stdafx.h"

#include "Mirage Editor.h" // For progress dialog
#include "Mirage Sysex_Strings.h" // For Sysex Strings
#include "CMidiReceiver.h"
#include "SysexParser.h" // For ParseSysex
#include "SendSysex.h"

void ChangeParameter(const char * Name, unsigned char Parameter, unsigned char Value)
{
	unsigned char ParmDecimal;
	unsigned char ParmDigit;
	int no_parms;
	bool progress_val_set = false;
	int progress_value;
	int maxval;

	if (Parameter == 60 || Parameter == 61 )
	{
		no_parms = 7;
	} else {
		no_parms = 1;
	}
	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA(Name);

	ParmDecimal = Parameter/10;
	ParmDigit = Parameter-(10*ParmDecimal);

	ParmChange[6] = ParmDecimal;
	ParmChange[7] = ParmDigit;
	ResetEvent(midi_in_event);
	SendData(ParmChange);
ParmChangeLoop:
//	SendData(GetCurrentValue);
	ResetEvent(midi_in_event);
	SendData(GetCurrentValue);
	for(int c=0; c<no_parms ; c++)
	{
		DWORD wait_state = WaitForSingleObject(midi_in_event,50);
		if (wait_state == WAIT_TIMEOUT)
		{
			return;
		}
//		if ( (c+1) < no_parms)
//		{
//			ResetEvent(midi_in_event);
//		}
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

	ResetEvent(midi_in_event);
	if ( ReceivedParmValue[Parameter] > Value )
	{
		SendData(ValueDown);
		DWORD wait_state = WaitForSingleObject(midi_in_event,24);
		goto ParmChangeLoop;
	}
	if ( ReceivedParmValue[Parameter] < Value )
	{
		SendData(ValueUp);
		DWORD wait_state = WaitForSingleObject(midi_in_event,24);
		goto ParmChangeLoop;
	}
	progress.DestroyWindow();
}