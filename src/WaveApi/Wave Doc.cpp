#include "stdafx.h"
#include "d3dx9.h"
#include "d3d9.h"

#include "../Globals.h"
#include "wavapi.h"

#ifdef _MIR_DEBUG_
#include "../Mirage Editor.h"
#endif

#include "Wave Doc.h"
#include "Wave View.h"
//#include "MirageSysex.h"
#include "wavesamples.h"
#include "../Gui/CntrItem.h"
#include "../Gui/MainFrm.h"
#include "../Midi/Mirage Helpers.h"
#include "../Dialogs/Dialog_BankSelect.h"
//#include "DiskImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMirageEditorDoc

IMPLEMENT_DYNCREATE(CMirageEditorDoc, COleDocument)

BEGIN_MESSAGE_MAP(CMirageEditorDoc, COleDocument)
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, &COleDocument::OnUpdateObjectVerbMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, &COleDocument::OnUpdateEditLinksMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, &COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, &COleDocument::OnEditConvert)
	ON_COMMAND(ID_OLE_EDIT_LINKS, &COleDocument::OnEditLinks)
	ON_COMMAND(ID_PLAY_AUDIO, &CMirageEditorDoc::OnPlayAudio)
	ON_COMMAND(ID_PLAYSND, &CMirageEditorDoc::OnPlayAudio)
	ON_COMMAND(ID_CLOSE_WINDOW, &CMirageEditorDoc::OnCloseWindow)
END_MESSAGE_MAP()

// CMirageEditorDoc construction/destruction

CMirageEditorDoc::CMirageEditorDoc()
{
	m_hWAV= NULL;
	m_sizeDoc = CSize(1,1);     // dummy value to make CScrollView happy
	m_ZoomLevel = 1.0;
	m_startpoint_selected = false;
	m_endpoint_selected = false;
	SelectionStart = -1;
	SelectionEnd = -1;
	m_DisplayType = 'w';
	m_ratio = 1.0;
	m_z_offset=100;
	m_pD3DDevice = NULL;
	m_PageSkip = 1;
	m_PageMultiplier = 1;
	m_LastMouse.x = -1;
	m_PitchYaw.x = 00;
	m_PitchYaw.y = -90; // Start with a straight top->down view
	m_pMesh = NULL;
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
	m_Resample = 0;
	m_selection = false;
	m_LoopOnly = false;
	m_Pitch = 0.0;
	m_fEst = 0.0;
	m_pQ = 0.0;
	m_ResynthFFT_Size = 1024;
	m_ResynthHopSize = 256;
	m_ResynthConvolution = 3;
	m_ResynthIterations = 3;
}

CMirageEditorDoc::~CMirageEditorDoc()
{
	m_pMesh = 0;
	m_pD3D = 0;
	m_pD3DDevice = 0;
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
	::GlobalUnlock((HGLOBAL) m_hWAV);
}

BOOL CMirageEditorDoc::CreateNewFromMirage(MWAV hWAV)
{
	DeleteContents();

	m_hWAV = hWAV;
	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) m_hWAV);
	m_sizeDoc = CSize((int) ::WAVSize(lpWAV), 1);
	::GlobalUnlock((HGLOBAL) m_hWAV);

	SetFromMirage();

	CheckPoint();

	SetModifiedFlag(FALSE);     // start off with unmodified

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

	if ( strstr(file.GetFileName(),"wav") == NULL )
	{
		CBankSelect BankSelectDlg;
		if ( BankSelectDlg.DoModal() == IDOK )
		{			
			theApp.DiskImage.SetFile(lpszPathName);
			switch (theApp.DiskImage.GetImageType())
			{
				case 0:	// Giebler EDM Image
					break;
				case 1: // Generic Image
					theApp.DiskImage.ReadGenericImage();
					SetPathName(lpszPathName);
					break;
				default: // Unknown file
					break;
			}
		}
	} else { // File seems to be a wave file
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
	}

	if (m_hWAV == NULL)
	{
		return FALSE;
	}

	InitWAVData();
	CheckPoint(); // save state for undo
	EndWaitCursor();

	SetPathName(lpszPathName);
	SetModifiedFlag(FALSE);     // start off with unmodified
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
		// TODO: Play in background
		//PlayWaveData(hWAV);
		DWORD ThreadEvent=WaitForSingleObject(AudioPlayingEvent,1);

		if ( ThreadEvent != WAIT_OBJECT_0 )
		{
			::AfxBeginThread((AFX_THREADPROC)PlayWaveData, hWAV);
		}
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
	m_ratio = 1.00025;
}

