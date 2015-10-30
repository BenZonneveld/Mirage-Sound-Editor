// MyDocTemplate.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "MyDocTemplate.h"

/////////////////////////////////////////////////////////////////////////////
// CMyDocTemplate construction/destruction
//IMPLEMENT_DYNAMIC(CMyDocTemplate, CMultiDocTemplate)

CMyDocTemplate::CMyDocTemplate(UINT nIDResource, 
															 CRuntimeClass* pDocClass,
															 CRuntimeClass* pFrameClass,
															 CRuntimeClass* pViewClass)
: CMultiDocTemplate(nIDResource, pDocClass, pFrameClass, pViewClass)
{
}

//void CMyDocTemplate::LoadTemplate() : CMultiDocTemplate::LoadTemplate()
//{
//}

//void CMyDocTemplate::SetServerInfo(UINT nIDOleEmbedding, UINT nIDOleInPlaceServer,
//	CRuntimeClass* pOleFrameClass, CRuntimeClass* pOleViewClass)
//{
//	ASSERT_VALID_IDR(nIDOleEmbedding);
//	if (nIDOleInPlaceServer != 0)
//		ASSERT_VALID_IDR(nIDOleInPlaceServer);
//	ASSERT(pOleFrameClass == NULL ||
//		pOleFrameClass->IsDerivedFrom(RUNTIME_CLASS(CFrameWnd)));
//	ASSERT(pOleViewClass == NULL ||
//		pOleViewClass->IsDerivedFrom(RUNTIME_CLASS(CView)));
//
//	m_pOleFrameClass = pOleFrameClass;
//	m_pOleViewClass = pOleViewClass;
//
//	m_nIDEmbeddingResource = nIDOleEmbedding;
//	m_nIDServerResource = nIDOleInPlaceServer;
//	if (!CDocManager::bStaticInit)
//		LoadTemplate();
//}
//
//void CMyDocTemplate::SetContainerInfo(UINT nIDOleInPlaceContainer)
//{
//	ASSERT(nIDOleInPlaceContainer != 0);
//
//	m_nIDContainerResource = nIDOleInPlaceContainer;
//	if (!CDocManager::bStaticInit)
//		LoadTemplate();
//}

CMyDocTemplate::~CMyDocTemplate()
{
	// delete OLE resources
	if (m_hMenuInPlace != NULL)
		::DestroyMenu(m_hMenuInPlace);
	if (m_hAccelInPlace != NULL)
		::FreeResource(m_hAccelInPlace);
	if (m_hMenuEmbedding != NULL)
		::DestroyMenu(m_hMenuEmbedding);
	if (m_hAccelEmbedding != NULL)
		::FreeResource(m_hAccelEmbedding);
	if (m_hMenuInPlaceServer != NULL)
		::DestroyMenu(m_hMenuInPlaceServer);
	if (m_hAccelInPlaceServer != NULL)
		::FreeResource(m_hAccelInPlaceServer);
}

/////////////////////////////////////////////////////////////////////////////
// CMyDocTemplate attributes

//BOOL CMyDocTemplate::GetDocString(CString& rString, enum DocStringIndex i) const
//{
//	return AfxExtractSubString(rString, m_strDocStrings, (int)i);
//}

/////////////////////////////////////////////////////////////////////////////
// Document management

