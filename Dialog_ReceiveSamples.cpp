// ReceiveSamples.cpp : implementation file
// $Id: ReceiveSamples.cpp,v 1.12 2008/02/23 23:58:52 root Exp $

#include "stdafx.h"
#include "Globals.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "Dialog_ReceiveSamples.h"
#include "SendSysex.h"
#include "SysexParser.h"
#include "MirageSysex.h"
#include "Mirage Helpers.h"
#include "Dialog_ProgressBar.h"
//#include "Midi View.h"

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
	CDialog::OnInitDialog();
	return true;
}

BOOL CReceiveSamples::UpdateSampleData()
{
	unsigned char	bank;
	unsigned char	wavesample;

	theApp.m_InDevice.Close();
	Sleep(10);
	theApp.StartMidiInput();
	Sleep(10);

	if (!GetConfigParms())
	{
		OnCancel();
		return false;
	}
	
	GetAvailableSamples();

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
	if (UpdateData(true))
	{
		char *sysexconstruct = NULL;
		int i,c=0;

		CListBox *LowerList = &m_LowerList;
		CListBox *UpperList = &m_UpperList;

		/* First get the selection(s) from the listboxes */
		for(i = 0 ; i < LowerList->GetCount(); i++)
		{
			if(LowerList->GetSel(i) > 0)
			{
				theApp.m_LowerSelectList.resize(theApp.m_LowerSelectList.size() + 1);
				theApp.m_LowerSelectList[c]=i;
				c++;
			}
		}

		c=0; // Reset the vector index
		for(i = 0 ; i < UpperList->GetCount(); i++)
		{
			if(UpperList->GetSel(i) > 0)
			{
				theApp.m_UpperSelectList.resize(theApp.m_UpperSelectList.size() + 1);
				theApp.m_UpperSelectList[c]=i;
				c++;
			}
		}
		theApp.PostThreadMessage(WM_GETSAMPLES, 0 ,0 );
	}
	m_LowerList.ResetContent();
	m_UpperList.ResetContent();
//	CDialog::OnOK();
	DestroyWindow();
}

void CReceiveSamples::OnBnClickedReceiveAbort()
{
	m_LowerList.ResetContent();
	m_UpperList.ResetContent();
	OnCancel();
}

BOOL CReceiveSamples::Create( UINT nId,  CWnd* pWnd)
{
	return CDialog::Create(nId, pWnd);
}