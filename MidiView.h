#pragma once



// CMidiView view

class CMidiView : public CScrollView
{
	DECLARE_DYNCREATE(CMidiView)

protected:
	CMidiView();           // protected constructor used by dynamic creation
	virtual ~CMidiView();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	CMidiDoc*		GetDocument()
								{ return (CMidiDoc*)m_pDocument; }
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

	DECLARE_MESSAGE_MAP()
};
