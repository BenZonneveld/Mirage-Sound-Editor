// Mirage EditorDoc.cpp : implementation of the CMirageEditorDoc class
// $Id: Mirage\040EditorDoc.cpp,v 1.20 2008/02/10 17:40:26 root Exp $

#include "stdafx.h"
#include "d3dx9.h"
#include "d3d9.h"

#include "Globals.h"
#include "wavapi.h"

#ifdef _MIR_DEBUG_
#include "Mirage Editor.h"
#endif

#include "Mirage EditorDoc.h"
//#include "MirageSysex.h"
#include "wavesamples.h"
#include "CntrItem.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMirageEditorDoc

IMPLEMENT_DYNCREATE(CMirageEditorDoc, COleDocument)

BEGIN_MESSAGE_MAP(CMirageEditorDoc, COleDocument)
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, &COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, &COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, &COleDocument::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, &COleDocument::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, &COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, &COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_PLAY_AUDIO, &CMirageEditorDoc::OnPlayAudio)
	ON_COMMAND(ID_PLAYSND, &CMirageEditorDoc::OnPlayAudio)
	ON_COMMAND(ID_CLOSE_WINDOW, &CMirageEditorDoc::OnCloseWindow)
END_MESSAGE_MAP()

// CMirageEditorDoc construction/destruction

CMirageEditorDoc::CMirageEditorDoc()
{
	m_hWAV= NULL;
	m_sizeDoc = CSize(1,1);     // dummy value to make CScrollView happy
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
	m_PageSkip = 1;
	m_PageMultiplier = 1;
	m_LastMouse.x = -1;
	m_PitchYaw.x = 00; 
	m_PitchYaw.y = -90; // Start with a straight top->down view
	m_pMesh = NULL;
}

CMirageEditorDoc::~CMirageEditorDoc()
{
	if (m_hWAV != NULL)
	{
		::GlobalFree((HGLOBAL) m_hWAV);
	}
}

BOOL CMirageEditorDoc::OnNewDocument()
{
	if (m_hWAV == NULL && theApp.m_AppInit == true )
	{
	//	CheckPoint(); // Save state for undo
		return FALSE;
	}

	if (!COleDocument::OnNewDocument())
	{
		CheckPoint(); // Save state for undo
		return FALSE;
	}
	return TRUE;
}

void CMirageEditorDoc::InitWAVData()
{
	if (m_hWAV == NULL)
	{
		return;
	}
	// Set up document size
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) m_hWAV);
	if (::WAVSize(lpWAV) > MAX_WAVESIZE ||::WAVChannels(lpWAV) > MAX_WAVCHANNELS /* Mirage is Mono Only */) 
	{
		::GlobalUnlock((HGLOBAL) m_hWAV);
		::GlobalFree((HGLOBAL) m_hWAV);
		m_hWAV = NULL;
		CString strMsg;
		strMsg.LoadString(IDS_WAV_NOT_MONO);
		MessageBox(NULL, strMsg, NULL, MB_ICONINFORMATION | MB_OK);
		return;
	}
	m_sizeDoc = CSize((int) ::WAVSize(lpWAV), 1);
	m_ZoomLevel = 1.0;
	m_PageSkip = 1;
	m_startpoint_selected = false;
	m_endpoint_selected = false;
	m_DisplayType = 'W';
	m_ratio = 1.0;
	m_LastMouse.x = -1;
	m_z_offset=100;
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
	m_pMesh = NULL;
	::GlobalUnlock((HGLOBAL) m_hWAV);
}

BOOL CMirageEditorDoc::CreateNewFromMirage(MWAV hWAV)
{
	m_hWAV = hWAV;
	return true;
}

BOOL CMirageEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;
	if (!file.Open(lpszPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		ReportSaveLoadException(lpszPathName, &fe,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		return FALSE;
	}

	DeleteContents();
	BeginWaitCursor();

	TRY
	{
		m_hWAV = ::ReadWAVFile(file);
	}
	CATCH (CFileException, eLoad)
	{
		file.Abort(); // will not throw an exception
		EndWaitCursor();
		ReportSaveLoadException(lpszPathName, eLoad,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
		m_hWAV = NULL;
		return FALSE;
	}
	END_CATCH

	InitWAVData();
	CheckPoint(); // save state for undo
	EndWaitCursor();

	if (m_hWAV == NULL)
	{
		return FALSE;
	}

	SetPathName(lpszPathName);
	SetModifiedFlag(FALSE);     // start off with unmodified
//	CMainFrame* pMainFrame = theApp.GetMainFrame();
//	CToolBar wndSampleTool = pMainFrame->GetTo;
//	wndSampleTool.GetToolBarCtrl().SetState(ID_PLAYSND,TBSTATE_ENABLED);
//	wndSampleTool.GetToolBarCtrl().EnableButton(ID_PLAYSND,TRUE);
	return TRUE;
}

BOOL CMirageEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CFile file;
	CFileException fe;

	if (!file.Open(lpszPathName, CFile::modeCreate |
	  CFile::modeReadWrite | CFile::shareExclusive, &fe))
	{
		ReportSaveLoadException(lpszPathName, &fe,
			TRUE, AFX_IDP_INVALID_FILENAME);
		return FALSE;
	}

	BOOL bSuccess = FALSE;
	TRY
	{
		BeginWaitCursor();
		bSuccess = ::SaveWAV(m_hWAV, file);
		file.Close();
	}
	CATCH (CException, eSave)
	{
		file.Abort(); // will not throw an exception
		EndWaitCursor();
		ReportSaveLoadException(lpszPathName, eSave,
			TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
		return FALSE;
	}
	END_CATCH

	EndWaitCursor();
	SetModifiedFlag(FALSE);     // back to unmodified

	if (!bSuccess)
	{
		// may be other-style WAV (load supported but not save)
		//  or other problem in SaveWAV
		CString strMsg;
		strMsg.LoadString(IDS_CANNOT_SAVE_WAV);
		MessageBox(NULL, strMsg, NULL, MB_ICONINFORMATION | MB_OK);
	}

	return bSuccess;
}

// CMirageEditorDoc serialization

void CMirageEditorDoc::Serialize(CArchive& ar)
{
	_WaveSample_ sWav;
	MWAV hWAV = GetMWAV();
		
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) hWAV);
	memcpy((unsigned char *)&sWav,lpWAV,sizeof(sWav));

	COleDocument::Serialize(ar);
	if (ar.IsStoring())
	{
		ar.Write(/*reinterpret_cast< char *>(&sWav)*/ lpWAV, sizeof(sWav));
	}
	else
	{
		ar.Read(lpWAV,sizeof(sWav));
		// TODO: add loading code here
	}
	// Calling the base class COleDocument enables serialization
	//  of the container document's COleClientItem objects.
}

