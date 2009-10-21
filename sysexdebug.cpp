#include "stdafx.h"
#include "Mirage Editor.h"
#include "sysexdebug.h"
#include <mmsystem.h>

void sysexdump(unsigned char *sysEx, char *Mode)
{
	char *SEMessage;
	unsigned char MessageID;

	SEMessage=(char *)malloc(255);
	MessageID=sysEx[4];
	switch (MessageID)
	{
		case CONFIG_PARM_REQ:
			SEMessage="Config Parameters Request";
			break;
		case COMMAND_CODE:
			SEMessage="Command Code";
			switch ( sysEx[5] )
			{
				case SELECT_LOWER:
					SEMessage="Select Lower Wavesample";
					break;
				case SELECT_UPPER:
					SEMessage="Select Upper Wavesample";
					break;
			}
			break;
		case CONFIG_PARM_DUMP:
			SEMessage="Config Parameters Dump Data";
			break;
		case LOWER_PRG_DUMP_REQ:
			SEMessage="Lower Program Dump Request";
			break;
		case UPPER_PRG_DUMP_REQ:
			SEMessage="Upper Program Dump Request";
			break;
		case WAVE_DUMP_REQ:
			SEMessage="Wave Dump Request";
			break;
		case PRG_DUMP_LOWER:
			SEMessage="Lower Program Dump Data";
			break;
		case PRG_DUMP_UPPER:
			SEMessage="Upper Program Dump Data";
			break;
		case WAVE_DUMP_DATA:
			SEMessage="Wave Dump Data";
			break;
		case PRG_STATUS_MSG:
			SEMessage="Program Status Message";
			break;
		case WAVE_STATUS_MSG:
			SEMessage="Wavesample Status Message";
			break;
		case WAVE_ACK:
			SEMessage="Wavesample acknowledge";
			break;
		case WAVE_NACK:
			SEMessage="Wavesample NOT acknowleged";
		default:
			SEMessage="Unknown Mirage Sysex";
	}
	fprintf(logfile,"%s %d bytes of %s data\n",Mode, sysEx[0],SEMessage);
	for(int i=1 ; i <= sysEx[0]; i++)
	{
		fprintf(logfile,"%02X ", sysEx[i]);
	}
	fprintf(logfile,"\n\n");
//	free(SEMessage);
}

void sysexerror(const unsigned char * ptr,DWORD BytesRecorded,char *Mode)
{
	char *SEMessage;
	unsigned char MessageID;
	BOOL	Show = TRUE;
//	unsigned char * ptr = (unsigned char *)(lpMidiHeader->lpData);
	SEMessage=(char *)malloc(255);
	MessageID=*(ptr+3);
	switch (MessageID)
	{
		case CONFIG_PARM_REQ:
			SEMessage="Config Parameters Request";
			Show = FALSE;
			break;
		case COMMAND_CODE:
			SEMessage="Command Code";
			switch ( *(ptr+4))
			{
				case SELECT_LOWER:
					SEMessage="Select Lower Wavesample";
					break;
				case SELECT_UPPER:
					SEMessage="Select Upper Wavesample";
					break;
			}
			break;
		case CONFIG_PARM_DUMP:
			SEMessage="Config Parameters Dump Data";
			//Show = TRUE;
			break;
		case LOWER_PRG_DUMP_REQ:
			SEMessage="Lower Program Dump Request";
			Show = FALSE;
			break;
		case UPPER_PRG_DUMP_REQ:
			Show = FALSE;
			SEMessage="Upper Program Dump Request";
			break;
		case WAVE_DUMP_REQ:
			SEMessage="Wave Dump Request";
			Show = FALSE;
			break;
		case PRG_DUMP_LOWER:
			SEMessage="Lower Program Dump Data";
			Show = FALSE;
			break;
		case PRG_DUMP_UPPER:
			SEMessage="Upper Program Dump Data";
			Show = FALSE;
			break;
		case WAVE_DUMP_DATA:
			SEMessage="Wave Dump Data";
			Show = FALSE;
			break;
		case PRG_STATUS_MSG:
			SEMessage="Program Status Message";
			break;
		case WAVE_STATUS_MSG:
			SEMessage="Wavesample Status Message";
			break;
		case WAVE_ACK:
			SEMessage="Wavesample acknowledge";
			break;
		case WAVE_NACK:
			SEMessage="Wavesample NOT acknowleged";
		default:
			SEMessage="Unknown Mirage Sysex";
	}
	if ( *(ptr) != 0xF0 && *(ptr+(BytesRecorded-1)) != 0xF7 )
		return;

	fprintf(logfile,"Received %d bytes of %s %s data:\n",BytesRecorded,Mode,SEMessage);
	
	if ( Show == FALSE )
		return;

	for(int i=0 ; i < BytesRecorded; i++)
	{
		fprintf(logfile,"%02X ", *(ptr+i));
		if ( (i+1) % 16 == 0 )
			fprintf(logfile,"\n");
	}
	fprintf(logfile,"\n\n");
//	free(SEMessage);
}