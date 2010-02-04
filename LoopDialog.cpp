// LoopDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "Mirage EditorDoc.h"

// For LoopView
#include "LoopDoc.h"
#include "LoopFrm.h"
#include "LoopDialog.h"
#include "LoopView.h"
#include "DFV/DFVCtrl.h"
#include "RepeatButton.h"

HANDLE LoopDialogPlayLoop;
// CLoopDialog dialog

IMPLEMENT_DYNAMIC(CLoopDialog, CDialog)

CLoopDialog::CLoopDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLoopDialog::IDD, pParent)
	//, PictureControl(0)
{
//	m_pView = NULL;
//	m_pPrevParent = NULL;
}

CLoopDialog::~CLoopDialog()
{
}

void CLoopDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOOP_TOGGLE, m_Loop_Toggle);
	DDX_Control(pDX, IDC_END_FINE_F, EndFine_Forward);
	DDX_Control(pDX, IDC_END_FINE_B, EndFine_Back);
	DDX_Control(pDX, IDC_DLG_PLAY_LOOP, m_PlayLoopStatus);
}


BEGIN_MESSAGE_MAP(CLoopDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CLoopDialog::OnLoopClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLoopDialog::OnLoopClickedCancel)
	ON_BN_CLICKED(IDC_END_F, &CLoopDialog::OnBnClickedEndF)
	ON_BN_CLICKED(IDC_END_B, &CLoopDialog::OnBnClickedEndB)
	ON_BN_CLICKED(IDC_END_FINE_F, &CLoopDialog::OnBnClickedEndFineF)
	ON_BN_CLICKED(IDC_END_FINE_B, &CLoopDialog::OnBnClickedEndFineB)
	ON_BN_CLICKED(IDC_START_F, &CLoopDialog::OnBnClickedStartF)
	ON_BN_CLICKED(IDC_START_B, &CLoopDialog::OnBnClickedStartB)
	ON_BN_CLICKED(IDC_PAGE_RESET, &CLoopDialog::OnBnClickedPageReset)
	ON_BN_CLICKED(IDC_LOOP_TOGGLE, &CLoopDialog::OnBnClickedLoopToggle)
	ON_BN_CLICKED(IDC_FORWARD_BACKWARD_LOOP, &CLoopDialog::OnBnClickedForwardBackwardLoop)
	ON_BN_CLICKED(IDC_DLG_PLAY_LOOP, &CLoopDialog::OnBnClickedDlgPlayLoop)
END_MESSAGE_MAP()


// CLoopDialog message handlers
BOOL CLoopDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_DFVCtrl1.Create(this, IDC_IMAGE,
        IDR_MAINFRAME,
        RUNTIME_CLASS(CLoopDoc),
        RUNTIME_CLASS(CLoopFrame),       // main SDI frame window
        RUNTIME_CLASS(CLoopView),
        WS_CHILD | WS_BORDER | WS_VISIBLE, 0L);

	m_bPlayLoop=FALSE;
	m_PlayLoopStatus.SetCheck(BST_UNCHECKED);

	LoopDialogPlayLoop=CreateEvent(NULL,
																	TRUE,
																	FALSE,
																	NULL);

	switch (CLoopView::LoopStatus())
	{
	case true:
		m_Loop_Toggle.SetWindowTextA("Loop On");
		m_Loop_Toggle.UpdateWindow();
		break;
	case false:
		m_Loop_Toggle.SetWindowTextA("Loop Off");
		m_Loop_Toggle.UpdateWindow();
		break;
	}

	m_bPlayLoop = FALSE;
	return TRUE;
}


void CLoopDialog::OnLoopClickedOk()
{
	OnOK();
	ResetEvent(LoopDialogPlayLoop);
	CloseHandle(LoopDialogPlayLoop);
	CLoopView::UseLoop();
}

void CLoopDialog::OnLoopClickedCancel()
{
	ResetEvent(LoopDialogPlayLoop);
	CloseHandle(LoopDialogPlayLoop);
	OnCancel();
}

void CLoopDialog::OnBnClickedEndF()
{
	CLoopView::LoopEndF();
	Invalidate(FALSE);
}

void CLoopDialog::OnBnClickedEndB()
{
	CLoopView::LoopEndB();
	Invalidate(FALSE);
}


void CLoopDialog::OnBnClickedEndFineF()
{
	CLoopView::LoopEndFineF();
	Invalidate(FALSE);
}

void CLoopDialog::OnBnClickedEndFineB()
{
	CLoopView::LoopEndFineB();
	Invalidate(FALSE);
}

