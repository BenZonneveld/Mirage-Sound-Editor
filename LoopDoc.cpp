// Mirage EditorDoc.cpp : implementation of the CLoopDoc class
//

#include "stdafx.h"
//#include "wavapi.h"
#include "Mirage Editor.h"

#include "Dialog_LoopEdit.h"
#include "LoopDoc.h"
//#include "MirageSysex.h"
//#include "wavesamples.h"
//#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLoopDoc

IMPLEMENT_DYNCREATE(CLoopDoc, CDFVDocument)

BEGIN_MESSAGE_MAP(CLoopDoc, CDFVDocument)
	// Enable default OLE container implementation
//	ON_COMMAND(ID_PLAY_AUDIO, &CLoopDoc::OnPlayAudio)
END_MESSAGE_MAP()


// CLoopDoc construction/destruction

CLoopDoc::CLoopDoc()
{
//	m_hWAV= pWAV;
//	m_sizeDoc = CSize(1,1);     // dummy value to make CScrollView happy
}

CLoopDoc::~CLoopDoc()
{
}

/*
void CLoopDoc::InitWAVData()
{
	if (m_hWAV == NULL)
	{
		return;
	}
	// Set up document size
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) m_hWAV);
	if (::WAVSize(lpWAV) > MAX_WAVESIZE ||::WAVChannels(lpWAV) > MAX_WAVCHANNELS /* Mirage is Mono Only *///) 
/*	{
		::GlobalUnlock((HGLOBAL) m_hWAV);
		::GlobalFree((HGLOBAL) m_hWAV);
		m_hWAV = NULL;
		CString strMsg;
		strMsg.LoadString(IDS_WAV_NOT_MONO);
		MessageBox(NULL, strMsg, NULL, MB_ICONINFORMATION | MB_OK);
		return;
	}
	m_sizeDoc = CSize((int) ::WAVSize(lpWAV), 256);
	::GlobalUnlock((HGLOBAL) m_hWAV);
}*/

// CLoopDoc diagnostics

#ifdef _DEBUG
void CLoopDoc::AssertValid() const
{
	CDFVDocument::AssertValid();
}

void CLoopDoc::Dump(CDumpContext& dc) const
{
	CDFVDocument::Dump(dc);
}
#endif //_DEBUG

// CLoopDoc commands

/*void CLoopDoc::OnPlayAudio()
{
	// Get The selected Wave
	MWAV hWAV = GetMWAV();
	if (hWAV != NULL)
	{
		_WaveSample_ sWav;
		
		LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	
		memcpy((unsigned char *)&sWav,lpWAV,sizeof(sWav.riff_header));
		memcpy((unsigned char *)&sWav,lpWAV,sWav.riff_header.riffSIZE+8);
		PlayWaveData(sWav);
	}
}*/