void CMirageEditorDoc::DeleteContents()
{
	COleDocument::DeleteContents();
}

// CMirageEditorDoc diagnostics

#ifdef _DEBUG
void CMirageEditorDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CMirageEditorDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG


// CMirageEditorDoc commands

void CMirageEditorDoc::OnPlayAudio()
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
}

void CMirageEditorDoc::ZoomInc()
{
	m_ZoomLevel += 0.01; 
	if ( m_ZoomLevel > 1 )
		m_ZoomLevel = 1;

	m_PageSkip=m_PageSkip/2;
	if ( m_PageSkip < 1 )
		m_PageSkip=1;

	m_z_offset++;
	if ( m_z_offset > 1000 )
		m_z_offset=1000;
}

void CMirageEditorDoc::ZoomIncTen()
{
	m_ZoomLevel += 0.1; 
	if ( m_ZoomLevel > 1 )
		m_ZoomLevel = 1;

	m_PageSkip=m_PageSkip/2;
	if ( m_PageSkip < 1 )
		m_PageSkip=1;

	m_z_offset += 10;
	if ( m_z_offset > 1000 )
		m_z_offset=1000;
}

void CMirageEditorDoc::ZoomDec()
{
	m_ZoomLevel -= 0.01;
	if ( m_ZoomLevel < 0.01 )
		m_ZoomLevel = 0.01;

	m_PageSkip=m_PageSkip*2;
	if ( m_PageSkip > 16 )
		m_PageSkip=16;

	m_z_offset--;
	if ( m_z_offset < 100 )
		m_z_offset=100;
}

void CMirageEditorDoc::ZoomDecTen()
{
	m_ZoomLevel -= .1;
	if ( m_ZoomLevel < 0.01 )
		m_ZoomLevel = 0.01;

	m_PageSkip=m_PageSkip*2;
	if ( m_PageSkip > 16 )
		m_PageSkip=16;

	m_z_offset -= 10;
	if ( m_z_offset < 100 )
		m_z_offset=100;
}

void CMirageEditorDoc::ResetZoom()
{
	m_ZoomLevel = 1.0;
}

void CMirageEditorDoc::RatioInc()
{
	m_ratio += 1.0/1000;
}

void CMirageEditorDoc::RatioDec()
{
	m_ratio -= 1.0/1000;
}

void CMirageEditorDoc::SetFromMirage()
{
	m_FromMirage = true;
}

void CMirageEditorDoc::NotFromMirage()
{
	m_FromMirage = false;
}

bool CMirageEditorDoc::DisplayTypeWavedraw()
{
	m_DisplayType = 'W';
	return 1;
}

bool CMirageEditorDoc::DisplayType3DTypeA()
{
	m_DisplayType = 'A';
	return 1;
}

bool CMirageEditorDoc::DisplayType3DTypeB()
{
	m_DisplayType = 'B';
	return 1;
}

void CMirageEditorDoc::OnCloseWindow()
{
	if (m_hWAV != NULL)
	{
		::GlobalFree((HGLOBAL) m_hWAV);
	}

	COleDocument::OnCloseDocument();
}

void CMirageEditorDoc::SetpD3D(LPDIRECT3D9 pD3D)
{
	m_pD3D=(LONG_PTR)pD3D;
}

void CMirageEditorDoc::SetpD3DDevice(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice=(LONG_PTR)pD3DDevice;
}

void CMirageEditorDoc::SetMesh(LPD3DXMESH pMesh)
{
	m_pMesh = (LONG_PTR)pMesh;
}

void CMirageEditorDoc::SetPageMultiplier(UINT Multiplier)
{
	m_PageMultiplier = Multiplier;
}

void CMirageEditorDoc::SetPitchYaw(CPoint point)
{
	if ( m_LastMouse.x == -1 )
		m_LastMouse = point;

	m_PitchYaw.x -= (point.x - m_LastMouse.x);
    m_PitchYaw.y -= (point.y - m_LastMouse.y);
    
	m_LastMouse=point;
}

void CMirageEditorDoc::SetLastMouse(CPoint point)
{
	m_LastMouse.x = point.x;
	m_LastMouse.y = point.y;
}