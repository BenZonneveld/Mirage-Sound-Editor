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
	DWORD wait_state;
	bool progress_val_set = false;
	int progress_value;
	int maxval;

	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA(Name);

	// First select the parameter number we are going to change
	ParmDecimal = Parameter/10;
	ParmDigit = Parameter-(10*ParmDecimal);
	ParmChange[6] = ParmDecimal;
	ParmChange[7] = ParmDigit;

	ResetEvent(midi_in_event);
	SendData(ParmChange);
	wait_state = WaitForSingleObject(midi_in_event,100); // Wait for confirmation
//GetInitialValue:

	ReceivedParmNumber = 0xff;
	ResetEvent(midi_in_event);
GetInitialValue:
	SendData(GetCurrentValue);
	wait_state = WaitForSingleObject(midi_in_event,100);

	if ( ReceivedParmNumber != Parameter )
	{
		ResetEvent(midi_in_event);
		goto GetInitialValue;
	}

ParmChangeLoop:
	ResetEvent(midi_in_event);
	wait_state = WaitForSingleObject(midi_in_event,50);
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
		wait_state = WaitForSingleObject(midi_in_event,24);
		goto ParmChangeLoop;
	}
	if ( ReceivedParmValue[Parameter] < Value )
	{
		SendData(ValueUp);
		wait_state = WaitForSingleObject(midi_in_event,24);
		goto ParmChangeLoop;
	}
	progress.DestroyWindow();
}

void SampleStartEnd(const char * Name, unsigned char Parameter, unsigned char Value)
{
	unsigned char ParmDecimal;
	unsigned char ParmDigit;
	DWORD wait_state;
	bool progress_val_set = false;
	int progress_value;
	int maxval;

	progress.Create(CProgressDialog::IDD, NULL);
	progress.SetWindowTextA(Name);

	// First select the parameter number we are going to change
	ParmDecimal = Parameter/10;
	ParmDigit = Parameter-(10*ParmDecimal);
	ParmChange[6] = ParmDecimal;
	ParmChange[7] = ParmDigit;

	ResetEvent(midi_in_event);
	SendData(ParmChange);
	wait_state = WaitForSingleObject(midi_in_event,100); // Wait for confirmation
//GetInitialValue:

	ReceivedParmNumber = 0xff;
	ResetEvent(midi_in_event);
//GetInitialValue:
//	SendData(GetCurrentValue);
//	wait_state = WaitForSingleObject(midi_in_event,100);

ParmChangeLoop:
//	ResetEvent(midi_in_event);
//	wait_state = WaitForSingleObject(midi_in_event,50);
	ResetEvent(midi_in_event);
	SendData(GetCurrentValue);

	for ( int no_parms = 0; no_parms < 7; no_parms++ )
	{
		wait_state = WaitForSingleObject(midi_in_event,24);
			if ( wait_state == WAIT_TIMEOUT )
				Sleep(2);
		ResetEvent(midi_in_event);
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
		for ( int no_parms = 0; no_parms < 14; no_parms++ )
		{
			wait_state = WaitForSingleObject(midi_in_event,24);
			if ( wait_state == WAIT_TIMEOUT )
				Sleep(2);
			ResetEvent(midi_in_event);
		}
		goto ParmChangeLoop;
	}
	if ( ReceivedParmValue[Parameter] < Value )
	{
		SendData(ValueUp);
		for ( int no_parms = 0; no_parms < 14; no_parms++ )
		{
			wait_state = WaitForSingleObject(midi_in_event,24);
			if ( wait_state == WAIT_TIMEOUT )
				Sleep(2);
			ResetEvent(midi_in_event);
		}
		goto ParmChangeLoop;
	}
	progress.DestroyWindow();
}