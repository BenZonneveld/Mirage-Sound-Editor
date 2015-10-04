#include "stdafx.h"
#include "Mirage Editor.h"
#include "sysexdebug.h"
#include <mmsystem.h>
#include <vector>

#ifdef NDEBUG
void sysexdump(unsigned char *sysEx, char *Mode)
{
	CString SEMessage;
	unsigned char MessageID;

//	SEMessage=(char *)malloc( 255 );
	MessageID=sysEx[4];
	switch (MessageID)
	{
		case CONFIG_PARM_REQ:
			SEMessage.Format("Config Parameters Request");
			break;
		case COMMAND_CODE:
			SEMessage.Format("Command Code");
			switch ( sysEx[5] )
			{
				case SELECT_LOWER:
					SEMessage.Format("Select Lower Wavesample");
					break;
				case SELECT_UPPER:
					SEMessage.Format("Select Upper Wavesample");
					break;
			}
			break;
		case CONFIG_PARM_DUMP:
			SEMessage.Format("Config Parameters Dump Data");
			break;
		case LOWER_PRG_DUMP_REQ:
			SEMessage.Format("Lower Program Dump Request");
			break;
		case UPPER_PRG_DUMP_REQ:
			SEMessage.Format("Upper Program Dump Request");
			break;
		case WAVE_DUMP_REQ:
			SEMessage.Format("Wave Dump Request");
			break;
		case PRG_DUMP_LOWER:
			SEMessage.Format("Lower Program Dump Data");
			break;
		case PRG_DUMP_UPPER:
			SEMessage.Format("Upper Program Dump Data");
			break;
		case WAVE_DUMP_DATA:
			SEMessage.Format("Wave Dump Data");
			break;
		case PRG_STATUS_MSG:
			SEMessage.Format("Program Status Message");
			break;
		case WAVE_STATUS_MSG:
			SEMessage.Format("Wavesample Status Message");
			break;
		case WAVE_ACK:
			SEMessage.Format("Wavesample acknowledge");
			break;
		case WAVE_NACK:
			SEMessage.Format("Wavesample NOT acknowleged");
		default:
			SEMessage.Format("Unknown Mirage Sysex");
	}
//	fprintf(logfile,"%s %d bytes of %s data\n",Mode, sysEx[0],SEMessage);
	for(int i=1 ; i <= sysEx[0]; i++)
	{
//		fprintf(logfile,"%02X ", sysEx[i]);
	}
//	fprintf(logfile,"\n\n");
//	free(SEMessage);
}

void sysexerror(const unsigned char * ptr,DWORD BytesRecorded,char *Mode)
{
	CString SEMessage;
	unsigned char MessageID;
	BOOL	Show = TRUE;

//	SEMessage=(char *)malloc(255);
	//SEMessage = new char[255];

	MessageID=*(ptr+3);
	switch (MessageID)
	{
		case CONFIG_PARM_REQ:
			SEMessage.Format("Config Parameters Request");
			Show = FALSE;
			break;
		case COMMAND_CODE:
			SEMessage.Format("Command Code");
			switch ( *(ptr+4))
			{
				case SELECT_LOWER:
					SEMessage.Format("Select Lower Wavesample");
					break;
				case SELECT_UPPER:
					SEMessage.Format("Select Upper Wavesample");
					break;
			}
			break;
		case CONFIG_PARM_DUMP:
			SEMessage.Format("Config Parameters Dump Data");
			//Show = TRUE;
			break;
		case LOWER_PRG_DUMP_REQ:
			SEMessage.Format("Lower Program Dump Request");
			//Show = FALSE;
			break;
		case UPPER_PRG_DUMP_REQ:
			//Show = FALSE;
			SEMessage.Format("Upper Program Dump Request");
			break;
		case WAVE_DUMP_REQ:
			SEMessage.Format("Wave Dump Request");
			Show = FALSE;
			break;
		case PRG_DUMP_LOWER:
			SEMessage.Format("Lower Program Dump Data");
			Show = FALSE;
			break;
		case PRG_DUMP_UPPER:
			SEMessage.Format("Upper Program Dump Data");
			Show = FALSE;
			break;
		case WAVE_DUMP_DATA:
			SEMessage.Format("Wave Dump Data");
			Show = TRUE;
			break;
		case PRG_STATUS_MSG:
			SEMessage.Format("Program Status Message");
			break;
		case WAVE_STATUS_MSG:
			SEMessage.Format("Wavesample Status Message");
			break;
		case WAVE_ACK:
			SEMessage.Format("Wavesample acknowledge");
			break;
		case WAVE_NACK:
			SEMessage.Format("Wavesample NOT acknowleged");
			break;
		default:
			SEMessage.Format("Unknown Mirage Sysex");
	}
	if ( *(ptr) != 0xF0 && *(ptr+(BytesRecorded-1)) != 0xF7 )
	{
		return;
	}

//	fprintf(logfile,"Received %d bytes of %s %s data:\n",BytesRecorded,Mode,SEMessage);
	
	if ( Show == FALSE )
		return;

	for(int i=0 ; i < BytesRecorded; i++)
	{
//		fprintf(logfile,"%02X ", *(ptr+i));
		if ( (i+1) % 16 == 0 )
			;;
//			fprintf(logfile,"\n");
	}
//	fprintf(logfile,"\n\n");
}

#endif