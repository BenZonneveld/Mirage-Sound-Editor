// Resample_Dialog.cpp : implementation file
//

#include "stdafx.h"
#include "../Mirage Editor.h"
#include "Dialog_Resample.h"
#include "../Dsp/samplerate.h"

#define	MAGIC_NUMBER	((int) ('S' << 16) + ('R' << 8) + ('C'))

typedef struct {	
	int			magic;

//	SNDFILE 	*sndfile;
//	SF_INFO 	sfinfo;

	SRC_STATE	*src_state;
	SRC_DATA	src_data;

	int			freq_point;
	int			buffer_out_start, buffer_out_end ;

	float		buffer_in	[4096] ;
	float		buffer_out	[4096] ;
} CALLBACK_DATA;

// Resample_Dialog dialog

IMPLEMENT_DYNAMIC(CResample_Dialog, CDialog)

CResample_Dialog::CResample_Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(CResample_Dialog::IDD, pParent)
{

}

CResample_Dialog::~CResample_Dialog()
{
}

void CResample_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONVERTER, mResampleConverterCombo);
	DDX_Control(pDX, IDC_PAGES_COMBO, m_PagesCombo);
}


BEGIN_MESSAGE_MAP(CResample_Dialog, CDialog)
	ON_BN_CLICKED(IDOK, &CResample_Dialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CResample_Dialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RESAMPLE_PRELISTEN, &CResample_Dialog::OnBnClickedResamplePrelisten)
END_MESSAGE_MAP()


// Resample_Dialog message handlers
BOOL CResample_Dialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	int k,SampleRateConverter,page_sel;
	const char * cptr;
	char pages[5];
	CComboBox	*ConverterType = &mResampleConverterCombo;
	CComboBox	*SamplePages = &m_PagesCombo;

	for (k = 0 ; (cptr = src_get_name (k)) != NULL ; k++)
	{
		ConverterType->AddString(cptr);
	}
	SampleRateConverter = theApp.GetProfileIntA("Settings","SampleRateConverter",0);
	ConverterType->SetCurSel(SampleRateConverter);

	page_sel = 0;
	for (k = 0 ; k <= 0x100 ; k += 0x08 )
	{
		if ( k == 0x100 )
			k = 0xFF;
		if ( (k > m_currentpages || k == m_currentpages )&& m_currentpages != 0 )
		{
			sprintf_s(pages,sizeof(pages),"%02X", m_currentpages);
			SamplePages->AddString(pages);
			SamplePages->SetCurSel(page_sel);

			if ( k > m_currentpages )
			{
				sprintf_s(pages,sizeof(pages),"%02X",k);
				SamplePages->AddString(pages);
			}
			m_currentpages = 0;
		}
		else 
		{
			sprintf_s(pages,sizeof(pages),"%02X",k);
			SamplePages->AddString(pages);
		}
		page_sel++;
	}
	SamplePages->LimitText(2);
	m_resample_cancel_ok = false;
	return true;
}

void CResample_Dialog::OnBnClickedOk()
{
	OnOK();
	char targetpages[3];
	int CurSel;
	CComboBox	*ConverterType = &mResampleConverterCombo;
	CComboBox	*SamplePages = &m_PagesCombo;

	theApp.WriteProfileInt("Settings","SampleRateConverter", ConverterType->GetCurSel());
	CurSel = SamplePages->GetCurSel();
	if ( CurSel == CB_ERR )
	{
		SamplePages->GetWindowText((LPSTR)targetpages,3);
	} else {
		SamplePages->GetLBText(CurSel,(LPSTR)targetpages);
	}
	m_targetpages = unsigned char(strtoul(targetpages, NULL, 16 /* Base 16 */));
	m_resample_cancel_ok = true;
}

void CResample_Dialog::OnBnClickedCancel()
{
	OnCancel();
	m_resample_cancel_ok = false;
}

void CResample_Dialog::OnBnClickedResamplePrelisten()
{
	CComboBox	*ConverterType = &mResampleConverterCombo;
	SRC_DATA	src_data;			// struct to pass audio and control data into the sample rate converter
	CALLBACK_DATA	*data ;
//	AUDIO_OUT		*audio_out ;
	int				error ;

	memcpy((unsigned char *)&src_data,m_lpSrcData,sizeof(src_data));

	/* Allocate memory for the callback data. */
	if ((data = (CALLBACK_DATA *)calloc (1, sizeof (CALLBACK_DATA))) == NULL)
	{	
		return;
	}

	data->magic = MAGIC_NUMBER;

	/* Initialize the sample rate converter. */
	if ((data->src_state = src_new (ConverterType->GetCurSel(), 1, &error)) == NULL)
	{	
		MessageBox(src_strerror(error), NULL, MB_ICONERROR | MB_OK);
		return;
	}

	// TODO: Add your control notification handler code here
}
