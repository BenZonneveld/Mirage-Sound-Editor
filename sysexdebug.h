#include <mmsystem.h>

#ifdef NDEBUG
void sysexdump(unsigned char *sysEx, char *Mode);
void sysexerror(const unsigned char * ptr,DWORD BytesRecorded,char *Mode);
#define	CONFIG_PARM_REQ		0x00
#define	COMMAND_CODE		0x01
#define	CONFIG_PARM_DUMP	0x02
#define	LOWER_PRG_DUMP_REQ	0x03
#define UPPER_PRG_DUMP_REQ	0x13
#define WAVE_DUMP_REQ		0x04
#define	PRG_DUMP_LOWER		0x05
#define PRG_DUMP_UPPER		0x15
#define	WAVE_DUMP_DATA		0x06
#define	PRG_STATUS_MSG		0x07
#define	WAVE_STATUS_MSG		0x08
#define WAVE_ACK			0x09
#define	WAVE_NACK			0x0A

#define SELECT_LOWER		0x15
#define SELECT_UPPER		0x14
#endif