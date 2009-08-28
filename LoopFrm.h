// ChildFrm.h : interface of the CChildFrame class
//


#pragma once
#include "DFV/DFVFrameWnd.h"

class CLoopFrame : public CDFVFrameWnd
{
	DECLARE_DYNCREATE(CLoopFrame)
public:
	CLoopFrame();

// Attributes
public:

// Operations
public:

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CLoopFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
