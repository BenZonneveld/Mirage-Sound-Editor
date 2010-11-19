// TransmitSamples.cpp : implementation file
// $Id: TransmitSamples.cpp,v 1.4 2008/02/23 23:58:51 root Exp $

#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "wavesamples.h"

#include "Dialog_TransmitSamples.h"
#include "SendSysex.h"
#include "MirageSysex.h"
#include "Mirage Helpers.h"
#include "Dialog_ProgressBar.h"
#include "Mirage EditorDoc.h"
#include "Mirage Helpers.h"
#include "float_cast.h"
#include "MainFrm.h"
#include "Sysex.h"

// CTransmitSamples dialog

IMPLEMENT_DYNAMIC(CTransmitSamples, CDialog)

CTransmitSamples::CTransmitSamples(CWnd* pParent /*=NULL*/)
	: CDialog(CTransmitSamples::IDD, pParent)
{

}

CTransmitSamples::~CTransmitSamples()
{
}

void CTransmitSamples::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOWER, m_LowerList);
}


BEGIN_MESSAGE_MAP(CTransmitSamples, CDialog)
	ON_BN_CLICKED(IDC_TRANSMIT_SAMPLES, &CTransmitSamples::OnBnClickedTransmitSamples)
	ON_BN_CLICKED(IDC_TRANSMIT_ABORT, &CTransmitSamples::OnBnClickedTransmitAbort)
END_MESSAGE_MAP()


// CTransmitSamples message handlers
BOOL CTransmitSamples::OnInitDialog()
{
	unsigned char	bank;
	unsigned char	wavesample;
	unsigned char	TransmitSamplePages;
	char liststring[26];
	char *bankname = NULL;
	_WaveSample_	*pWav;

	MWAV hWAV = theApp.m_CurrentDoc->GetMWAV();
	if (hWAV == NULL)
	{
		return false;
	}
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) hWAV);

	TransmitSamplePages = GetNumberOfPages(pWav) - 1;
	
	CDialog::OnInitDialog();

	CListBox *LowerList = &m_LowerList;

	for(bank = 0; bank < 2 ; bank++)
	{
		for(wavesample = 0; wavesample < 8; wavesample++)
		{
			/* Check if sample fits in the destination */
			if ( CheckForValidTransmitSample(bank,wavesample) >= TransmitSamplePages )
			{
				switch(bank)
				{
				case 0:
						bankname = "Lower";
						break;
				case 1:
						bankname = "Upper";
						break;
				}
				sprintf_s(liststring,sizeof(liststring),"%s Sample %d (%02X Pages)",
					bankname,
					(wavesample+1),
					ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleEnd - ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleStart);

				LowerList->AddString((CString)liststring);
			}
		}
	}
	return true;
}

void CTransmitSamples::OnBnClickedTransmitSamples()
{	
	bool LoopOnly = theApp.m_CurrentDoc->GetLoopOnly();
	char *sysexconstruct = NULL;
	char bankname[26];
	int i;

	OnOK();
	CListBox *LowerList = &m_LowerList;

	/* First get the selection(s) from the listbox */
	for(i = 0 ; i< LowerList->GetCount(); i++)
	{
		if(LowerList->GetSel(i))
		{
			LowerList->GetText(i,bankname);

			if (strncmp(bankname, "Lower",5) == 0)
			{
				/* Construct the select lower sample front pannel command */
 				unsigned char SelectSample[]={7,
											MirID[0],
											MirID[1],
											MirID[2],
											0x01, // Commando Code
											0x15, // Lower Sample Select
											0x7F,
											0xF7}; // Select Lower Sample

				PutSample(SelectSample,i, LoopOnly);
			} else {
				/* Construct the select upper sample front pannel command */
				unsigned char SelectSample[]={7,
											MirID[0],
											MirID[1],
											MirID[2],
											0x01, // Commando Code
											0x14, // Upper Sample Select
											0x7F,
											0xF7}; // Select Upper Sample

				PutSample(SelectSample,i, LoopOnly);
			}
		}
	}
}

void CTransmitSamples::OnBnClickedTransmitAbort()
{
	OnCancel();
}
