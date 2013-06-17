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
	unsigned int PrevValue = 512;
	unsigned char ExpectedValue;
	int no_parms;
	DWORD wait_state;
//	bool progress_val_set = false;
//	int progress_value;
	int maxval;
	unsigned char tries = 0;

/*	if (Parameter == 60 || Parameter == 61 )
	{
		no_parms = 7;
	} else {
		no_parms = 1;
	}
*/
//	progress.Create(CProgressDialog::IDD, NULL);
//	progress.SetWindowTextA(Name);

	// First select the parameter number we are going to change
	ParmDecimal = Parameter/10;
	ParmDigit = Parameter-(10*ParmDecimal);
	ParmChange[6] = ParmDecimal;
	ParmChange[7] = ParmDigit;

	ResetEvent(midi_in_event);
	SendData(ParmChange);
	wait_state = WaitForSingleObject(midi_in_event,100); // Wait for confirmation
//GetInitialValue:
	ResetEvent(midi_in_event);
	ReceivedParmNumber = 0xff;
	SendData(GetCurrentValue);
GetInitialValue:
	wait_state = WaitForSingleObject(midi_in_event,100);

	if ( ReceivedParmNumber != Parameter )
	{
		ResetEvent(midi_in_event);
		goto GetInitialValue;
	}

	ExpectedValue = ReceivedParmValue[Parameter];

ParmChangeLoop:
	ResetEvent(midi_in_event);
	ReceivedParmNumber = 0xff;
	SendData(GetCurrentValue);
GetParameter:
	wait_state = WaitForSingleObject(midi_in_event,50);
	
	if (ReceivedParmNumber != Parameter )
	{
		ResetEvent(midi_in_event);
		goto GetParameter;
	}
	
	if ( ReceivedParmValue[Parameter] != ExpectedValue )
	{
		Sleep(50);
		tries++;
		if ( tries < 4 )
			goto ParmChangeLoop;
	}

	tries = 0;
	PrevValue = ReceivedParmValue[Parameter];
	// Update the progressbar
	//if (progress_val_set == false )
	//{
	//	if ( ReceivedParmValue[Parameter] > Value )
	//	{
	//		maxval = ReceivedParmValue[Parameter] - Value;
	//	} else {
	//		maxval = Value - ReceivedParmValue[Parameter];
	//	}
	//	progress.Bar.SetRange32(0,maxval);
	//	progress_val_set = true;
	//	progress_value = 0;
	//} else {
	//	progress_value++;
	//	progress.progress(progress_value);
	//}

	ResetEvent(midi_in_event);
	if ( ReceivedParmValue[Parameter] > Value )
	{
		ExpectedValue--;
		SendData(ValueDown);
		wait_state = WaitForSingleObject(midi_in_event,24);
		goto ParmChangeLoop;
	}
	if ( ReceivedParmValue[Parameter] < Value )
	{
		ExpectedValue++;
		SendData(ValueUp);
		wait_state = WaitForSingleObject(midi_in_event,24);
		goto ParmChangeLoop;
	}
//	progress.DestroyWindow();
}