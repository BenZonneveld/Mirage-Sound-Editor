#include "StdAfx.h"
#include "MirageSysex.h"
#include "Mirage Editor.h"
#include "Wave Doc.h"
#include "wavesamples.h"
#include "DiskImage.h"
#include "Dialog_MemLayout.h"

CDiskImage::CDiskImage(void)
{
}

CDiskImage::~CDiskImage(void)
{
	if ( m_file.m_hFile != CFile::hFileNull )
		m_file.Close();
}

unsigned int CDiskImage::GotoSector(unsigned int sector_number)
{
	unsigned int sector_offset = 0;

	if ( sector_number < 6 )
		sector_offset = sector_number * 1024;
	
/*	if ( sector_number == 5 ) // Sector 5 is only 512 bytes
		sector_offset = 4.5 * 1024;
*/
	return (sector_offset);
}

BOOL CDiskImage::SetFile(LPCTSTR lpszPathName)
{
	if ( m_file.m_hFile != CFile::hFileNull )
		m_file.Close();
	CFileException fe;
	if (!m_file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
/*		ReportSaveLoadException(lpszPathName, &fe,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);*/
		return FALSE;
	}
	return TRUE;
}

int CDiskImage::GetImageType()
{
	CMirageMemLayout MemLayout;

	if ( m_file.GetLength() == 450560 )
	{
		ReadOS();
		ReadSysParam();
//		int c=0;
		for (int c=0; c < LoadBank.size(); c++)
		{
			ReadBankParams(LoadBank[c]);
			ReadBankParams(LoadBank[c]+1);
#ifdef _DEBUG
			MemLayout.DoModal();
#endif
			ReadFullBank(LoadBank[c]);
			ReadFullBank(LoadBank[c]+1);
		}
		LoadBank.clear();
		return (1); // Generic Disk Image
	}
	return (2); // Unknown Image Type
}

void CDiskImage::ReadOS(void)
{
	// The OS Starts on track 0
	int track_pointer=0;
	UINT BytesRead = 0;

	m_file.SeekToBegin();
	BytesRead=m_file.Read(m_MirageOS,11*1024); // Read the first two tracks

	for (track_pointer = 2 ; track_pointer < 11 ; track_pointer++)
	{
		m_file.Seek(track_pointer*(5*1024), CFile::begin);
		BytesRead=m_file.Read(&m_MirageOS[BytesRead*(track_pointer-2)],512);
	}
}

void CDiskImage::ReadSysParam(void)
{
	// System Parameters are on track 11, sector 5
	m_file.Seek(GotoTrack(11)+GotoSector(5),CFile::begin);
	m_file.Read(&m_ConfigDumpTable,sizeof(m_ConfigDumpTable));
	memcpy(&ConfigDump,&m_ConfigDumpTable,sizeof(m_ConfigDumpTable));
}

void CDiskImage::ReadBankParams(int bank)
{
	m_file.Seek(GotoTrack(2)+bank*(13*5632),CFile::begin);
	m_file.Read(&m_ProgramDumpTable[bank % 2],624);
	memcpy(&ProgramDumpTable[bank % 2],&m_ProgramDumpTable[bank % 2],sizeof(ProgramDumpTable[bank % 2]));
}

