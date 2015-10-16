/* $Id: Globals.h,v 1.4 2008/05/05 16:13:25 root Exp $ */
#define _MIR_DEBUG_

//#ifndef _GLOBALS_
//#define _GLOBALS_

#define SYSEXBUFFER		256 //*1024
#define SYSEXOUTBUFFER 128
#define MIDI_TIMEOUT	2500
#define DUMP_DATA		0x55
#define CONFIG_DATA		0x44
#define WAVE_DATA		0x33
#define STATUS_MESSAGE	0x22
#define WAVSTAT_MESSAGE	0x11
#define LONG_TIMEOUT	1
#define SHORT_TIMEOUT	0
#define MIRAGE_PAGESIZE	256
#define MAX_WAVESIZE	0xFFFF
#define MAX_WAVCHANNELS	1
#define CHANNELS		1
#define KEY_TRANSPOSE	21.27
#define FILTSIZE		64
#define BUFFSIZE		256
#define PROGDUMP_TIMEOUT 5000
#define EXTEND			512
#define	Y_SCALE			1000
#define WM_MIDIMONITOR (WM_APP + 1)
#define WM_PARSESYSEX	 (WM_APP + 2)
#define WM_GETSAMPLES	 (WM_APP + 3)
#define WM_PROGRESS		 (WM_APP + 4)
#define WM_WAVESAMPLERECEIVED (WM_APP +5)
/* Sysex Messages */
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
#define WAVEDUMPABSREQ		0x0B
#define WAVEDUMPABSDATA		0x0C
#define PRG_PARM_MSG		0x0D
#define SMP_PARM_MSG		0x0E
#define	WAVEMANIPCMD		0x0F
#define SELECT_LOWER		0x15
#define SELECT_UPPER		0x14

#define NO_MIDI			0x10
#define MIDI_DATA_TIMEOUT 0x11
#define MIDI_EXPECTED_SUCCESS 0x12
#define MANTIS_URL "http://sourceforge.net/apps/mantisbt/ensoniqmirageed/bug_report_page.php"