void CMirageEditorDoc::SetRatio(double ratio)
{
	m_ratio = ratio;
}

void CMirageEditorDoc::RatioDec()
{
	m_ratio = 0.99975;
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
	m_DisplayType = 'w';
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
		SAFE_RELEASE(m_pMesh);
		SAFE_RELEASE(m_pD3DDevice);
		SAFE_RELEASE(m_pD3D);
		::GlobalFree((HGLOBAL) m_hWAV);
	}

	COleDocument::OnCloseDocument();
}

BOOL CMirageEditorDoc::CreateD3DWindow(CDC *pDC, CRect WindowRect)
{
	HDC hDC=pDC->GetSafeHdc();
	HWND hWnd;

	hWnd=WindowFromDC(hDC);
	
  // Did We Get A Device Context?
  if (!(hDC))	
  {
		m_pD3DDevice = 0;
		m_pD3D = 0;
		MessageBox(hWnd,"Can't Create A Device Context.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
  }

    // Check For The Correct DirectX 3D version
	if ( NULL == (m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
  {
		m_pD3DDevice = 0;
		m_pD3D = 0;
		MessageBox(hWnd,"Can't find D3D SDK Version 9.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
  }
	
	// get the display mode
	D3DDISPLAYMODE d3ddm;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

    // Tell the window how we want things to be..
  D3DPRESENT_PARAMETERS d3dpp={	4*MIRAGE_PAGESIZE/*WindowRect.right*/,			// Back Buffer Width
																2*WindowRect.bottom,			// Back Buffer Height
																d3ddm.Format,		// Back Buffer Format (Color Depth)
																1,			// Back Buffer Count (Double Buffer)
																D3DMULTISAMPLE_NONE,	// No Multi Sample Type
																0,			// No Multi Sample Quality
																D3DSWAPEFFECT_DISCARD,	// Swap Effect (Fast)
																NULL,			// The Window Handle (Use Focus window)
																TRUE,		// Windowed
																TRUE,			// Enable Auto Depth Stencil  
																D3DFMT_D16,		// 16Bit Z-Buffer (Depth Buffer)
																0,			// No Flags
																D3DPRESENT_RATE_DEFAULT,   // Default Refresh Rate
																D3DPRESENT_INTERVAL_DEFAULT	// Presentation Interval (vertical sync)
    													};
	m_d3dpp = d3dpp;

	// Check The Wanted Surface Format
  if ( FAILED( m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
																					D3DDEVTYPE_HAL,
																					d3dpp.BackBufferFormat,
																					D3DUSAGE_DEPTHSTENCIL,
																					D3DRTYPE_SURFACE, d3dpp.AutoDepthStencilFormat ) ) )
  {
		m_pD3DDevice = 0;
		m_pD3D = 0;
		MessageBox(hWnd,"Can't Find Surface Format.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
  }

  // Create The DirectX 3D Device 
	if(FAILED( m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
																	D3DDEVTYPE_HAL,
																	hWnd,
																	/*D3DCREATE_MIXED_VERTEXPROCESSING|D3DCREATE_MULTITHREADED,*/
																	D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED,
																	&d3dpp,
																	&m_pD3DDevice ) ) )
	{
		m_pD3DDevice = 0;
		m_pD3D = 0;
		MessageBox(hWnd,"Can't Create DirectX 3D Device.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
  }

  ReSizeD3DScene(WindowRect.right, WindowRect.bottom);	// Set Up Our Perspective D3D Screen

  // Initialize Our Newly Created D3D Window
  if (!InitD3D())
  {
		m_pD3DDevice = 0;
		m_pD3D = 0;
		MessageBox(hWnd,"Initialization Failed.",
		"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;		// Return FALSE
  }

	return TRUE;			// Success
}

void CMirageEditorDoc::KillD3DWindow()
{
	if ( theApp.m_AppInit == false )
	{
		SAFE_RELEASE(m_pMesh);
		if ( m_pD3DDevice != NULL )
			m_pD3DDevice->Reset(&m_d3dpp);
		SAFE_RELEASE(m_pD3DDevice);
		SAFE_RELEASE(m_pD3D);
	}
}

BOOL CMirageEditorDoc::InitD3D()				// Setup For D3D Goes Here	
{
   int i;
   D3DLIGHT9	pLight[6];
   D3DXVECTOR3	vecDir[6];

   m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,  TRUE ); // Z-Buffer (Depth Buffer)
//   m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // Disable Backface Culling
   m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE); // Enable Light
//   pDoc->GetpD3DDevice()->SetRenderState(D3DRS_SPECULARENABLE, TRUE); // Enable specular lighting 
   m_pD3DDevice->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(255,255,255)); // Enable ambient light

   ZeroMemory(&pLight,sizeof(D3DLIGHT9)*6);

   // Create a direction for out light - it must be normalized
   vecDir[0] = D3DXVECTOR3(0.0f,-1.0f,0.0f);
   vecDir[1] = D3DXVECTOR3(0.0f,1.0f,0.0f);
   vecDir[2] = D3DXVECTOR3(1.0f,0.0f,0.0f);
   vecDir[3] = D3DXVECTOR3(-1.0f,0.0f,0.0f);
   vecDir[4] = D3DXVECTOR3(0.0f,0.0f,1.0f);
   vecDir[5] = D3DXVECTOR3(0.0f,0.0f,-1.0f);

   for ( i=0 ; i< 2 ; i++ )
   {
		pLight[i].Type = D3DLIGHT_DIRECTIONAL;
		D3DXVec3Normalize( (D3DXVECTOR3*)&pLight[i].Direction, &vecDir[i]);

		pLight[i].Diffuse.r=1.0f;
		pLight[i].Diffuse.g=1.0f;
		pLight[i].Diffuse.b=1.0f;
		pLight[i].Diffuse.a=1.0f;
		pLight[i].Range = 100000.0f;

		pLight[i].Ambient.r=1.0f;
		pLight[i].Ambient.g=1.0f;
		pLight[i].Ambient.b=1.0f;
		pLight[i].Ambient.a=1.0f;

		m_pD3DDevice->SetLight(i,&pLight[i]);
		m_pD3DDevice->LightEnable(i,TRUE);
   }
   return TRUE;				// Initialization Went OK
}

void CMirageEditorDoc::ReSizeD3DScene(int width, int height)
{
   if (height==0)				// Prevent A Divide By Zero By
   {
	height=1;				// Making Height Equal One
   }

   D3DXMATRIXA16 matProjection;		// Create A Projection Matrix

   // Calculate The Aspect Ratio Of The Window
   D3DXMatrixPerspectiveFovLH(&matProjection, D3DXToRadian( 45.0f ), static_cast<float>(width/height), 0.1f, 10000.0f);

   m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &matProjection );
   D3DXMatrixIdentity(&matProjection);	// Reset The Projection Matrix
}

void CMirageEditorDoc::SetMesh(LPD3DXMESH pMesh)
{
	m_pMesh = pMesh;
}

void CMirageEditorDoc::ReleaseMesh()
{
	SAFE_RELEASE(m_pMesh);
}

void CMirageEditorDoc::Create3DMesh(CRect Rect)
{
	_WaveSample_ *pWav;
	unsigned char MiragePages = 0;
	int z_pos = 0;
	int z_increment = 1.25;
	int x_pos = 0;

	HRESULT hr;
	UINT	Multiplier = m_PageMultiplier;

	unsigned char PageSkip = m_PageSkip;

	LPSTR lpWAV = (LPSTR) ::GlobalLock((HGLOBAL) m_hWAV);
	pWav = (_WaveSample_ *)lpWAV;
	::GlobalUnlock((HGLOBAL) m_hWAV);

	int z_factor = (Multiplier*MIRAGE_PAGESIZE) / GetNumberOfPages(pWav);

	struct mesh_vertex{
		    D3DXVECTOR3 p;
			D3DXVECTOR3 n;
			DWORD color;
	};

	DWORD	VertexCount=0;
//	int		nPages			= (int)ceil((float)(GetNumberOfPages(pWav)/Multiplier*PageSkip)); 
	int		nPages			= (int)ceil((float)(((pWav->data_header.dataSIZE & 0xFF00)/256)/Multiplier*PageSkip)); 
	int		nWidth			= (MIRAGE_PAGESIZE*Multiplier);
	int		nNumStrips      = nPages-1;
  int		nQuadsPerStrip  = nWidth-1;
	DWORD	m_dwNumFaces    = nNumStrips * nQuadsPerStrip * 2;
	DWORD	m_dwNumVertices = nPages * nWidth;

	hr = D3DXCreateMeshFVF(m_dwNumFaces,
							m_dwNumVertices,
							D3DXMESH_MANAGED|D3DXMESH_32BIT/*|D3DXMESH_WRITEONLY*/,
							D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE,
							m_pD3DDevice,
							&m_pMesh);

	DWORD* pIndexBuffer = NULL;
	m_pMesh->LockIndexBuffer(0,(void**) &pIndexBuffer );

	int iStrip, iQuad;

	for( iStrip = 0; iStrip<nNumStrips; iStrip++ )
	{
		WORD nCurRow1 = (iStrip+0)*nWidth;
		WORD nCurRow2 = (iStrip+1)*nWidth;

		for( iQuad = 0; iQuad<nQuadsPerStrip; iQuad++ )
		{
			// first tri 
			*pIndexBuffer++ = nCurRow1;
			*pIndexBuffer++ = nCurRow1 + 1;
			*pIndexBuffer++ = nCurRow2;

			// second tri 
			*pIndexBuffer++ = nCurRow1 + 1;
			*pIndexBuffer++ = nCurRow2 + 1;
			*pIndexBuffer++ = nCurRow2;

			nCurRow1++;
			nCurRow2++;
		}
	}
	
	hr = m_pMesh->UnlockIndexBuffer();

	mesh_vertex*	pVertexBuffer = NULL;
	hr = m_pMesh->LockVertexBuffer( 0, (void**) &pVertexBuffer );
	
	LPDIRECT3DVERTEXBUFFER9 pVB9;
	m_pMesh->GetVertexBuffer(&pVB9);
			
	D3DVERTEXBUFFER_DESC pDesc;
	pVB9->GetDesc(&pDesc);

	DWORD VertexBufferStart=(DWORD)pVertexBuffer;

	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );

	for( DWORD p = 0; p < pWav->data_header.dataSIZE ; p++ ) 
	{
		if ( p < pWav->data_header.dataSIZE)
		{
			if ( (p % (MIRAGE_PAGESIZE*Multiplier)) == 0 )
			{
				if ( p > 0 )
				{						
					MiragePages++;
					z_pos = z_pos+ z_increment;
					x_pos = 0;
				}
			} else {
				x_pos++;
			}
			/* Only Draw values larger than zero */
			//if ( buffer[ p ] > 0 ) 
			{
				pVertexBuffer->p=D3DXVECTOR3(static_cast<float>(x_pos/(Multiplier+0.0f)),
											GetWaveValue(pWav,x_pos,z_pos),
											static_cast<float>(z_factor*z_pos));
				// Compute the normal by hand
				D3DXVECTOR3 vecN;
				D3DXVECTOR3 vPt = D3DXVECTOR3(static_cast<float>(x_pos/(Multiplier+0.0f)),
											GetWaveValue(pWav,x_pos,z_pos),
											static_cast<float>(z_factor*z_pos));
				D3DXVECTOR3 vN = D3DXVECTOR3(static_cast<float>(x_pos/(Multiplier+0.0f)),
											GetWaveValue(pWav,x_pos,z_pos+1),
											static_cast<float>((z_factor*z_pos)+1.0f ));
				D3DXVECTOR3 vE = D3DXVECTOR3( static_cast<float>(x_pos/(Multiplier+0.0f))+1.0f,
											GetWaveValue(pWav,x_pos+1,z_pos),
											static_cast<float>((z_factor*z_pos) ));					D3DXVECTOR3 v1 = vN - vPt;
				D3DXVECTOR3 v2 = vE - vPt;
				D3DXVec3Cross( &vecN, &v1, &v2 );
				D3DXVec3Normalize(&vecN, &vecN);

				pVertexBuffer->n = vecN;
				if ( p < (pWav->sampler.Loops.dwStart+1) || p > pWav->sampler.Loops.dwEnd )
				{
					pVertexBuffer->color = D3DCOLOR_ARGB(255,0,255,0);
				} else {
					pVertexBuffer->color = D3DCOLOR_ARGB(255,255,0,0);
				}
			}
			if ( ((DWORD)pVertexBuffer - VertexBufferStart) > pDesc.Size )
				break;
			pVertexBuffer++;
			VertexCount++;
		}
	}
	pVB9->Release();

	m_pMesh->UnlockVertexBuffer();

	DWORD* pdwAdjacency  = NULL;
	pdwAdjacency = new DWORD[ 3 * m_pMesh->GetNumFaces() ];

	hr = m_pMesh->GenerateAdjacency(0.0f, pdwAdjacency);
	
	DWORD* pdwAdjacencyOut = NULL;
	pdwAdjacencyOut = new DWORD[ 3 * m_pMesh->GetNumFaces() ];

	DWORD dwFlags = D3DXMESHOPT_VERTEXCACHE; // Was D3DXMESHOPT_VERTEXCACHE
	dwFlags |= D3DXMESHOPT_COMPACT;
	hr = m_pMesh->OptimizeInplace( dwFlags, pdwAdjacency, pdwAdjacencyOut, NULL, NULL );

	if ( 0 == 0 )
	{
		DWORD* pdwAdjacencyOutWeld = NULL;
		pdwAdjacencyOutWeld = new DWORD[ 3 * m_pMesh->GetNumFaces() ];

		hr = D3DXWeldVertices(m_pMesh,
								D3DXWELDEPSILONS_WELDALL,
								NULL,
								pdwAdjacencyOut/*pAdjacencyBuffer->GetBufferPointer()*/,
								pdwAdjacencyOutWeld,
								NULL,
								NULL);
		SAFE_DELETE_ARRAY( pdwAdjacencyOutWeld);
	}

	SAFE_DELETE_ARRAY( pdwAdjacencyOut);

	SAFE_DELETE_ARRAY( pdwAdjacency );
	NormalizeMesh(m_pMesh, Rect.right * 1.0f, TRUE);

	m_pD3DDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID|D3DFILL_WIREFRAME);
  m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
}