void CDiskImage::ReadFullBank(int bank)
{
	int counter=0;
	int SampleStart=0;
	int SampleLength=0;
	int LoopStart=0;
	int LoopEnd=0;
	int n=0;
	MWAV hWAV;
	LPSTR pWAV;

	char titlestring[20];

#ifdef _DEBUG
	CFile rawwave;
	char filename[20];
#endif

// Read the Full Bankdata
	m_file.Seek(GotoTrack(2)+1024+bank*(13*5632),CFile::begin);
	m_file.Read(&FullBank,(4*1024));
	counter=counter+(4*1024);
	m_file.Seek(512,CFile::current);

	while ( counter < 65536 )
	{
		m_file.Read(&FullBank[counter],(5*1024));
		counter=counter+(5*1024);
		m_file.Seek(512,CFile::current);
	}

	for(n=0; n<8;n++)
	{
		if (IsValidWave(bank % 2,n) == FALSE )
			continue;
	  SampleStart = MIRAGE_PAGESIZE * m_ProgramDumpTable[bank % 2].WaveSampleControlBlock[n].SampleStart;
		SampleLength = MIRAGE_PAGESIZE * ((m_ProgramDumpTable[bank % 2].WaveSampleControlBlock[n].SampleEnd+1) - 
								   m_ProgramDumpTable[bank % 2].WaveSampleControlBlock[n].SampleStart);
		LoopStart = MIRAGE_PAGESIZE * m_ProgramDumpTable[bank % 2].WaveSampleControlBlock[n].LoopStart;
		LoopEnd = (MIRAGE_PAGESIZE * m_ProgramDumpTable[bank % 2].WaveSampleControlBlock[n].LoopEnd) + 
							m_ProgramDumpTable[bank % 2].WaveSampleControlBlock[n].LoopEndFine;

		CDiskImage::CreateRiffHeader(bank % 2,n,SampleStart,SampleLength);

		if ( (bank%2) == 0 )
		{
			sprintf_s(titlestring,sizeof(titlestring),"LowerBank %d Wave %d", (bank/2)+1,n+1);
		}
		else
		{
			sprintf_s(titlestring,sizeof(titlestring),"UpperBank %d Wave %d", (bank/2)+1,n+1);
		}

		CMirageEditorDoc* pDoc=NULL; 
		CMirageEditorApp* pApp = (CMirageEditorApp*)AfxGetApp();
		pDoc = (CMirageEditorDoc *)pApp->m_pDocTemplate->OpenDocumentFile(NULL);
		pDoc->SetTitle(titlestring);

		hWAV = (MWAV) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(WaveSample));
		if (hWAV == 0)
		{
			return;
		}
		pWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);

		memcpy(pWAV,(unsigned char *)&WaveSample,sizeof(WaveSample));
//		memcpy(pWAV,(unsigned char *)&FullBank[SampleStart],SampleLength);

		pDoc->CreateNewFromMirage(hWAV);
		::GlobalUnlock((HGLOBAL) hWAV);

#ifdef _DEBUG
		sprintf(filename,"bank_%i_wave_%i.raw",bank,n);
		rawwave.Open(LPCTSTR(&filename), CFile::modeCreate|CFile::modeWrite);
		rawwave.Write(&FullBank[SampleStart],SampleLength);
		rawwave.Close();
#endif
	}
}