//void CMyDocTemplate::AddDocument(CDocument* pDoc)
//{
//	ASSERT_VALID(pDoc);
//	ASSERT(pDoc->m_pDocTemplate == NULL);   // no template attached yet
//	pDoc->m_pDocTemplate = this;
//}
//
//void CMyDocTemplate::RemoveDocument(CDocument* pDoc)
//{
//	ASSERT_VALID(pDoc);
//	ASSERT(pDoc->m_pDocTemplate == this);   // must be attached to us
//	pDoc->m_pDocTemplate = NULL;
//}
//
//CMyDocTemplate::Confidence CMyDocTemplate::MatchDocType(LPCTSTR lpszPathName,
//	CDocument*& rpDocMatch)
//{
//	ASSERT(lpszPathName != NULL);
//	rpDocMatch = NULL;
//
//	// go through all documents
//	POSITION pos = GetFirstDocPosition();
//	while (pos != NULL)
//	{
//		CDocument* pDoc = GetNextDoc(pos);
//		if (AfxComparePath(pDoc->GetPathName(), lpszPathName))
//		{
//			// already open
//			rpDocMatch = pDoc;
//			return yesAlreadyOpen;
//		}
//	}
//
//	// see if it matches our default suffix
//	CString strFilterExt;
//	if (GetDocString(strFilterExt, CMyDocTemplate::filterExt) &&
//	  !strFilterExt.IsEmpty())
//	{
//		// see if extension matches
//		ASSERT(strFilterExt[0] == '.');
//		LPCTSTR lpszDot = ::PathFindExtension(lpszPathName);
//		if (lpszDot != NULL)
//        {
//            if(::AfxComparePath(lpszDot, static_cast<const TCHAR *>(strFilterExt)))
//            {
//			    return yesAttemptNative; // extension matches, looks like ours
//            }
//        }
//	}
//
//	// otherwise we will guess it may work
//	return yesAttemptForeign;
//}
//
//CDocument* CMyDocTemplate::CreateNewDocument()
//{
//	// default implementation constructs one from CRuntimeClass
//	if (m_pDocClass == NULL)
//	{
//		TRACE(traceAppMsg, 0, "Error: you must override CMyDocTemplate::CreateNewDocument.\n");
//		ASSERT(FALSE);
//		return NULL;
//	}
//	CDocument* pDocument = (CDocument*)m_pDocClass->CreateObject();
//	if (pDocument == NULL)
//	{
//		TRACE(traceAppMsg, 0, "Warning: Dynamic create of document type %hs failed.\n",
//			m_pDocClass->m_lpszClassName);
//		return NULL;
//	}
//	ASSERT_KINDOF(CDocument, pDocument);
//	AddDocument(pDocument);
//	return pDocument;
//}

/////////////////////////////////////////////////////////////////////////////
// Default frame creation

CFrameWnd* CMyDocTemplate::CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther)
{
	if (pDoc != NULL)
		ASSERT_VALID(pDoc);
	// create a frame wired to the specified document

	ASSERT(m_nIDResource != 0); // must have a resource ID to load from
	CCreateContext context;
	context.m_pCurrentFrame = pOther;
	context.m_pCurrentDoc = pDoc;
	context.m_pNewViewClass = m_pViewClass;
	context.m_pNewDocTemplate = (CDocTemplate*)this;

	if (m_pFrameClass == NULL)
	{
		TRACE(traceAppMsg, 0, "Error: you must override CMyDocTemplate::CreateNewFrame.\n");
		ASSERT(FALSE);
		return NULL;
	}
	CFrameWnd* pFrame = (CFrameWnd*)m_pFrameClass->CreateObject();
	if (pFrame == NULL)
	{
		TRACE(traceAppMsg, 0, "Warning: Dynamic create of frame %hs failed.\n",
			m_pFrameClass->m_lpszClassName);
		return NULL;
	}
	ASSERT_KINDOF(CFrameWnd, pFrame);

	if (context.m_pNewViewClass == NULL)
		TRACE(traceAppMsg, 0, "Warning: creating frame with no default view.\n");

	// create new from resource
	if (!pFrame->LoadFrame(m_nIDResource,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
			NULL, &context))
	{
		TRACE(traceAppMsg, 0, "Warning: CMyDocTemplate couldn't create a frame.\n");
		// frame will be deleted in PostNcDestroy cleanup
		return NULL;
	}

	// it worked !
	return pFrame;
}

