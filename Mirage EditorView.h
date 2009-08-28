// $Id: Mirage\040EditorView.h,v 1.17 2008/05/05 16:13:25 root Exp $ Mirage EditorView.h : interface of the CMirageEditorView class
//

#pragma once

class CMirageEditorCntrItem;

class CMirageEditorView : public CScrollView
{
protected: // create from serialization only
	CMirageEditorView();
	DECLARE_DYNCREATE(CMirageEditorView)

// Attributes
public:
	CMirageEditorDoc* GetDocument() const;
	// m_pSelection holds the selection to the current CMirageEditorCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CMirageEditorCntrItem objects.  This selection
	//  mechanism is provided just to help you get started

	// TODO: replace this selection mechanism with one appropriate to your app
	CMirageEditorCntrItem* m_pSelection;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void Mode_Wavedraw(CDC* pDC);
	virtual void Mode_3dTypeA(CDC* pDC);
	virtual void Mode_3dTypeB(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);

protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL IsSelected(const CObject* pDocItem) const;// Container support
	
// Implementation
public:
	virtual ~CMirageEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
//	afx_msg void OnLButtonUp();
	afx_msg void OnMouseMove( UINT nFlags, CPoint point);
	afx_msg void OnToolsLoopwindow();
	afx_msg void OnMirageSendsample();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnToolsReversesample();
	afx_msg void OnToolsResample();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnToolsNormalize();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDisplaytypeWavedraw();
	afx_msg void OnUpdateDisplaytype3dtypea();
	afx_msg void OnUpdateDisplaytype3dtypeb();
};

#ifndef _DEBUG  // debug version in Mirage EditorView.cpp
inline CMirageEditorDoc* CMirageEditorView::GetDocument() const
   { return reinterpret_cast<CMirageEditorDoc*>(m_pDocument); }
#endif