void CDiskImage::CreateRiffHeader(int bank,int SampleNumber,int SampleStart,int SampleLength)
{
	if ( SampleLength > 0xFFFF )
		SampleLength = 0xFFFF;

	// Create RIFF Header
	memcpy(WaveSample.riff_header.riffID,"RIFF",4);
	WaveSample.riff_header.riffSIZE = sizeof(_riff_)+sizeof(_fmt_)+sizeof(_sampler_)+sizeof(_instrument_)+SampleLength;
	memcpy(WaveSample.riff_header.riffFORMAT,"WAVE",4);

	// Create Format Header
	memcpy(WaveSample.waveFormat.fmtID,"fmt ",4);
	WaveSample.waveFormat.fmtSIZE = 16;
	WaveSample.waveFormat.fmtFORMAT.wFormatTag = 1; 
	WaveSample.waveFormat.fmtFORMAT.nChannels = 1; // Mono Samples
	WaveSample.waveFormat.fmtFORMAT.wBitsPerSample = 8; // 8 Bit Samples
	WaveSample.waveFormat.fmtFORMAT.nSamplesPerSec = 30000; // 30 Khz
	WaveSample.waveFormat.fmtFORMAT.nAvgBytesPerSec = 30000;
	WaveSample.waveFormat.fmtFORMAT.nBlockAlign = 256;

	// Create Sampler Header
	memcpy(WaveSample.sampler.samplerID,"smpl",4);
	WaveSample.sampler.samplerSize = 60; // Size of the chunk is 60 bytes
	WaveSample.sampler.Manufacturer = 0x0F; // For Ensoniq
	WaveSample.sampler.Product = 0x01; // For Mirage
	WaveSample.sampler.SamplePeriod = (DWORD)floor((double)1e9 / (double)WaveSample.waveFormat.fmtFORMAT.nSamplesPerSec);
	WaveSample.sampler.MIDIUnityNote = 60;
	WaveSample.sampler.MIDIPitchFraction = 0;
	WaveSample.sampler.SMPTEFormat = 0; // Mirage does not use SMPTE offsets
	WaveSample.sampler.SMPTEOffset = 0; // No SMPTE offset for the Mirage
	WaveSample.sampler.cbSamplerData = 0; 
	WaveSample.sampler.cSampleLoops = 1; // The Mirage always has loop points set
	// Now get the loop for this wavesample
	WaveSample.sampler.Loops.dwIdentifier = 0;
	WaveSample.sampler.Loops.dwType = 0;
	WaveSample.sampler.Loops.dwStart = ((m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopStart -																				m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleStart) * MIRAGE_PAGESIZE); 
	WaveSample.sampler.Loops.dwEnd = ((m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopEnd -											m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleStart) * MIRAGE_PAGESIZE) + m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopEndFine;

	WaveSample.sampler.Loops.dwPlayCount = 1 - m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].LoopSwitch;
	WaveSample.sampler.Loops.dwFraction = 0;

	// Create the instrument chunk
	memcpy(WaveSample.instrument.chunk_id,"inst",4);
	WaveSample.instrument.inst_size = 8; // Size of the chunk is 8 bytes
	WaveSample.instrument.unshifted_note = WaveSample.sampler.MIDIUnityNote;
	WaveSample.instrument.fine_tune = 0;
	WaveSample.instrument.gain = 0;
	WaveSample.instrument.low_note = 0; 
	WaveSample.instrument.high_note = 0x7F;
	WaveSample.instrument.low_velocity = 0x01;
	WaveSample.instrument.high_velocity = 0x7F;
	WaveSample.instrument.padding = 0;

	// Create Data Chunk
	memcpy(WaveSample.data_header.dataID,"data",4);
	
	WaveSample.data_header.dataSIZE = SampleLength;
	WaveSample.data_header.dataSIZE = WaveSample.data_header.dataSIZE;

	memcpy(WaveSample.SampleData,(unsigned char *)&FullBank[SampleStart],SampleLength);
//	RemoveZeroSamples(&WaveSample);
}

void CDiskImage::ReadGenericImage(void)
{
}

BOOL CDiskImage::IsValidWave(int bank, int SampleNumber)
{
//	return TRUE;
	byte ProgramTopKey=0;
	byte TopKey=m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].TopKey;
	byte Start=m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleStart;
	byte End=m_ProgramDumpTable[bank].WaveSampleControlBlock[SampleNumber].SampleEnd;
	if ( End < Start )
		return FALSE;

	// TODO:This check should be done beter!
	// Detect the Topkey for the Initial WaveSample in each program
/*	for ( int p=0; p < 4 ; p++)
	{
		ProgramTopKey=m_ProgramDumpTable[bank].WaveSampleControlBlock[m_ProgramDumpTable[bank].ProgramParameterBlock[p].InitialWavesample].TopKey;
		if ( m_ProgramDumpTable[bank].ProgramParameterBlock[p].InitialWavesample >= SampleNumber &&
			m_ProgramDumpTable[bank].WaveSampleControlBlock[m_ProgramDumpTable[bank].ProgramParameterBlock[p].InitialWavesample].TopKey >= ProgramTopKey )
		{
			return TRUE;
		}
	}
	*/
	return TRUE;
//	return FALSE;
}