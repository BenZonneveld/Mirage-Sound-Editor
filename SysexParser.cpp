#include "stdafx.h"

#include "Globals.h"
#include "MirageSysex.h"
#include "wavesamples.h"
#include "Nybble.h"
#include "Mirage Sysex_Strings.h"

void ParseSysEx(unsigned char* LongMessage)
{
	unsigned char	sysex_byte;
	unsigned char	* sysex_ptr = NULL;
	unsigned char * ptr = NULL;
	int byte_counter = 0;
	char MessageID;
	int sysexlength;

	MessageID=*(LongMessage+3);
	switch(MessageID)
	{
		case CONFIG_PARM_DUMP:
			sysex_ptr = (unsigned char*)&ConfigDump;
			break;
		case PRG_DUMP_LOWER:
		case PRG_DUMP_UPPER:
			lower_upper_select = ((*(LongMessage+3) & 0xF0 ) >>4);
			sysex_ptr = (unsigned char *)&ProgramDumpTable[lower_upper_select];
			break;
		case WAVE_DUMP_DATA:
			sysex_ptr = ((unsigned char *)&WaveSample.SampleData);
			memset(sysex_ptr,0, sizeof(WaveSample.SampleData));
			ptr = LongMessage; 
			sysexlength = LongMsg.GetLength();
			if ( *(LongMessage) == 0xF0 )
			{
				LongMessage += 4; /* First 4 bytes are the sysex header */
				/* Next two bytes are the pagecount */
				WaveSample.samplepages = de_nybblify(*(LongMessage),*(LongMessage+1));
				byte_counter += 6;
				LongMessage += 2;
			}
			while ( byte_counter < (sysexlength - 2) )
			{
				/* Reconstruct the byte from the nybbles and copy it to the correct structure*/
				sysex_byte = de_nybblify(*(LongMessage),*(LongMessage+1));
				memcpy(sysex_ptr++, &sysex_byte,1);
				LongMessage += 2;
				byte_counter += 2;
			}
			LongMessage = ptr;
			WaveSample.checksum = (unsigned char)*(LongMessage + (sysexlength - 2 ));
			return;
			break;
		case SMP_PARM_MSG:
			sysex_ptr = ((unsigned char *)&ParmCurValue);
			memset(sysex_ptr,0,9);
			ptr = LongMessage; 
			sysexlength = LongMsg.GetLength();
			if ( *(LongMessage) == 0xF0 )
			{
				/* the 5th byte is the parameter number */
				ReceivedParmNumber = *(LongMessage+5);
				ReceivedParmValue[ReceivedParmNumber] = de_nybblify(*(LongMessage+6),*(LongMessage+7));
			}
			return;
			break;
		case PRG_STATUS_MSG:
			return;
			break;
		case WAVE_STATUS_MSG:
			return;
			break;
		case WAVE_ACK:
			return;
			break;
		case WAVE_NACK:
			return;
			break;
		default:
			;;
	}
	if ( sysex_ptr == NULL )
		return;
	if ( *(LongMessage) == 0xF0 && *(LongMessage + 1) == MirID[1] && *(LongMessage + 2) == MirID[2] )
	{
		LongMessage = LongMessage + 4; /* First 4 bytes are the sysex header */
		byte_counter += 4;
	}
	sysexlength = LongMsg.GetLength();
	while ( byte_counter <= sysexlength )
	{
		/* Reconstruct the byte from the nybbles and copy it to the correct structure*/
		sysex_byte = de_nybblify(*(LongMessage),*(LongMessage+1));
		LongMessage += 2;
		memcpy(sysex_ptr, &sysex_byte,1);
		sysex_ptr++;
		byte_counter += 2;
	}
}
