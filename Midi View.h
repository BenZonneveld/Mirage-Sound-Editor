#pragma once
#include "stdafx.h"
// CMidiView view

class CMidiView : public CScrollView
{
	DECLARE_DYNCREATE(CMidiView)
private:
	using CScrollView::Create;

protected:

	CMidiView();           // protected constructor used by dynamic creation
	virtual ~CMidiView();

public:
	BOOL Create(LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName, DWORD dwStyle,
	const RECT& rect,
	CWnd* pParentWnd, UINT nID,
	CCreateContext* pContext);
	//BOOL Create(LPCTSTR lpszClassName, LPCTSTR szTitle, DWORD style,
	//				const RECT& rect,
	//				CWnd* pParent,
	//				UINT nID,
	//				CCreateContext* pContext = 0);
	//virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	CMidiDoc*		GetDocument();
	static CMidiView * GetView();
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

private:
	CBitmap CacheBitmap;
public:
	BOOL CacheValid;
	CRect CacheRect;
	void SetCacheSize(SIZE sz) {
		if (CacheRect.IsRectNull()) 
			CacheRect=CRect(CPoint(0,0),sz);
	}

protected:
	afx_msg LRESULT OnPutData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnParseSysex(WPARAM wParam, LPARAM lParam);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnPaint();

private:
	CFont m_ftTimes;
	int m_nLineHt;
};
