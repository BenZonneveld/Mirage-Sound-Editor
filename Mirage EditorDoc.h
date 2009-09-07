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
	MWAV GetMWAV() const
		{ return m_hWAV; }
	CSize GetDocSize() const
		{ return m_sizeDoc; }
	void ZoomInc();
	void ZoomIncTen();
	void ZoomDec();
	void ZoomDecTen();
	void RatioInc();
	void RatioDec();
	char DisplayType() const
		{ return m_DisplayType; }
	bool DisplayTypeWavedraw();
	bool DisplayType3DTypeA();
	bool DisplayType3DTypeB();
	double ZoomLevel() const
		{ return m_ZoomLevel; }
	unsigned char PageSkip() const
		{ return m_PageSkip; }
	double GetRatio() const
		{ return m_ratio; } 
	void ResetZoom();
	bool	m_startpoint_selected;
	bool	m_endpoint_selected;
	void	SetFromMirage();
	void	NotFromMirage();
	bool	FromMirage() const
		{ return m_FromMirage; }
	LPDIRECT3D9 GetpD3D() const
		{ return (LPDIRECT3D9)m_pD3D; }
	void	SetpD3D(LPDIRECT3D9 pD3D);
	LPDIRECT3DDEVICE9 GetpD3DDevice() const
		{ return (LPDIRECT3DDEVICE9)m_pD3DDevice; }
	void	SetpD3DDevice(LPDIRECT3DDEVICE9 pD3DDevice);

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
	LONG_PTR		m_pD3D;	//LPDIRECT3D9 DirectX 3D Version 9
	LONG_PTR		m_pD3DDevice;	//LPDIRECT3DDEVICE9 DirectX 3D Rendering Device

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPlayAudio();
	void OnCloseWindow();
};


