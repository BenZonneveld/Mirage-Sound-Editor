// ChildFrm.h : interface of the CChildFrame class
//

#include "ruler.h"
#pragma once


class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

private:
	CRulerSplitterWnd m_Rulers; //Ruler object
public:
	void ShowRulers(BOOL bShow);//Toggle the ruler
    void UpdateRulersInfo(stRULER_INFO stRulerInfo);//Update the ruler

// Attributes
public:

// Operations
public:

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
