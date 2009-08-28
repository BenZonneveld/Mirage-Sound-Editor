/* $Id: MidiReceive.h,v 1.4 2008/01/23 22:42:32 root Exp $ */
#pragma once
#include "afxwin.h"

extern unsigned char	sysex_mode;
extern unsigned int	pagecount;

extern	HMIDIIN			midi_in_handle;
extern	MIDIHDR			midiInHdr;
extern short closed;

int StartMidiReceiveData(void);
int StopMidiReceiveData(void);