HRESULT CMirageEditorDoc::CalcBounds(ID3DXMesh *pMesh, D3DXVECTOR3 *vCenter, float *radius)
{
	BYTE *ptr=NULL;
	HRESULT hr;

	// return failure if no mesh pointer provided
	if (!pMesh)
		return D3DERR_INVALIDCALL;

	// get the face count
	DWORD numVerts=pMesh->GetNumVertices();

	// get the FVF flags
	DWORD fvfSize=D3DXGetFVFVertexSize(pMesh->GetFVF());  // See DX8 Version

	// lock the vertex buffer
	if (FAILED(hr=pMesh->LockVertexBuffer(0,(LPVOID *)&ptr)))

		// return on failure
		return hr;

	// compute bounding sphere
	if (FAILED(hr=D3DXComputeBoundingSphere((D3DXVECTOR3 *) ptr, 
						numVerts, 
						fvfSize,   // See DX8 Version
						vCenter, radius )))
		// return on failure
		return hr;

	// unlock the vertex buffer
	if (FAILED(hr=pMesh->UnlockVertexBuffer()))

		// return on failure
		return hr;
		
	// return success to caller
	return S_OK;
}

HRESULT CMirageEditorDoc::NormalizeMesh(ID3DXMesh *pMesh, float scaleTo=1.0f, BOOL bCenter=TRUE)
{
	D3DXVECTOR3 vCenter;
	float radius;
	HRESULT hr;

	// calculate bounds of mesh
	if (FAILED(hr=CalcBounds(pMesh,&vCenter,&radius)))
		return hr;

	// calculate scaling factor
	float scale=scaleTo/radius;

	// calculate offset if centering requested
	D3DXVECTOR3 vOff;
	if (bCenter) 
		vOff=-vCenter;
	else
		vOff=D3DXVECTOR3(0.0f,0.0f,0.0f);

	// scale and offset mesh
	return ScaleMesh(pMesh,scale,&vOff);
}