//CFrameWnd* CMyDocTemplate::CreateOleFrame(CWnd* pParentWnd, CDocument* pDoc,
//	BOOL bCreateView)
//{
//	CCreateContext context;
//	context.m_pCurrentFrame = NULL;
//	context.m_pCurrentDoc = pDoc;
//	context.m_pNewViewClass = bCreateView ? m_pOleViewClass : NULL;
//	context.m_pNewDocTemplate = this;
//
//	if (m_pOleFrameClass == NULL)
//	{
//		TRACE(traceAppMsg, 0, "Warning: pOleFrameClass not specified for doc template.\n");
//		return NULL;
//	}
//
//	ASSERT(m_nIDServerResource != 0); // must have a resource ID to load from
//	CFrameWnd* pFrame = (CFrameWnd*)m_pOleFrameClass->CreateObject();
//	if (pFrame == NULL)
//	{
//		TRACE(traceAppMsg, 0, "Warning: Dynamic create of frame %hs failed.\n",
//			m_pOleFrameClass->m_lpszClassName);
//		return NULL;
//	}
//
//	// create new from resource (OLE frames are created as child windows)
//	if (!pFrame->LoadFrame(m_nIDServerResource,
//		WS_CHILD|WS_CLIPSIBLINGS, pParentWnd, &context))
//	{
//		TRACE(traceAppMsg, 0, "Warning: CMyDocTemplate couldn't create an OLE frame.\n");
//		// frame will be deleted in PostNcDestroy cleanup
//		return NULL;
//	}
//
//	// it worked !
//	return pFrame;
//}

void CMyDocTemplate::InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
	BOOL bMakeVisible)
{
	// just delagate to implementation in CFrameWnd
	pFrame->InitialUpdateFrame(pDoc, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CMyDocTemplate commands and command helpers

//BOOL CMyDocTemplate::SaveAllModified()
//{
//	POSITION pos = GetFirstDocPosition();
//	while (pos != NULL)
//	{
//		CDocument* pDoc = GetNextDoc(pos);
//		if (!pDoc->SaveModified())
//			return FALSE;
//	}
//	return TRUE;
//}
//
//
//void CMyDocTemplate::CloseAllDocuments(BOOL)
//{
//	POSITION pos = GetFirstDocPosition();
//	while (pos != NULL)
//	{
//		CDocument* pDoc = GetNextDoc(pos);
//		pDoc->OnCloseDocument();
//	}
//}
//
//void CMyDocTemplate::OnIdle()
//{
//	POSITION pos = GetFirstDocPosition();
//	while (pos != NULL)
//	{
//		CDocument* pDoc = GetNextDoc(pos);
//		ASSERT_VALID(pDoc);
//		ASSERT_KINDOF(CDocument, pDoc);
//		pDoc->OnIdle();
//	}
//}

//BOOL CMyDocTemplate::OnCmdMsg(UINT nID, int nCode, void* pExtra,
//	AFX_CMDHANDLERINFO* pHandlerInfo)
//{
//	BOOL bReturn;
//	CCmdTarget* pFactory = DYNAMIC_DOWNCAST(CCmdTarget, m_pAttachedFactory);
//
//	if (nCode == CN_OLE_UNREGISTER && pFactory != NULL)
//		bReturn = pFactory->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
//	else
//		bReturn = CCmdTarget::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
//
//	return bReturn;
//}

/////////////////////////////////////////////////////////////////////////////
// CMyDocTemplate diagnostics

//#ifdef _DEBUG
//void CMyDocTemplate::Dump(CDumpContext& dc) const
//{
//	CCmdTarget::Dump(dc);
//
//	dc << "m_nIDResource = " << m_nIDResource;
//	dc << "\nm_strDocStrings: " << m_strDocStrings;
//
//	if (m_pDocClass)
//		dc << "\nm_pDocClass = " << m_pDocClass->m_lpszClassName;
//	else
//		dc << "\nm_pDocClass = NULL";
//
//	if (dc.GetDepth() > 0)
//	{
//		dc << "\ndocument list = {";
//		POSITION pos = GetFirstDocPosition();
//		while (pos != NULL)
//		{
//			CDocument* pDoc = GetNextDoc(pos);
//			dc << "\ndocument " << pDoc;
//		}
//		dc << "\n}";
//	}
//
//	dc << "\n";
//}
//
//void CMyDocTemplate::AssertValid() const
//{
//	CCmdTarget::AssertValid();
//
//	POSITION pos = GetFirstDocPosition();
//	while (pos != NULL)
//	{
//		CDocument* pDoc = GetNextDoc(pos);
//		ASSERT_VALID(pDoc);
//	}
//}
//#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CMyDocTemplate, CCmdTarget)
END_MESSAGE_MAP()



// CMyDocTemplate message handlers
