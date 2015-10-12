// Wave Doc.h : interface of the CLoopDoc class
//
#include "Wavapi.h"
#include "DFV/DFVDocument.h"

#pragma once


class CLoopDoc : public CDFVDocument
{
protected: // create from serialization only
	CLoopDoc();
	DECLARE_DYNCREATE(CLoopDoc)

// Attributes
public:
	MWAV GetMWAV() const
		{ return m_hWAV; }
	CSize GetDocSize() const
		{ return m_sizeDoc; }
	MWAV	pWAV;

// Operations
public:
//	void InitWAVData();
//	BOOL CreateLoopFromActive(MWAV hWAV);

// Overrides
public:

// Implementation
public:
	virtual ~CLoopDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	MWAV	m_hWAV;
	CSize	m_sizeDoc;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnPlayAudio();
};