HRESULT CMirageEditorDoc::ScaleMesh(ID3DXMesh *pMesh, float scale, D3DXVECTOR3 *offset=NULL)
{
	BYTE *ptr=NULL;
	HRESULT hr;
	D3DXVECTOR3 vOff;

	// return failure if no mesh pointer set
	if (!pMesh)
		return D3DERR_INVALIDCALL;

	// select default or specified offset vector
	if (offset)
		vOff=*offset;
	else
		vOff=D3DXVECTOR3(0.0f,0.0f,0.0f);

	// get the face count
	DWORD numVerts=pMesh->GetNumVertices();

	// get the FVF flags
	DWORD fvf=pMesh->GetFVF();

	// calculate vertex size
	DWORD vertSize=D3DXGetFVFVertexSize(fvf);

	// lock the vertex buffer
	if (FAILED(hr=pMesh->LockVertexBuffer(0,(LPVOID *)&ptr)))
	
		// return on failure
		return hr;

	// loop through the vertices
	for (DWORD i=0;i<numVerts;i++) {

		// get pointer to location
		D3DXVECTOR3 *vPtr=(D3DXVECTOR3 *) ptr;

		// scale the vertex
		*vPtr+=vOff;
		vPtr->x*=scale;
		vPtr->y*=scale;
		vPtr->z*=scale;

		// increment pointer to next vertex
		ptr+=vertSize;
	}

	// unlock the vertex buffer
	if (FAILED(hr=pMesh->UnlockVertexBuffer()))

		// return on failure
		return hr;
		
	// return success to caller
	return S_OK;
}


