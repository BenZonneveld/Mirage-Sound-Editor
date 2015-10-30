
#pragma once


// CMyDocTemplate command target

class CMyDocTemplate : public CMultiDocTemplate
{
//	DECLARE_DYNAMIC(CMyDocTemplate)
//	using CMultiDocTemplate;
protected:

public:
	//	CMyDocTemplate();
	CMyDocTemplate(UINT nIDResource,
								 CRuntimeClass* pDocClass,
								 CRuntimeClass* pFrameClass,
								 CRuntimeClass* pViewClass);
//	void LoadTemplate();
	virtual ~CMyDocTemplate();
	CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
	void InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,	BOOL bMakeVisible = TRUE);


protected:
	//UINT m_nIDResource;
	//UINT m_nIDServerResource;
	//UINT m_nIDEmbeddingResource;
	//UINT m_nIDContainerResource;

	//CRuntimeClass* m_pDocClass;
	//CRuntimeClass* m_pFrameClass;
	//CRuntimeClass* m_pViewClass;
	//CRuntimeClass* m_pOleFrameClass;
	//CRuntimeClass* m_pOleViewClass;

	//CObject*	m_pAttachedFactory;
	//HMENU		m_hMenuInPlace;
	//HACCEL	m_hAccelInPlace;
	//HMENU		m_hMenuEmbedding;
	//HACCEL	m_hAccelEmbedding;
	//HMENU		m_hMenuInPlaceServer;
	//HACCEL	m_hAccelInPlaceServer;

	//BOOL m_bAutoDelete;

//	CString m_strDocStrings;
	DECLARE_MESSAGE_MAP()
};


