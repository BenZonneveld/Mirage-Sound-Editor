// MidiDoc.cpp : implementation file
//

#include "stdafx.h"
#include "Mirage Editor.h"
#include "MidiDoc.h"


// CMidiDoc

IMPLEMENT_DYNCREATE(CMidiDoc, CDocument)

CMidiDoc::CMidiDoc()
{
}

BOOL CMidiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CMidiDoc::~CMidiDoc()
{
}


BEGIN_MESSAGE_MAP(CMidiDoc, COleDocument)
END_MESSAGE_MAP()


// CMidiDoc diagnostics

#ifdef _DEBUG
void CMidiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CMidiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CMidiDoc serialization

void CMidiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
#endif


// CMidiDoc commands
