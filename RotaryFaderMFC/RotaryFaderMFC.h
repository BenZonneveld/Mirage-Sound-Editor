// RotaryFaderMFC.h : Haupt-Header-Datei f�r die Anwendung ROTARYFADERMFC
//

#if !defined(AFX_ROTARYFADERMFC_H__E2AA282A_11D8_473E_BA0D_35AB0885C1FF__INCLUDED_)
#define AFX_ROTARYFADERMFC_H__E2AA282A_11D8_473E_BA0D_35AB0885C1FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CRotaryFaderMFCApp:
// Siehe RotaryFaderMFC.cpp f�r die Implementierung dieser Klasse
//

class CRotaryFaderMFCApp : public CWinApp
{
public:
	CRotaryFaderMFCApp();

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CRotaryFaderMFCApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CRotaryFaderMFCApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_ROTARYFADERMFC_H__E2AA282A_11D8_473E_BA0D_35AB0885C1FF__INCLUDED_)
