// CntrItem.h : interface of the CMirageEditorCntrItem class
//

#pragma once

class CMirageEditorDoc;
class CMirageEditorView;

class CMirageEditorCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CMirageEditorCntrItem)

// Constructors
public:
	CMirageEditorCntrItem(CMirageEditorDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer

// Attributes
public:
	CMirageEditorDoc* GetDocument()
		{ return reinterpret_cast<CMirageEditorDoc*>(COleClientItem::GetDocument()); }
	CMirageEditorView* GetActiveView()
		{ return reinterpret_cast<CMirageEditorView*>(COleClientItem::GetActiveView()); }

	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);

// Implementation
public:
	~CMirageEditorCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

