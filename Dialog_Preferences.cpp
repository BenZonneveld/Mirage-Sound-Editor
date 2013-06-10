// Preferences.cpp : implementation file
//
/* $Id: Preferences.cpp,v 1.3 2008/01/04 10:40:21 root Exp $ */

#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "afxwin.h"

#include <windows.h>
//#include <mmsystem.h>
#include "Mirage Editor.h"
#include "Dialog_Preferences.h"
#include "MidiWrapper/MIDIInDevice.h"
#include "MidiWrapper/MIDIOutDevice.h"

// CPreferences dialog

IMPLEMENT_DYNAMIC(CPreferences, CDialog)

CPreferences::CPreferences(CWnd* pParent /*=NULL*/)
	: CDialog(CPreferences::IDD, pParent)
{

}

CPreferences::~CPreferences()
{
}

void CPreferences::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MIDI_OUTPORT, PrefsOutCombo);
	DDX_Control(pDX, IDC_COMBO_MIDI_INPORT, PrefsInCombo);
	DDX_Control(pDX, IDC_DO_RESAMPLING, mDoResampling);
	DDX_Control(pDX, IDC_STEREO2MONO, mStereoToMono);
	DDX_Control(pDX, IDC_CHECK_UPDATES, mUpdateCheck);
}



BEGIN_MESSAGE_MAP(CPreferences, CDialog)
	ON_BN_CLICKED(IDOK, &CPreferences::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CPreferences::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_DO_RESAMPLING, &CPreferences::OnBnClickedDoResampling)
	ON_BN_CLICKED(IDC_STEREO2MONO, &CPreferences::OnBnClickedStereo2mono)
END_MESSAGE_MAP()


// CPreferences message handlers

void CPreferences::OnBnClickedOk()
{
	CComboBox	*OutCombo = &PrefsOutCombo;
	CComboBox	*InCombo = &PrefsInCombo;
	CButton		*Resampling = &mDoResampling;
	CButton		*Stereo2Mono = &mStereoToMono;
	CButton		*CheckUpdates = &mUpdateCheck;
	MIDIOUTCAPS		moutCaps;
	MIDIINCAPS		minCaps;

	midi::CMIDIOutDevice::GetDevCaps(OutCombo->GetCurSel(),moutCaps);
	midi::CMIDIInDevice::GetDevCaps(InCombo->GetCurSel(), minCaps);
	theApp.WriteProfileStringA("Settings","OutPort", (LPCTSTR)moutCaps.szPname);
	theApp.WriteProfileStringA("Settings","InPort", (LPCTSTR)minCaps.szPname);
	theApp.WriteProfileInt("Settings","DoResampling", Resampling->GetCheck());
	theApp.WriteProfileInt("Settings","Stereo To Mono", Stereo2Mono->GetCheck());
	theApp.WriteProfileInt("Settings","AutoCheckForUpdates", CheckUpdates->GetCheck());
	theApp.m_InDevice.Close();
	theApp.StartMidiInput();

	CDialog::OnOK();
}

void CPreferences::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CPreferences::OnInitDialog()
{
	CDialog::OnInitDialog();

	UINT			outDevs;
	UINT			RegInPort;
	UINT			RegOutPort;
	UINT			inDevs;
	UINT			idx;
//	MyReceiver		Receiver;
	midi::CMIDIInDevice	InDevice;
	midi::CMIDIOutDevice OutDevice;
  MIDIOUTCAPS		moutCaps;
	MIDIINCAPS		minCaps;

	CComboBox	*OutCombo = &PrefsOutCombo;
	CComboBox	*InCombo = &PrefsInCombo;
	CButton		*Resampling = &mDoResampling;
	CButton		*Stereo2Mono = &mStereoToMono;
	CButton		*CheckUpdates = &mUpdateCheck;

	outDevs = midi::CMIDIOutDevice::GetNumDevs();
	inDevs = midi::CMIDIInDevice::GetNumDevs();

	// For the midi out devices
    for (idx = 0; idx < outDevs; idx++)
    {
		midi::CMIDIOutDevice::GetDevCaps(idx,moutCaps);
		OutCombo->AddString(moutCaps.szPname);
    }
	
	// For the midi in devices
	for (idx = 0 ; idx < inDevs ; idx++)
	{
		midi::CMIDIInDevice::GetDevCaps(idx, minCaps);
		InCombo->AddString(minCaps.szPname);
	}

	// Now set the current values from the registry
	RegOutPort = midi::CMIDIOutDevice::GetIDFromName(theApp.GetProfileStringA("Settings","OutPort","not connected"));
	if ( RegOutPort > outDevs )
		RegOutPort = 0;
	RegInPort = midi::CMIDIInDevice::GetIDFromName(theApp.GetProfileStringA("Settings","InPort","not connected"));
	if ( RegInPort > inDevs )
		RegInPort = 0;

	Resampling->SetCheck(theApp.GetProfileIntA("Settings","DoResampling",true));
	Stereo2Mono->SetCheck(theApp.GetProfileIntA("Settings","Stereo To Mono",true));
	CheckUpdates->SetCheck(theApp.GetProfileIntA("Settings","AutoCheckForUpdates",true));
	OutCombo->SetCurSel(RegOutPort);
	InCombo->SetCurSel(RegInPort);

	return true;
}

void CPreferences::OnBnClickedDoResampling()
{
	// TODO: Add your control notification handler code here
}

void CPreferences::OnBnClickedStereo2mono()
{
	// TODO: Add your control notification handler code here
}
