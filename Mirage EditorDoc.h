// Mirage EditorDoc.h : interface of the CMirageEditorDoc class
// $Id: Mirage\040EditorDoc.h,v 1.12 2008/05/05 16:13:25 root Exp $
#include "Wavapi.h"
#include "d3dx9.h"
#include "d3d9.h"

#pragma once


class CMirageEditorDoc : public COleDocument, public CUndo
{
protected: // create from serialization only
	CMirageEditorDoc();
	DECLARE_DYNCREATE(CMirageEditorDoc)

// Attributes
public:
	MWAV	GetMWAV() const
		{ return m_hWAV; }
	CSize	GetDocSize() const
		{ return m_sizeDoc; }
	void	ZoomInc();
	void	ZoomIncTen();
	void	ZoomDec();
	void	ZoomDecTen();
	void	RatioInc();
	void	RatioDec();
	void	SetRatio(double ratio);
	char	DisplayType() const
		{ return m_DisplayType; }
	bool	DisplayTypeWavedraw();
	bool	DisplayType3DTypeA();
	bool	DisplayType3DTypeB();
	double	ZoomLevel() const
		{ return m_ZoomLevel; }
	unsigned char PageSkip() const
		{ return m_PageSkip; }
	double	GetRatio() const
		{ return m_ratio; } 
	void	ResetZoom();
	bool	GetSelection() const
		{ return m_selection; }
	void	SetSelection(bool selection);
	bool	m_startpoint_selected;
	bool	m_endpoint_selected;
	int		SelectionStart;
	int		SelectionEnd;
	void	SetFromMirage();
	void	NotFromMirage();
	bool	FromMirage() const
		{ return m_FromMirage; }
	void	SetPageMultiplier(UINT Multiplier);
	UINT	GetPageMultiplier() const
		{ return m_PageMultiplier; }

	/* 3D Functions */
	BOOL	CreateD3DWindow(CDC *pDC, CRect WindowRect);
	BOOL	InitD3D();
	void	KillD3DWindow();
	void	ReSizeD3DScene(int width, int height);
	LPDIRECT3DDEVICE9 GetpD3DDevice() const
		{ return m_pD3DDevice; }
	void	Create3DMesh(CRect Rect);
	HRESULT	CalcBounds(ID3DXMesh *pMesh, D3DXVECTOR3 *vCenter, float *radius);
	HRESULT	NormalizeMesh(ID3DXMesh *pMesh, float scaleTo, BOOL bCenter);
	HRESULT	ScaleMesh(ID3DXMesh *pMesh, float scale, D3DXVECTOR3 *offset);
	void	SetpD3DDevice(LPDIRECT3DDEVICE9 pD3DDevice);

	float	GetWaveValue(_WaveSample_ *pWav,int x, int z);
	LPD3DXMESH GetMesh() const
		{ return m_pMesh; }
	void	FreeD3DDevice();
	void	SetMesh(LPD3DXMESH pMesh);
	void	FreeMesh();
	void	SetPitchYaw(CPoint point);
	void	SetLastMouse(CPoint point);
	CPoint	GetPitchYaw() const
		{ return m_PitchYaw; }
	int		GetZ_Offset() const
		{ return m_z_offset; }
	long	GetResample() const
		{ return m_Resample; }
	bool	GetLoopOnly() const
		{ return m_LoopOnly; }
	void	SetLoopOnly(bool LoopOnly);
	void	SetValPointer(bool ValPointer, int ValPosition);
	bool	GetValPointer() const
		{ return m_bValPointer; }
  int   GetValPosition() const
    { return m_iValPosition; }
	void ReleaseMesh();
 // Operations
public:
	void ReplaceMWAV(MWAV hWAV);
	void InitWAVData();
	BOOL CreateNewFromMirage(MWAV hWAV);

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();

// Implementation
public:
	virtual ~CMirageEditorDoc();

	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	MWAV			m_hWAV;
	CSize			m_sizeDoc;
	double			m_ZoomLevel;
	unsigned char	m_PageSkip;
	bool			m_FromMirage;
	char			m_DisplayType;
	double			m_ratio;
	UINT			m_PageMultiplier;
	int				m_z_offset;
	D3DPRESENT_PARAMETERS m_d3dpp;
//	CComPtr<IDirect3D9>m_pD3D;
	LPDIRECT3D9 m_pD3D;
//	CComPtr<IDirect3DDevice9>m_pD3DDevice;
	LPDIRECT3DDEVICE9		m_pD3DDevice;	//LPDIRECT3DDEVICE9 DirectX 3D Rendering Device
//	CComPtr<ID3DXMesh>m_pMesh; // LPD3DXMESH DirectX 3D Mesh
	LPD3DXMESH		m_pMesh; // LPD3DXMESH DirectX 3D Mesh
	CPoint			m_PitchYaw;
	CPoint			m_LastMouse;
	long			m_Resample;
	bool			m_selection;
	bool			m_LoopOnly;
	bool			m_bValPointer;
  int       m_iValPosition;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPlayAudio();
	void OnCloseWindow();
};


