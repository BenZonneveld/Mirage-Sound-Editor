// MidiDoc.cpp : implementation file
//

#include "stdafx.h"
#include <string>
#include "Mirage Editor.h"
#include "Midi Doc.h"

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


BEGIN_MESSAGE_MAP(CMidiDoc, CDocument)
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
void CMidiDoc::OnUpdateAllViews()
{
	for(POSITION pos = GetFirstViewPosition(); pos != NULL; )
	{
		CView* pView = GetNextView(pos);
		pView->Invalidate();
		pView->UpdateWindow();
	} // end of loop through views
}

// CMidiDoc commands
void CMidiDoc::PutData(string Data, BOOL IO_Dir)
{
	if ( ! Data.empty() )
	{
		int queSize = m_MidiData.size();
		if ( queSize > m_MaxQueue )
		{
			m_MidiData.pop_front();
			m_MidiIO.pop_front();
		}
		m_MidiData.push_back(Data);
		m_MidiIO.push_back(IO_Dir);
		ResetEvent(theApp.m_MidiMonitorThread->MessageFlag);
		OnUpdateAllViews();
	}
}

string CMidiDoc::GetData(int line)
{
	return m_MidiData.at(line);
}

BOOL CMidiDoc::GetIO(int line)
{
	return m_MidiIO.at(line);
}

int CMidiDoc::GetSize()
{
	return m_MidiData.size();
}
