; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CRotaryFaderMFCDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "RotaryFaderMFC.h"

ClassCount=3
Class1=CRotaryFaderMFCApp
Class2=CRotaryFaderMFCDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_ROTARYFADERMFC_DIALOG

[CLS:CRotaryFaderMFCApp]
Type=0
HeaderFile=RotaryFaderMFC.h
ImplementationFile=RotaryFaderMFC.cpp
Filter=N

[CLS:CRotaryFaderMFCDlg]
Type=0
HeaderFile=RotaryFaderMFCDlg.h
ImplementationFile=RotaryFaderMFCDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BTN_FLIPKNOB

[CLS:CAboutDlg]
Type=0
HeaderFile=RotaryFaderMFCDlg.h
ImplementationFile=RotaryFaderMFCDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_ROTARYFADERMFC_DIALOG]
Type=1
Class=CRotaryFaderMFCDlg
ControlCount=4
Control1=IDC_SLIDER1,msctls_trackbar32,1342242840
Control2=IDC_BTN_FLIPSCALE,button,1342275584
Control3=IDC_BTN_FLIPKNOB,button,1342275584
Control4=IDC_BTN_FLIPDOT,button,1342275584