float CMirageEditorDoc::GetWaveValue(_WaveSample_ *pWav,int x, int z)
{	
	const AudioByte *buffer = reinterpret_cast< AudioByte* >( &pWav->SampleData );
	
	float WaveValue;

	int samplepos;
	int maxsize=pWav->data_header.dataSIZE;
	
	samplepos=x+(m_PageMultiplier*MIRAGE_PAGESIZE*z);
	if ( samplepos < maxsize )
	{
		if ( buffer[samplepos] >= 1 )
		{
		  WaveValue=static_cast<float>((255-buffer[samplepos])/2.0f);
		} else {
			WaveValue=64.0f*2;
		}

		return (WaveValue);
	}
	return 0;
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
void CMirageEditorDoc::SetSelection(bool selection)
{
	m_selection = selection;
}

void CMirageEditorDoc::SetLoopOnly(bool LoopOnly)
{
	m_LoopOnly = LoopOnly;
}

void CMirageEditorDoc::SetValPointer(bool ValPointer, int ValPosition)
{
	m_bValPointer = ValPointer;
  m_iValPosition = ValPosition;
}

void CMirageEditorDoc::SetPitch(double pitch)
{
	m_Pitch = pitch;
}

void CMirageEditorDoc::SetFreqEst(double freq)
{
	m_fEst = freq;
}

void CMirageEditorDoc::SetEstQ(double q)
{
	m_pQ = q;
}