void CLoopDialog::OnBnClickedStartF()
{
	CLoopView::LoopStartF();
	Invalidate(FALSE);
}

void CLoopDialog::OnBnClickedStartB()
{
	CLoopView::LoopStartB();
	Invalidate(FALSE);
}

void CLoopDialog::OnBnClickedPageReset()
{
	CLoopView::LoopEndPage();
	Invalidate(FALSE);
}

void CLoopDialog::OnBnClickedLoopToggle()
{
	switch (CLoopView::ToggleLoop())
	{
	case false:
		m_Loop_Toggle.SetWindowTextA("Loop Off");
		break;
	case true:
		m_Loop_Toggle.SetWindowTextA("Loop On");
		break;
	}
}

void CLoopDialog::OnBnClickedForwardBackwardLoop()
{
	CLoopView::AlternateLoop();
	Invalidate(FALSE);
}

void CLoopDialog::OnBnClickedDlgPlayLoop()
{
	switch (m_bPlayLoop)
	{
	case false:
		m_PlayLoopStatus.SetCheck(BST_CHECKED);
		m_bPlayLoop=TRUE;
		SetEvent(LoopDialogPlayLoop);
	  /*m_Thread = */::AfxBeginThread((AFX_THREADPROC)PlayLoop,this,THREAD_PRIORITY_LOWEST);
		break;
	case true:
		m_PlayLoopStatus.SetCheck(BST_UNCHECKED);
		m_bPlayLoop=FALSE;
		ResetEvent(LoopDialogPlayLoop);
		break;
	}
	// TODO: Add your control notification handler code here
}

DWORD CLoopDialog::PlayLoop(LPVOID param)
{
	MMRESULT	mResult;
	HANDLE		hData  = NULL;  // handle of waveform data memory 
	HPSTR		lpData = NULL;  // pointer to waveform data memory 
	HWAVEOUT	hWaveOut; 
	HGLOBAL		hWaveHdr; 
	LPWAVEHDR	lpWaveHdr; 
	UINT		wResult; 
	WAVEFORMAT	*pFormat; 

	MWAV		hWAV;
	_WaveSample_ WaveData;

	pFormat = (WAVEFORMAT *)&CLoopView::m_sWav.waveFormat.fmtFORMAT;

	// Open a waveform device for output using window callback. 

	pFormat->nBlockAlign = 1;

	if ((mResult = waveOutOpen((LPHWAVEOUT)&hWaveOut, WAVE_MAPPER, 
					(LPWAVEFORMATEX)pFormat,
					0L, 0L, CALLBACK_EVENT|WAVE_ALLOWSYNC)) != MMSYSERR_NOERROR )
	{ 
		return 0; 
	} 
 
	// Allocate and lock memory for the header. 

	hWaveHdr = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, 
		(DWORD) sizeof(WAVEHDR)); 
	if (hWaveHdr == NULL) 
	{ 
		return 0; 
	} 
 
	lpWaveHdr = (LPWAVEHDR) GlobalLock(hWaveHdr); 
	if (lpWaveHdr == NULL) 
	{ 
		GlobalUnlock(hData); 
		GlobalFree(hData); 
		return 0; 
	} 
 
	// After allocation, set up and prepare header. 

	while (WaitForSingleObject(LoopDialogPlayLoop,0)==WAIT_OBJECT_0)
	{
		WaveData=CLoopView::m_sWav;
		lpData = (HPSTR)&WaveData.SampleData;
		lpWaveHdr->lpData = lpData+CLoopView::m_LoopStart; 
		lpWaveHdr->dwBufferLength = CLoopView::m_LoopEnd-CLoopView::m_LoopStart; 
		lpWaveHdr->dwFlags = 0; 
		lpWaveHdr->dwLoops = 0L; 
		waveOutPrepareHeader(hWaveOut, lpWaveHdr, sizeof(WAVEHDR)); 

		// Now the data block can be sent to the output device. The 
		// waveOutWrite function returns immediately and waveform 
		// data is sent to the output device in the background. 
	 
		wResult = waveOutWrite(hWaveOut, lpWaveHdr, sizeof(WAVEHDR)); 
		if (wResult != 0) 
		{ 
			waveOutUnprepareHeader(hWaveOut,
															lpWaveHdr, 
															sizeof(WAVEHDR)); 
			GlobalUnlock( hData); 
			GlobalFree(hData);
		}
	}

	waveOutReset(hWaveOut);
	waveOutClose(hWaveOut);
	return 0;
}