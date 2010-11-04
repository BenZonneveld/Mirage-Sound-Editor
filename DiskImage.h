/* This file describes the Mirage Disk Image Format
 *
 * Track layout:
 * 5 sectors of 1024 bytes followed by
 * 1 sector of 512 bytes
 */
//#include "StdAfx.h"
#include "MirageSysex.h"

#ifndef _DISK_IMAGE_
#define _DISK_IMAGE_

class CDiskImage
{
private:
	_program_dump_table_ m_ProgramDumpTable[6];
	char m_MirageOS[16384];
	char unsigned FullBank[65536];
	CFile m_file;
	LONGLONG GotoTrack(unsigned int track)
		{ return (track*5632); }
	void CreateRiffHeader(int bank,int SampleNumber,int SampleStart,int SampleLength);
	BOOL IsValidWave(int bank, int SampleNumber);

public:
	CDiskImage(void);
	~CDiskImage(void);

	BOOL SetFile(LPCTSTR lpszPathName);
	int	GetImageType();
	// Read all of the OS Sectors */
	void ReadOS(void);
	void ReadSysParam(void);
	void ReadBankParams(int bank);
	void ReadFullBank(int bank);
	void ReadGenericImage(void);
};

#endif