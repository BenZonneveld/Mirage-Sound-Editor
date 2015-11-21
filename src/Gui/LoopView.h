// SampleView.h : interface of the CSampleView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLEVIEW_H__9C01C9C9_9F87_11D5_8F75_0048546F01E7__INCLUDED_)
#define AFX_SAMPLEVIEW_H__9C01C9C9_9F87_11D5_8F75_0048546F01E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../WaveApi/wavesamples.h"

class CLoopView : public CView
{
protected: // create from serialization only
	CLoopView();
	DECLARE_DYNCREATE(CLoopView)

// Attributes
public:
	CMirageEditorDoc* GetDocument();
	static void		LoopEndF();
	static void		LoopEndB();
	static void		LoopEndFineF();
	static void		LoopEndFineB();
	static void		LoopStartF();
	static void		LoopStartB();
	static void		LoopEndPage();
	static void		AlternateLoop();
	static bool		ToggleLoop();
	static bool		LoopStatus();
	static void		UseLoop();
	static _WaveSample_	m_sWav;
	static DWORD	m_LoopStart;
	static DWORD	m_LoopEnd;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnDrawOld(CDC * pDC); // Old draw
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLoopView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Generated message map functions
protected:
	//{{AFX_MSG(CSampleView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

};

#ifndef _DEBUG  // debug version in SampleView.cpp
inline CMirageEditorDoc* CLoopView::GetDocument()
   { return (CMirageEditorDoc*)theApp.m_CurrentDoc; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAMPLEVIEW_H__9C01C9C9_9F87_11D5_8F75_0048546F01E7__INCLUDED_)
