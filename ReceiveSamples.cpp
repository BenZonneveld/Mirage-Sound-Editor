// ReceiveSamples.cpp : implementation file
// $Id: ReceiveSamples.cpp,v 1.12 2008/02/23 23:58:52 root Exp $

#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "ReceiveSamples.h"
#include "MidiReceive.h"
#include "SendSysex.h"
#include "MirageSysex.h"
#include "Mirage Helpers.h"
#include "ProgressBar.h"

// CReceiveSamples dialog

IMPLEMENT_DYNAMIC(CReceiveSamples, CDialog)

CReceiveSamples::CReceiveSamples(CWnd* pParent /*=NULL*/)
	: CDialog(CReceiveSamples::IDD, pParent)
{

}

CReceiveSamples::~CReceiveSamples()
{
}

void CReceiveSamples::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOWER, m_LowerList);
	DDX_Control(pDX, IDC_LIST_UPPER, m_UpperList);
}


BEGIN_MESSAGE_MAP(CReceiveSamples, CDialog)
	ON_BN_CLICKED(IDC_RECEIVE_SAMPLES, &CReceiveSamples::OnBnClickedReceiveSamples)
	ON_BN_CLICKED(IDC_RECEIVE_ABORT, &CReceiveSamples::OnBnClickedReceiveAbort)
END_MESSAGE_MAP()


// CReceiveSamples message handlers
BOOL CReceiveSamples::OnInitDialog()
{
	unsigned char	bank;
	unsigned char	wavesample;
	CDialog::OnInitDialog();

	char liststring[20];
	CListBox *LowerList = &m_LowerList;
	CListBox *UpperList = &m_UpperList;

	for(bank = 0; bank < 2 ; bank++)
	{
		for(wavesample = 0; wavesample < 8; wavesample++)
		{
			if ( CheckForValidWaveSample(bank,wavesample) == true )
			{
				sprintf_s(liststring,sizeof(liststring),"Sample %d (%02X - %02X)",
					(wavesample+1),
					ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleStart,
					ProgramDumpTable[bank].WaveSampleControlBlock[wavesample].SampleEnd);
				switch(bank)
				{
				case 0:	LowerList->AddString((CString)liststring);
						break;
				case 1:	UpperList->AddString((CString)liststring);
						break;
				}
			}
		}
	}
	return true;
}

void CReceiveSamples::OnBnClickedReceiveSamples()
{	
	char *sysexconstruct = NULL;
	int i;

	OnOK();
	// TODO: Add your control notification handler code here
	CListBox *LowerList = &m_LowerList;
	CListBox *UpperList = &m_UpperList;

	/* First get the selection(s) from the listboxes */
	for(i = 0 ; i< LowerList->GetCount(); i++)
	{
		if(LowerList->GetSel(i))
		{
			/* Construct the select sample front pannel command */
 			unsigned char SelectSample[]={8,
											MirID[0],
											MirID[1],
											MirID[2],
											0x01, // Commando Code
											0x15, // Lower Sample Select
											(i+1),
											0x7F,
											0xF7}; // Select Lower Sample
#ifdef _DEBUG_
			fprintf(logfile,"\nLower Sample %d selected\n", (i+1));
#endif
			GetSample(SelectSample);
		}
	}
	for(i = 0 ; i< UpperList->GetCount(); i++)
	{
		if(UpperList->GetSel(i))
		{
			/* Construct the select sample front pannel command */
			unsigned char SelectSample[]={8,
											MirID[0],
											MirID[1],
											MirID[2],
											0x01, // Commando Code
											0x14, // Upper Sample Select
											(i+1),
											0x7F,
											0xF7}; // Select Upper Sample
#ifdef _DEBUG_
			fprintf(logfile,"\nUpper Sample %d selected\n", (i+1));
#endif
			GetSample(SelectSample);
		}
	}
}

void CReceiveSamples::OnBnClickedReceiveAbort()
{
	OnCancel();
}
